
#include "mtd.h"

#include <errno.h>

int mtd_init(mtd_dev_t *mtd)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->init) {
        return mtd->driver->init(mtd);
    }
    else {
        return -ENOTSUP;
    }
}

int mtd_read(mtd_dev_t *mtd, void *dest, uint32_t addr, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->read) {
        return mtd->driver->read(mtd, dest, addr, count);
    }
    else {
        return -ENOTSUP;
    }
}

int mtd_write(mtd_dev_t *mtd, const void *src, uint32_t addr, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->write) {
        return mtd->driver->write(mtd, src, addr, count);
    }
    else {
        return -ENOTSUP;
    }
}

int mtd_erase(mtd_dev_t *mtd, uint32_t addr, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->erase) {
        return mtd->driver->erase(mtd, addr, count);
    }
    else {
        return -ENOTSUP;
    }
}

int mtd_ioctl(mtd_dev_t *mtd, unsigned char ctrl, void *buf)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->ioctl) {
        return mtd->driver->ioctl(mtd, ctrl, buf);
    }
    else {
        return -ENOTSUP;
    }
}
