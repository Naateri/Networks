 /* Client code in C++ */

/*

INTEGRANTES:
-Renato Luis Postigo Avalos
-Jazmine Alexandra Alfaro Llerena

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
#include <fstream>
#include <chrono>

//recieve message

typedef std::string str;

int SocketFD;
char buffer[256];

int port = 47000;
str ip = "127.0.0.1";
bool exit_prog = false;
bool ACK = false;

void open_another_port(int);

str read_txt(str file){
	std::ifstream my_file(file.c_str());
	
	str temp_msg, full_msg;
	full_msg.clear();
	
	while (getline(my_file, temp_msg)){
		full_msg += temp_msg;
		full_msg += '\n';
	}
	
	my_file.close();
	
	return full_msg;
}

void recieve_msg(int ConnectFD){
	int n;
	char buffer[6];
	str num;
	do{
		//READS ACKS
		bzero(buffer, 6);
		n = read(ConnectFD, buffer, 6);
		buffer[n] = '\0';
		if (strcmp(buffer, "ACK.") == 0){
			printf("ACK recieved.\n");
		}
	} while (!exit_prog);
}	

void send_msg(int ConnectFD){
	int n;
	str sign;
	char buffer[6];
	int port;
	
	printf("First port FD: %d\n", ConnectFD);
	
	std::cin >> sign;
	write(ConnectFD, sign.c_str(), sign.size());
	n = read(ConnectFD, buffer, 6); //First ACK.
	buffer[n] = '\0';
	printf("Server: [%s]\n", buffer);
	if (strcmp(buffer, "ACK.") == 0){
		port = stoi(sign);
		printf("Acknowledged. Opening connection at port %d.\n", port);
		//open_another_port(port);
		std::thread t_algo(open_another_port, port);
		t_algo.detach();
		usleep(500);
	}
	
	//SEND ACKS
	
	do{
		while (!ACK);
		printf("Sending ACK to %d\n", ConnectFD);
		write(ConnectFD, "ACK.", 4);
		printf("ACK sent\n");
		ACK = false;
	} while(!exit_prog);
}

void write_ftp(int ConnectFD){
	int n;
	str sign;
	char buffer[1000];
	
	printf("Second port FD: %d\n", ConnectFD);
	
	getline(std::cin, sign);
	
	do{
		bzero(buffer, 1000);
		std::cin.clear();
		getline(std::cin, sign);
		n = write(ConnectFD, sign.c_str(), sign.size());
		sign[n] = '\0';
		std::cout << "Message sent: " << sign << std::endl;
		
		if (sign[0] == '1'){ //get response from server
			n = read(ConnectFD, buffer, 1000);
			buffer[n] = '\0';
			printf("Server: [%s]\n", buffer);
		}
		
		if (sign[0] == '3'){
			ACK = true;
			printf("Sending ACK\n");
			exit_prog = true;
		}
		
		ACK = true; //send ACK
	}while (!exit_prog);
}

void open_another_port(int port){
	struct sockaddr_in stSockAddr;
	int Res;
	SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	int n;
	
	if (-1 == SocketFD)
	{
		perror("cannot create socket");
		exit(EXIT_FAILURE);
	}
	
	memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
	
	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(port); //port
	Res = inet_pton(AF_INET, ip.c_str(), &stSockAddr.sin_addr);
	
	printf("Opened socket at port %d.\n", port);
	
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
	
	//connection with other port
	write_ftp(SocketFD);
}
 
int main(void){
	str temp_file;
	//strcpy(Renato.photo, temp_photo.c_str());
	
	//my_file.close();
	
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
    Res = inet_pton(AF_INET, ip.c_str(), &stSockAddr.sin_addr);
 
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
	
	
	std::thread t1(recieve_msg, SocketFD);
	std::thread t2(send_msg, SocketFD);
	t1.detach();
	t2.detach();
	
	while(!exit_prog);
	
    shutdown(SocketFD, SHUT_RDWR);
 
    close(SocketFD);

    return 0;
  }
