
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "storage/flash_sim.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define FLASH_SIM_FILENAME "flash_sim.dat"

bool _valid_struct_params(flash_sim *fs) {
    return fs->page_size > 0 &&
           fs->block_size > 0 &&
           fs->storage_size > 0 &&
           (fs->block_size % fs->page_size) == 0 &&
           (fs->storage_size & fs->block_size) == 0;
}

int flash_sim_init(flash_sim *fs) {
    if(!_valid_struct_params(fs)) {
        return -EINVAL;
    }

    fs->pages = fs->storage_size / fs->page_size;
    struct stat buffer;
    if(stat(FLASH_SIM_FILENAME, &buffer) == 0) {
        fs->_fp = fopen(FLASH_SIM_FILENAME, "r+");
    } else {
        fs->_fp = fopen(FLASH_SIM_FILENAME, "w+");
    }

    if(fs->_fp == NULL) {
        DEBUG("failed opening file %s\n", FLASH_SIM_FILENAME);
        return -EBADFD;
    }

    // Make sure that the file has exactly the right size
    fseek(fs->_fp, 0, SEEK_END);
    long size = ftell(fs->_fp);

    if(size == (long) fs->storage_size) {
        return 0;
    } else {
        return flash_sim_format(fs);
    }
}

int flash_sim_format(flash_sim *fs) {
    if(fs->_fp == NULL) {
        DEBUG("struct was not initialized\n");
        return -ENODEV;
    }

    int ret = fclose(fs->_fp);
    if(ret != 0) {
        DEBUG("could not close file\n");
        return -EBADFD;
    }

    fs->_fp = fopen(FLASH_SIM_FILENAME, "w+");
    if(fs->_fp == NULL) {
        DEBUG("failed opening file %s\n", FLASH_SIM_FILENAME);
        return -EBADFD;
    }

    unsigned char *buffer = malloc(fs->storage_size);
    memset(buffer, 0xFF, fs->storage_size);
    fwrite(buffer, fs->storage_size, 1, fs->_fp);

    return 0;
}

int flash_sim_read(const flash_sim *fs, void *buffer, uint32_t page) {
    DEBUG("page = %u\n", page);

    if(fs->_fp == NULL) {
        DEBUG("struct was not initialized\n");
        return -ENODEV;
    }

    int ret = fseek(fs->_fp, page * fs->page_size, SEEK_SET);
    if(ret != 0) {
        DEBUG("seek failed: out of range\n");
        return -EFAULT;
    }

    ret = fread(buffer, fs->page_size, 1, fs->_fp);
    if(ret != 1) {
        DEBUG("read failed: out of range\n");
        return -EFAULT;
    }

    return 0;
}

int flash_sim_read_partial(const flash_sim *fs, void *buffer, uint32_t page, uint32_t offset, uint32_t length) {
    unsigned char *page_buffer = malloc(fs->page_size);

    int ret = flash_sim_read(fs, page_buffer, page);
    if(ret != 0) {
        free(page_buffer);
        return ret;
    }

    memcpy(buffer, page_buffer + offset, length);
    free(page_buffer);
    return 0;
}

int flash_sim_write_partial(const flash_sim *fs, const void *buffer, uint32_t page, uint32_t offset, uint32_t length) {
    unsigned char *page_buffer = malloc(fs->page_size);
    int ret = flash_sim_read(fs, page_buffer, page);
    if(ret != 0) {
        DEBUG("read failed page = %u\n", page);
        free(page_buffer);
        return ret;
    }

    memcpy(page_buffer+offset, buffer, length);
    ret = flash_sim_write(fs, page_buffer, page);
    if(ret != 0) {
        DEBUG("write failed page = %u\n", page);
        free(page_buffer);
        return ret;
    }

    return 0;
}

int flash_sim_write(const flash_sim *fs, const void *buffer, uint32_t page) {
    DEBUG("page = %u\n", page);

    if(fs->_fp == NULL) {
        DEBUG("struct was not initialized\n");
        return -ENODEV;
    }

    bool valid_write = true;
    unsigned char *page_buffer = malloc(fs->page_size);
    int ret = flash_sim_read(fs, page_buffer, page);
    if(ret != 0) {
        DEBUG("read failed page = %u\n", page);
        free(page_buffer);
        return ret;
    }
    // Make sure that its impossible to set bits back to 1 without erasing
    for(unsigned int i=0; i<fs->page_size; i++) {
        if((((unsigned char*) buffer)[i] | page_buffer[i]) > page_buffer[i]) {
            valid_write = false;
            break;
        }
    }
    free(page_buffer);

    if(!valid_write) {
        DEBUG("write failed - would have set bits back to 1\n");
        return -EIO;
    }

    ret = fseek(fs->_fp, page * fs->page_size, SEEK_SET);
    if(ret != 0) {
        DEBUG("seek failed: out of range\n");
        return -EFAULT;
    }

#if ENABLE_DEBUG
    long pos = ftell(fs->_fp);
    DEBUG("writing to offset %ld\n", pos);
#endif

    ret = fwrite(buffer, fs->page_size, 1, fs->_fp);
    if(ret != 1) {
        DEBUG("write failed\n");
        return -EBADFD;
    }

    fflush(fs->_fp);
    return 0;
}

int flash_sim_erase(const flash_sim *fs, uint32_t block) {
    DEBUG("block = %u\n", block);

    if(fs->_fp == NULL) {
        DEBUG("struct was not initialized\n");
        return -ENODEV;
    }

    unsigned char *buffer = malloc(fs->block_size);
    memset(buffer, 0xff, fs->block_size);

    int ret = fseek(fs->_fp, block * fs->block_size, SEEK_SET);
    if(ret != 0) {
        free(buffer);
        DEBUG("seek failed: out of range\n");
        return -EFAULT;
    }

    ret = fwrite(buffer, fs->block_size, 1, fs->_fp);
    if(ret != 1) {
        free(buffer);
        DEBUG("write failed\n");
        return -EBADFD;
    }

    free(buffer);
    return 0;
}



/* ==========================================
 * Functions for a flash_sim based FTL device
 * ========================================== */

int flash_sim_ftl_write(flash_sim *fs,
                  const char *buffer,
                  uint32_t page,
                  uint32_t offset,
                  uint16_t length) {

    int ret = flash_sim_write_partial(fs, buffer, page, offset, length);
    if(ret != 0) {
        return -EIO;
    }
    return 0;
}

int flash_sim_ftl_read(flash_sim *fs,
                 char *buffer,
                 uint32_t page,
                 uint32_t offset,
                 uint16_t length) {

    int ret = flash_sim_read_partial(fs, buffer, page, offset, length);
    if(ret != 0) {
        return -EIO;
    }
    return 0;
}

int flash_sim_ftl_erase(flash_sim *fs, uint32_t block) {
    int ret = flash_sim_erase(fs, block);
    if(ret != 0) {
        return -EIO;
    }
    return 0;
}
