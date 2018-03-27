#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>

char *port;
int safety = 0;

void catch_child(int signal){
	if (safety==1) return;
	if(strcmp(port, "7892")==0){
		
		printf("Either 2 players are already in game, or the server is not running. Please try again later. \n");
		exit(0);
	}
	printf("error occured connecting on port %s \n", port);
	if(strcmp(port, "7891")!=0){
		stpcpy(port, "7892");
		printf("attempting connection on port %s \n", port);
		char *newArgs[3];
		newArgs[0]="client";
		newArgs[1]=port;
		newArgs[2]=NULL;
		execv("./client",newArgs);  
	}
	
	printf("error occured connecting on port %s \n", port);

	stpcpy(port, "7892");
	printf("attempting connection on port %s \n", port);
	char *newArgs[3];
	newArgs[0]="client";
	newArgs[1]=port;
	newArgs[2]=NULL;
	execv("./client",newArgs);  
	
}



int main(int argc, char** argv){
	
  pid_t pid;
  signal(SIGCHLD, catch_child);
  
  if (argc>2){
	  printf("Usage: client <optional port> If you're not sure what you're looking at, leave port blank\n");
	  exit(0);
  }
  if (argc==1){
	  port=malloc(sizeof(char)*5);
	  stpcpy(port, "7891");
  }
  if (argc==2)port=argv[1];
  int clientSocket;
  char buffer[1024];
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  
  int porti=atoi(port);
  
  pid =fork();
  if(pid==0){//timer in case of attempt to connect on already busy port
		int i=0;
		while(i<2){
			sleep(1);
			i++;
			
		}
		exit(0);
	}

  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(porti);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//localhost hardcode please fix
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
  addr_size = sizeof serverAddr;
  
  
  
  int i=0;
  i=connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
  if(i==-1){
	printf("error occured connecting on port %s \n", port);
	stpcpy(port, "7892");
	printf("attempting connection on port %s \n", port);
	char *newArgs[3];
	newArgs[0]="client";
	newArgs[1]=port;
	newArgs[2]=NULL;
	execv("./client",newArgs);  
  }
  
	recv(clientSocket, buffer, 1024, 0);
	printf("%s",buffer);
  
	safety=1;
	kill(pid, SIGTERM);
	sleep(1);
	 
   
	pid =fork();
	
	if(pid){//recieve half of client
		while(1){
			recv(clientSocket, buffer, 1024, 0);
			if(strcmp(buffer,"kill")==0){
				printf("shutdown command recieved \n");
				exit(0);
			}
			printf("%s",buffer); 
		}
		
	}
	
	while(1){//send client half
		fgets(buffer, 1024,stdin);
		send(clientSocket,buffer,1024,0);
	}

}