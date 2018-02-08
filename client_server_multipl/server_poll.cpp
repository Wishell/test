#include <iostream>
#include <sys/types.h>
#include <set>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <sys/poll.h>

#define POLL_SIZE  2048

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
	struct pollfd Set[POLL_SIZE];
	Set[0].fd = Master_socket;
	Set[0].events = POLLIN;

	while (true){

		int index =1;
		for (auto iter = Slave_sockets.begin();iter != Slave_sockets.end();++iter){
			Set[index].fd = *iter;
			Set[index].events = POLLIN;
			++index; 
		}
		
		int Set_SIZE = 1 + Slave_sockets.size();
		poll (Set, Set_SIZE,-1);


		for (int i = 0; i<Set_SIZE;++i){
			if (Set[i].revents & POLLIN){
					if(Set[i].fd == Master_socket){
						int Slave_socket = accept(Master_socket,0,0);
						set_nonblock(Slave_socket);
						Slave_sockets.insert(Slave_socket);
					} else {
						static char Buffer[1024];
					int RecvSize = recv(Set[i].fd, Buffer, 1024,MSG_NOSIGNAL);
						if ((RecvSize == 0) && (errno != EAGAIN)){
							shutdown (Set[i].fd, SHUT_RDWR);
							close(Set[i].fd);
							Slave_sockets.erase(i);
						} else if (RecvSize!=0){
							send(Set[i].fd,Buffer,RecvSize,MSG_NOSIGNAL);
						}
					}
				}
			}		
		}
return 0;
}




