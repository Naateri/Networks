/* Server code in C++ */

/*

Server code programmed by:
-Jazmine Alexandra Alfaro Llerena
-Renato Luis Postigo Avalos

*/

//g++ game.cpp -o server -std=c++11 -lpthread


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
#include <vector>
#include <time.h>
#include <chrono>

using namespace std;
//recieve message parses the message recieved

typedef std::string str;

//ahnmm...

char p[100][100];
char tab_copy[100][100];

int SocketFD, Client_connectFD;
//char msg[256];

char player_symbols[] = {'@', '?', '$', '&', '!', '%', '|', '°'};

char fresita = '*';

int cur_players = 0;

int port = 42800;

int idle_time = 2; //idle time in seconds 

bool fresa_active = false;
bool display_fresas = false;
bool now_playing = false;

bool straight_to_gameplay = false; //change to false when wanting to try registering
//change to true if you just want to play

bool end_for_everybody = false; //when max_score limit is hit

int max_score = 100;

class Player{
public:
	int x,y;
	int score = 0;
	str name;
	char symbol;
	int ConnectFD;
	str password;
	bool status = 0;
	bool logged_in = 0;
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;
};

std::vector<Player*> players;

void draw_map(int);

void rcv_msg(int ConnectFD){
	bool end = false;
	char buffer[2];
	str opt_c;
	int n, opt;
	
	///STILL ON MENU
	
	bool cur_user_logged_in = false;
	while (!cur_user_logged_in){
		printf("Not logged in\n");
		sleep(1);
		for(int i = 0; i < players.size(); i++){
			if (players[i]->ConnectFD == ConnectFD
				&& players[i]->logged_in == true){
				cur_user_logged_in = true;
				break;
			}
		}
	}
	
	///Already logged in
	
	do{
		bzero(buffer,2);
		n = read(ConnectFD,buffer,2);
		printf("Message from %d: %s\n", ConnectFD, buffer);
		
		char key = buffer[0];
		
		Player* pl;
		int i = 0;
		for(i; i < players.size(); i++){
			if (players[i]->ConnectFD == ConnectFD){
				pl = players[i];
				break;
			}
		}
		
		if (pl->status == false || pl->logged_in == false) continue;
		if (key == 'w'){
			pl->x--;
		} else if (key == 's'){
			pl->x++;
		} else if (key == 'a'){
			pl->y--;
		} else if (key == 'd'){
			pl->y++;
		} else if (key == 'q'){
			pl->logged_in = 0;
			p[pl->x][pl->y] = tab_copy[pl->x][pl->y];
			players.erase(players.begin() + i);
			end = true;
		}
		
	}while ( !end );
}

	
void generate_fresa(){
	int max_x = 24;
	int max_y = 60;
	int pos_x, pos_y ; 
	
	srand(time(0));

	pos_x = rand()%max_x;	
	pos_y = rand()%max_y;
	
	char actual = p[pos_x][pos_y];
	
	while(actual != ' '){
		pos_x = rand()%max_x;	
		pos_y = rand()%max_y;
		actual = p[pos_x][pos_y];
	}
	
	p[pos_x][pos_y] = fresita;
	
}
	
void fresas(int num){
	for(int i=0;i<num;++i){
		generate_fresa();
	}
}

