 /* Client code in C++ */

//g++ client.cpp -o client -std=c++11 -lpthread

//All 6 structures included

//1: orientado a estructuras
//2: orientado a comandos
//3: JSON
//4: XML
//5: CSV
//6: struct
//7: exit program
 
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
char buffer[256];

int port = 45000;
str ip = "192.168.1.62";
str photo = "snoo.txt";
bool exit_prog = false;

struct User{
	char name[99];
	char inicial[99];
	char surname[999];
	char DOB[99];
	char address[999];
	char profession[99];
	char bio[999];
	char photo[300000];
	char photo_name[99];
	
	unsigned int DNI;
	
	void print(){
		printf("Name: %s %s %s\n", this->name, this->inicial, this->surname);
		printf("DOB: %s\n", this->DOB);
		printf("Address: %s\n", this->address);
		printf("Profession: %s\n", this->profession);
		printf("Bio: %s\n", this->bio);
		printf("Photo name: %s\n", this->photo_name);
	}
};

User Renato;

/*
--------------
---------------
Struct
----------------
---------------
*/

void send_msg_struct(int ConnectFD){ //struct
	send(ConnectFD, &Renato, sizeof(Renato), 0);
	printf("Message sent\n");
}

/*
--------------
---------------
ORIENTADO
	A
		COMANDOS
----------------
---------------
*/

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
	command += "bio Peruvian computer science student. Currently taking the networks and communications course.\n";
	command += "photo_name snoo.png\n";
	command += "photo ";
	str temp_photo;
	std::ifstream my_file(photo.c_str());
	
	getline(my_file, temp_photo);
	
	command += temp_photo;
	command += "\n";
	
	//my_file.close();
	
	n = write(ConnectFD, command.c_str(), command.size());
}

/*
--------------
---------------
JSON
----------------
---------------
*/

void send_JSON(int ConnectFD){
	str my_obj;
	str temp_photo;
	my_obj = "{\"name\":\"Renato\",";
	my_obj += "\"initial\":\"L.\",";
	my_obj += "\"surname\":\"Postigo Avalos\",";
	my_obj += "\"dob\":\"281298\",";
	my_obj += "\"dni\":\"72221876\",";
	my_obj += "\"address\":\"Urb. Santo Domingo Segunda Etapa B-3 Dpto. 2\",";
	my_obj += "\"profession\":\"Computer Science\",";
	my_obj += "\"bio\":\"Peruvian computer science student. Currently taking the networks and communication course.\",";
	my_obj += "\"photo_name\":\"snoo.png\",\"photo\":\"";
	
	std::ifstream my_file(photo.c_str());
	
	getline(my_file, temp_photo);
	
	my_obj += temp_photo;
	my_obj += "\"}\n";
	
	int n = write(ConnectFD, my_obj.c_str(), my_obj.size());
}

/*
--------------
---------------
XML
----------------
---------------
*/

void send_XML(int ConnectFD){
	str my_obj;
	my_obj = "<User>\n";
	my_obj += "<name>Renato</name>\n";
	my_obj += "<initial>L.</initial>\n";
	my_obj += "<surname>Postigo Avalos</surname>\n";
	my_obj += "<dob>281298</dob>\n";
	my_obj += "<dni>72221876</dni>\n";
	my_obj += "<address>Urb. Santo Domingo Segunda Etapa B-3 Dpto. 2</address>\n";
	my_obj += "<profession>Computer Science</profession>\n";
	my_obj += "<bio>Peruvian computer science student. Currently taking the networks and communications course.</bio>\n";
	my_obj += "<photo_name>snoo.png</photo_name>\n";
	my_obj += "<photo>";
		
	str temp_photo;
	std::ifstream my_file(photo.c_str());
	
	getline(my_file, temp_photo);
	my_obj += temp_photo;
	my_obj += "</photo>";
	
	int n = write(ConnectFD, my_obj.c_str(), my_obj.size());
}

/*
--------------
---------------
CSV
----------------
---------------
*/

