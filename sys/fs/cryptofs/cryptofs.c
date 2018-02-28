/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_fs_cryptofs
 * @{
 *
 * @file
 * @brief       CryptoFS implementation
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <errno.h>
#include <fcntl.h>

#include "byteorder.h"
#include "kernel_defines.h"
#include "fs/cryptofs.h"

#define MIN(a, b)   ((a) < (b) ? (a) : (b))

enum {
    STATE_INVALID,
    STATE_ENCRYPTED,
    STATE_UNENCRYPTED,
};

static int real_name(cryptofs_t *fs, const char *name, cryptofs_file_t *file)
{
    memset(file->name, 0, sizeof(file->name));

    if (fs->real_fs->mount_point_len > 1) {
        return snprintf(file->name, sizeof(file->name) - 1, "%s/%s",
                        fs->real_fs->mount_point, name);
    }
    else {
        return snprintf(file->name, sizeof(file->name) - 1, "%s%s",
                        fs->real_fs->mount_point, name);
    }
}

static int _mount(vfs_mount_t *mountp)
{
    cryptofs_t *fs = mountp->private_data;

    BUILD_BUG_ON(VFS_FILE_BUFFER_SIZE < sizeof(cryptofs_file_t));

    return vfs_mount(fs->real_fs);
}

static int _umount(vfs_mount_t *mountp)
{
    cryptofs_t *fs = mountp->private_data;

    return vfs_umount(fs->real_fs);
}

static int create_file(cryptofs_file_t *file, int fd)
{
    /* Write Magic */
    be_uint32_t magic = byteorder_htonl(CRYPTOFS_MAGIC_WORD);
    if (vfs_write(fd, &magic, sizeof(magic)) != sizeof(magic)) {
        vfs_close(fd);
        vfs_unlink(file->name);
        return -EIO;
    }
    /* Write padding */
    for (unsigned i = 0; i < CRYPTOFS_HEAD_PAD; i++) {
        uint8_t c = i;
        if (vfs_write(fd, &c, 1) != 1) {
            vfs_close(fd);
            vfs_unlink(file->name);
            return -EIO;
        }
    }
    /* Reserve sha space */
    for (unsigned i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        uint8_t c = i;
        if (vfs_write(fd, &c, 1) != 1) {
            vfs_close(fd);
            vfs_unlink(file->name);
            return -EIO;
        }
    }
    file->real_fd = fd;
    return 0;
}

static int calc_hash(cryptofs_file_t *file, int fd, uint8_t *sha)
{
    int res = vfs_lseek(fd, CRYPTOFS_HEAD_SIZE, 0);
    uint8_t buf[SHA256_INTERNAL_BLOCK_SIZE];
    sha256_init(&file->hash_ctx);
    while ((res = vfs_read(fd, buf, sizeof(buf))) > 0) {
        sha256_update(&file->hash_ctx, buf, res);
    }
    sha256_final(&file->hash_ctx, sha);

    return 0;
}

static int load_head(cryptofs_file_t *file, int fd)
{
    int res = vfs_lseek(fd, 0, 0);
    if (res < 0) {
        return res;
    }
    be_uint32_t magic;
    res = vfs_read(fd, &magic, sizeof(magic));
    if (res < 0) {
        return res;
    }
    if (byteorder_ntohl(magic) != CRYPTOFS_MAGIC_WORD) {
        return -EBADF;
    }
    res = vfs_read(fd, &file->nb_pad, 1);
    if (res < 0) {
        return res;
    }
    res = vfs_lseek(fd, CRYPTOFS_HEAD_HASH_OFFSET, 0);
    if (res < 0) {
        return res;
    }
    res = vfs_read(fd, file->hash, SHA256_DIGEST_LENGTH);
    if (res != SHA256_DIGEST_LENGTH) {
        return res;
    }

    return 0;
}

static int check_hash(cryptofs_file_t *file, int fd)
{
    uint8_t sha[SHA256_DIGEST_LENGTH];
    int res = calc_hash(file, fd, sha);
    vfs_lseek(fd, CRYPTOFS_HEAD_HASH_OFFSET, 0);
    if (res < 0) {
        return res;
    }
    return memcmp(file->hash, sha, SHA256_DIGEST_LENGTH);
}

static int encrpyt_and_write_block(vfs_file_t *filp, cryptofs_file_t *file, const uint8_t *block)
{
    if (cipher_encrypt(&file->cipher, block, file->block) < 1) {
        return -EIO;
    }
    file->state = STATE_ENCRYPTED;
    vfs_lseek(file->real_fd, filp->pos + CRYPTOFS_HEAD_SIZE - (filp->pos % AES_BLOCK_SIZE), SEEK_SET);
    int res = vfs_write(file->real_fd, file->block, sizeof(file->block));
    if (res < 0) {
        return res;
    }
    file->state = STATE_INVALID;
    return 0;
}

static int read_and_decrypt_block(vfs_file_t *filp, cryptofs_file_t *file, uint8_t *block)
{
    vfs_lseek(file->real_fd, filp->pos + CRYPTOFS_HEAD_SIZE - (filp->pos % AES_BLOCK_SIZE), SEEK_SET);
    int res = vfs_read(file->real_fd, file->block, sizeof(file->block));
    if (res < 0) {
        return res;
    }
    file->state = STATE_ENCRYPTED;
    if (cipher_decrypt(&file->cipher, file->block, block) < 1) {
        return -EIO;
    }
    return 0;
}

static int sync(vfs_file_t *filp, cryptofs_file_t *file)
{
    off_t rem = filp->pos % AES_BLOCK_SIZE;
    int pad = AES_BLOCK_SIZE - rem;
    if (rem) {
        memset(file->block + rem, pad, pad);
        if (encrpyt_and_write_block(filp, file, file->block) < 0) {
            return -EIO;
        }
        file->nb_pad = pad;
    }
    else {
        file->nb_pad = 0;
    }

    return 0;
}

static int _open(vfs_file_t *filp, const char *name, int flags, mode_t mode, const char *abs_path)
{
    (void)abs_path;
    cryptofs_t *fs = filp->mp->private_data;
    cryptofs_file_t *file = (cryptofs_file_t *)filp->private_data.buffer;

    memset(file, 0, sizeof(*file));

    real_name(fs, name, file);
    if (cipher_init(&file->cipher, CIPHER_AES_128, fs->key,
                    sizeof(fs->key)) != CIPHER_INIT_SUCCESS) {
        return -EINVAL;
    }

    int fd = vfs_open(file->name, flags, mode);
    /* Does not exist and cannot be created */
    if (fd < 0) {
        return fd;
    }
    /* Opened with O_CREAT -> new file */
    else if ((flags & O_CREAT) == O_CREAT) {
        return create_file(file, fd);
    }
    else {
        off_t size = vfs_lseek(fd, 0, SEEK_END);
        if (load_head(file, fd) < 0) {
            return -EBADF;
        }
        file->size = size - file->nb_pad - CRYPTOFS_HEAD_SIZE;
        if (!check_hash(file, fd)) {
            file->real_fd = fd;
            return 0;
        }
        else {
            vfs_close(fd);
            return -EBADF;
        }
    }
}

static int _close(vfs_file_t *filp)
{
    cryptofs_file_t *file = (cryptofs_file_t *)filp->private_data.buffer;

    if (file->state == STATE_UNENCRYPTED) {
        sync(filp, file);
    }
    if (calc_hash(file, file->real_fd, file->hash) == 0) {
        vfs_lseek(file->real_fd, 4, 0);
        vfs_write(file->real_fd, &file->nb_pad, 1);
        vfs_lseek(file->real_fd, CRYPTOFS_HEAD_HASH_OFFSET, 0);
        vfs_write(file->real_fd, file->hash, SHA256_DIGEST_LENGTH);
    }
    return vfs_close(file->real_fd);
}

static ssize_t _read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    cryptofs_file_t *file = (cryptofs_file_t *)filp->private_data.buffer;
    uint8_t *dest_ = dest;

    if (filp->pos + nbytes > file->size) {
        nbytes = file->size - filp->pos;
    }
    ssize_t read = 0;
    off_t rem = filp->pos % AES_BLOCK_SIZE;
    if (rem) {
        if (file->state == STATE_UNENCRYPTED) {
            read = MIN((size_t)(AES_BLOCK_SIZE - rem), nbytes);
            memcpy(dest_, file->block + rem, read);
            nbytes -= read;
            filp->pos += read;
            dest_ += read;
        }
        else {
            filp->pos -= rem;
        }
    }
    size_t nb_block = nbytes / AES_BLOCK_SIZE;
    if (nbytes % AES_BLOCK_SIZE) {
        nb_block++;
    }
    while (nb_block--) {
        if (nbytes > AES_BLOCK_SIZE) {
            if (read_and_decrypt_block(filp, file, dest_) < 0) {
                return -EIO;
            }
            nbytes -= AES_BLOCK_SIZE;
            filp->pos += AES_BLOCK_SIZE;
            read += AES_BLOCK_SIZE;
            dest_ += AES_BLOCK_SIZE;
        }
        else {
            if (read_and_decrypt_block(filp, file, file->block) < 0) {
                return -EIO;
            }
            file->state = STATE_UNENCRYPTED;
            memcpy(dest_, file->block, nbytes);
            filp->pos += nbytes;
            read += nbytes;
            return read;
        }
    }

    return read;
}

