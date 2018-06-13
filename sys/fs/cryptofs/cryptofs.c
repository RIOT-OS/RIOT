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
#ifdef MODULE_PERIPH_HWCRYPTO
#include "periph/hwcrypto.h"
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define MIN(a, b)   ((a) < (b) ? (a) : (b))

enum {
    STATE_INVALID,
    STATE_ENCRYPTED,
    STATE_UNENCRYPTED,
};

static int _init_cipher(cryptofs_t *fs)
{
#ifdef MODULE_PERIPH_HWCRYPTO
    (void)fs;
    return 0;
#else
    mutex_init(&fs->lock);

    if (cipher_init(&fs->cipher, CIPHER_AES_128, fs->key,
                    sizeof(fs->key)) != CIPHER_INIT_SUCCESS) {
        return -EINVAL;
    }

    return 0;
#endif
}

static int _decrypt(cryptofs_t *fs, const void *in, void *out, size_t nbytes)
{
    assert(nbytes % AES_BLOCK_SIZE == 0);
#ifdef MODULE_PERIPH_HWCRYPTO
    hwcrypto_acquire(HWCRYPTO_DEV(0));
    hwcrypto_cipher_init(HWCRYPTO_DEV(0), HWCRYPTO_AES128, HWCRYPTO_MODE_ECB);
    hwcrypto_cipher_set(HWCRYPTO_DEV(0), HWCRYPTO_OPT_KEY, fs->key, sizeof(fs->key));
    hwcrypto_cipher_decrypt(HWCRYPTO_DEV(0), in, out, nbytes);
    hwcrypto_release(HWCRYPTO_DEV(0));
    return 1;
#else
    mutex_lock(&fs->lock);
    int ret = 0;
    const uint8_t *in_ = in;
    uint8_t *out_ = out;
    for (size_t i = 0; i < nbytes; i += AES_BLOCK_SIZE) {
        ret = cipher_decrypt(&fs->cipher, in_, out_);
        if (ret < 1) {
            break;
        }
        in_ += AES_BLOCK_SIZE;
        out_ += AES_BLOCK_SIZE;
    }
    mutex_unlock(&fs->lock);

    return ret;
#endif
}

static int _encrypt(cryptofs_t *fs, const void *in, void *out, size_t nbytes)
{
    assert(nbytes % AES_BLOCK_SIZE == 0);
#ifdef MODULE_PERIPH_HWCRYPTO
    hwcrypto_acquire(HWCRYPTO_DEV(0));
    hwcrypto_cipher_init(HWCRYPTO_DEV(0), HWCRYPTO_AES128, HWCRYPTO_MODE_ECB);
    hwcrypto_cipher_set(HWCRYPTO_DEV(0), HWCRYPTO_OPT_KEY, fs->key, sizeof(fs->key));
    hwcrypto_cipher_encrypt(HWCRYPTO_DEV(0), in, out, nbytes);
    hwcrypto_release(HWCRYPTO_DEV(0));
    return 1;
#else
    mutex_lock(&fs->lock);
    int ret = 0;
    const uint8_t *in_ = in;
    uint8_t *out_ = out;
    for (size_t i = 0; i < nbytes; i += AES_BLOCK_SIZE) {
        ret = cipher_encrypt(&fs->cipher, in_, out_);
        if (ret < 1) {
            break;
        }
        in_ += AES_BLOCK_SIZE;
        out_ += AES_BLOCK_SIZE;
    }
    mutex_unlock(&fs->lock);

    return ret;
#endif
}

static int real_dir_name(cryptofs_t *fs, const char *name, char *real_name, size_t len)
{
    memset(real_name, 0, len);

    return snprintf(real_name, len - 1, "%s%s", fs->real_fs->mount_point, name);
}

static int real_name(cryptofs_t *fs, const char *name, char *real_name, size_t len)
{
    memset(real_name, 0, len);

    return snprintf(real_name, len - 1, "%s%s.sec", fs->real_fs->mount_point, name);
}

