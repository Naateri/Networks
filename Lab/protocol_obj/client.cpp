 /* Client code in C++ */

//g++ client.cpp -o client -std=c++11 -lpthread

//1: struct
//2: orientado a estructuras
//3: orientado a comandos
//4: JSON
//5: XML
//6: CSV
 
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

//recieve message

typedef std::string str;

int SocketFD;
bool end = false;
char buffer[256];

int port = 45500;

std::ifstream my_file("photo.txt");

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

User Renato;

void send_msg(){ //struct
	send(SocketFD, &Renato, sizeof(Renato), 0);
	printf("Message sent\n");
}

void send_orientado_comandos(int ConnectFD){
	str command;
	int n;
	command = "name Renato\n";
	command += "initial L.\n";
	command += "surname Postigo Avalos\n";
	command += "dob 281298\n";
	command += "dni 72221876\n";
	command += "address Urb. Santo Domingo Segunda Etapa B-3 Dpto. 2\n";
	command += "profession Computer Science\n";
	command += "bio Peruvian computer science student. Currently taking the networks and communication course.\n";
	n = write(ConnectFD, command.c_str(), command.size());
}

void send_JSON(int ConnectFD){
	str my_obj;
	my_obj = "{\"name\":\"Renato\",";
	my_obj += "\"initial\":\"L.\",";
	my_obj += "\"surname\":\"Postigo Avalos\",";
	my_obj += "\"dob\":\"281298\",";
	my_obj += "\"dni\":\"72221876\",";
	my_obj += "\"address\":\"Urb. Santo Domingo Segunda Etapa B-3 Dpto. 2\",";
	my_obj += "\"profession\":\"Computer Science\",";
	my_obj += "\"bio\":\"Peruvian computer science student. Currently taking the networks and communication course.\"}";
	int n = write(ConnectFD, my_obj.c_str(), my_obj.size());
}

void send_XML(int ConnectFD){
	str my_obj;
	my_obj = "<User>\n";
	my_obj += "<name>Renato</name>\n";
	my_obj += "<surname>L.</surname>\n";
	my_obj += "<dob>281292</dob>\n";
	my_obj += "<dni>72221876</dni>\n";
	my_obj += "<address>Urb. Santo Domingo Segunda Etapa B-3 Dpto. 2</address>\n";
	my_obj += "<profession>Computer Science</profession>";
	my_obj += "<bio>Peruvian computer science student. Currently taking the networks and communication course.</bio>";
	int n = write(ConnectFD, my_obj.c_str(), my_obj.size());
}

void rcv_msg(){
    int n;
	do{	
	bzero(buffer,256);
    n = read(SocketFD,buffer,255);
   	if (n < 0) perror("ERROR reading from socket");
	buffer[n] = '\0';
   	//printf("Here is the message: [%s]\n",buffer);
	printf("[%s]\n",buffer);
	n = write(SocketFD,"Ok. Message recieved.",21); 
	} while(!end);
}
 
int main(void){
	strcpy(Renato.name, "Renato");
	strcpy(Renato.inicial, "L.");
	strcpy(Renato.surname, "Postigo Avalos");
	strcpy(Renato.DOB, "1 281298");
	strcpy(Renato.address, "Urb. Santo Domingo Segunda Etapa B-3 Dpto. 2");	
	strcpy(Renato.profession, "Computer Science");
	strcpy(Renato.bio, "Peruvian computer science student. Currently taking the networks and communication course.");
	Renato.DNI = 72221876;
	
	str temp_photo;
	
	getline(my_file, temp_photo);
	strcpy(Renato.photo, temp_photo.c_str());
	
	//std::cout << Renato.photo << std::endl;
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
    Res = inet_pton(AF_INET, "192.168.199.92", &stSockAddr.sin_addr);
 
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
	
	//send_msg();
	//send_orientado_comandos(SocketFD);
	send_JSON(SocketFD);
	
    shutdown(SocketFD, SHUT_RDWR);
 
    close(SocketFD);

    return 0;
  }
