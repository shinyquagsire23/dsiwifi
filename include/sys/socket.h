// DSWifi Project - socket emulation layer defines/prototypes (sys/socket.h)
// Copyright (C) 2005-2006 Stephen Stair - sgstair@akkit.org - http://www.akkit.org
/****************************************************************************** 
DSWifi Lib and test materials are licenced under the MIT open source licence:
Copyright (c) 2005-2006 Stephen Stair

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/



#ifndef SYS_SOCKET_H
#define SYS_SOCKET_H

#define FD_SETSIZE	32
#include <sys/time.h>


#define PF_UNSPEC		0
#define PF_INET			2
#define PF_INET6		10

#define AF_UNSPEC		PF_UNSPEC
#define AF_INET			PF_INET
#define AF_INET6		PF_INET6

#define SOCK_STREAM		1
#define SOCK_DGRAM		2

// need to sync FIO* values with commonly accepted ones sometime
#define FIONBIO			1
#define FIONREAD		2

#define SOCKET_ERROR	-1

// send()/recv()/etc flags
// at present, only MSG_PEEK is implemented though.
#define MSG_WAITALL		0x40000000
#define MSG_TRUNC		0x20000000
#define MSG_PEEK		0x10000000
#define MSG_OOB			0x08000000
#define MSG_EOR			0x04000000
#define MSG_DONTROUTE	0x02000000
#define MSG_CTRUNC		0x01000000

// shutdown() flags:
#define SHUT_RD			1
#define SHUT_WR			2
#define SHUT_RDWR		3

struct sockaddr {
	unsigned short		sa_family;
	char				sa_data[14];
};

#ifndef ntohs
#define ntohs(num) htons(num)
#define ntohl(num) htonl(num)
#endif

/*
typedef struct fdset_ {
	int fdbits[(FD_SETSIZE+31)/32];
} fd_set;
*/

#ifdef __cplusplus
extern "C" {
#endif

	extern int socket(int domain, int type, int protocol);
	extern int bind(int socket, const struct sockaddr * addr, int addr_len);
	extern int connect(int socket, const struct sockaddr * addr, int addr_len);
	extern int send(int socket, const void * data, int sendlength, int flags);
	extern int recv(int socket, void * data, int recvlength, int flags);
	extern int sendto(int socket, const void * data, int sendlength, int flags, const struct sockaddr * addr, int addr_len);
	extern int recvfrom(int socket, void * data, int recvlength, int flags, struct sockaddr * addr, int * addr_len);
	extern int listen(int socket, int max_connections);
	extern int accept(int socket, struct sockaddr * addr, int * addr_len);
	extern int shutdown(int socket, int shutdown_type);
	extern int closesocket(int socket);

	extern int ioctl(int socket, long cmd, void * arg);

	extern int setsockopt(int socket, int level, int option_name, const void * data, int data_len);
	extern int getsockopt(int socket, int level, int option_name, void * data, int * data_len);

	extern int getpeername(int socket, struct sockaddr *addr, int * addr_len);
	extern int getsockname(int socket, struct sockaddr *addr, int * addr_len);


	unsigned short htons(unsigned short num);
	unsigned long htonl(unsigned long num);


	extern int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout);
//	extern void FD_CLR(int fd, fd_set * fdset);
//	extern int FD_ISSET(int fd, fd_set * fdset);
//	extern void FD_SET(int fd, fd_set * fdset);
//	extern void FD_ZERO(fd_set * fdset);


#ifdef __cplusplus
};
#endif


#endif
