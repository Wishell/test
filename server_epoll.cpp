#include <iostream>
#include <sys/types.h>
#include <set>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <sys/epoll.h>


#define MAX_EVENTS 32

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

	int EPoll = epoll_create1(0);

	struct epoll_event Event;
	Event.data.fd = Master_socket;
	Event.events = EPOLLIN;

	epoll_ctl(EPoll,EPOLL_CTL_ADD,Master_socket, &Event);
	while (true){
		struct epoll_event Events[MAX_EVENTS];
		int N = epoll_wait (EPoll,Events,MAX_EVENTS,-1);
		
		for (int i = 0 ; i < N; ++i){
		 	if (Events[i].data.fd == Master_socket){
				int Slave_socket = accept(Master_socket,0,0);
				set_nonblock(Slave_socket);
				
				struct epoll_event Event;
				Event.data.fd = Slave_socket;
				Event.events = EPOLLIN;
			
				epoll_ctl(EPoll, EPOLL_CTL_ADD,Slave_socket,&Event);
			
			}else {
				static char Buffer[1024];
				int RecvResult = recv (Events[i].data.fd, Buffer , 1024,MSG_NOSIGNAL);
					if ((RecvResult == 0) && (errno != EAGAIN)){
						shutdown(Events[i].data.fd, SHUT_RDWR);
						close(Events[i].data.fd);
					} else if (RecvResult != 0){
						send (Events[i].data.fd ,  Buffer , RecvResult , MSG_NOSIGNAL);
					}
				}

			}
		}
	
return 0;
}




