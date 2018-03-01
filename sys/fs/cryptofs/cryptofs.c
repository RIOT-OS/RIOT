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

static int real_name(cryptofs_t *fs, const char *name, char *crypto_name, size_t len)
{
    memset(crypto_name, 0, len);

    if (fs->real_fs->mount_point_len > 1) {
        return snprintf(crypto_name, len - 1, "%s/%s.sec",
                        fs->real_fs->mount_point, name);
    }
    else {
        return snprintf(crypto_name, len - 1, "%s%s.sec",
                        fs->real_fs->mount_point, name);
    }
}

static int _mount(vfs_mount_t *mountp)
{
    cryptofs_t *fs = mountp->private_data;

    BUILD_BUG_ON(VFS_FILE_BUFFER_SIZE < sizeof(cryptofs_file_t));

    mutex_init(&fs->lock);

    if (cipher_init(&fs->cipher, CIPHER_AES_128, fs->key,
                    sizeof(fs->key)) != CIPHER_INIT_SUCCESS) {
        return -EINVAL;
    }

    return vfs_mount(fs->real_fs);
}

static int _umount(vfs_mount_t *mountp)
{
    cryptofs_t *fs = mountp->private_data;

    return vfs_umount(fs->real_fs);
}

static int _unlink(vfs_mount_t *mountp, const char *name)
{
    cryptofs_t *fs = mountp->private_data;

    char name_[VFS_NAME_MAX + 1];
    real_name(fs, name, name_, sizeof(name_));

    return vfs_unlink(name_);
}

static int store_head(cryptofs_t *fs, cryptofs_file_t *file, int fd)
{
    uint8_t buf[AES_BLOCK_SIZE];

    int res = vfs_lseek(fd, 0, 0);
    if (res < 0) {
        return res;
    }

    /* Write Magic */
    be_uint32_t magic = byteorder_htonl(CRYPTOFS_MAGIC_WORD);
    memset(buf, AES_BLOCK_SIZE - sizeof(magic) - 1, sizeof(buf));
    memcpy(buf, &magic, sizeof(magic));
    buf[CRYPTOFS_NBPAD_OFFSET] = file->nb_pad;
    mutex_lock(&fs->lock);
    if (cipher_encrypt(&fs->cipher, buf, buf) < 1) {
        mutex_unlock(&fs->lock);
        return -EIO;
    }
    mutex_unlock(&fs->lock);
    if (vfs_write(fd, buf, sizeof(buf)) != sizeof(buf)) {
        vfs_close(fd);
        vfs_unlink(file->name);
        return -EIO;
    }

    /* Store Hash */
    memset(buf, 0, sizeof(buf));
    mutex_lock(&fs->lock);
    if (cipher_encrypt(&fs->cipher, file->hash, buf) < 1) {
        mutex_unlock(&fs->lock);
        return -EIO;
    }
    mutex_unlock(&fs->lock);
    if (vfs_write(fd, buf, sizeof(buf)) != sizeof(buf)) {
        vfs_close(fd);
        vfs_unlink(file->name);
        return -EIO;
    }

    return 0;
}

static int calc_hash(int fd, uint8_t *sha)
{
    int res = vfs_lseek(fd, CRYPTOFS_HEAD_SIZE, 0);
    uint8_t buf[SHA256_INTERNAL_BLOCK_SIZE];
    sha256_context_t hash_ctx;
    sha256_init(&hash_ctx);
    while ((res = vfs_read(fd, buf, sizeof(buf))) > 0) {
        sha256_update(&hash_ctx, buf, res);
    }
    sha256_final(&hash_ctx, sha);

    return 0;
}

