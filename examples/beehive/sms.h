#ifdef __cplusplus
extern "C" {
#endif
#ifndef SMS_H_
#define SMS_H_

#define CIPSHUT "AT+CIPSHUT\r\n"
#define CGDCONT "AT+CGDCONT=3,\"IPV6\"\r\n"
#define CSTT "AT+CSTT=\"internet\",\"\",\"\"\r\n"
#define CIICR "AT+CIICR\r\n"
#define CIFSR "AT+CIFSR\r\n"
#define CIPSTART "AT+CIPSTART=\"TCP\",\"128.199.36.229\",3333\r\n"

void rx_cb(void *arg, char data);
void sms_init(void);
void set_message_format(void);
void sms_send(int argc, char **argv);
void get_imei(int argc, char **argv);
void send_command(char* command, unsigned int count);
void send_command_string(char* command);

void print_buffer(void);
void send_to_uart1(int argc, char **argv);

void tcp_send(int argc, char** argv);
int wait_for_ok(void);


#ifdef __cplusplus
}
#endif
#endif