/* a native printf is provided by the builtin libc of wamr */

#ifdef __cplusplus
extern "C" int printf( const char * ,...);
#define WASM_EXPORT __attribute__((visibility("default"))) extern "C"
#else
extern int printf( const char * ,...);
#define WASM_EXPORT __attribute__((visibility("default")))
#endif

WASM_EXPORT int main(int argc, char **argv)
{
    printf("Hello RIOT %i\n" , 2001);
    return 2468;
}
