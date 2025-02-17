# following line a hints for build options mostly untested
# some are not matching RIOTs application targets
# they default to off

# set (WAMR_BUILD_LIBC_WASI 1) #enable libc wasi support
# set (WAMR_BUILD_THREAD_MGR 1) #enable thread manager support
# set (WAMR_BUILD_APP_FRAMEWORK 1) #enable WAMR app framework support
# set (WAMR_BUILD_LIB_PTHREAD 1) #enable pthread support

# set (WAMR_BUILD_JIT 1) #enable WAMR JIT
# set (WAMR_BUILD_FAST_INTERP 1) #enable Fast interpreter
# set (WAMR_BUILD_MULTI_MODULE 1) #enable Multiple modules
# set (WAMR_BUILD_SPEC_TEST 1) #enable spec test compatible mode is on
# set (WAMR_BUILD_BULK_MEMORY 1) #enable Bulk memory feature
# set (WAMR_BUILD_SHARED_MEMORY 1) #enable Shared memory
# set (WAMR_BUILD_MINI_LOADER 1) #enable WASM mini loader
# set (WAMR_DISABLE_HW_BOUND_CHECK 1) #enable Hardware boundary check disabled
# set (WAMR_BUILD_MEMORY_PROFILING 1) #enable Memory profiling
# set (WAMR_APP_THREAD_STACK_SIZE_MAX ${WAMR_APP_THREAD_STACK_SIZE_MAX})  #set maximum thread stack size
# set (WAMR_BUILD_CUSTOM_NAME_SECTION 1) #enable Custom name section
# set (WAMR_BUILD_TAIL_CALL 1) #enable Tail call

# the builtin wamr libc is enabled by default
# set (WAMR_BUILD_LIBC_BUILTIN 0) #disable builtin libc
