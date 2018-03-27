#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define HEIGHT 8
#define WIDTH 9
#define TURNTIME 15
#define MAXPLAYERS 20


//Some quick notes, Open the server client (this one) first, and then open 2 clients
//the included makefile will generate an executable for both the server and the client by default. Server is ./gameroom and client is ./client
//I have included some previously generated leaderboard results to show off that feature
//the program can generate a leaderboard on it's own, but the leaderBoard file MUST BE FULLY DELETED AND NOT JUST WIPED
//sometimes the client input gets stuck. If it does get stuck, make sure the server isn't waiting on the other client to do something first
//if it's still stuck, smashing enter a bunch seems to have helped for memccpy
//the server adress and ports are hard programmed into the code as local host just so that you don't have to tweak anything to get it to work on any machine
//you can input the ports mannually into the client, but the server is only set up to listen on 2 specific ports, which the clients will default to if you dont give them any arguements



void printBoard(int gameBoard[][HEIGHT],int socket);
int checkWinner(int gameBoard[][HEIGHT], int playerID,int *socket);
void printBoardToFile(int gameBoard[][HEIGHT], FILE *fp,int playerID);
int doMove(int gameBoard[][HEIGHT], int  move , int playerID);
void updateLeaderboard(char players[MAXPLAYERS][16], int playerdata[MAXPLAYERS][2], int player1Index, int player2Index, FILE *fp,int winner,int numberOfPlayers);

int playerID =1;
int safety = 0;
int timer=0;


void catch_child(int signal){
	if(safety) return;
	timer=1;
}

