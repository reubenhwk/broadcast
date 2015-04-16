/*
 * build instructions:
 *
 * gcc -o bserver bserver.c
 *
 * Usage:
 * ./bserver
 */

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXBUF 65536

int main()
{
	int sock, status;
	struct sockaddr_in sock_in = {};

	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
	sock_in.sin_port = htons(9800);
	sock_in.sin_family = PF_INET;

	status = bind(sock, (struct sockaddr *)&sock_in, sizeof(sock_in));

	char hostname[128];
	gethostname(hostname, sizeof(hostname));
	char reply[128];
	snprintf(reply, sizeof(reply), "server %s", hostname);
	size_t replylen = strlen(reply) +1;

	while (1) {
		char buffer[MAXBUF];
		socklen_t sinlen = sizeof(sock_in);
		status = recvfrom(sock, buffer, sizeof(buffer), 0,
				(struct sockaddr *)&sock_in, &sinlen);
		if (status > 0) {
			char addrstr[128];
			if (inet_ntop(sock_in.sin_family, &sock_in.sin_addr,
				addrstr, sizeof(addrstr))) {
				printf("info request received from %s\n", addrstr);
				status = sendto(sock, reply, replylen, 0,
					(struct sockaddr *)&sock_in, sizeof(sock_in));
				if (-1 == status) {
					perror("sendto");
					exit(-1);
				}
			}
		} else {
			perror("recvfrom");
			exit(-1);
		}

	}

	return 0;
}