static int _format(vfs_mount_t *mountp)
{
    cryptofs_t *fs = mountp->private_data;

    char name[VFS_NAME_MAX + 1];
    name[VFS_NAME_MAX] = '\0';
    snprintf(name, VFS_NAME_MAX, "%s/%s", fs->real_fs->mount_point, CRYPTOFS_ROOT_FILENAME);

    int fd = vfs_open(name, O_RDWR | O_CREAT, 0);
    if (fd < 0) {
        return -EINVAL;
    }

    if (_init_cipher(fs) < 0) {
        return -EINVAL;
    }

    be_uint32_t magic = byteorder_htonl(CRYPTOFS_MAGIC_WORD);
    uint8_t buf[AES_BLOCK_SIZE];
    for (unsigned i = 0; i < AES_BLOCK_SIZE / sizeof(magic); i += sizeof(magic)) {
        memcpy(buf + i, &magic, sizeof(magic));
    }
    if (_encrypt(fs, buf, buf, AES_BLOCK_SIZE) < 1) {
        return -EIO;
    }

    int res = vfs_write(fd, buf, sizeof(buf));
    vfs_close(fd);

    if (res < (int)sizeof(buf)) {
        return -EINVAL;
    }

    return 0;
}

static int _mount(vfs_mount_t *mountp)
{
    cryptofs_t *fs = mountp->private_data;

    BUILD_BUG_ON(VFS_FILE_BUFFER_SIZE < sizeof(cryptofs_file_t));

    if (_init_cipher(fs) < 0) {
        return -EINVAL;
    }

    char name[VFS_NAME_MAX + 1];
    name[VFS_NAME_MAX] = '\0';
    snprintf(name, VFS_NAME_MAX, "%s/%s", fs->real_fs->mount_point, CRYPTOFS_ROOT_FILENAME);

    int fd = vfs_open(name, O_RDWR, 0);
    if (fd < 0) {
        return -EINVAL;
    }

    uint8_t buf[AES_BLOCK_SIZE];
    int res = vfs_read(fd, buf, sizeof(buf));
    if (res < (int)sizeof(buf)) {
        vfs_close(fd);
        return -EINVAL;
    }
    if (_decrypt(fs, buf, buf, AES_BLOCK_SIZE) < 1) {
        return -EIO;
    }
    be_uint32_t magic;
    for (unsigned i = 0; i < AES_BLOCK_SIZE / sizeof(magic); i += sizeof(magic)) {
        memcpy(&magic, buf + i, sizeof(magic));
        if (byteorder_ntohl(magic) != CRYPTOFS_MAGIC_WORD) {
            vfs_close(fd);
            return -EINVAL;
        }
    }
    fs->root_fd = fd;
    return 0;
}

