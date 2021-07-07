/**
 * @defgroup pkg_mynewt_core mynewt-core
 * @ingroup  pkg
 * @brief    Apache MyNewt package for MyNewt based packages: uwb-core, nimble
 * @see      https://github.com/apache/mynewt-core
 */

# Apache MyNewt mynewt-core Package

Packages like @ref pkg_uwb_core and @ref nimble where developed with MyNewt
as their default OS. Some of the features provided by that OS are not abstracted.
For those cases and to avoid header re-definitions mynewt-core is pulled in.
