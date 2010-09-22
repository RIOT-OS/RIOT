#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>

#include "ringbuffer.h"

//#define DEBUG(...) printf (__VA_ARGS__)
#define DEBUG(...)

void ringbuffer_init(ringbuffer *rb, char* buffer, unsigned int bufsize) {
    rb->buf = buffer;
    rb->start = 0;
    rb->end = 0;
    rb->size = bufsize;
    rb->avail = 0;
}

void rb_add_element(ringbuffer* rb, char c) {
    rb->buf[rb->end++] = c;
    rb->avail++;

    if (rb->end >= rb->size) rb->end = 0;
    if (rb->end == rb->start) rb->start++;
    if (rb->start >= rb->size) rb->start = 0;
}

int rb_get_element(ringbuffer *rb) {
    if (rb->avail == 0) return -1;
    
    rb->avail--;

    int c = (char)rb->buf[rb->start++];
    
    if (rb->start >= rb->size) rb->start = 0;
    return c;
}

int rb_get_elements(ringbuffer *rb, char* buf, int n) {
    int count = 0;
    while (rb->avail > 0) {
        rb->avail--;
        *buf++ = rb->buf[rb->start++];
        count++;
        if (rb->start >= rb->size) rb->start = 0;
    }
    return count;
}

/*int main(int argc, char *argv[] ){
    ringbuffer r;
    char buffer[100];
    ringbuffer_init(&r, buffer, sizeof(buffer));

    rb_add_element(&r, 'a');
    rb_add_element(&r, 'b');
    rb_add_element(&r, 'c');
    rb_add_element(&r, 'd');
    rb_add_element(&r, 'f');
    rb_add_element(&r, 'g');
    rb_add_element(&r, 'h');
    rb_add_element(&r, 'i');
    rb_add_element(&r, 'j');
    rb_add_element(&r, 'k');
    
    int c;
    while ( 1 ) {
        c = rb_get_element(&r);
        if (c == -1) break;
        printf("c=%i\n", (int)c);
        rb_add_element(&r, c);
    }

    return 0;
}*/
