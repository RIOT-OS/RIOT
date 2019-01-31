#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "registry/store/registry_store_file.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int registry_file_load(registry_store_t *store, load_cb_t cb,
                               void *cb_arg);
static int registry_file_save(registry_store_t *store, const char *name,
                               const char *value);
static int registry_file_line_find(registry_store_t *store, const char *name,
                                   off_t *pos);
static int registry_file_line_make(char *buf, int buf_len, const char *name,
                                   const char *value);
static int registry_file_line_get(int *file, char *buf, int buf_len,
                                  off_t *off);
static int registry_file_line_parse(char *buf, char **namep, char **valp);


static registry_store_itf_t interface = {
    .load = registry_file_load,
    .save = registry_file_save
};

int registry_file_src(registry_file_t *file)
{
    file->store.itf = &interface;
    registry_src_register(&file->store);
    return 0;
}

int registry_file_dst(registry_file_t *file)
{
    file->store.itf = &interface;
    registry_dst_register(&file->store);
    return 0;
}

static int registry_file_save(registry_store_t *store, const char *name,
                              const char *value)
{
    int len = 0;
    int file;
    int whence;
    off_t location;
    registry_file_t *file_store = (registry_file_t *)store;
    char buf[REGISTRY_MAX_NAME_LEN + REGISTRY_MAX_VAL_LEN + REGISTRY_EXTRA_LEN];

    len = registry_file_line_make(buf, sizeof(buf), name, value);
    if (len < 0 || (len + 2) > (int)sizeof(buf)) {
        return -EINVAL;
    }
    memset(buf + len + 1, 0, sizeof(buf) - (len + 1));
    buf[sizeof(buf)-1] = '\n';

    DEBUG("[registry_file_save] saving: %s\n", buf);

    if (registry_file_line_find(store, name, &location)) {
        /* Add new entry */
        DEBUG("[registry_file_save] entry not found. Creating...\n");
        location = 0;
        whence = SEEK_END;
    }
    else {
        /* Edit entry */
        DEBUG("[registry_file_save] entry found. Editing...\n");
        whence = SEEK_SET;
    }

    file = open(file_store->file_name, O_RDWR | O_APPEND);
    if (file < 0) {
        goto error;
    }

    lseek(file, location, whence);
    write(file, buf, sizeof(buf));
    close(file);
    return 0;

error:
    DEBUG("[registry_file_save] error while opening %s. Error: %d\n",
          file_store->file_name, file);
    return -EINVAL;
}

static int registry_file_line_find(registry_store_t *store, const char *name,
                                   off_t *pos)
{
    char buf[REGISTRY_MAX_NAME_LEN + REGISTRY_MAX_VAL_LEN + REGISTRY_EXTRA_LEN];
    char *name_str = NULL;
    char *val_str = NULL;
    int file;
    int res;
    off_t current_loc = 0;
    off_t prev_loc = 0;
    registry_file_t *file_store = (registry_file_t *)store;

    file = open(file_store->file_name, O_RDONLY);

    if (file < 0) {
        DEBUG("[registry_file_load] error while trying to read %s\n",
              file_store->file_name);
        return -EINVAL;
    }

    while(1) {
        prev_loc = current_loc;
        res = registry_file_line_get(&file, buf, sizeof(buf), &current_loc);
        if (current_loc == 0) {
            break;
        }

        res = registry_file_line_parse(buf, &name_str, &val_str);
        if (res != 0) {
            continue;
        }

        if (!strcmp(name, name_str)) {
            *pos = prev_loc;
            DEBUG("[registry_file_line_find] found %s in %ld. Closing file.\n",
                  name, prev_loc);
            close(file);
            return 0;
        }
    }

    DEBUG("[registry_file_line_find] Could not find %s. Closing file.\n", name);
    close(file);
    return -1;
}

static int registry_file_line_make(char *buf, int buf_len, const char *name,
                                   const char *value)
{
    int nlen = 0;
    int vlen = 0;
    int off;

    nlen = strlen(name);

    if (value) {
        vlen = strlen(value);
    }

    if (nlen + vlen + REGISTRY_EXTRA_LEN > buf_len) {
        return -1;
    }

    memcpy(buf, name, nlen);
    off = nlen;
    buf[off++] = '=';

    memcpy(buf + off, value, vlen);
    off += vlen;
    buf[off] = '\0';

    return off;
}

static int registry_file_line_get(int *file, char *buf, int buf_len, off_t *off)
{
    int res;
    char *end;

    /* Apply the passed offset */
    res = lseek(*file, *off, SEEK_SET);

    if (res < 0) {
        *off = 0;
        return -1;
    }

    /* Try to fill the buffer */
    res = read(*file, buf, buf_len);

    if (res <= 0) {
        *off = 0;
        return -1;
    }

    if (res == buf_len) {
        res--;
    }
    buf[res] = '\0';

    end = buf;
    while (*end != '\n' && (end-buf) < res) {
        end++;
    }

    /* Calculate the length of the line */
    buf_len = end - buf;
    *off += (buf_len + 1);

    return buf_len;
}

static int registry_file_line_parse(char *buf, char **namep, char **valp)
{
    char *cp;
    enum {
        FIND_NAME,
        FIND_NAME_END,
        FIND_VAL,
        FIND_VAL_END
        } state = FIND_NAME;

    *valp = NULL;

    for (cp = buf; *cp != '\0'; cp++) {
        switch (state) {
        case FIND_NAME:
            if (!isspace((unsigned char)*cp)) {
                *namep = cp;
                state = FIND_NAME_END;
            }
            break;
        case FIND_NAME_END:
            if (*cp == '=') {
                *cp = '\0';
                state = FIND_VAL;
            } else if (isspace((unsigned char)*cp)) {
                *cp = '\0';
            }
            break;
        case FIND_VAL:
            if (!isspace((unsigned char)*cp)) {
                *valp = cp;
                state = FIND_VAL_END;
            }
            break;
        case FIND_VAL_END:
            if (isspace((unsigned char)*cp)) {
                *cp = '\0';
            }
            break;
        }
    }

    if (state == FIND_VAL_END || state == FIND_VAL) {
        return 0;
    } else {
        return -1;
    }

}

static int registry_file_load(registry_store_t *store, load_cb_t cb,
                              void *cb_arg)
{
    char buf[REGISTRY_MAX_NAME_LEN + REGISTRY_MAX_VAL_LEN + REGISTRY_EXTRA_LEN];
    char *name_str = NULL;
    char *val_str = NULL;
    int file;
    int res;
    off_t current_loc = 0;
    registry_file_t *file_store = (registry_file_t *)store;

    file = open(file_store->file_name, O_RDONLY);

    if (file < 0) {
        DEBUG("[registry_file_load] error while trying to read %s\n",
              file_store->file_name);
        return -EINVAL;
    }

    while(1) {
        res = registry_file_line_get(&file, buf, sizeof(buf), &current_loc);
        if (current_loc == 0) {
            break;
        }

        res = registry_file_line_parse(buf, &name_str, &val_str);
        if (res != 0) {
            continue;
        }

        cb(name_str, val_str, cb_arg);
    }

    res = close(file);
    return 0;
}
