#ifndef SELECT_H_
#define SELECT_H_

#include <syslog.h>

#define SOL_SOCKET	0xffff

#define SO_BINDTODEVICE	0x000
#define SO_ERROR	0x1007

int select(int nfds, fd_set *readfds, fd_set *writefds,
		fd_set *exceptfds, struct timeval *timeout) {

	syslog(LOG_ERR, "%s not implemented", __func__);

	return 1;
}

int setsockopt(int sockfd, int level, int optname,
		const void *optval, socklen_t optlen) {
	
	syslog(LOG_ERR, "%s not implemented", __func__);

	return 0;
}

int getsockopt(int sockfd, int level, int optname,
		void *optval, socklen_t *optlen) {

	syslog(LOG_ERR, "%s not implemented", __func__);

	return 0;
}

#endif /* SELECT_H_ */
