/* Server code in C++ */

//g++ server.cpp -o server -std=c++11 -lpthread
 
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

//recieve message parses the message recieved

typedef std::string str;
typedef std::map<str, int> socket_nickname;

socket_nickname nicknames;

#define nickname 1
#define list 2
#define le_message 3
#define disconnect 8

#define server_list 6
#define server_to_client 7

int SocketFD, Client_connectFD;
char msg[256];

void store_nickname(char* msg, int socketFD){ //1 len_nickname nick_name
	str length, nickn;
	int real_len, n;
	for (auto& x: nicknames) {
		if (socketFD == x.second){
			std::cout << "This socket already has a nickname registered.\n";
			n = write(socketFD, "Your socket has already registered a nickname. Try again.", 57);
			return;
		}
	}
	length = msg[2];
	length += msg[3];
	real_len = atoi(length.c_str());
	std::cout << real_len << std::endl;
	for(int i = 5; i < 5 + real_len; i++){
		nickn += msg[i];
	}
	socket_nickname::iterator it = nicknames.find(nickn);
	if (it != nicknames.end()){
		n = write(socketFD, "This nickname is already registered. Try again.", 47);
		return;
	}
	nicknames.insert(std::pair<str, int>(nickn, socketFD));
	str message = "Nickname " + nickn + " stored.";
	n = write(socketFD, message.c_str(), message.size());
}

void print_nicknames(int ClientFD){ // 2, also sends them
	str cur_nicknames;
	for (auto& x: nicknames) {
		std::cout << "nickname: " << x.first << ", socketFD: " << x.second << '\n';
		cur_nicknames += x.first;
		cur_nicknames += ", ";
	}
	int n = write(ClientFD, cur_nicknames.c_str(), cur_nicknames.size());
}

void send_msg_client(char* msg, int ConnectFD){
	//3 nick_size reciever msg_size msg
	//eg: 06 donald 004 hola
    int n;
	str nick_size, nick, msg_size, msge, sender;
	int nick_sz, msg_sz;
	nick_size = msg[2];
	nick_size += msg[3];
	nick_sz = atoi(nick_size.c_str()); //nick size
	int i = 5, k;
	//getting nickname
	for(; i < 5 + nick_sz; i++){
		nick += msg[i];
	}
	i++; //space
	for(int j = 0; j < 3; j++){
		msg_size += msg[i++];
	}
	i++; //space
	k = i;
	msg_sz = atoi(msg_size.c_str());
	
	//finding who's the sender
	for(auto& x: nicknames){
		if (x.second == ConnectFD){
			sender = x.first;
			break;
		}
	}
	
	//getting message
	msge = sender + "says: ";
	for(; i < k + msg_sz; i++){
		msge += msg[i];
	}
	n = write(nicknames[nick],msge.c_str(),msge.size());
	if (n < 0) perror("ERROR writing to socket");
	std::cout << "Sending message " << msge << " to " << nick << std::endl;
	n = write(ConnectFD, "Message sent.", 14); //OK
}

void end_connection(int ConnectFD, bool& end){
	int n = write(ConnectFD, "Ok. Disconnecting from server.", 31);
	if (n < 0) perror("ERROR writing to socket");
	shutdown(ConnectFD, SHUT_RDWR);
	close(ConnectFD);
	socket_nickname::iterator it;
	for (it = nicknames.begin();
			it != nicknames.end(); it++){ //deleting from the map
		if (it->second == ConnectFD){
			break;
		}
	}
	nicknames.erase(it);
	end = true;
}

void rcv_msg(int ConnectFD){
	bool end = false;
	char buffer[256];
	str opt_c;
    int n, opt;
	do{
	bzero(buffer,256);
    n = read(ConnectFD,buffer,255);
    if (n < 0) perror("ERROR reading from socket");
	opt_c = buffer[0];
	opt = atoi(opt_c.c_str());
	printf("Client: [%s]\n",buffer);
	switch (opt){
		case nickname:
			store_nickname(buffer, ConnectFD);
			printf("Nickname stored\n");
			break;
		case list:
			printf("Retrieving nicknames\n");
			print_nicknames(ConnectFD);
			break;
		case le_message:
			send_msg_client(buffer, ConnectFD);
			break;
		case disconnect:
			end_connection(ConnectFD, end);
			break;
	}
	}while ( !end );
}

void send_list(char* msg){
	str list_size, le_list;
	int list_sz;
	list_size = msg[2];
	list_size += msg[3];
	list_size += msg[4];
	
	list_sz = atoi(list_size.c_str());
	
}

void server_send_msg(char* msg){
	int n;
	str nick_size, nick, msg_size, msge, sender;
	int nick_sz, msg_sz;
	nick_size = msg[2];
	nick_size += msg[3];
	nick_sz = atoi(nick_size.c_str()); //nick size
	int i = 5, k;
	//getting nickname
	for(; i < 5 + nick_sz; i++){
		nick += msg[i];
	}
	i++; //space
	for(int j = 0; j < 3; j++){
		msg_size += msg[i++];
	}
	i++; //space
	k = i;
	msg_sz = atoi(msg_size.c_str());
	
	//finding who's the sender
	
	//getting message
	msge = "server says: ";
	for(; i < k + msg_sz; i++){
		msge += msg[i];
	}
	n = write(nicknames[nick],msge.c_str(),msge.size());
	if (n < 0) perror("ERROR writing to socket");
	std::cout << "Sending message " << msge << " to " << nick << std::endl;
}

void send_msg(){
	char msg[256];
	int n;
	do{
		std::cin.getline(msg, 255);
		str opt_write;
		int option;
		opt_write = msg[0];
		option = atoi(opt_write.c_str());
		
		switch (option){
			case server_list:
				send_list(msg);
				break;
			case server_to_client:
				server_send_msg(msg);
				break;
		}
		msg[n] = '\0';
	} while(1);
}

int main(void){
    struct sockaddr_in stSockAddr;
    SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    
	int Res;
 
    if(-1 == SocketFD)
    {
    	perror("can not create socket");
    	exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
	
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(45500);
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
	
	/*std::thread write_msg(send_msg);
	write_msg.join();*/
	
    for(;;){
      	Client_connectFD = accept(SocketFD, NULL, NULL);
 
      	if(0 > Client_connectFD){
      		perror("error accept failed");
      		close(SocketFD);
      		exit(EXIT_FAILURE);
      	}
 
     /* perform read write operations ... */
		std::thread t1(rcv_msg, Client_connectFD);
     	//std::thread t2(send_msg);
		t1.detach();
		//t2.detach();
 
      //shutdown(ConnectFD, SHUT_RDWR);
 
      //close(ConnectFD);
    }
 
    close(SocketFD);
    return 0;
  }
