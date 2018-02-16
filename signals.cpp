
#include<signal.h>
#include<stdio.h>
#include<fstream>
#include<unistd.h>


int main (int argc, char** argv){

std::ofstream fpar;
fpar.open(" /home/osboxes/sig/pid_parent.txt");
std::ofstream fchld;
fchld.open(" /home/osboxes/sig/pid_child.txt");

pid_t pid_par;
pid_t pid_chld;

pid_par = getpid();
fpar << pid_par;
fpar.close();

if (fork()){
//parent
printf("parent proc pid=%d \n",pid_par);
signal(SIGTERM,SIG_IGN);
signal(SIGABRT,SIG_IGN);
while(1)
{
sleep(1);

}
} else {
//chld
pid_chld = getpid();
fchld << pid_chld;
fchld.close();
printf("child proc pid=%d \n",pid_chld);

signal(SIGTERM,SIG_DFL);
signal(SIGABRT,SIG_IGN);

while(1){

    sleep(1);
}

}








    return 0;
}