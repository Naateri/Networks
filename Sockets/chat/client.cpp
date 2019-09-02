 /* Client code in C++ */

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

int SocketFD;
char buffer[256];

void send_msg(){
	char msg[256];
    int n;
	do{
	//std::cout << "Type your message: ";
	std::cin.getline(msg, 255);
	n = write(SocketFD,msg,255); //cuantos bytes estoy mandando

   	//n dice cuantos bytes se han mandado
	msg[n] = '\0';
	} while(strcmp(buffer, "chau") != 0);
}

void rcv_msg(){
	//char buffer[256];
    int n;
	do{	
	bzero(buffer,256);
    n = read(SocketFD,buffer,255);
   	if (n < 0) perror("ERROR reading from socket");
	buffer[n] = '\0';
   	//printf("Here is the message: [%s]\n",buffer);
	printf("Server: [%s]\n",buffer);
	} while(strcmp(buffer, "chau") != 0);
}
 
  int main(void)
  {
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
    stSockAddr.sin_port = htons(45500); //port
    Res = inet_pton(AF_INET, "192.168.80.81", &stSockAddr.sin_addr);
 
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
	/*do{
	}while(strcmp(buffer, "chau") != 0); */
	std::thread t1(send_msg);
	std::thread t2(rcv_msg);

	t1.join();
	t2.join();
    shutdown(SocketFD, SHUT_RDWR);
 
    close(SocketFD);

    return 0;
  }
