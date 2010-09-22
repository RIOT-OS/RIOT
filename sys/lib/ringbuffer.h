#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H 

typedef struct ringbuffer {
    char* buf;
    unsigned int        start;
    unsigned int        end;
    unsigned int        size;
    unsigned int        avail;
} ringbuffer;

void ringbuffer_init(ringbuffer *rb, char* buffer, unsigned int bufsize);
void rb_add_element(ringbuffer* rb, char c);
int rb_get_element(ringbuffer *rb);

#endif /* __RINGBUFFER_H */
