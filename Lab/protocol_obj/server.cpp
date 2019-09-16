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

str professions[] = {"Computer Science", "Lawyer", "Electrical Engineer", "Civil Engineer", "Teacher"};

int SocketFD, Client_connectFD;
char msg[256];
int port = 45000;

struct User{
	char name[99];
	char inicial[99];
	char surname[999];
	char DOB[99];
	char address[999];
	char profession[99];
	char bio[999];
	char photo[300000];
	unsigned int DNI;
	
	void print(){
		printf("Name: %s %s %s\n", this->name, this->inicial, this->surname);
		printf("DOB: %s\n", this->DOB);
		printf("Address: %s\n", this->address);
		printf("Profession: %s\n", this->profession);
		printf("Bio: %s\n", this->bio);
	}
};

void rcv_msg(int ConnectFD){ //struct
	User my_user;
	recv(ConnectFD, &my_user, sizeof(my_user), 0);
	my_user.print();
}

str get_type(char* message, int& index, str& value){
	str type;
	value.clear();
	while(message[index] != ' '){
		type += message[index++];
	}
	index++; //out of space
	
	while (message[index] != '\n'){
		value += message[index++];
	}
	index++; //new line or message[index] = '\0'
	return type;
}

void rcv_msg_comandos(int ConnectFD){
	char buffer[303393];
	bzero(buffer, 303393);
	str type, value;
	User my_user;
	int n = read(ConnectFD, buffer, 303393);
	std::cout << "message: " << buffer << std::endl;
	buffer[n] = '\0';
	int i = 0;
	while(buffer[i] != '\0'){
		type = get_type(buffer, i, value);
		if (type == "address") 
			strcpy(my_user.address, value.c_str());
		else if (type == "bio")
			strcpy(my_user.bio, value.c_str());
		else if (type == "dni")
			my_user.DNI = stoi(value);
		else if (type == "initial")
			strcpy(my_user.inicial, value.c_str());
		else if (type == "dob")
			strcpy(my_user.DOB, value.c_str());
		else if (type == "name")
			strcpy(my_user.name, value.c_str());
		else if (type == "photo")
			strcpy(my_user.photo, value.c_str());
		else if (type == "profession")
			strcpy(my_user.profession, value.c_str());
		else if (type == "surname")
			strcpy(my_user.surname, value.c_str());			
	}
	my_user.print();
}

str get_JSON_type(char* message, int& index, str& value){
	str type;
	value.clear();
	if (message[index] == '}'){
		return "";
	}
	if (message[index] == '"') index++;
	while(message[index] != '"'){
		type += message[index++];
	}
	
	index++; //after '"'
	index++; //index after ':'
	index++; //index after '"'
	
	while (message[index] != '"'){
		value += message[index++];
	}
	
	index++; //message[index] = ','
	index++; //new type or message[index] = '}'
	return type;
}


void rcv_msg_JSON(int ConnectFD){
	char buffer[303393];
	bzero(buffer, 303393);
	str type, value;
	User my_user;
	int n = read(ConnectFD, buffer, 303393);
	buffer[n] = '\0';
	int i = 1;
	while(buffer[i-1] != '}'){
		type = get_JSON_type(buffer, i, value);
		if (type == "address") 
			strcpy(my_user.address, value.c_str());
		else if (type == "bio")
			strcpy(my_user.bio, value.c_str());
		else if (type == "dni")
			my_user.DNI = stoi(value);
		else if (type == "initial")
			strcpy(my_user.inicial, value.c_str());
		else if (type == "dob")
			strcpy(my_user.DOB, value.c_str());
		else if (type == "name")
			strcpy(my_user.name, value.c_str());
		else if (type == "photo")
			strcpy(my_user.photo, value.c_str());
		else if (type == "profession")
			strcpy(my_user.profession, value.c_str());
		else if (type == "surname")
			strcpy(my_user.surname, value.c_str());			
	}
	my_user.print();
}

