@defgroup pkg_driver_cryptocell_310 ARM CryptoCell 310 Driver
@ingroup  pkg drivers_crypto
@brief    Provides the driver for the ARM CryptoCell 310 hardware accelerator
@see      https://infocenter.nordicsemi.com/topic/sdk_nrf5_v17.1.0/group__cryptocell__api.html

@note    The source of this package is not a git repository, but a zip file downloaded
         from the Nordic Semiconductor software center. It is quite large and takes a
         while to download.

@warning The CryptoCell 310 peripheral on the nRF52840 MCU can only access data residing in RAM,
         not in ROM (see [nRF52840 Product Specification], Section 6.6.7).
         When using this driver as a backend for PSA Crypto API, API function will return
         `PSA_ERROR_DATA_INVALID` when provided input data resides in ROM.

[nRF52840 Product Specification]: https://docs-be.nordicsemi.com/bundle/ps_nrf52840/attach/nRF52840_PS_v1.11.pdf
