

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
#include <stdio.h>
static char gsm_rx_buf_mem[256];
static ringbuffer_t gsm_rx_buf;
static char ctrl_z[] = {0x1a, '\0'};
void rx_callback(void *arg, char data)
{
	ringbuffer_add_one(&gsm_rx_buf, data);
}
void sms_init(void)
{
	ringbuffer_init(&gsm_rx_buf, gsm_rx_buf_mem, 128);
	uart_init(UART_1, 115200, rx_callback, 0, 0);
	vtimer_usleep(100000);

    // do{
    // 	send_command(ACTIVATE, strlen(ACTIVATE));
    //     vtimer_usleep(5000);
    // } while (wait_for_string(OK) == -1);

	// send_command_string(CIPSHUT);
	// send_command_string("AT+CGACT=0,1\r\n");
	// send_command_string(CGATT);
	// send_command_string(CGDCONT);
	// send_command(CSTT, strlen(CSTT));
	// wait_for_string("OK");
	// send_command_string(CGACT);
	// wait_for_string("OK");
	// send_command(CIICR, strlen(CIICR));
	// wait_for_string("OK");
	// send_command(CIFSR, strlen(CIFSR));
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
	// send_command_string(CIPSTART);
	// wait_for_string("CONNECT OK");
	// send_command_string("AT+CIPSEND\r\n");
	// send_command_string("GET ");
	// send_command_string(URL);
	// send_command_string(" HTTP:/1.1\r\n");
	// //send_command_string("Host: beehivemonitor.azurewebsites.net\r\n");
	// //send_command_string("Connection: keep-alive\r\n");
	// send_command(ctrl_z,2);
	// //send_command_string("AT+CIPCLOSE\r\n");
  	send_command(SETCONTYPE, strlen(SETCONTYPE));
	wait_for_string(OK);
    send_command(SETAPN, strlen(SETAPN));
	 wait_for_string(OK);
    send_command(SETUSER, strlen(SETUSER));
	wait_for_string(OK);
    send_command(SETPWD, strlen(SETPWD));
	wait_for_string(OK);
	send_command(ACTIVATE, strlen(ACTIVATE));
	wait_for_string(OK);

	send_command_string("AT+HTTPINIT\r\n");
	send_command_string("AT+HTTPPARA=\"CID\",1\r\n");
	send_command_string("AT+HTTPPARA=\"URL\",\"");
	send_command_string(URL);
	send_command_string("\"");
	send_command_string("\r\n");
	send_command_string("AT+HTTPACTION=0\r\n");
	send_command_string("AT+HTTPTERM\r\n");


}
void set_message_format(void)
{
	char message[] = "AT+CMGF=1\r\n";
	send_command_string(message);
}
void sms_send(int argc, char **argv)
{
  	char end[2];
  	end[0]=0x1a;
  	end[1]='\0';
  	set_message_format();
	char start[]   = "AT+CMGS=\"30622610\"\r";
	send_command_string(start);
	send_command_string(argv[1]);
	send_command(end, 2);
	//send_command("\r\n", 2);
	vtimer_usleep(1000);
	print_buffer();

}
//void init_tcp_connection(ip address, )
void post_http(int argc, char **argv)
{
	send_command_string("AT+CDNSGIP=");
	send_command_string("\"");
	send_command_string(argv[1]);
	send_command_string("\"");
	send_command_string("\r\n");
	// send_command_string(CREG);
	// send_command_string(SAPBR);
	// send_command_string(WAIT);
	// send_command_string(HTTPINIT);
	// send_command_string(URL);
	// send_command_string(CID);
	// send_command_string(HTTPACTION);
	// send_command_string(HTTPREAD);
	// send_command_string(WAIT);
	// send_command_string(TERMINATE);
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
/* DOES NOT USE TIMEOUT YET */
void sendATAndWait(char* command, char* waitString, int timeout)
{
	send_command(command, strlen(command));
	wait_for_string(waitString);
}
int wait_for_string(char* string, int timeout)
{
	int length = strlen(string);
	int i;
	char value;
	int timeout_counter = 0;
	while(1)
	{
		if(ringbuffer_empty(&gsm_rx_buf))
		{
			timeout_counter++;
			vtimer_usleep(10000); //Change?
			if(timeout_counter > timeout)
			{
				DEBUG("NOT FOUND\r\n");
				return -1;
			}
			continue;
		}
		value = ringbuffer_get_one(&gsm_rx_buf);
		DEBUG("%c", value);
		if(value != string[i])
		{
			i = 0;
		}
		else
		{
			i++;
		}
		if(i >= length)
		{
			return 1;
		}
	}
	return -1;
}
int wait_for_ok(void)
{
	while(!ringbuffer_empty(&gsm_rx_buf))
	{
		vtimer_usleep(1000);
		char value = ringbuffer_get_one(&gsm_rx_buf);
		DEBUG("%c", value);
		if(value == 'O')
		{
			if(ringbuffer_get_one(&gsm_rx_buf) == 'K')
			{
				return 1;
			}
		}
	}
	return -1;
}