static int load_head(cryptofs_t *fs, cryptofs_file_t *file, int fd)
{
    int res = vfs_lseek(fd, 0, 0);
    if (res < 0) {
        return res;
    }
    uint8_t buf[2 * AES_BLOCK_SIZE];
    be_uint32_t magic;
    res = vfs_read(fd, buf, AES_BLOCK_SIZE);
    if (res < 0) {
        return res;
    }
    mutex_lock(&fs->lock);
    if (cipher_decrypt(&fs->cipher, buf, buf) < 1) {
        mutex_unlock(&fs->lock);
        return -EIO;
    }
    mutex_unlock(&fs->lock);
    memcpy(&magic, buf, sizeof(magic));
    if (byteorder_ntohl(magic) != CRYPTOFS_MAGIC_WORD) {
        return -EBADF;
    }
    file->nb_pad = buf[CRYPTOFS_NBPAD_OFFSET];

    res = vfs_read(fd, buf, sizeof(buf));
    if (res != sizeof(buf)) {
        return res;
    }
    mutex_lock(&fs->lock);
    if (cipher_decrypt(&fs->cipher, buf, buf) < 1 ||
            cipher_decrypt(&fs->cipher, buf + AES_BLOCK_SIZE, buf + AES_BLOCK_SIZE) < 1) {
        mutex_unlock(&fs->lock);
        return -EIO;
    }
    mutex_unlock(&fs->lock);

    memcpy(file->hash, buf, sizeof(buf));

    return 0;
}

static int check_hash(cryptofs_file_t *file, int fd)
{
    uint8_t sha[SHA256_DIGEST_LENGTH];
    int res = calc_hash(fd, sha);
    vfs_lseek(fd, CRYPTOFS_HEAD_HASH_OFFSET, 0);
    if (res < 0) {
        return res;
    }
    return memcmp(file->hash, sha, SHA256_DIGEST_LENGTH);
}

static int encrpyt_and_write_block(cryptofs_t *fs, vfs_file_t *filp, cryptofs_file_t *file, const uint8_t *block)
{
    mutex_lock(&fs->lock);
    if (cipher_encrypt(&fs->cipher, block, file->block) < 1) {
        mutex_unlock(&fs->lock);
        return -EIO;
    }
    mutex_unlock(&fs->lock);
    file->state = STATE_ENCRYPTED;
    vfs_lseek(file->real_fd, filp->pos + CRYPTOFS_HEAD_SIZE - (filp->pos % AES_BLOCK_SIZE), SEEK_SET);
    int res = vfs_write(file->real_fd, file->block, sizeof(file->block));
    if (res < 0) {
        return res;
    }
    file->state = STATE_INVALID;
    return 0;
}

static int read_and_decrypt_block(cryptofs_t *fs, vfs_file_t *filp, cryptofs_file_t *file, uint8_t *block)
{
    vfs_lseek(file->real_fd, filp->pos + CRYPTOFS_HEAD_SIZE - (filp->pos % AES_BLOCK_SIZE), SEEK_SET);
    int res = vfs_read(file->real_fd, file->block, sizeof(file->block));
    if (res < 0) {
        return res;
    }
    file->state = STATE_ENCRYPTED;
    mutex_lock(&fs->lock);
    if (cipher_decrypt(&fs->cipher, file->block, block) < 1) {
        mutex_unlock(&fs->lock);
        return -EIO;
    }
    mutex_unlock(&fs->lock);
    file->state = STATE_UNENCRYPTED;
    return 0;
}

