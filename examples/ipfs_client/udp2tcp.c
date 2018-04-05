#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#define BUFFER_SIZE (2048)
#define WORKERS (10)
#define NB_ARGS (5)

struct worker_t {
 pthread_t tpid;
 pthread_mutex_t lock;
 pthread_cond_t go;
 char buffer[BUFFER_SIZE];
 size_t bufferlen;
 int sock;
 struct sockaddr_in6 clientaddr;
};
struct worker_t workers[WORKERS];

static const uint8_t connect_ip[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
static const uint16_t connect_port = 5001;

int tcp_send(int sock, char *buffer, size_t buffer_len) {
 size_t sent;
 ssize_t sent_;
 sent = 0;
 do {
  if (0 > (sent_ = send(sock, buffer+sent, buffer_len-sent, 0))) {
   puts("sending error");
   return -1;
  }
  sent += sent_;
 } while (sent < buffer_len);
 return 0;
}


void* work(void *data) {
 struct worker_t *w = (struct worker_t*) data;
 ssize_t n;
 int clientlen;
 clientlen = sizeof(w->clientaddr);
 while(1) {
  pthread_mutex_lock(&(w->lock));
  pthread_cond_wait(&(w->go), &(w->lock));


  /* parse args */
  char *arg[NB_ARGS+1];
  char *current_pos, *tmp;
  int argc = 1;
  arg[0] = w->buffer;
  current_pos = w->buffer;
  if(NULL != (tmp = strstr(current_pos, "\n")) ) {
   *tmp = '\0';
  }

  for(int i=0 ; i < NB_ARGS ; i++) {
   if(NULL == (tmp = strstr(current_pos, " ")) ) {
    arg[i+1] = NULL;
    current_pos = data+strlen(data);
   } else {
    *tmp = '\0';
    arg[i+1] = tmp+1;
    current_pos = tmp+1;
    argc += 1;
   }
  }

  /* exec command */
  int sock;
  char buffer[BUFFER_SIZE];
  size_t buffer_len;
  struct sockaddr_in6 clientaddr;
  if (0 >= (sock = socket(AF_INET6, SOCK_STREAM, 0))) {
   puts("unable to create socket");
   pthread_mutex_unlock(&(w->lock));
   continue;
  }

  clientaddr.sin6_family = AF_INET6;
  memcpy(clientaddr.sin6_addr.s6_addr, connect_ip, sizeof(connect_ip));
  clientaddr.sin6_port = htons(connect_port);

  if (0 > connect(sock, (struct sockaddr *) &clientaddr, sizeof(clientaddr))) {
   puts("unable to connect");
   pthread_mutex_unlock(&(w->lock));
   continue;
  }

  /* send request */
  if (argc < 2) {
   puts("not enough arguments");
   goto error;
  }

  buffer_len = snprintf(buffer, BUFFER_SIZE, "%s /%s?args=%s HTTP/1.0\r\n", (0==strcmp(arg[0], "get"))?"GET":"PUT", (0==strcmp(arg[0], "get"))?"get":"put", arg[1] );
  if ( 0 > tcp_send(sock, buffer, buffer_len) ) { puts("unable to send"); pthread_mutex_unlock(&(w->lock)); continue; }

  if (argc >= 3) {
   buffer_len = snprintf(buffer, BUFFER_SIZE, "Content-Type: multipart/form-data; boundary=xxx\r\nContent-Length: %d\r\n\r\n", strlen(arg[2])+7+59+42+11+1 );
   if ( 0 > tcp_send(sock, buffer, buffer_len) ) { puts("unable to send"); pthread_mutex_unlock(&(w->lock)); continue; }
   buffer_len = snprintf(buffer, BUFFER_SIZE, "--xxx\r\n" ); // 7
   if ( 0 > tcp_send(sock, buffer, buffer_len) ) { puts("unable to send"); pthread_mutex_unlock(&(w->lock)); continue; }
   buffer_len = snprintf(buffer, BUFFER_SIZE, "Content-Disposition: form-data; name=\"file\"; filename=\"f\"\r\n" ); // 59
   if ( 0 > tcp_send(sock, buffer, buffer_len) ) { puts("unable to send"); pthread_mutex_unlock(&(w->lock)); continue; }
   buffer_len = snprintf(buffer, BUFFER_SIZE, "Content-Type: application/octet-stream\r\n\r\n" ); // 42
   if ( 0 > tcp_send(sock, buffer, buffer_len) ) { puts("unable to send"); pthread_mutex_unlock(&(w->lock)); continue; }
   buffer_len = snprintf(buffer, BUFFER_SIZE, "%s\n", arg[2] ); // 1
   if ( 0 > tcp_send(sock, buffer, buffer_len) ) { puts("unable to send"); pthread_mutex_unlock(&(w->lock)); continue; }
   buffer_len = snprintf(buffer, BUFFER_SIZE, "\r\n--xxx--\r\n" ); // 11
   if ( 0 > tcp_send(sock, buffer, buffer_len) ) { puts("unable to send"); pthread_mutex_unlock(&(w->lock)); continue; }
  } else {
   buffer_len = snprintf(buffer, BUFFER_SIZE, "\r\n" );
   if ( 0 > tcp_send(sock, buffer, buffer_len) ) { puts("unable to send"); pthread_mutex_unlock(&(w->lock)); continue; }
  }

  /* read answer */
  ssize_t r;
  int http_status;
  size_t response_size, read_;
  char *end_header;
  response_size = http_status = 0;

  do {
   if (0 > (r = read(sock, buffer, BUFFER_SIZE)) ) { puts("unable to read"); goto error; }
   buffer[r] = '\0';

   char *ss, *s, tmp;
   if (http_status == 0 && NULL != strstr(buffer, "HTTP/1.1")) {
    if (NULL != (ss = strstr(buffer+9, " "))) {
     tmp = *ss;
     *ss = '\0';
     http_status = atoi(buffer+9);
     *ss = tmp;
    }
   }

   if (response_size == 0 && NULL != (s = strstr(buffer, "Content-Length:")) ) {
    s += 16;
    if (NULL != (ss = strstr(s, "\r\n"))) {
     tmp = *ss;
     *ss = '\0';
     response_size = atoi(s);
     *ss = tmp;
    }
   }
   printf("%s\n", buffer);
  } while(NULL == (end_header = strstr(buffer, "\r\n\r\n")));


  for ( read_ = r-(end_header-buffer); read_ < response_size; read_+= r) {
   if (0 > (r = read(sock, buffer, BUFFER_SIZE)) ) { puts("unable to read"); goto error; }
   buffer[r] = '\0';
   printf("%s\n", buffer);
  }

  puts("ok2");
  buffer_len = snprintf(buffer, BUFFER_SIZE, "OK\n" );
  n = sendto(w->sock, buffer, buffer_len, 0, (struct sockaddr *) &(w->clientaddr), clientlen);
  printf("%d\n", n);
  close(sock);
  pthread_mutex_unlock(&(w->lock));
  continue;
error:
  buffer_len = snprintf(buffer, BUFFER_SIZE, "KO\n" );
  n = sendto(w->sock, buffer, buffer_len, 0, (struct sockaddr *) &(w->clientaddr), clientlen);
  close(sock);
  pthread_mutex_unlock(&(w->lock));
 }
 return NULL;
}

void daemonize() {
 pid_t pid;
 if ( -1 == (pid = fork()) ) {
  fprintf(stderr, "cannot daemonize\n");
  return;
 }
 if ( 0 != pid ) {
  exit(0);
 }
 if ( -1 == setsid() ) {
  fprintf(stderr, "cannot daemonize\n");
  return;
 }
 if ( -1 == chdir("/") ) {
  fprintf(stderr, "cannot daemonize\n");
  return;
 }
 umask(0777);
}


int main(int argc, char **argv) {
 int sock, listen_port, optval, clientlen, i;
 char buffer[BUFFER_SIZE];
 struct sockaddr_in6 serveraddr;
 struct sockaddr_in6 clientaddr;
 ssize_t n;
 if (2 > argc) {
  puts("usage: udp2tcp <port>");
  return 1;
 }
 listen_port = atoi(argv[1]);

 /* daemonize */
 daemonize();

 if (0 >= (sock = socket(PF_INET6, SOCK_DGRAM, 0))) {
  puts("unable to create socket");
  return 1;
 }
 optval = 1;
 setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
 bzero((char *) &serveraddr, sizeof(serveraddr));
 serveraddr.sin6_family = AF_INET6;
 serveraddr.sin6_addr = in6addr_any;
 serveraddr.sin6_port = htons(listen_port);
 if ( 0 > bind(sock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) ) {
  perror("bind");
  puts("unable to bind");
  return 1;
 }
 clientlen = sizeof(clientaddr);

 /* start workers */
 for(i=0; i < WORKERS; i++) {
  pthread_mutex_init(&(workers[i].lock), NULL);
  pthread_cond_init(&(workers[i].go), NULL);
  pthread_create(&(workers[i].tpid), NULL, work, (void*) &(workers[i]));
 }

 i = 0;
 while (1) {
  n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &clientaddr, &clientlen);
  printf("%s", buffer);
  pthread_mutex_lock(&(workers[i].lock));
  workers[i].bufferlen = n;
  memcpy(&(workers[i].buffer), buffer, n);
  workers[i].sock = sock;
  workers[i].clientaddr = clientaddr;
  pthread_mutex_unlock(&(workers[i].lock));
  pthread_cond_signal(&(workers[i].go));
  i++;
  i %= WORKERS;
  /* parse request */

 }
 return 0;
}
