#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main (int argc, char **argv){
	
	int Master_socket = socket (AF_INET,SOCK_STREAM,IPPROTO_TCP );
	
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(12345);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	bind (Master_socket, (struct sockaddr*)(&sa) , sizeof(sa));
	
	listen (Master_socket, SOMAXCONN); // SOMAXCONN - max queury len who waits connection
	

	while (1){
		int Slave_socket = accept(Master_socket,0,0);
		int Buffer[5]={0,0,0,0,0};
//		char Buffer [1024];
//		while (1){
//		int res = recv(Slave_socket,Buffer,sizeof(Buffer),MSG_NOSIGNAL);
//			if(res>0){
//			send(Slave_socket,Buffer,res,MSG_NOSIGNAL);
//			}else{
//			break;
//			}	
//		
//		}
		recv (Slave_socket,Buffer,4,MSG_NOSIGNAL);
		send (Slave_socket,Buffer,4,MSG_NOSIGNAL);
		shutdown(Slave_socket, SHUT_RDWR);
		close(Slave_socket);
		
		printf("%s\n",(char*)Buffer);
	}
return 0;
}
