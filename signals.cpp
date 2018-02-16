
#include<signal.h>
#include<stdio.h>
#include<fstream>
#include<unistd.h>


int main (int argc, char** argv){

std::ofstream fpar;
std::ofstream fchld;
int answ;
pid_t pid_par=fork();
pid_t pid_chld;

if (pid_par){
//parent
pid_par = getpid();
fpar.open("/home/box/pid_parent");
fpar << pid_par;
fpar.close();
printf("parent proc pid=%d \n",pid_par);
signal(SIGTERM,SIG_IGN);
signal(SIGABRT,SIG_IGN);
printf("chld proc pid=%d \n",pid_chld);

waitpid(pid_chld,answ,0);


pause();

}
} else {
//chld
pid_chld = getpid();
fchld.open("/home/box/pid_child");
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
