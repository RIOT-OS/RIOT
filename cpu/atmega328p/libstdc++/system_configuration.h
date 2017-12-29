/*
 * Configuration for Arduino's compiler. 
 *   >>> This is a guess <<<
 */
/*
 * Version Number
 */
#define __UCLIBCXX_MAJOR__ 0
#define __UCLIBCXX_MINOR__ 2
#define __UCLIBCXX_SUBLEVEL__ 4

/*
 * Target Features and Options
 */
#define __UCLIBCXX_HAS_FLOATS__
#undef __UCLIBCXX_HAS_TLS__
#define __WARNINGS__ "-Wall"
#define __BUILD_EXTRA_LIBRARIES__ ""
#define __HAVE_DOT_CONFIG__ 1

/*
 * String and I/O Stream Support
 */
#undef __UCLIBCXX_HAS_WCHAR__
#define __UCLIBCXX_IOSTREAM_BUFSIZE__ 32
#undef __UCLIBCXX_HAS_LFS__
#undef __UCLIBCXX_SUPPORT_CDIR__
#define __UCLIBCXX_SUPPORT_CIN__
#define __UCLIBCXX_SUPPORT_COUT__
#define __UCLIBCXX_SUPPORT_CERR__
/*
 * STL and Code Expansion
 */
//#define __UCLIBCXX_STL_BUFFER_SIZE__ 32
#define __UCLIBCXX_STL_BUFFER_SIZE__ 8
#undef __UCLIBCXX_CODE_EXPANSION__

/*
 * Library Installation Options
 */
#define __UCLIBCXX_RUNTIME_PREFIX__ "/usr/uClibc++"
#define __UCLIBCXX_RUNTIME_INCLUDE_SUBDIR__ "/include"
#define __UCLIBCXX_RUNTIME_LIB_SUBDIR__ "/lib"
#define __UCLIBCXX_RUNTIME_BIN_SUBDIR__ "/bin"
#undef __UCLIBCXX_EXCEPTION_SUPPORT__
#define __BUILD_STATIC_LIB__ 1
#define __BUILD_ONLY_STATIC_LIB__ 1
#undef __DODEBUG__
