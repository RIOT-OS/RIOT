/* Copyright (C) World
 * a native printf is provided by the builtin libc of wamr */

#ifdef __cplusplus
extern "C" int printf( const char *, ...);
#define WASM_EXPORT __attribute__((visibility("default"))) extern "C"
#else
extern int printf( const char *, ...);
#define WASM_EXPORT __attribute__((visibility("default")))
#endif

WASM_EXPORT int main(int argc, char **argv)
{
    if (argc > 1) {
        printf("Hello %s %i\n", argv[1], 2001);
    }
    else {
        printf("Hello %s %i\n", "RIOT", 2001);
    }
    return 2468;
}