string separar(string s1){
	str username;
	
	int i = 0;
	while (s1[i] != ' '){
		username += s1[i++];
	}
	return username;
}
	
	
void menu(int SocketFD){
	int n;
	char buffer[256];
	bool user_logged = false; 
	str message;
	do {
		message = "Press key accordingly.\n";
		message += "1. Register.\n";
		message += "2. Login.\n";
		n = write(SocketFD,message.c_str(),message.size()+1); //cuantos bytes estoy mandando
		
		bzero(buffer,256);
		printf("Waiting for message...\n");
		n = read(SocketFD,buffer,255);
		printf("Message from %d: %s\n", SocketFD, buffer);
		//int key = atoi(buffer);
		char key = buffer[0];
	
		if (key == '1'){ //register user
			//register_user(SocketFD);
			Player* pl;
			message = "REGISTER\nSend as follows:\n";
			message += "Username space Password\n";
			n = write(SocketFD,message.c_str(),message.size()); //cuantos bytes estoy mandando
			
			bzero(buffer,256);
			n = read(SocketFD,buffer,255);
			printf("Message from %d: %s\n", SocketFD, buffer);
			
			buffer[n] = '\0';
			
			//look if username is repeated
			bool nuevo_nombre = false;
			do{
				int nn;
				string s1(buffer);
				string user = separar(s1);
				
				if(players.size() == 0){
					nuevo_nombre = true;
					break;
				}
				
				for(int i=0;i<players.size();++i){
					if(players[i]->name == user){
						string o = "Invalid username. Please try again. \n";
						nn = write(SocketFD,o.c_str(),o.size());
						bzero(buffer,256);
						n = read(SocketFD,buffer,255);
						i=0;
						//break;
					}
					if(i==players.size()-1){
						string o = "Valid username. \n";
						nn = write(SocketFD,o.c_str(),o.size());
						nuevo_nombre = true;
						break;
					}
				}	
				
			}while(!nuevo_nombre);
			
			
			
			//look if FD is already used
			bool r = true; // saber si ya se conecto
			for(int i=0;i<players.size();++i){
				
				if(players[i]->ConnectFD == SocketFD){
					
					int a;
					string o = "You already registered. Your username is :"+ players[i]->name+" \n";
					a = write(SocketFD,o.c_str(),o.size());
					r=false;
				}
			}
			
			bool num = true;
			if(players.size()==8){
				num = false;
			}
			
			
			if(r && num){
				str full_data(buffer);
				str username, password;
				
				int i = 0;
				while (buffer[i] != ' '){
					username += buffer[i++];
				}
				
				while (buffer[i] != '\0'){
					password += buffer[i++];
				}
				
				pl = new Player;
				pl->name = username;
				
				//other useful data
				pl->ConnectFD = SocketFD; //logging in does this
				pl->x = 10; pl->y = 10;
				pl->symbol = player_symbols[cur_players];
				cur_players++;
				
				pl->password = password;
				
				players.push_back(pl);
				
				printf("Nickname stored\n");
				
				message = "Nickname succesfully stored.\n";
				n = write(SocketFD,message.c_str(),message.size()); //cuantos bytes estoy mandando
			}
		} else if (key == '2'){ //logging user
			//login_user(SocketFD);
			
			printf("Logging in\n");
			
			message = "LOGIN\nSend as follows:\n";
			message += "Username space Password\n";
			n = write(SocketFD,message.c_str(),message.size()); //cuantos bytes estoy mandando
			
			bzero(buffer,256);
			n = read(SocketFD,buffer,255);
			printf("Message from %d: %s\n", SocketFD, buffer);
			
			buffer[n] = '\0';
			
			str full_data(buffer);
			str username, password;
			
			int i = 0;
			while (buffer[i] != ' '){
				username += buffer[i++];
			}
			
			while (buffer[i] != '\0'){
				password += buffer[i++];
			}
			
			for(int i = 0; i < players.size(); i++){
				if (players[i]->name == username){
					if (players[i]->password == password){
						message = "Logging in successful.\n";
						n = write(SocketFD, message.c_str(), message.size());
						players[i]->ConnectFD = SocketFD;
						players[i]->logged_in = true;
						players[i]->status = 1;
						now_playing = true;
						user_logged = true;
						sleep(3);
						//draw_map(SocketFD);				
						return;
					}
				}
			}
			
			message = "Logging in unsuccessful\n";
			n = write(SocketFD, message.c_str(), message.size());
		} else { 
			message = "Try again.\n";
			n = write(SocketFD, message.c_str(), message.size());
			//menu(SocketFD);
		}
	
	} while (!user_logged);
}

