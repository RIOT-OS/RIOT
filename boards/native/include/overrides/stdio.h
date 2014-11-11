#ifndef RIOT__NATIVE__STDIO_H__
#define RIOT__NATIVE__STDIO_H__

#include "sys/types.h"
#include "limits.h"
#include "stddef.h"

#include <stdarg.h>

#ifdef __cplusplus
#   define STDIO_RESTRICT __restrict__
#else
#   define STDIO_RESTRICT restrict
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define EOF (-1)

struct FILE;
typedef struct FILE FILE;

struct fpos;
typedef struct fpos fpos_t;

extern FILE *stdin, *stdout, *stderr;

void     clearerr(FILE *);
char    *ctermid(char *);
int      fclose(FILE *);
FILE    *fdopen(int, const char *);
int      feof(FILE *);
int      ferror(FILE *);
int      fflush(FILE *);
int      fgetc(FILE *);
int      fgetpos(FILE *STDIO_RESTRICT, fpos_t *STDIO_RESTRICT);
char    *fgets(char *STDIO_RESTRICT, int, FILE *STDIO_RESTRICT);
int      fileno(FILE *);
void     flockfile(FILE *);
FILE    *fopen(const char *STDIO_RESTRICT, const char *STDIO_RESTRICT);
int      fprintf(FILE *STDIO_RESTRICT, const char *STDIO_RESTRICT, ...);
int      fputc(int, FILE *);
int      fputs(const char *STDIO_RESTRICT, FILE *STDIO_RESTRICT);
size_t   fread(void *STDIO_RESTRICT, size_t, size_t, FILE *STDIO_RESTRICT);
FILE    *freopen(const char *STDIO_RESTRICT, const char *STDIO_RESTRICT, FILE *STDIO_RESTRICT);
int      fscanf(FILE *STDIO_RESTRICT, const char *STDIO_RESTRICT, ...);
int      fseek(FILE *, long, int);
int      fseeko(FILE *, off_t, int);
int      fsetpos(FILE *, const fpos_t *);
long     ftell(FILE *);
off_t    ftello(FILE *);
int      ftrylockfile(FILE *);
void     funlockfile(FILE *);
size_t   fwrite(const void *STDIO_RESTRICT, size_t, size_t, FILE *STDIO_RESTRICT);
int      getc(FILE *);
int      getchar(void);
int      getc_unlocked(FILE *);
int      getchar_unlocked(void);
char    *gets(char *);
int      pclose(FILE *);
void     perror(const char *);
FILE    *popen(const char *, const char *);
int      printf(const char *STDIO_RESTRICT, ...);
int      putc(int, FILE *);
int      putchar(int);
int      putc_unlocked(int, FILE *);
int      putchar_unlocked(int);
int      puts(const char *);
int      remove(const char *);
int      rename(const char *, const char *);
void     rewind(FILE *);
int      scanf(const char *STDIO_RESTRICT, ...);
void     setbuf(FILE *STDIO_RESTRICT, char *STDIO_RESTRICT);
int      setvbuf(FILE *STDIO_RESTRICT, char *STDIO_RESTRICT, int, size_t);
int      snprintf(char *STDIO_RESTRICT, size_t, const char *STDIO_RESTRICT, ...);
int      sprintf(char *STDIO_RESTRICT, const char *STDIO_RESTRICT, ...);
int      sscanf(const char *STDIO_RESTRICT, const char *STDIO_RESTRICT, ...);
char    *tempnam(const char *, const char *);
FILE    *tmpfile(void);
char    *tmpnam(char *);
int      ungetc(int, FILE *);
int      vfprintf(FILE *STDIO_RESTRICT, const char *STDIO_RESTRICT, va_list);
int      vfscanf(FILE *STDIO_RESTRICT, const char *STDIO_RESTRICT, va_list);
int      vprintf(const char *STDIO_RESTRICT, va_list);
int      vscanf(const char *STDIO_RESTRICT, va_list);
int      vsnprintf(char *STDIO_RESTRICT, size_t, const char *STDIO_RESTRICT, va_list);
int      vsprintf(char *STDIO_RESTRICT, const char *STDIO_RESTRICT, va_list);
int      vsscanf(const char *STDIO_RESTRICT, const char *STDIO_RESTRICT, va_list);

#ifdef __cplusplus
}
#endif

#endif /* ifndef RIOT__NATIVE__STDIO_H__ */