int main(int argc, char** argv){
	
	char *leaderBoard="Connect_Four_Leader_Board.txt";
	char *users="Connect_Four_Users.txt";
	char *fileName="Connect_Four_Log.txt";
	FILE *fp;
	FILE *fplb;
	int player1hasid=-1;
	int player2hasid=-1;
	char player1id[16];
	char player2id[16];
	char players[MAXPLAYERS+1][16];
	int playerdata[MAXPLAYERS][2];
	char temp;
	char numbuf[3]={0};
	char buffer[1024];

	signal(SIGCHLD, catch_child);
	
	if(access(leaderBoard, F_OK) != -1){
		fplb=fopen(leaderBoard,"r");
	}
	if(access(leaderBoard, F_OK) == -1){
		printf("no leaderBoard found, generating a new one \n");
		fplb=fopen(leaderBoard,"w");
		fputs("Format: User/totalgames/wins \nend",fplb);//dont mess with the formating of this, WILL MAKE IT SEG FAULT
	}
	fclose(fplb);
	fplb=fopen(leaderBoard,"r");
	
	int l =0;//will hold the total number of entries in the array. I wish i had named this better
	int m=0;
	fgets(buffer,1024,fplb);
	while(1){//reading the leaderboard into arrays
		fgets(buffer,1024,fplb);
		if(strcmp(buffer,"end")==0)break;
		int m=0;
		int k=0;
		while((temp=buffer[m])!='/'){
			players[l][k]=temp;
			k++;
			m++;
		}
		m++;
		k=0;
		while((temp=buffer[m])!='/'){
			numbuf[k]=temp;
			k++;
			m++;
		}
		playerdata[l][0]=atoi(numbuf);
		numbuf[1]=0;
		numbuf[2]=0;
		numbuf[3]=0;
		k=0;
		m++;
		while((temp=buffer[m])!='\n'){
			numbuf[k]=temp;
			k++;
			m++;
		}
		playerdata[l][1]=atoi(numbuf);
		numbuf[1]=0;
		numbuf[2]=0;
		numbuf[3]=0;
		l++;
		
	}
	fclose(fplb);
	
	int welcomeSocket, newSocket[2];//making connections
	struct sockaddr_in serverAddr1,serverAddr2;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;

  
	welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
	serverAddr1.sin_family = AF_INET;
	serverAddr1.sin_port = htons(7891);
	serverAddr1.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(serverAddr1.sin_zero, '\0', sizeof serverAddr1.sin_zero); 
	bind(welcomeSocket, (struct sockaddr *) &serverAddr1, sizeof(serverAddr1));
	printf("waiting for first player...\n");
	listen(welcomeSocket,1);
 
	addr_size = sizeof serverStorage;
	newSocket[0] = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);

	send(newSocket[0],"You are player 1 \n",1024,0);
  
	welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
	serverAddr2.sin_family = AF_INET;
	serverAddr2.sin_port = htons(7892);
	serverAddr2.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(serverAddr2.sin_zero, '\0', sizeof serverAddr2.sin_zero); 
	bind(welcomeSocket, (struct sockaddr *) &serverAddr2, sizeof(serverAddr2));
	printf("waiting for second player...\n");
	listen(welcomeSocket,1);
  
	newSocket[1] = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
	
	send(newSocket[1],"You are player 2 \n",1024,0);
	
	int board[WIDTH][HEIGHT];//two bigger than it needs to be as a buffer on each side when checking
	int i=0;
	int j;
	
	
	char input[]="-2";
	char *ptr;
	pid_t pid;
	
	
	
	while(i<HEIGHT){//initialize board to 0's
		j=0;
		while(j<WIDTH){
			board[j][i]=0;
			j++;
		}
		i++;
	}
	
	send(newSocket[0],"Welcome to Connect Four! \n \n Please enter your user id, exactly as you made. If you do not have one, whatever string you type in will be your user id. No more than 15 characters please\n" ,256,0);
	send(newSocket[1],"Welcome to Connect Four! \n \n Please enter your user id, exactly as you made. If you do not have one, whatever string you type in will be your user id. No more than 15 characters please\n"  ,256,0);
	
	
	read(newSocket[0],buffer,1024);
	stpcpy(player1id,buffer);
	read(newSocket[1],buffer,1024);
	stpcpy(player2id,buffer);

	j=0;//input sanitization
	while(j<15){
		if(player1id[j]=='\n'||player1id[j]==' ')player1id[j]=0;
		j++;
	}
	j=0;
	while(j<15){
		if(player2id[j]=='\n'||player2id[j]==' ')player2id[j]=0;
		j++;
	}
	
	printf("player 1 id: %s\n", player1id);
	printf("player 2 id: %s\n", player2id);
	m=0;
	while(m<l){
		if(strcmp(players[m],player1id)==0){
			player1hasid=m;
			printf("player 1 id found \n");
			sprintf(buffer,"\n Welcome back, %s! You have won %i  out of your %i games! \n \n",players[m],playerdata[m][1],playerdata[m][0]);
			send(newSocket[0],buffer,1024,0 );
			break;
		}
		m++;
	}
	if(m==l){
		player1hasid=l;
		stpcpy(players[m],player1id);
		playerdata[m][0]=0;
		playerdata[m][1]=0;
		l++;
	}
	m=0;
	while(m<l){
		if(strcmp(players[m],player2id)==0){
			player2hasid=m;
			printf("player 2 id found \n");
			sprintf(buffer,"\n Welcome back, %s! You have won %i out of your %i games! \n \n",players[m],playerdata[m][1],playerdata[m][0]);
			send(newSocket[1],buffer,1024,0 );
			break;
		}
		m++;
	}
	if(m==l){
		player2hasid=l;
		stpcpy(players[m],player2id);
		playerdata[m][0]=0;
		playerdata[m][1]=0;
		l++;
	}
	
	while(1){//main gameplay loop
	
	
		char str[256];
		sprintf(str,"It's your turn, Player %i, Please enter a line number between 1 and %i, or -1 to quit \n\n", playerID, WIDTH-2);
		printBoard(board,newSocket[playerID-1]);
		
		pid=fork();
		if(!pid){
			int i=0;
			while(i<TURNTIME){
				sleep(1);
				i++;
			if(TURNTIME-i==10)send(newSocket[playerID-1],"You have 10 seconds remaining \n",64,0);
			if(TURNTIME-i==0)send(newSocket[playerID-1],"You've run out out time. Please enter literally anything that isn't whitespace. \n", 128,0);
			}
			exit(0);
		}
		char input[]="-2";
		int j=-2;
		while(doMove(board,j,playerID)==0){
			send(newSocket[playerID-1],str,256,0);
			while((( j<-1) || (j>WIDTH-1) )|| (j==0)){
				if(timer)break;
				read(newSocket[playerID-1], input,1024);
				j= strtol(input,&ptr,10);
				if(timer)break;
			}
		} 
		safety=1;
		kill(pid, SIGTERM);
		sleep(1);
		safety=0;
		if(timer) j=-1;
		if(j==-1){
			sprintf(str,"Player %i has surrendered! \n", playerID);
			send(newSocket[0],str,256,0);
			send(newSocket[1],str,256,0);
			safety=1;
			kill(pid, SIGTERM);
			sleep(1);
			safety=0;
			fplb=fopen(leaderBoard,"w");
			updateLeaderboard(players,playerdata,player1hasid,player2hasid,fplb,(playerID%2)+1,l);
			fclose(fplb);
			send(newSocket[0],"kill",256,0);
			send(newSocket[1],"kill",256,0);
			exit(0);
		}
		
		

		i=checkWinner(board,playerID,newSocket);
		if(i){
			printBoard(board, newSocket[playerID-1]);
			fp=fopen(fileName,"w");
			printBoardToFile(board,fp,i);
			fclose(fp);
			fplb=fopen(leaderBoard,"w");
			updateLeaderboard(players,playerdata,player1hasid,player2hasid,fplb,playerID,l);
			fclose(fplb);
			send(newSocket[0],"kill",256,0);
			send(newSocket[1],"kill",256,0);
			break;
		}
	
	playerID=(playerID%2)+1;
	
	}
}