str get_XML_type(char* message, int& index, str& value){
	str type;
	value.clear();
	if (message[index] == '<') index++;
	while(message[index] != '>'){
		type += message[index++];
	}
	
	index++;
	
	if (message[index] == '\n'){
		index++;
		return type;
	}
	
	while (message[index] != '<'){
		value += message[index++];
	}
	
	while (message[index++] != '>'); //closing brackets
	index++;
	
	return type;
}

void rcv_msg_XML(int ConnectFD){
	char buffer[303393];
	bzero(buffer, 303393);
	str type, value;
	User my_user;
	int n = read(ConnectFD, buffer, 303393);
	buffer[n] = '\0';
	int i = 0;
	while(buffer[i] != '\0'){
		type = get_XML_type(buffer, i, value);
		//std::cout << type << ' ' << value << std::endl;
		if (type == "address") 
			strcpy(my_user.address, value.c_str());
		else if (type == "bio")
			strcpy(my_user.bio, value.c_str());
		else if (type == "dni")
			my_user.DNI = stoi(value);
		else if (type == "initial")
			strcpy(my_user.inicial, value.c_str());
		else if (type == "dob")
			strcpy(my_user.DOB, value.c_str());
		else if (type == "name")
			strcpy(my_user.name, value.c_str());
		else if (type == "photo")
			strcpy(my_user.photo, value.c_str());
		else if (type == "profession")
			strcpy(my_user.profession, value.c_str());
		else if (type == "surname")
			strcpy(my_user.surname, value.c_str());			
	}
	my_user.print();
}

void rcv_msg_csv(int ConnectFD){
	//name,initial,surname,dob,dni,address,profession,bio,photo
	char buffer[303393];
	bzero(buffer, 303393);
	str value;
	User my_user;
	int n = read(ConnectFD, buffer, 303393);
	buffer[n] = '\0';
	int cur_iteration = 0, i = 0;
	while(buffer[i] != '\0'){
		value.clear();
		while(buffer[i] != ','){
			value += buffer[i++];
		}
		i++;
		if (cur_iteration == 0)
			strcpy(my_user.name, value.c_str());
		else if (cur_iteration == 1)
			strcpy(my_user.inicial, value.c_str());
		else if (cur_iteration == 2)
			strcpy(my_user.surname, value.c_str());
		else if (cur_iteration == 3)
			strcpy(my_user.DOB, value.c_str());
		else if (cur_iteration == 4)
			my_user.DNI = stoi(value);
		else if (cur_iteration == 5)
			strcpy(my_user.address, value.c_str());
		else if (cur_iteration == 6)
			strcpy(my_user.profession, value.c_str());
		else if (cur_iteration == 7)
			strcpy(my_user.bio, value.c_str());
		else
			strcpy(my_user.photo, value.c_str());
		cur_iteration++;
	}
	my_user.print();
}

