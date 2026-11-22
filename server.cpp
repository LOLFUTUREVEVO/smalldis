#include <assert.h>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>


const size_t k_max_msg = 4096;

static void dead(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

static int32_t read_full(int fd, char * buf, size_t n) {
	while(n > 0) {
		ssize_t rv = read(fd, buf, n);
		if(rv <= 0) {
			return -1;
		}
		assert((size_t)rv <= n);
		n -= (size_t)rv;
		buf += rv;
	}
	return 0;
}

static int32_t write_all(int fd, char * buf, size_t n) {
	while(n > 0) {
		ssize_t rv = write(fd, buf ,n);
		if(rv <= 0) {
			return -1;
		}
		assert((size_t)rv <= n);
		n -= (size_t)rv;
		buf+= rv;
	}
	return 0;
}



static int32_t processing_confd(int connfd) {
	char rbuf[4 + k_max_msg];
	errno = 0;
	int32_t err = read_full(connfd, rbuf, 4);

	if(err) {
		printf(errno == 0 ? "EOF" : "ERROR READING: [%d]");
		return err;
	}
	uint32_t len = 0;
	memcpy(&len, rbuf, 4);
	if(len > k_max_msg) {
		printf("Message Too Long\n");
		return -1;
	}
	// Req body
	err = read_full(connfd, &rbuf[4], len);
	if(err){
		printf("Read() Error");
		return err;
	}	
	printf("Client Message: %.*s\n", len, &rbuf[4]);
	const char reply[] = "World!";

	char wbuf[4 + sizeof(reply)];
	len = (uint32_t)strlen(reply);
	memcpy(wbuf, &len, 4);
	memcpy(&wbuf[4], reply, len);
	return write_all(connfd, wbuf, 4 + len);

}



int main(int argc, char** argv) {

	
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	int val = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	struct sockaddr_in addr = {}; 
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1234);
	addr.sin_addr.s_addr = htonl(0);
	int rv = bind(fd, (const struct sockaddr *)&addr , sizeof(addr));
	if(rv) {dead("bind()");}
	
	rv = listen(fd, SOMAXCONN);
	if(rv) {
		dead("listen()");
	}

	while(true) {
		struct sockaddr_in client_addr = {};
		socklen_t addrlen = sizeof(client_addr);
		int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
		if(connfd < 0) {
			continue;
		}
		processing_confd(connfd);	
		close(connfd);
	}

	
}