void updateLeaderboard(char players[MAXPLAYERS][16], int playerdata[MAXPLAYERS][2], int player1Index, int player2Index, FILE *fplb,int winner,int numberOfPlayers){
	fputs("Format: User/totalgames/wins \n",fplb);
	int i=0;
	
	while(i<numberOfPlayers){
		char buffer[30]={0};
		if(i==player1Index&&winner==1){
			sprintf(buffer,"%s/%i/%i\n",players[i],playerdata[i][0]+1,playerdata[i][1]+1);
			fputs(buffer,fplb);
			i++;
			continue;
		}
		if(i==player1Index&&winner==2){
			sprintf(buffer,"%s/%i/%i\n",players[i],playerdata[i][0]+1,playerdata[i][1]);
			fputs(buffer,fplb);
			i++;
			continue;
		}
		if(i==player2Index&&winner==2){
			sprintf(buffer,"%s/%i/%i\n",players[i],playerdata[i][0]+1,playerdata[i][1]+1);
			fputs(buffer,fplb);
			i++;
			continue;
		}
		if(i==player1Index&&winner==1){
			sprintf(buffer,"%s/%i/%i\n",players[i],playerdata[i][0]+1,playerdata[i][1]);
			fputs(buffer,fplb);
			i++;
			continue;
		}
		sprintf(buffer,"%s/%i/%i\n",players[i],playerdata[i][0],playerdata[i][1]);
		fputs(buffer,fplb);
		
		i++;
	}
	fputs("end",fplb);
	
}

void printBoard(int gameBoard[][HEIGHT],int socket){
	
	char str[1024];
	char strbuilder[1024];
	int i=1;
	int j=1;
	sprintf(strbuilder,"\t1\t2\t3\t4\t5\t6\t7\n");
	//printf("\t1\t2\t3\t4\t5\t6\t7\n");
	
	while(i<HEIGHT-1){//start and stop one short as to not print out buffer spaces
		sprintf(str,"%i\t",i);
		strcat(strbuilder,str);
		//send(socket,str, 5,0);
		j=1;
		while(j<WIDTH-1){
			sprintf(str,"%i\t",gameBoard[j][i]);
			strcat(strbuilder,str);
			//send(socket,str, 5,0);
			//printf("%i\t",gameBoard[j][i]);
			j++;
		}
		strcat(strbuilder, "\n");
		//send(socket,"\n",2,0);
		//printf("\n");
		i++;
		
	}
	send(socket,strbuilder, 1024,0);
	//printf("%s \n", strbuilder);
}

void printBoardToFile(int gameBoard[][HEIGHT], FILE *fp, int playerID){
	
	if (playerID!=-1)fprintf(fp,"Player %i won this game. Can you spot where? \n", playerID);
	if (playerID==-1)fprintf(fp,"Nobody won this game. \n");
	
	int i=1;
	int j=1;
	fprintf(fp,"\t1\t2\t3\t4\t5\t6\t7\n");
	
	while(i<HEIGHT-1){//start and stop one short as to not print out buffer spaces
		fprintf(fp,"%i\t",i);
		j=1;
		while(j<WIDTH-1){
			fprintf(fp,"%i\t",gameBoard[j][i]);
			j++;
		}
		fprintf(fp,"\n");
		i++;
	}
}