static int _umount(vfs_mount_t *mountp)
{
    cryptofs_t *fs = mountp->private_data;

    return vfs_close(fs->root_fd);
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
    uint8_t buf[SHA256_DIGEST_LENGTH];

    int res = vfs_lseek(fd, 0, 0);
    if (res < 0) {
        return res;
    }

    /* Write Magic */
    be_uint32_t magic = byteorder_htonl(CRYPTOFS_MAGIC_WORD);
    memset(buf, AES_BLOCK_SIZE - sizeof(magic) - 1, AES_BLOCK_SIZE);
    memcpy(buf, &magic, sizeof(magic));
    buf[CRYPTOFS_NBPAD_OFFSET] = file->nb_pad;
    if (_encrypt(fs, buf, buf, AES_BLOCK_SIZE) < 1) {
        return -EIO;
    }
    if (vfs_write(fd, buf, AES_BLOCK_SIZE) != AES_BLOCK_SIZE) {
        vfs_close(fd);
        vfs_unlink(file->name);
        return -EIO;
    }

    /* Store Hash */
    memset(buf, 0, sizeof(buf));
    if (_encrypt(fs, file->hash, buf, SHA256_DIGEST_LENGTH) < 1) {
        return -EIO;
    }
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
    uint8_t buf[SHA256_DIGEST_LENGTH];
    be_uint32_t magic;
    res = vfs_read(fd, buf, AES_BLOCK_SIZE);
    if (res < 0) {
        return res;
    }
    if (_decrypt(fs, buf, buf, AES_BLOCK_SIZE) < 1) {
        return -EIO;
    }
    memcpy(&magic, buf, sizeof(magic));
    if (byteorder_ntohl(magic) != CRYPTOFS_MAGIC_WORD) {
        return -EBADF;
    }
    file->nb_pad = buf[CRYPTOFS_NBPAD_OFFSET];

    res = vfs_read(fd, buf, sizeof(buf));
    if (res != sizeof(buf)) {
        return res;
    }
    if (_decrypt(fs, buf, file->hash, SHA256_DIGEST_LENGTH) < 1) {
        return -EIO;
    }

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

static int encrpyt_and_write_block(cryptofs_t *fs, vfs_file_t *filp, cryptofs_file_t *file, const uint8_t *block, size_t nbytes, bool seek)
{
    DEBUG("cryptofs: write_block (pos=%ld)\n", filp->pos);
    if (_encrypt(fs, block, file->block, nbytes) < 1) {
        return -EIO;
    }
    file->state = STATE_ENCRYPTED;
    if (seek) {
        vfs_lseek(file->real_fd, filp->pos + CRYPTOFS_HEAD_SIZE - (filp->pos % AES_BLOCK_SIZE), SEEK_SET);
    }
    int res = vfs_write(file->real_fd, file->block, nbytes);
    if (res < 0) {
        return res;
    }
    file->state = STATE_INVALID;
    return 0;
}

static int read_and_decrypt_block(cryptofs_t *fs, vfs_file_t *filp, cryptofs_file_t *file, uint8_t *block, size_t nbytes, bool seek)
{
    DEBUG("cryptofs: read_block (pos=%ld)\n", filp->pos);
    if (seek) {
        vfs_lseek(file->real_fd, filp->pos + CRYPTOFS_HEAD_SIZE - (filp->pos % AES_BLOCK_SIZE), SEEK_SET);
    }
    int res = vfs_read(file->real_fd, file->block, nbytes);
    if (res < 0) {
        return res;
    }
    file->state = STATE_ENCRYPTED;
    if (_decrypt(fs, file->block, block, nbytes) < 1) {
        return -EIO;
    }
    file->state = STATE_UNENCRYPTED;
    return 0;
}

static int sync(cryptofs_t *fs, vfs_file_t *filp, cryptofs_file_t *file)
{
    DEBUG("cryptofs: sync\n");

    off_t rem = filp->pos % AES_BLOCK_SIZE;
    int pad = AES_BLOCK_SIZE - rem;
    if (rem) {
        memset(file->block + rem, pad, pad);
        if (encrpyt_and_write_block(fs, filp, file, file->block, AES_BLOCK_SIZE, true) < 0) {
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
    DEBUG("cryptofs: opening %s (%s)\n", name, file->name);

    int fd = vfs_open(file->name, flags, mode);
    /* Does not exist and cannot be created */
    if (fd < 0) {
        DEBUG("cryptofs: unable to open %s (%d)\n", name, fd);
        return fd;
    }
    /* Opened with O_CREAT -> new file */
    else if ((flags & O_CREAT) == O_CREAT) {
        int res = store_head(fs, file, fd);
        if (res < 0) {
            DEBUG("cryptofs: open: error when creating %s (%d)\n", name, res);
            vfs_close(fd);
            return res;
        }
    }
    else {
        off_t size = vfs_lseek(fd, 0, SEEK_END);
        if (load_head(fs, file, fd) < 0) {
            DEBUG("cryptofs: open: error reading header %s\n", name);
            vfs_close(fd);
            return -EBADF;
        }
        file->size = size - file->nb_pad - CRYPTOFS_HEAD_SIZE;
        if (fs->hash && check_hash(file, fd) != 0) {
            DEBUG("cryptofs: open: error wrong hash %s\n", name);
            vfs_close(fd);
            return -EBADF;
        }
    }

    if (flags & O_APPEND) {
        filp->pos = file->size;
        off_t rem = filp->pos % AES_BLOCK_SIZE;
        if (rem) {
            DEBUG("cryptofs: open: loading last block\n");
            /* Load last block and seek back to the begining of the block */
            read_and_decrypt_block(fs, filp, file, file->block, AES_BLOCK_SIZE, true);
            vfs_lseek(file->real_fd, filp->pos + CRYPTOFS_HEAD_SIZE - (filp->pos % AES_BLOCK_SIZE), SEEK_SET);
        }
    }

    file->real_fd = fd;
    return 0;
}

static int _close(vfs_file_t *filp)
{
    cryptofs_t *fs = filp->mp->private_data;
    cryptofs_file_t *file = (cryptofs_file_t *)filp->private_data.buffer;
    DEBUG("cryptofs: closing %s\n", file->name);

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

    DEBUG("cryptofs: reading %u bytes\n", (unsigned)nbytes);

    if (filp->pos + nbytes > file->size) {
        nbytes = file->size - filp->pos;
    }
    ssize_t read = 0;
    off_t rem = filp->pos % AES_BLOCK_SIZE;
    if (rem) {
        DEBUG("cryptofs: reading from buffer\n");
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
    while (nb_block) {
        if (nbytes >= AES_BLOCK_SIZE) {
            size_t to_dec = nbytes > sizeof(file->block) ? sizeof(file->block) : nbytes - nbytes % AES_BLOCK_SIZE;
            if (read_and_decrypt_block(fs, filp, file, dest_, to_dec, false) < 0) {
                DEBUG("cryptofs: error when reading\n");
                return -EIO;
            }
            nbytes -= to_dec;
            filp->pos += to_dec;
            read += to_dec;
            dest_ += to_dec;
            nb_block -= to_dec / AES_BLOCK_SIZE;
        }
        else {
            if (read_and_decrypt_block(fs, filp, file, file->block, AES_BLOCK_SIZE, false) < 0) {
                DEBUG("cryptofs: error when reading\n");
                return -EIO;
            }
            /* Seek back to the beginning of the block */
            vfs_lseek(file->real_fd, filp->pos + CRYPTOFS_HEAD_SIZE - (filp->pos % AES_BLOCK_SIZE), SEEK_SET);
            file->state = STATE_UNENCRYPTED;
            memcpy(dest_, file->block, nbytes);
            filp->pos += nbytes;
            read += nbytes;
            nb_block--;
        }
    }

    DEBUG("cryptofs: read %u bytes\n", (unsigned)read);

    return read;
}

static ssize_t _write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    cryptofs_t *fs = filp->mp->private_data;
    cryptofs_file_t *file = (cryptofs_file_t *)filp->private_data.buffer;
    const uint8_t *src_ = src;

    DEBUG("cryptofs: writing %u bytes\n", (unsigned)nbytes);

    int res;
    ssize_t written = 0;
    off_t rem = filp->pos % AES_BLOCK_SIZE;
    if (rem) {
        DEBUG("cryptofs: write: writing in last block\n");
        if (file->state != STATE_UNENCRYPTED) {
            DEBUG("cryptofs: write: fatal, wrong state\n");
            return -EIO;
        }
        written = MIN((size_t)(AES_BLOCK_SIZE - rem), nbytes);
        memcpy(file->block + rem, src_, written);
        nbytes -= written;
        filp->pos += written;
        src_ += written;
        if (filp->pos % AES_BLOCK_SIZE == 0) {
            res = encrpyt_and_write_block(fs, filp, file, file->block, AES_BLOCK_SIZE, false);
            if (res < 0) {
                DEBUG("cryptofs: error when writing\n");
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
    while (nb_block) {
        if (nbytes >= AES_BLOCK_SIZE) {
            size_t to_enc = nbytes > sizeof(file->block) ? sizeof(file->block) : nbytes - nbytes % AES_BLOCK_SIZE;
            res = encrpyt_and_write_block(fs, filp, file, src_, to_enc, false);
            if (res < 0) {
                DEBUG("cryptofs: error when writing\n");
                return res;
            }
            nbytes -= to_enc;
            src_ += to_enc;
            written += to_enc;
            filp->pos += to_enc;
            nb_block -= to_enc / AES_BLOCK_SIZE;
        }
        else {
            if ((size_t)filp->pos < file->size) {
                if (read_and_decrypt_block(fs, filp, file, file->block, AES_BLOCK_SIZE, false) < 0) {
                    DEBUG("cryptofs: error when writing\n");
                    return -EIO;
                }
                /* Seek back to the beginning of the block */
                vfs_lseek(file->real_fd, filp->pos + CRYPTOFS_HEAD_SIZE - (filp->pos % AES_BLOCK_SIZE), SEEK_SET);
            }
            file->state = STATE_UNENCRYPTED;
            memcpy(file->block, src_, nbytes);
            filp->pos += nbytes;
            written += nbytes;
            nb_block--;
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
            encrpyt_and_write_block(fs, filp, file, file->block, AES_BLOCK_SIZE, false);
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
    read_and_decrypt_block(fs, filp, file, file->block, AES_BLOCK_SIZE, false);
    /* Seek back to the beginning of the block */
    vfs_lseek(file->real_fd, filp->pos + CRYPTOFS_HEAD_SIZE - (filp->pos % AES_BLOCK_SIZE), SEEK_SET);
    return filp->pos;
}

static int _opendir(vfs_DIR *dirp, const char *dirname, const char *abs_path)
{
    (void)abs_path;
    cryptofs_t *fs = dirp->mp->private_data;
    char name[VFS_NAME_MAX + 1];

    real_dir_name(fs, dirname, name, sizeof(name));

    mutex_lock(&fs->lock);
    unsigned i;
    for (i = 0; i < sizeof(fs->dir) / sizeof(fs->dir[0]); i++) {
        if (!fs->dir_used[i]) {
            dirp->private_data.ptr = &fs->dir[i];
            fs->dir_used[i] = 1;
            break;
        }
    }
    mutex_unlock(&fs->lock);

    if (i == sizeof(fs->dir) / sizeof(fs->dir[0])) {
        return -ENOMEM;
    }

    return vfs_opendir(dirp->private_data.ptr, name);
}

static int is_crypto_file(vfs_dirent_t *entry)
{
    size_t len = strlen(entry->d_name);

    return (len > 3) && ((entry->d_name[len - 4] == '.') &&
            (entry->d_name[len - 3] == 's') && (entry->d_name[len - 2] == 'e') &&
            (entry->d_name[len - 1] == 'c'));
}

static int _readdir(vfs_DIR *dirp, vfs_dirent_t *entry)
{
    int res;
    do {
        res = vfs_readdir(dirp->private_data.ptr, entry);
    } while (res != 0 && !is_crypto_file(entry));

    if (res == 1) {
        size_t len = strlen(entry->d_name);
        entry->d_name[len - 4] = '\0'; /* truncate '.sec' */
    }

    return res;
}

static int _closedir(vfs_DIR *dirp)
{
    cryptofs_t *fs = dirp->mp->private_data;

    int res = vfs_closedir(dirp->private_data.ptr);

    mutex_lock(&fs->lock);
    fs->dir_used[(vfs_DIR *)dirp->private_data.ptr - fs->dir] = 0;
    mutex_unlock(&fs->lock);

    return res;
}

static const vfs_file_system_ops_t fs_ops = {
    .format = _format,
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

static const vfs_dir_ops_t dir_ops = {
    .readdir = _readdir,
    .opendir = _opendir,
    .closedir = _closedir,
};

const vfs_file_system_t cryptofs_file_system = {
    .fs_op = &fs_ops,
    .f_op = &file_ops,
    .d_op = &dir_ops,
};
