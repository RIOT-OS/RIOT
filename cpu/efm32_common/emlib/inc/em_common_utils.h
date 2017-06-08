#ifndef __SILICON_LABS_EM_COMMON_UTILS_H__
#define __SILICON_LABS_EM_COMMON_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @brief
 *   Create a structure to combine two designated initializers. With this macro
 *   it is possible to create an intializer using default values, and override
 *   values at compile time.
 *
 * @note
 *   The resulting variable is called `name`. The actual result is referred by
 *   `name.conf`.
 ******************************************************************************/
#define EFM32_CREATE_INIT(name, type, defaults, ...) \
struct {                                             \
  type conf;                                         \
} name = {                                           \
  .conf = defaults,                                  \
  __VA_ARGS__                                        \
};

#ifdef __cplusplus
}
#endif

#endif /* __SILICON_LABS_EM_COMMON_UTILS_H__ */