static ssize_t _write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    cryptofs_file_t *file = (cryptofs_file_t *)filp->private_data.buffer;
    const uint8_t *src_ = src;

    int res;
    ssize_t written = 0;
    off_t rem = filp->pos % AES_BLOCK_SIZE;
    if (rem) {
        if (file->state != STATE_UNENCRYPTED) {
            return -EIO;
        }
        memcpy(file->block + rem, src_, AES_BLOCK_SIZE - rem);
        res = encrpyt_and_write_block(filp, file, file->block);
        if (res < 0) {
            return res;
        }
        nbytes -= rem;
        filp->pos += rem;
        written += rem;
        src_ += rem;
    }
    size_t nb_block = nbytes / AES_BLOCK_SIZE;
    if (nbytes % AES_BLOCK_SIZE) {
        nb_block++;
    }
    while (nb_block--) {
        if (nbytes >= AES_BLOCK_SIZE) {
            res = encrpyt_and_write_block(filp, file, src_);
            if (res < 0) {
                return res;
            }
            nbytes -= AES_BLOCK_SIZE;
            src_ += AES_BLOCK_SIZE;
            written += AES_BLOCK_SIZE;
            filp->pos += nbytes;
        }
        else {
            if ((size_t)filp->pos < file->size) {
                if (read_and_decrypt_block(filp, file, file->block) < 0) {
                    return -EIO;
                }
            }
            file->state = STATE_UNENCRYPTED;
            memcpy(file->block, src_, nbytes);
            filp->pos += nbytes;
            written += nbytes;
            if ((size_t)filp->pos > file->size) {
                file->size = filp->pos;
            }
            return written;
        }
    }

    return written;
}

static off_t _lseek(vfs_file_t *filp, off_t off, int whence)
{
    cryptofs_file_t *file = (cryptofs_file_t *)filp->private_data.buffer;

    if (file->state == STATE_UNENCRYPTED) {
        if ((size_t)filp->pos + AES_BLOCK_SIZE < file->size) {
            encrpyt_and_write_block(filp, file, file->block);
        }
        else {
            sync(filp, file);
        }
    }

    switch (whence) {
    case SEEK_SET:
        off += CRYPTOFS_HEAD_SIZE;
        break;
    case SEEK_END:
        off -= file->nb_pad;
        break;
    }

    file->state = STATE_INVALID;
    filp->pos = vfs_lseek(file->real_fd, off, whence) - CRYPTOFS_HEAD_SIZE;
    return filp->pos;
}

static const vfs_file_system_ops_t fs_ops = {
    .mount = _mount,
    .umount = _umount,
};

static const vfs_file_ops_t file_ops = {
    .open = _open,
    .close = _close,
    .read = _read,
    .write = _write,
    .lseek = _lseek,
};

const vfs_file_system_t cryptofs_file_system = {
    .fs_op = &fs_ops,
    .f_op = &file_ops,
};