void real_draw_map(){
	int c,f,x=0, n;	
	
	std::string message;
	strcpy(p[x++],"||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");
	strcpy(p[x++],"||||||||||||||||||||||||||||||||  |||||||||        |||||||||");
	strcpy(p[x++],"|||||| ||||||||||||||||||||||||    ||||||||        |||||||||");
	strcpy(p[x++],"|||||   ||||||||||||||||||||||      ||||||||        ||||||||");
	strcpy(p[x++],"||||     ||||||||||||||| ||||        |||||||||       ||  | |");
	strcpy(p[x++],"|||      ||||||||| ||||   |||         |||||||            |  ");
	strcpy(p[x++],"||        ||||||    ||    |||         ||||||                ");
	strcpy(p[x++],"          |||||            |          ||||||                ");
	strcpy(p[x++],"           |||                         ||||                 ");
	strcpy(p[x++],"           ||                          ||||                 ");
	strcpy(p[x++],"                                        ||                  ");
	strcpy(p[x++],"                                        ||                  ");
	strcpy(p[x++],"                                                            ");
	strcpy(p[x++],"                                ||                          ");
	strcpy(p[x++],"    ||                         |##|                         ");
	strcpy(p[x++],"   |##|                       |####|            |     |     ");
	strcpy(p[x++],"   |##|                 |    |######|          |#|||||#|    ");
	strcpy(p[x++],"  |####|         ||    |#|   |######|         |#########|   ");
	strcpy(p[x++]," |######|       |##|  |##|  |########|        |#########|   ");
	strcpy(p[x++],"|########|     |####||####| |########|       |###########|  ");
	strcpy(p[x++],"##########|   |###########||###########|     |###########|  ");
	strcpy(p[x++],"##########|  |#########################|     |###########|  ");
	strcpy(p[x++],"###########||###########################|  |################");
	strcpy(p[x++],"########################################|  |################");
	
	int SocketFD;
	
	do{
		message.clear();
		
		if(!display_fresas){
			fresas(5);
			display_fresas = true;
			fresa_active = true;
		}
		
		if (!fresa_active){
			fresas(1);
			fresa_active = true;
		}
		
		for(f=0;f<=24;f++){
			p[f][60]=p[f][0];
		}
		
		for(f=0;f<=24;f++){
			for(c=0;c<=60;c++){
				p[f][c]=p[f][c+1];
			}
		}		
		
		for(f=0; f<=24; f++){
			strcpy(tab_copy[f], p[f]);
		}
		
		for (int i = 0; i < players.size(); i++){
			
			Player* cur_player = players[i];
			char actual = p[cur_player->x][cur_player->y];  
			
			if((cur_player->status== true) && (actual == '|' || actual == '#' || actual == '_')){//colisiona
				cur_player->status = false;
				cur_player->begin = std::chrono::steady_clock::now();
			}
			
			if((actual == fresita) && (fresa_active) && cur_player->status == true ){//agarra fresa
				tab_copy[cur_player->x][cur_player->y] = ' ';
				cur_player->score +=2;
				fresa_active = false;
			}
			
			if (cur_player->status== true && cur_player->logged_in == true){
				p[cur_player->x][cur_player->y] = cur_player->symbol;
				//printf("Symbol updated\n");
			} else {
				if (cur_player->logged_in == true){
					cur_player->end = std::chrono::steady_clock::now();
					std::chrono::duration<double> elapsed_seconds = cur_player->end - cur_player->begin;
					//int time = std::chrono::duration_cast<std::chrono::seconds>(end-begin).count();
					double time = elapsed_seconds.count();
					printf("time %d %f\n", cur_player->ConnectFD, time);
					if (time >= idle_time) cur_player->status = true;
				}
			}
			
			if (cur_player->score >= max_score) end_for_everybody = true;
		}
		
		//////////ERASE FROM HERE//////////
		for(f=0;f<=24;f++){
			message = p[f];
			message += '\n';
			for (int i = 0; i < players.size(); i++){
				if (players[i]->logged_in == true){
					SocketFD = players[i]->ConnectFD;
					n = write(SocketFD,message.c_str(),message.size()); //cuantos bytes estoy mandando
				}
			}
		}
		
		message = "Scores:\n";// + to_string(cur_player->score);
		for (int i = 0; i < players.size(); i++){
			message += players[i]->name + ' ' + players[i]->symbol + ' ' + to_string(players[i]->score) + '\n';
		}
		
		for(int i = 0; i < players.size(); i++){
			if (players[i]->logged_in == true){
				SocketFD = players[i]->ConnectFD;
				n = write(SocketFD,message.c_str(),message.size());
			}
		}
		
		/////////ERASE UNTIL HERE/////////
		
		for(f=0; f<=24; f++){
			strcpy(p[f], tab_copy[f]);
		}
		
		
		//usleep(90000*(players.size()+1)); 
		usleep(100000);
		
	} while(!end_for_everybody);
	
	str winner_name;
	for(int i = 0; i < players.size(); i++){
		if (players[i]->score >= max_score){
			winner_name = players[i]->name;
			break;
		}
	}
	
	for(int i = 0; i < players.size(); i++){
		message = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nCONGRATULATIONS " + winner_name + ", YOU HAVE WON THE MATCH!\n";
		SocketFD = players[i]->ConnectFD;
		n = write(SocketFD, message.c_str(), message.size());
		
		message = "End game";
		n = write(SocketFD, message.c_str(), message.size());
	}
	
}

int main(void){
	struct sockaddr_in stSockAddr;
	SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	int Res;
	
	//draw_map();
	
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
	
	std::thread t3(real_draw_map);
	t3.detach();
	//std::thread t2(draw_map, 0);
	//t2.detach();
	
	for(;;){
		Client_connectFD = accept(SocketFD, NULL, NULL);
		
		if(0 > Client_connectFD){
			perror("error accept failed");
			close(SocketFD);
			exit(EXIT_FAILURE);
		}
		
		if (straight_to_gameplay){
			Player* p = new Player;
			p->ConnectFD = Client_connectFD;
			p->x = 10; p->y = 10;
			p->symbol = player_symbols[cur_players];
			p->status = 1;
			p->logged_in = true;
			cur_players++;
			players.push_back(p);
			//std::thread t2(draw_map, Client_connectFD);
			//t2.detach();
		}
		
		/* perform read write operations ... */
		else {
			//menu(Client_connectFD);
			std::thread temp_thread(menu, Client_connectFD);
			temp_thread.detach();
			//sleep(2);
		}
		
		std::thread t1(rcv_msg, Client_connectFD);
		//std::thread t2(send_msg);
		t1.detach();
		
	}
	
	shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);
	return 0;
}
