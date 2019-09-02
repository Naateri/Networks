 /* Client code in C */
 
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
 
  int main(void)
  {
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int n;
	char buffer[256];
 
    if (-1 == SocketFD)
    {
      perror("cannot create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(45500); //port
    Res = inet_pton(AF_INET, "192.168.80.72", &stSockAddr.sin_addr);
 
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
    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("connect failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    n = write(SocketFD,"Hi, this is Renato.",19); //cuantos bytes estoy mandando
    /* perform read write operations ... */

    //n dice cuantos bytes se han mandado
	
	bzero(buffer,256);
    n = read(SocketFD,buffer,255);
    if (n < 0) perror("ERROR reading from socket");
	buffer[n] = '\0';
    printf("Here is the message: [%s]\n",buffer);
    /* perform read write operations ... */
 
    shutdown(SocketFD, SHUT_RDWR);
 
    close(SocketFD);

    return 0;
  }