static int sync(cryptofs_t *fs, vfs_file_t *filp, cryptofs_file_t *file)
{
    off_t rem = filp->pos % AES_BLOCK_SIZE;
    int pad = AES_BLOCK_SIZE - rem;
    if (rem) {
        memset(file->block + rem, pad, pad);
        if (encrpyt_and_write_block(fs, filp, file, file->block) < 0) {
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

    real_name(fs, name, file->name, sizeof(file->name));

    int fd = vfs_open(file->name, flags, mode);
    /* Does not exist and cannot be created */
    if (fd < 0) {
        return fd;
    }
    /* Opened with O_CREAT -> new file */
    else if ((flags & O_CREAT) == O_CREAT) {
        int res = store_head(fs, file, fd);
        if (res < 0) {
            return res;
        }
    }
    else {
        off_t size = vfs_lseek(fd, 0, SEEK_END);
        if (load_head(fs, file, fd) < 0) {
            return -EBADF;
        }
        file->size = size - file->nb_pad - CRYPTOFS_HEAD_SIZE;
        if (fs->hash && check_hash(file, fd) != 0) {
            vfs_close(fd);
            return -EBADF;
        }
    }

    if (flags & O_APPEND) {
        filp->pos = file->size;
    }

    file->real_fd = fd;
    return 0;
}

static int _close(vfs_file_t *filp)
{
    cryptofs_t *fs = filp->mp->private_data;
    cryptofs_file_t *file = (cryptofs_file_t *)filp->private_data.buffer;

    if ((filp->flags & O_ACCMODE) != O_RDONLY) {
        if (file->state == STATE_UNENCRYPTED) {
            sync(fs, filp, file);
        }
        if (!fs->hash || calc_hash(file->real_fd, file->hash) == 0) {
            int res = store_head(fs, file, file->real_fd);
            if (res < 0) {
                return res;
            }
        }
    }
    return vfs_close(file->real_fd);
}

static ssize_t _read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    cryptofs_t *fs = filp->mp->private_data;
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
            if (read_and_decrypt_block(fs, filp, file, dest_) < 0) {
                return -EIO;
            }
            nbytes -= AES_BLOCK_SIZE;
            filp->pos += AES_BLOCK_SIZE;
            read += AES_BLOCK_SIZE;
            dest_ += AES_BLOCK_SIZE;
        }
        else {
            if (read_and_decrypt_block(fs, filp, file, file->block) < 0) {
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
    cryptofs_t *fs = filp->mp->private_data;
    cryptofs_file_t *file = (cryptofs_file_t *)filp->private_data.buffer;
    const uint8_t *src_ = src;

    int res;
    ssize_t written = 0;
    off_t rem = filp->pos % AES_BLOCK_SIZE;
    if (rem) {
        if (file->state != STATE_UNENCRYPTED) {
            return -EIO;
        }
        written = MIN((size_t)(AES_BLOCK_SIZE - rem), nbytes);
        memcpy(file->block + rem, src_, written);
        nbytes -= written;
        filp->pos += written;
        src_ += written;
        if (filp->pos % AES_BLOCK_SIZE == 0) {
            res = encrpyt_and_write_block(fs, filp, file, file->block);
            if (res < 0) {
                return res;
            }
        }
        if ((size_t)filp->pos > file->size) {
            file->size = filp->pos;
        }
    }
    size_t nb_block = nbytes / AES_BLOCK_SIZE;
    if (nbytes % AES_BLOCK_SIZE) {
        nb_block++;
    }
    while (nb_block--) {
        if (nbytes >= AES_BLOCK_SIZE) {
            res = encrpyt_and_write_block(fs, filp, file, src_);
            if (res < 0) {
                return res;
            }
            nbytes -= AES_BLOCK_SIZE;
            src_ += AES_BLOCK_SIZE;
            written += AES_BLOCK_SIZE;
            filp->pos += AES_BLOCK_SIZE;
        }
        else {
            if ((size_t)filp->pos < file->size) {
                if (read_and_decrypt_block(fs, filp, file, file->block) < 0) {
                    return -EIO;
                }
            }
            file->state = STATE_UNENCRYPTED;
            memcpy(file->block, src_, nbytes);
            filp->pos += nbytes;
            written += nbytes;
        }
        if ((size_t)filp->pos > file->size) {
            file->size = filp->pos;
        }
    }

    return written;
}

static off_t _lseek(vfs_file_t *filp, off_t off, int whence)
{
    cryptofs_t *fs = filp->mp->private_data;
    cryptofs_file_t *file = (cryptofs_file_t *)filp->private_data.buffer;

    if (file->state == STATE_UNENCRYPTED) {
        if ((size_t)filp->pos + AES_BLOCK_SIZE < file->size) {
            encrpyt_and_write_block(fs, filp, file, file->block);
        }
        else {
            sync(fs, filp, file);
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
    .unlink = _unlink,
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