void rcv_msg_estructuras(int ConnectFD){
	char buffer[303393];
	bzero(buffer, 303393);
	str value, size_str;
	User my_user;
	int n = read(ConnectFD, buffer, 303393);
	buffer[n] = '\0';
	int cur_iteration = 0, i = 0, j, cur_size, size;
	while(buffer[i] != '\0'){
		value.clear();
		cur_size = i;
		//i++;
		if (cur_iteration == 0){
			size_str = buffer[i];
			size_str += buffer[i+1];
			size_str += buffer[i+2];
			size = stoi(size_str);
			i += 3;
			if (buffer[i] == ' ') i++;
			for(j = i; j < (i + size); j++){
				value += buffer[j];
			}
			strcpy(my_user.name, value.c_str());
			i = j;
			i++; //newline
		}
		else if (cur_iteration == 1){
			size_str = buffer[i];
			size_str += buffer[i+1];
			
			size = stoi(size_str);
			i += 2;
			if (buffer[i] == ' ') i++;
			for(j = i; j < (i + size); j++){
				value += buffer[j];
			}
			strcpy(my_user.inicial, value.c_str());
			i = j;
			i++; //newline
		}
		else if (cur_iteration == 2){
			size_str = buffer[i];
			size_str += buffer[i+1];
			size_str += buffer[i+2];
			
			size = stoi(size_str);
			i += 3;
			if (buffer[i] == ' ') i++;
			for(j = i; j < (i + size); j++){
				value += buffer[j];
			}
			
			i = j;
			strcpy(my_user.surname, value.c_str());
			i++; //newline
		}
		else if (cur_iteration == 3){
			for(j = i+2; j < (i + 8); j++){
				value += buffer[j];
			}
			
			strcpy(my_user.DOB, value.c_str());
			i = j;
			i++;
		}
		else if (cur_iteration == 4){
			for (j = i; j < (i + 8); j++){
				value += buffer[j];
			}
			
			my_user.DNI = stoi(value);
			i = j;
			i++;
		}
		else if (cur_iteration == 5){
			size_str = buffer[i];
			size_str += buffer[i+1];
			size_str += buffer[i+2];
			
			size = stoi(size_str);
			i += 3;
			if (buffer[i] == ' ') i++;
			for(j = i; j < (i + size); j++){
				value += buffer[j];
			}
			i = j;
			strcpy(my_user.address, value.c_str());
			i++; //newline
		}
		else if (cur_iteration == 6){
			value = buffer[i++];
			strcpy(my_user.profession, professions[stoi(value)].c_str());
			i++;
		}
		else if (cur_iteration == 7){
			size_str = buffer[i];
			size_str += buffer[i+1];
			size_str += buffer[i+2];
			size_str += buffer[i+3];
			
			size = stoi(size_str);
			i += 4;
			if (buffer[i] == ' ') i++;
			for(j = i; j < (i + size); j++){
				value += buffer[j];
			}
			i = j;
			strcpy(my_user.bio, value.c_str());
			i++; //newline
		}
		else{
			strcpy(my_user.photo, value.c_str());
		}
		cur_iteration++;
		//i++;
	}
	my_user.print();
}

void recieve_msg(int ConnectFD){
	int n;
	char buffer[2];
	str num;
	
	do{
	
		bzero(buffer, 2);
		printf("Waiting for message...\n");
		n = read(ConnectFD, buffer, 2);
		num = buffer[0];
		n = stoi(num);
		write(ConnectFD, "OK.", 3);
		switch(n){
		case 1:
			printf("Whole struct\n");
			rcv_msg(ConnectFD);
			break;
		case 2:
			printf("Comandos\n");
			rcv_msg_comandos(ConnectFD);
			break;
		case 3:
			printf("Estructuras\n");
			rcv_msg_estructuras(ConnectFD);
			break;
		case 4:
			printf("JSON\n");
			rcv_msg_JSON(ConnectFD);
			break;
		case 5:
			printf("XML\n");
			rcv_msg_XML(ConnectFD);
			break;
		case 6:
			printf("CSV\n");
			rcv_msg_csv(ConnectFD);
			break;
		}
		printf("\n");
	}while(1);
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
	
    for(;;){
      	Client_connectFD = accept(SocketFD, NULL, NULL);
		
		printf("Connection made\n");
 
      	if(0 > Client_connectFD){
      		perror("error accept failed");
      		close(SocketFD);
      		exit(EXIT_FAILURE);
      	}
 
     /* perform read write operations ... */
		//std::thread t1(rcv_msg, Client_connectFD);
		//std::thread t1(rcv_msg_comandos, Client_connectFD);
		//std::thread t1(rcv_msg_JSON, Client_connectFD);
		//std::thread t1(rcv_msg_XML, Client_connectFD);
		//std::thread t1(rcv_msg_csv, Client_connectFD);
		//std::thread t1(rcv_msg_estructuras, Client_connectFD);
		std::thread t1(recieve_msg, Client_connectFD);
     	//std::thread t2(send_msg);
		t1.detach();
		//t2.detach();
 
      //shutdown(ConnectFD, SHUT_RDWR);
 
      //close(ConnectFD);
    }
 
    close(SocketFD);
    return 0;
  }
