/*
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <httpsrv.h>

#include <urlfile.h>

#define BUFSIZE 128
#define PATHSIZE 128

typedef struct URLFile_Object {
    int length;
    char prefix[];
} URLFile_Object;

static const char * defPrefix = "./www/";

/*
 * Fills in the supplied string with a content type string that
 * matches the supplied filename.
 */
static const char * getContentType(const char * fileName)
{
   const char * ext = fileName;

   /*  ext will point at the ".xxx" part of filename */
   /*  If fileName does not have an extension it will point to terminating char */
   while (*ext && *ext != '.') {
       ext++;
   }

   if      (!strcmp(".au",    ext)) return (CONTENT_TYPE_AU);
   else if (!strcmp(".class", ext)) return (CONTENT_TYPE_APPLET);
   else if (!strcmp(".css",   ext)) return (CONTENT_TYPE_CSS);
   else if (!strcmp(".doc",   ext)) return (CONTENT_TYPE_DOC);
   else if (!strcmp(".gif",   ext)) return (CONTENT_TYPE_GIF);
   else if (!strcmp(".htm",   ext)) return (CONTENT_TYPE_HTML);
   else if (!strcmp(".html",  ext)) return (CONTENT_TYPE_HTML);
   else if (!strcmp(".jpg",   ext)) return (CONTENT_TYPE_JPG);
   else if (!strcmp(".mpg",   ext)) return (CONTENT_TYPE_MPEG);
   else if (!strcmp(".mpeg",  ext)) return (CONTENT_TYPE_MPEG);
   else if (!strcmp(".pdf",   ext)) return (CONTENT_TYPE_PDF);
   else if (!strcmp(".wav",   ext)) return (CONTENT_TYPE_WAV);
   else if (!strcmp(".zip",   ext)) return (CONTENT_TYPE_ZIP);
   else if (!strcmp(".txt",   ext)) return (CONTENT_TYPE_PLAIN);
   else return (CONTENT_TYPE_APPLET);
}

static long getFileSize(FILE * fp)
{
    long size;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    return (size);
}

URLHandler_Handle URLFile_create(void * params)
{
    URLFile_Handle urlfile;
    const char * prefix = params ? params : defPrefix;

    if ((urlfile = malloc(sizeof(URLFile_Object) + strlen(prefix) + 1))) {
        strcpy(urlfile->prefix, prefix);
    }

    return ((URLHandler_Handle)urlfile);
}

int URLFile_process(URLHandler_Handle h, int method, const char * url,
                    const char * urlArgs, int contentLength, Ssock_Handle ssock)
{
    URLFile_Handle u = (URLFile_Handle)h;
    FILE * fp;
    char buf[BUFSIZE];
    char fileName[PATHSIZE];
    int status = URLHandler_ENOTHANDLED;

    if (method == URLHandler_GET) {
        int numRead;

        strncpy(fileName, u->prefix, strlen(u->prefix) + 1);
        strncat(fileName, url, PATHSIZE - strlen(u->prefix));

        if ((fp = fopen(fileName, "rb")) != NULL) {

            HTTPSrv_sendResponse(ssock, HTTP_OK, getContentType(fileName),
                               getFileSize(fp), NULL);

            while ((numRead = fread(buf, sizeof(char), sizeof(buf), fp)) > 0) {
                Ssock_send(ssock, buf, numRead, 0);
            }
            fclose(fp);

            status = URLHandler_EHANDLED;
        }
    }

    return (status);
}

void URLFile_delete(URLHandler_Handle * h)
{
    URLFile_Handle * u = (URLFile_Handle *)h;

    free(*h);
    *u = NULL;
}
