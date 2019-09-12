/* Server code in C++ */

//g++ server.cpp -o server -std=c++11 -lpthread

/*
	rules:
	send 1 to confirm connection and play
	send 2 position (1-9) to put your play
	send 3 to get the meaning of each position
*/

/*
	integrantes:
	-Jazmine Alexandra Alfaro Llerena
	-Renato Luis Postigo Avalos
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

typedef std::string str;

int SocketFD, Client_connectFD;
int port = 50000;
char msg[256];
std::list<int> connections;

std::pair<int, int> match;

str tablero = "   |   |   \n---|---|---\n   |   |   \n---|---|---\n   |   |   \n";
str tablero_inst = " 1 | 2 | 3 \n---|---|---\n 4 | 5 | 6 \n---|---|---\n 7 | 8 | 9 \n";
bool last_mov = 0; //0 -> le toca 'X', 1-> le toca a 'O'

/*tablero =  "   |   |   \n";
tablero += "---|---|---\n";
tablero += "   |   |   \n";
tablero += "---|---|---\n";
tablero += "   |   |   \n";*/

void send_tablero(int ConnectFD){
	int n = write(ConnectFD, tablero.c_str(), tablero.size());
}

void send_instr_tablero(int ConnectFD){
	int n = write(ConnectFD, tablero_inst.c_str(), tablero_inst.size());
}

void confirm_registration(int ConnectFD){
	str msg = "Registration confirmed. You're user " + std::to_string(ConnectFD) + ".";
	int n = write(ConnectFD, msg.c_str(), msg.size());
	connections.push_back(ConnectFD);
	n = write(ConnectFD, "Waiting for an opponent...", 26);
	if (connections.size() == 2){ //building match
		int opponent;
		msg = "Opponent found. You're playing id" + std::to_string(connections.front()) + ".\n";
		n = write(ConnectFD, msg.c_str(), msg.size());
		match.first = connections.front();
		match.second = ConnectFD;
		msg = "Opponent found. You're playing id" + std::to_string(ConnectFD) + ".";
		n = write(connections.front(), msg.c_str(), msg.size());
		
		n = write(match.first, "Your turn. (Type 2 pos(1-9), example: 2 4).\n",44); 
		
		send_instr_tablero(match.first);
		send_instr_tablero(match.second);		
		/*send_tablero(match.first);
		send_tablero(match.second);*/
	}
}

bool did_win(){
	char symbol = 'x';
	//checking x
	if (tablero[1] == symbol && tablero[5] == symbol && tablero[9] == symbol 
		|| tablero[33] == symbol && tablero[29] == symbol && tablero[33] == symbol
		|| tablero[49] == symbol && tablero[53] == symbol && tablero[57] == symbol
		|| tablero[1] == symbol && tablero[25] == symbol && tablero[49] == symbol
		|| tablero[5] == symbol && tablero[29] == symbol && tablero[53] == symbol
		|| tablero[9] == symbol && tablero[33] == symbol && tablero[57] == symbol
		|| tablero[1] == symbol && tablero[29] == symbol && tablero[57] == symbol
		|| tablero[9] == symbol && tablero[29] == symbol && tablero[49] == symbol){
		return 1;
	}
	symbol = 'o';
	if (tablero[1] == symbol && tablero[5] == symbol && tablero[9] == symbol 
		|| tablero[33] == symbol && tablero[29] == symbol && tablero[33] == symbol
		|| tablero[49] == symbol && tablero[53] == symbol && tablero[57] == symbol
		|| tablero[1] == symbol && tablero[25] == symbol && tablero[49] == symbol
		|| tablero[5] == symbol && tablero[29] == symbol && tablero[53] == symbol
		|| tablero[9] == symbol && tablero[33] == symbol && tablero[57] == symbol
		|| tablero[1] == symbol && tablero[29] == symbol && tablero[57] == symbol
		|| tablero[9] == symbol && tablero[29] == symbol && tablero[49] == symbol){
		return 1;
	}
	return 0;
}

void parse_play(char* message, int ConnectFD){
	
	int n;
	if ((last_mov && ConnectFD == match.first) || !last_mov && ConnectFD == match.second){
		n = write(ConnectFD,"Not your turn, please wait.", 27);
		return;
	}
	char num = message[2];
	int posicion = atoi(&num);
	char simbolo;
	if(ConnectFD == match.first) simbolo = 'x';
	else simbolo = 'o';
	
	switch(posicion){
	case 1: {
		tablero[1] = simbolo;
		break;}
	case 2 : {
		tablero[5] = simbolo;
		break;}
	case 3 : {
		tablero[9] = simbolo;
		break;}
	case 4 : {
		tablero[25] = simbolo;
		break;}
	case 5 : {
		tablero[29] = simbolo;
		break;}
	case 6 : {
		tablero[33] = simbolo;
		break;}
	case 7 : {
		tablero[37+12] = simbolo;
		break;}
	case 8 : {
		tablero[41+12] = simbolo;
		break;}
	case 9 : {
		tablero[46+11] = simbolo;
		break;}
	}
	n = write(ConnectFD, "Nice play!\n", 11);
	send_tablero(match.first);
	send_tablero(match.second);
	if (ConnectFD == match.first){
		n = write(match.second, "Your turn\n", 10);
		last_mov = 1;
	} else {
		n = write(match.first, "Your turn\n", 10);
		last_mov = 0;
	}
	if (did_win()){
		if (last_mov){
			n = write(match.first, "YOU WIN!!", 9);
			n = write(match.second, "YOU SUCK!!", 10);
		} else {
			n = write(match.second, "YOU WIN!!", 9);
			n = write(match.first, "YOU SUCK!!", 10);
		}
		shutdown(match.first, SHUT_RDWR);
		close(match.first);
		shutdown(match.second, SHUT_RDWR);
		close(match.second);
	}
	
}

void rcv_msg(int ConnectFD){
	bool end = false;
	char buffer[256];
	str tablero;
	str opt_c;
	int n, opt;
	do{
		bzero(buffer,256);
		n = read(ConnectFD,buffer,255);
		if (n < 0) perror("ERROR reading from socket");
		opt_c = buffer[0];
		opt = atoi(opt_c.c_str());
		switch (opt){
		case 1:
			confirm_registration(ConnectFD);
			printf("id stored\n");
			break;
		case 2:
			parse_play(buffer, ConnectFD);
			break;
		case 3:
			send_instr_tablero(ConnectFD);
			break;
		}
	}while ( !end );
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
		t1.detach();
 
    }
 
    close(SocketFD);
    return 0;
  }
