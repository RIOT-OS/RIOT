#ifndef GDBSTUB_H
#define GDBSTUB_H

#ifdef __cplusplus
extern "C" {
#endif

void gdbstub_init(void);
void gdbstub_do_break(void);

#ifdef __cplusplus
}
#endif

#endif /* GDBSTUB_H */
