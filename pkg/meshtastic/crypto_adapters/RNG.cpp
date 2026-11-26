#include "kernel_defines.h"
#include "random.h"


#if IS_USED(MODULE_PERIPH_HWRNG)
#include "periph/hwrng.h"
#endif

#include "RNG.h"

RNGClass CryptRNG;

RNGClass::RNGClass()
{
}

RNGClass::~RNGClass()
{
}

void RNGClass::begin(const char *tag)
{
    (void)tag;
    hwrng_init();
}

void RNGClass::rand(uint8_t *dest, size_t sz)
{
#if IS_USED(MODULE_PERIPH_HWRNG)
    hwrng_read(dest, sz);
#else
    random_bytes(dest, sz);
#endif
}

bool RNGClass::available(size_t len) const
{
    (void)len;

    // assume the entrophy is always available
    return true;
}

void RNGClass::addNoiseSource(NoiseSource &source)
{
    (void)source;
}

void RNGClass::setAutoSaveTime(uint16_t minutes)
{
    (void)minutes;
}

void RNGClass::stir(const uint8_t *data, size_t len, unsigned int credit)
{
    (void)data;
    (void)len;
    (void)credit;
}

void RNGClass::save()
{
}

void RNGClass::loop()
{
}

void RNGClass::destroy()
{
    // No software RNG state to erase; RIOT exposes no deinit here.
}

void RNGClass::rekey()
{
}

void RNGClass::mixTRNG()
{
}
