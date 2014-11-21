

#include "sms.h"
#define ENABLE_DEBUG    (1)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#endif
#include "debug.h"
#include "string.h"
#include "periph/uart.h"
#include "ringbuffer.h"
#include "vtimer.h"
static char gsm_rx_buf_mem[128];
static ringbuffer_t gsm_rx_buf;
void rx_callback(void *arg, char data)
{
	ringbuffer_add_one(&gsm_rx_buf, data);
}
void sms_init(void)
{
	ringbuffer_init(&gsm_rx_buf, gsm_rx_buf_mem, 128);
	uart_init(UART_1, 115200, rx_callback, 0, 0);
	send_command_string(CIPSHUT);
	send_command_string(CSTT);
	send_command_string(CIICR);
	send_command_string(CIFSR);
	//send_command_string(CGDCONT);
	//send_command_string(CIPSTART);

}
void send_to_uart1(int argc, char **argv)
{
	printf("%s\n", strlen(argv[1]));
	send_command(argv[1], strlen(argv[1]));
	send_command("\r\n", 2);
	vtimer_usleep(100000);

	print_buffer();
}
void tcp_send(int argc, char** argv)
{
	send_command_string("AT+CIPSEND=20\r\n");
}
void set_message_format(void)
{
	char message[] = "AT+CMGF=1\r";
	send_command(message, strlen(message));
	vtimer_usleep(1000);
	print_buffer();
}
void sms_send(int argc, char **argv)
{
  	char end[2];
  	end[0]=0x1a;
  	end[1]='\0';
  	set_message_format();
	char start[]   = "AT+CMGS=\"30622610\"\r";
	send_command(start, strlen(start));
	vtimer_usleep(1000);
	send_command(argv[1], strlen(argv[1]));
	send_command(end, 2);
	send_command("\r\n", 2);
	vtimer_usleep(1000);
	print_buffer();

}
//void init_tcp_connection(ip address, )
void post_http(int argc, char **argv)
{
	send_command_string(CREG);
	send_command_string(SAPBR);
	send_command_string(WAIT);
	send_command_string(HTTPINIT);
	send_command_string(URL);
	send_command_string(CID);
	send_command_string(HTTPACTION);
	send_command_string(HTTPREAD);
	send_command_string(WAIT);
	send_command_string(TERMINATE);
}
void get_imei(int argc, char **argv)
{
	char cmd[]  = "AT+GSN\r\n";
	send_command(cmd, strlen(cmd));
	vtimer_usleep(1000);
	print_buffer();
}
void send_command_string(char* command)
{
	send_command(command, strlen(command));
	vtimer_usleep(1000000);
	print_buffer();

}
void send_command(char* command, unsigned int count)
{
    for (int i = 0; i < count; i++) {
        uart_write_blocking(UART_1, command[i]);
    }
}
void print_buffer(void)
{
	while(!ringbuffer_empty(&gsm_rx_buf))
	{
		printf("%c", ringbuffer_get_one(&gsm_rx_buf));
	}
}
int wait_for_ok(void)
{
	while(!ringbuffer_empty(&gsm_rx_buf))
	{
		vtimer_usleep(1000);
		if(ringbuffer_get_one(&gsm_rx_buf) == 'O')
		{
			if(ringbuffer_get_one(&gsm_rx_buf) == 'K')
			{
				return 1;
			}
		}
	}
	return -1;
}