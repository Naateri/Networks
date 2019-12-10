/* Server code in C++ */

//g++ server.cpp -o server -std=c++11 -lpthread

/*

INTEGRANTES:
-Renato Luis Postigo Avalos
-Jazmine Alexandra Alfaro Llerena

*/
 
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
#include <map>
#include <list>
#include <string>
#include <fstream>
#include <chrono>

typedef std::string str;

int port = 47000;

void open_another_port(int);

bool end = false;
bool ACK = false;

void mensaje_recibido(str, int);

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

void read_acks(int ConnectFD){
	int n;
	char buffer[6];
	str num;
	do{
		//READS ACKS
		bzero(buffer, 6);
		printf("Waiting for ACK...\n");
		n = read(ConnectFD, buffer, 6);
		buffer[n] = '\0';
		printf("Client: [%s]\n", buffer);
		if (strcmp(buffer, "ACK.") == 0){
			printf("ACK recieved.\n");
		} else {
			printf("ACK not recieved.\n");
		}
	} while (!end);
}

void recieve_msg(int ConnectFD){
	int n;
	char buffer[10];
	str num;
	
	bzero(buffer, 10);
	printf("Waiting for message...\n");
	n = read(ConnectFD, buffer, 10);
	buffer[n] = '\0';
	num = buffer;
	n = stoi(num);
	write(ConnectFD, "ACK.", 4);
	printf("Opening connection at port %d\n", n);
	//open_another_port(n);
	std::thread t_algo(open_another_port, n); //opening other port
	t_algo.detach();
	
	std::this_thread::sleep_for( std::chrono::milliseconds(500) );
	
	printf("First port FD: %d\n", ConnectFD);
	
	std::thread t1(read_acks, ConnectFD);
	t1.detach();
	
	do{		
		//SENDS ACKS
		while (!ACK);
		printf("Sending ACK\n");
		write(ConnectFD, "ACK.", 4);
		ACK = false;
	} while(!end);
	
}

void parse_put_get(int ConnectFD){
	int n;
	char buffer[1000];
	str s;
	
	printf("Second port FD: %d\n", ConnectFD);
	
	do{
		bzero(buffer, 1000);
		printf("Waiting for message...\n");
		n = read(ConnectFD, buffer, 1000);
		
		s = buffer;
		
		mensaje_recibido(s, ConnectFD);
		
		//write(ConnectFD, "ACK.", 4);
	} while (!end);
}

void get_func(str file, int ConnectFD){
	str res = read_txt(file);
	write(ConnectFD, res.c_str(), res.size());
}

void put_func(str s){
	
	//2 archivo.txt mensaje del archivo
	
	int i=0;
	while(s[i] != ' '){
		++i;
	}
	str file_name = s.substr(0, i);
	str put = s.substr(i+1, s.size()-i-1);
	
	std::ofstream file;
	file.open(file_name);
	file << put;
	file.close();
}

void mensaje_recibido(str s, int ConnectFD){  // interpretar comandos 
	
	std::cout << "Recieved: " << s << std::endl;
	
	switch(s[0]){
		case '1':{ //GET
			str file = s.substr(2,s.size()-2);
			get_func(file, ConnectFD);
			break;
		}
		
		case '2':{ //PUT
			str file = s.substr(2,s.size()-2);
			put_func(file);
			break;
		}
			
		case '3':{ //CLOSE
			end = true;
			break;
		}
	}
	
	ACK = true;
	
}

void open_another_port(int port){
	struct sockaddr_in stSockAddr;
	int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	int Res;
	
	if(-1 == SocketFD)
	{
		perror("can not create socket");
		exit(EXIT_FAILURE);
	}
	
	printf("Socket created, fd: %d\n", SocketFD);
	
	memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
	
	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(port);
	stSockAddr.sin_addr.s_addr = INADDR_ANY;
	
	if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
	{
		perror("error bind failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}
	
	printf("Bind\n");
	
	if(-1 == listen(SocketFD, 10))
	{
		perror("error listen failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}
	
	printf("Listen\n");
	
	int Client_connectFD = accept(SocketFD, NULL, NULL);
	
	if(0 > Client_connectFD){
		perror("error accept failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}
	
	printf("Connection at port %d made.\n", port);
	
	/*std::thread t1(parse_put_get, Client_connectFD);
	t1.detach();
	
	while (!end);*/
	
	parse_put_get(Client_connectFD);
	
	shutdown(Client_connectFD, SHUT_RDWR);
	
	close(Client_connectFD);
}

int main(void){
    struct sockaddr_in stSockAddr;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    
	int Res;
 
    if(-1 == SocketFD)
    {
    	perror("can not create socket");
    	exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
	
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(port);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;
 
    if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
    	perror("error bind failed");
    	close(SocketFD);
      	exit(EXIT_FAILURE);
    }
 
    if(-1 == listen(SocketFD, 10))
    {
      	perror("error listen failed");
      	close(SocketFD);
      	exit(EXIT_FAILURE);
    }
	int Client_connectFD;
    //for(;;){
      	Client_connectFD = accept(SocketFD, NULL, NULL);
		
		printf("Connection made\n");
 
      	if(0 > Client_connectFD){
      		perror("error accept failed");
      		close(SocketFD);
      		exit(EXIT_FAILURE);
      	}
 
     /* perform read write operations ... */
		std::thread t1(recieve_msg, Client_connectFD);
		t1.detach();
		
		while(!end);
 
      shutdown(Client_connectFD, SHUT_RDWR);
 
      close(Client_connectFD);
   // }
 
    close(SocketFD);
	printf("Closed both sockets\n");
    return 0;
  }
