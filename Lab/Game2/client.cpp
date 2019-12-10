/* Client code in C++ */

/*

Client code programmed by:
-Jazmine Alexandra Alfaro Llerena
-Renato Luis Postigo Avalos

*/

//g++ client.cpp -o client -std=c++11 -lpthread
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <termios.h>

typedef std::string str;

using namespace std;

char getch() {
	char buf = 0;
	struct termios old = {0};
	if (tcgetattr(0, &old) < 0)
		perror("tcsetattr()");
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &old) < 0)
		perror("tcsetattr ICANON");
	if (read(0, &buf, 1) < 0)
		perror ("read()");
	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if (tcsetattr(0, TCSADRAIN, &old) < 0)
		perror ("tcsetattr ~ICANON");
	return (buf);
}

//recieve message

int SocketFD;
bool end1 = false;
char buffer[100];
std::string server_ip = "192.168.1.62";
int port = 42800;

bool logged_in = false; //change to true if just testing gameplay
//change back to false for full functionality

void send_msg(){
	char msg[256];
	str real_msg;
    int n;
	int ch;
	std::string key;
	do{
		if (logged_in){
			ch = getch();
			msg[0] = char(ch);
			msg[1] = '\0';
			//key = char(ch);
		
			n = write(SocketFD,msg,2); //cuantos bytes estoy mandando
		
			if (ch == 'q') end1 = true;
		} else {
			std::cin.getline(msg, 255);
			//real_msg.clear();
			//std::getline (std::cin, real_msg);
			//std::cout << "Message: " << real_msg << std::endl;
			std::cout << "Message: " << msg << std::endl;
			n = write(SocketFD,msg,255); //cuantos bytes estoy mandando
			
			msg[n] = '\0';
		}
	} while(!end1);
}

void rcv_msg(){
    int n;
	
	do{	
	bzero(buffer,100 + 1);
    n = read(SocketFD,buffer,100 + 1);
   	if (n < 0) perror("ERROR reading from socket");
	buffer[n] = '\0';
   	//printf("Here is the message: [%s]\n",buffer);
	printf("%s",buffer);
	//n = write(SocketFD,"Ok. Message recieved.",21); 
	if (strcmp(buffer, "Logging in successful.\n") == 0){
		logged_in = true;
		printf("Server said we're logged in. Press enter.\n");
	}
	
	if (strcmp(buffer, "End game") == 0){
		end1 = true;
	}
	} while(!end1);
}
 
int main(void){
    struct sockaddr_in stSockAddr;
    int Res;
    SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int n;
    char msg[256];
 
    if (-1 == SocketFD)
    {
      perror("cannot create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(port); //port
    Res = inet_pton(AF_INET, server_ip.c_str() , &stSockAddr.sin_addr);
 
    if (0 > Res)
    {
      perror("error: first parameter is not a valid address family");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    else if (0 == Res)
    {
      perror("char string (second parameter does not contain valid ipaddress");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 	//hacer un chat para la siguiente clase
	int cnct = connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in));
    if (-1 == cnct)
    {
    	perror("connect failed");
    	close(SocketFD);
    	exit(EXIT_FAILURE);
    }
	
	std::thread t1(send_msg);
	std::thread t2(rcv_msg);

	t1.detach();
	t2.detach();
	
	while(!end1);
	//rcv_msg(); //confirming disconnection
    shutdown(SocketFD, SHUT_RDWR);
 
    close(SocketFD);

    return 0;
  }