int checkWinner(int board[][HEIGHT], int playerID,int socket[]){
	int i=1;
	int j=1;
	char buf[1024];
	char buf2[1024]={0};

	while(i<HEIGHT-1){
		j=1;
		while(j<WIDTH-1){
			if(board[j][i]==playerID){//check current square
				if( (board[j][i-1]==playerID) && (board[j][i-2]==playerID) && (board[j][i-3]==playerID)){//check North 
					sprintf(buf,"Connect 4 found, North Starting at %i,%i.\n",j,i);
					strcat(buf2,buf);
					sprintf(buf,"Congrats, Player %i! You've won! Check the log file if you want a copy of the final board.\n",playerID);
					strcat(buf2,buf);
					send(socket[0],buf2,1024,0);
					send(socket[1],buf2,1024,0);
					return playerID;	
				}
				if( (board[j+1][i-1]==playerID) && (board[j+2][i-2]==playerID) && (board[j+3][i-3]==playerID)){//check North East 
					sprintf(buf,"Connect 4 found, North East Starting at %i,%i.\n",j,i);
					strcat(buf2,buf);
					sprintf(buf,"Congrats, Player %i! You've won! Check the log file if you want a copy of the final board.\n",playerID);
					strcat(buf2,buf);
					send(socket[0],buf2,1024,0);
					send(socket[1],buf2,1024,0);
					return playerID;
				}
				if( (board[j+1][i]==playerID) && (board[j+2][i]==playerID) && (board[j+3][i]==playerID)){//check East 
					sprintf(buf,"Connect 4 found, East Starting at %i,%i.\n",j,i);
					strcat(buf2,buf);
					sprintf(buf,"Congrats, Player %i! You've won! Check the log file if you want a copy of the final board.\n",playerID);
					strcat(buf2,buf);
					send(socket[0],buf2,1024,0);
					send(socket[1],buf2,1024,0);
					return playerID;
				}
				if( (board[j+1][i+1]==playerID) && (board[j+2][i+2]==playerID) && (board[j+3][i+3]==playerID)){//check South East 
					sprintf(buf,"Connect 4 found, South East Starting at %i,%i.\n",j,i);
					strcat(buf2,buf);
					sprintf(buf,"Congrats, Player %i! You've won! Check the log file if you want a copy of the final board.\n",playerID);
					strcat(buf2,buf);
					send(socket[0],buf2,1024,0);
					send(socket[1],buf2,1024,0);
					return playerID;
				}
				if( (board[j][i+1]==playerID) && (board[j][i+2]==playerID) && (board[j][i+3]==playerID)){//check South 
					sprintf(buf,"Connect 4 found, South Starting at %i,%i.\n",j,i);
					strcat(buf2,buf);
					sprintf(buf,"Congrats, Player %i! You've won! Check the log file if you want a copy of the final board.\n",playerID);
					strcat(buf2,buf);
					send(socket[0],buf2,1024,0);
					send(socket[1],buf2,1024,0);
					return playerID;
				}
				if( (board[j-1][i+1]==playerID) && (board[j-2][i+2]==playerID) && (board[j-3][i+3]==playerID)){//check South West
					sprintf(buf,"Connect 4 found, South West Starting at %i,%i.\n",j,i);
					strcat(buf2,buf);
					sprintf(buf,"Congrats, Player %i! You've won! Check the log file if you want a copy of the final board.\n",playerID);
					strcat(buf2,buf);
					send(socket[0],buf2,1024,0);
					send(socket[1],buf2,1024,0);
					return playerID;
				} 
				if( (board[j-1][i]==playerID) && (board[j-2][i]==playerID) && (board[j-3][i]==playerID)){//check West
					sprintf(buf,"Connect 4 found, West Starting at %i,%i.\n",j,i);
					strcat(buf2,buf);
					sprintf(buf,"Congrats, Player %i! You've won! Check the log file if you want a copy of the final board.\n",playerID);
					strcat(buf2,buf);
					send(socket[0],buf2,1024,0);
					send(socket[1],buf2,1024,0);
					return playerID;
				}
				if( (board[j-1][i-1]==playerID) && (board[j-2][i-2]==playerID) && (board[j-3][i-3]==playerID)){//check North West
					sprintf(buf,"Connect 4 found, North West Starting at %i,%i.\n",j,i);
					strcat(buf2,buf);
					sprintf(buf,"Congrats, Player %i! You've won! Check the log file if you want a copy of the final board.\n",playerID);
					strcat(buf2,buf);
					send(socket[0],buf2,1024,0);
					send(socket[1],buf2,1024,0);
					return playerID;
				}
			}
			j++;
		}
		i++;
	}
	
	j=1;
	
	while(j<WIDTH-1){//check full board condition
		if(!board[j][1]) break;
		if(j==WIDTH-2){
			send(socket[0],"You've filled the whole board up with no winner. It's a tie! \n",1024,0);
			send(socket[1],"You've filled the whole board up with no winner. It's a tie! \n",1024,0);
			return -1;
		}
		j++;
	}
	return 0;
}

int doMove(int board[][HEIGHT], int  move , int playerID){
	
	if(board[move][1])return 0;//check to see if row is full
	if (move==-2) return 0;
	
	int i =1;
	while(i<HEIGHT-1){
		if(i==HEIGHT-2){
			board[move][i]=playerID;
			return 1;
		}
		if(board[move][i+1]){
			board[move][i]=playerID;
			return 1;
		}
		i++;
	}
}