/*
 * build instructions
 *
 * gcc -o bclient bclient.c
 *
 * Usage:
 * ./bclient <serverport>
 */

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAXBUF 65536

int main(int argc, char*argv[])
{
	int sock, status;
	char buffer[MAXBUF];
	struct sockaddr_in sock_in = {};

	sock = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
	sock_in.sin_family = PF_INET;

	status = bind(sock, (struct sockaddr *)&sock_in, sizeof(sock_in));

	/* -1 = 255.255.255.255 this is a BROADCAST address,
	 * a local broadcast address could also be used.
	 * you can compute the local broadcast using NIC address and its NETMASK 
	 */ 

	status = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (int[]){1}, sizeof(int));
	sock_in.sin_addr.s_addr = htonl(-1); /* send message to 255.255.255.255 */
	sock_in.sin_port = htons(9800); /* port number */
	sock_in.sin_family = PF_INET;
	sprintf(buffer, "?");
	status = sendto(sock, buffer, strlen(buffer) +1, 0,
		(struct sockaddr *)&sock_in, sizeof(sock_in));

	struct timespec start, now;
	clock_gettime(CLOCK_MONOTONIC, &start);
	now = start;
	double total_time = 0;
	while (total_time < 0.25) {
		char buffer[MAXBUF];
		socklen_t sinlen = sizeof(sock_in);
		status = recvfrom(sock, buffer, sizeof(buffer), MSG_DONTWAIT,
				(struct sockaddr *)&sock_in, &sinlen);
		if (status > 0) {
			char addrstr[128];
			if (inet_ntop(sock_in.sin_family, &sock_in.sin_addr,
				addrstr, sizeof(addrstr))) {
				printf("%s %s\n", buffer, addrstr);
			}
		}
		clock_gettime(CLOCK_MONOTONIC, &now);
		total_time = (now.tv_sec + now.tv_nsec/1000000000.0);
		total_time -= (start.tv_sec + start.tv_nsec/1000000000.0);
	}

	shutdown(sock, 2);
	close(sock);

	return 0;
}

