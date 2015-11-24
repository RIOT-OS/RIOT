/**
 * This example connects to a tcp server. It is assumed to work with RIOT using a tap device
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>

#define REQUESTED_DATA 4031

char *dst = "fc00::1";
char *port = "2000"; 

/* Request for data */
const char request_str[] = "Send me x bytes\n";
typedef struct {
    char msg[sizeof(request_str)];
    uint16_t nByte;
}request_t;


/* Setup Socket, connects to server */
int setupSocket(void){
    int err = 0;
    int sock = 0;
    struct addrinfo hints, *pRes, *ptr;

    /* Fill hints struct */ 
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if( (err = getaddrinfo(dst, port, &hints, &pRes)) != 0){
        return err;
    }

    /* Create Socket */ 
    for(ptr = pRes; ptr != NULL ; ptr = ptr->ai_next){
        if(ptr == NULL){
    	    return err;
        }
        if( (sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) == -1 ){
            continue;
        }
        if((err = connect(sock, ptr->ai_addr, ptr->ai_addrlen)) == 0){
            break;
        }
    }
    freeaddrinfo(pRes);
    return sock;
}


/* RIOT internet checksum function, used for verification */
uint16_t inet_csum_slice(uint16_t sum, const char *buf, uint16_t len, size_t accum_len)
{
    uint32_t csum = sum;

    if (len == 0)
        return csum;

    if (accum_len & 1) {      
        csum += *buf;         
        buf++;
        len--;
        accum_len++;
    }

    for (int i = 0; i < (len >> 1); buf += 2, i++) {
        csum += (*buf << 8) + *(buf + 1);
    }

    if ((accum_len + len) & 1)
        csum += (*buf << 8);

    while (csum >> 16) {
        uint16_t carry = csum >> 16;
        csum = (csum & 0xffff) + carry;
    }
    return csum;
}


int main(int argc, char **argv){
    int  sock = setupSocket();
    int  rcv = 0;
    char buf[REQUESTED_DATA];
    request_t req;

    if(sock <= 0){
        printf("Failed to setup socket\n");
        return -1;
    }

    strncpy(req.msg, request_str, sizeof(request_str));
    req.nByte = REQUESTED_DATA;

    /* Send request */
    while((rcv += write(sock, &req + rcv, sizeof(req))) < sizeof(req)){};
    printf("Request sent to server, requesting %d byte\n", req.nByte);

    /* Read reply */
    rcv = 0;
    while((rcv += read(sock, buf + rcv, sizeof(buf))) < sizeof(buf)){};

    /* Data received */
    printf("Byte read from the Server: %d\n", rcv);
    printf("Checksum for received data is: %d\n", inet_csum_slice(0, buf, rcv, 0));

    /* Close connection */
    close(sock);

    return 0;
}
