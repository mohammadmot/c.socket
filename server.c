// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080

int main(int argc, char const *argv[])
{
	printf("server> run application\n");

	int server_fd, new_socket, valread;
	struct sockaddr_in address; // Structures for handling internet addresses
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
	char *hello = "Hello from server";
	
	// ----------------------------
	/*
	int sockfd = socket(domain, type, protocol)

	sockfd: socket descriptor, an integer (like a file-handle)
	domain: integer, communication domain e.g.
		AF_INET (IPv4 protocol)
		AF_INET6 (IPv6 protocol)
	type: communication type
		SOCK_STREAM: TCP(reliable, connection oriented)
		SOCK_DGRAM: UDP(unreliable, connectionless)
	protocol: Protocol value for Internet Protocol(IP), which is 0. This is the same number which appears on protocol field in the IP header of a packet.(man protocols for more details)
	*/

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	printf("server> create socket file\n");

	// ----------------------------
	/*
	This helps in manipulating options for the socket referred by the file descriptor sockfd.
	This is completely optional, but it helps in reuse of address and port.
	Prevents error such as: “address already in use”
	*/

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
												&opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	printf("server> set socket option\n");
	
	// ----------------------------
	/*
	After creation of the socket, bind function binds the socket to the address and port number
	 specified in addr(custom data structure).
	 we bind the server to the localhost, hence we use INADDR_ANY to specify the IP address.

	#include <netinet/in.h>
	struct sockaddr_in {
		short            sin_family;   // e.g. AF_INET
		unsigned short   sin_port;     // e.g. htons(3490)
		struct in_addr   sin_addr;     // see struct in_addr, below
		char             sin_zero[8];  // zero this if you want to
	};
	struct in_addr {
		unsigned long s_addr;  // load with inet_aton()
	}
	*/
	address.sin_family = AF_INET;
	/* These are the basic structures for all syscalls and functions that 
	deal with internet addresses. In memory, the struct sockaddr_in is the same size
	as struct sockaddr, and you can freely cast the pointer of one type to the other
	 without any harm*/
	address.sin_addr.s_addr = INADDR_ANY;
	// inet_aton("127.0.0.1", &address.sin_addr.s_addr);
	address.sin_port = htons( PORT );
	/*
	Conversion:  
	htons(), htonl(): host to network short/long  
	ntohs(), ntohl(): network order to host short/long
	*/
		
	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr *)&address,
								sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("server> specified ip and port to socket [%i:%i]\n", address.sin_addr.s_addr, PORT);

	/*
	It puts the server socket in a passive mode, where it waits for the client to approach
	the server to make a connection. The backlog, defines the maximum length to which the queue
	of pending connections for sockfd may grow. If a connection request arrives when the queue 
	is full, the client may receive an error with an indication of ECONNREFUSED.
	*/
	printf("server> blocking, listen for any new client request ...\n");
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	/*
	It extracts the first connection request on the queue of pending connections for the listening
	socket, sockfd, creates a new connected socket, and returns a new file descriptor referring
	to that socket. At this point, connection is established between client and server,
	and they are ready to transfer data.
	*/
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
					(socklen_t*)&addrlen))<0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	valread = read( new_socket , buffer, 1024);
	printf("server> read data from client: %s\n", buffer);

	send(new_socket , hello , strlen(hello) , 0);
	printf("server> hello message sent to client.\n");
	
	return 0;
}