void send_csv(int ConnectFD){
	str my_obj;
	my_obj += "Renato,L.,Postigo Avalos,281298,72221876";
	my_obj += ",Urb. Santo Domingo Segunda Etapa B-3 Dpto. 2,Computer Science";
	my_obj += ",Peruvian computer science student. Currently taking the networks and communications course.,";
	my_obj += "snoo.png,";
	
	str temp_photo;
	std::ifstream my_file(photo.c_str());
	
	getline(my_file, temp_photo);
	
	my_obj += temp_photo;
	
	int n = write(ConnectFD, my_obj.c_str(), my_obj.size());
}

/*
--------------
---------------
ORIENTADO
	A
		ESTRUCTURAS
----------------
---------------
*/

void send_orientado_estruct(int ConnectFD){
	str my_obj;
	my_obj += "006 Renato\n";
	my_obj += "02 L.\n";
	my_obj += "014 Postigo Avalos\n";
	my_obj += "1 281298\n";
	my_obj += "72221876\n";
	my_obj += "044 Urb. Santo Domingo Segunda Etapa B-3 Dpto. 2\n";
	my_obj += "0\n";
	my_obj += "091 Peruvian computer science student. Currently taking the networks and communications course.\n";
	my_obj += "10 snoo.png\n";
	
	str temp_photo;
	std::ifstream my_file(photo.c_str());
	
	getline(my_file, temp_photo);
	
	int n = write(ConnectFD, my_obj.c_str(), my_obj.size());
}

void send_msg(int ConnectFD){
	int n;
	str sign;
	char buffer[5];
	std::cout << "Ingrese un numero segun el caso\n";
	std::cout << "1. Orientado a comandos.\n2. Orientado a estructuras.\n";
	std::cout << "3. JSON\n4. XML\n5. CSV\n6. Struct completo.\n";
	do{
	std::cin >> n;
	sign = std::to_string(n);
	write(ConnectFD, sign.c_str(), sign.size());
	read(ConnectFD, buffer, 5); 
	buffer[n] = '\0';
	printf("Server: [%s]\n", buffer);
	switch(n){
	case 1:
		printf("Sending orientado a comandos\n");
		send_orientado_comandos(ConnectFD);
		break;
	case 2:
		printf("Sending orientado a estructuras\n");
		send_orientado_estruct(ConnectFD);
		break;
	case 3:
		printf("Sending JSON\n");
		send_JSON(ConnectFD);
		break;
	case 4:
		printf("Sending XML\n");
		send_XML(ConnectFD);
		break;
	case 5:
		printf("Sending CSV\n");
		send_csv(ConnectFD);
		break;
	case 6:
		send_msg_struct(ConnectFD);
		break;
	case 7:
		printf("Bye\n");
		exit_prog = true;
		break;
	}
	}while(!exit_prog);
}
 
int main(void){
	strcpy(Renato.name, "Renato");
	strcpy(Renato.inicial, "L.");
	strcpy(Renato.surname, "Postigo Avalos");
	strcpy(Renato.DOB, "1 281298");
	strcpy(Renato.address, "Urb. Santo Domingo Segunda Etapa B-3 Dpto. 2");	
	strcpy(Renato.profession, "Computer Science");
	strcpy(Renato.bio, "Peruvian computer science student. Currently taking the networks and communications course.");
	strcpy(Renato.photo_name, "snoo.png");
	Renato.DNI = 72221876;
	
	str temp_photo;
	std::ifstream my_file(photo.c_str());
	
	getline(my_file, temp_photo);
	strcpy(Renato.photo, temp_photo.c_str());
	
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
	
	//send_msg();
	//send_orientado_comandos(SocketFD);
	//send_JSON(SocketFD);
	//send_XML(SocketFD);
	//send_csv(SocketFD);
	//send_orientado_estruct(SocketFD);
	
	/*std::thread t1(send_msg, SocketFD);
	t1.join();*/
	send_msg(SocketFD);
	
    shutdown(SocketFD, SHUT_RDWR);
 
    close(SocketFD);

    return 0;
  }
