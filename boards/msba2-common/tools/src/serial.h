#ifndef SERIAL_H
#define SERIAL_H

extern char *baud_rate;

int open_serial_port(const char *port_name);
int write_serial_port(const void *buf, int num);
void input_flush_serial_port(void);
int read_serial_port_nb(unsigned char *buf, int bufsize);
int read_serial_port(unsigned char *buf, int bufsize);
void close_serial_port(void);
void send_break_signal(void);
int set_baud(const char *baud_name);
int serial_port_fd(void);
void set_rts(int val);
void set_dtr(int val);
void change_baud(const char *baud_name);

#endif // SERIAL_H
