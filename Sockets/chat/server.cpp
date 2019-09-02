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

int SocketFD, ConnectFD;
char msg[256];

void send_msg(){
    int n;
	do{
	std::cin.getline(msg, 255);
    n = write(ConnectFD,msg,255);
    if (n < 0) perror("ERROR writing to socket");
	}while ( strcmp(msg, "chau") != 0);
}

void rcv_msg(){
	char buffer[256];
    int n;
	do{
	bzero(buffer,256);
    n = read(ConnectFD,buffer,255);
    if (n < 0) perror("ERROR reading from socket");
	printf("Client: [%s]\n",buffer);
	}while ( strcmp(msg, "chau") != 0);
}

  int main(void)
  {
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
 
    for(;;)
    {
      ConnectFD = accept(SocketFD, NULL, NULL);
 
      if(0 > ConnectFD)
      {
        perror("error accept failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
      }
 
 	 /*do{
		std::thread t1(rcv_msg);
     	std::thread t2(send_msg);

		t1.join();
		t2.join();
	}while ( strcmp(msg, "chau") != 0); */
 
     /* perform read write operations ... */
		std::thread t1(rcv_msg);
     	std::thread t2(send_msg);

		t1.join();
		t2.join();
 
      shutdown(ConnectFD, SHUT_RDWR);
 
      close(ConnectFD);
    }
 
    close(SocketFD);
    return 0;
  }
