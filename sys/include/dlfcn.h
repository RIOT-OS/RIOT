#ifndef	_DLFCN_H
#define	_DLFCN_H

#include <stdio.h>

#define RTLD_NOW 0x002

void *dlopen (const char *file, int mode) {
	printf("NOT IMPLEMENTED: trying to open %s in mode %x\n", file, mode);
	return 0;
}

/* Unmap and close a shared object opened by `dlopen'.
   The handle cannot be used again after calling `dlclose'.  */
int dlclose (void *__handle) {
	return 0;
}


#endif	/* dlfcn.h */
