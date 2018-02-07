#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys//socket.h>
#include <netinet/in.h>


int main(int argc, char** argv){
	int Socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(12345);
	sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	connect(Socket,(struct sockaddr *)(&sa), sizeof(sa));
	char Buffer[]="PING";
	send(Socket,Buffer,4,MSG_NOSIGNAL);
	recv(Socket,Buffer,4,MSG_NOSIGNAL);
	shutdown(Socket,SHUT_RDWR);
	close(Socket);
	printf("%s\n",Buffer);
 



return 0;
}
	
	
