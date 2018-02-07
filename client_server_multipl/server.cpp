#include <iostream>
#include <sys/types.h>
#include <set>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>

int set_nonblock(int fd ){
int flags;
#if defined(O_NONBLOCK)
	if (-1 == (flags=fcntl(fd,F_GETFL,0)))
		flags=0;
	return fcntl(fd,F_SETFL,flags | O_NONBLOCK);
#else
	flags=1;
	return  ioctl(fd,FIOBIO,&flags);
#endif
}


int main (int argc, char** argv){

	int Master_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	std::set<int> Slave_sockets;
	// catch error


	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(12345);
	sa.sin_addr.s_addr = htonl (INADDR_ANY);

	bind (Master_socket, (struct sockaddr*) &sa, sizeof(sa));

	set_nonblock(Master_socket);

	listen(Master_socket, SOMAXCONN);

	while (true){
		fd_set Set;
		FD_ZERO(&Set);
		FD_SET(Master_socket, &Set);
		for (auto iter = Slave_sockets.begin();
			iter != Slave_sockets.end();
			++iter){
			FD_SET(*iter,&Set);
		}
		int Max = std::max(Master_socket, *std::max_element(Slave_sockets.begin(), Slave_sockets.end()));
		
		select(Max+1,&Set, NULL , NULL, NULL);

		for (auto iter = Slave_sockets.begin();
			iter!= Slave_sockets.end();
			++iter){
			if (FD_ISSET(*iter, &Set)){
				static char Buffer[1024];
				int RecvSize = recv(*iter, Buffer, 1024,MSG_NOSIGNAL);
				if ((RecvSize == 0) && (errno != EAGAIN)){
				std::cout<<errno<<"\n";
				shutdown (*iter, SHUT_RDWR);
				close(*iter);
				Slave_sockets.erase(iter);
				} else if (RecvSize!=0){
					send(*iter,Buffer,RecvSize,MSG_NOSIGNAL);
				}
			}
		}	
		if (FD_ISSET (Master_socket, &Set)){
			int Slave_socket = accept(Master_socket,0,0);
			set_nonblock(Slave_socket);
			Slave_sockets.insert(Slave_socket);
				
		}

	}		




return 0;
}




