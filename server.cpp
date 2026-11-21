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




static void dead(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

static void processing_confd(int connfd) {
	char rbuf[64] = {};
	ssize_t n = read(connfd, rbuf,sizeof(rbuf) - 1);
	if(n < 0) {
		printf("read() err.\n");
		return;
	}

	printf("client says: %s\n", rbuf);
	
	char wbuf[] = "World";
	write(connfd, wbuf, strlen(wbuf));
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
		
		close(connfd);
	}

	
}
