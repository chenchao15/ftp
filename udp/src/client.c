#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     /* defines STDIN_FILENO, system calls,etc */
#include <sys/types.h>  /* system data type definitions */
#include <sys/socket.h> /* socket specific definitions */
#include <netinet/in.h> /* INET constants and stuff */
#include <arpa/inet.h>  /* IP address conversion stuff */
#include <netdb.h>      /* gethostbyname */

#define MAXBUF 10*1024

int main() {
  int sk;
  struct sockaddr_in server;
  struct hostent *hp;
  char buf[MAXBUF]="hello\n";
  int count = 0;
  /* create a socket
     IP protocol family (PF_INET)
     UDP (SOCK_DGRAM)
  */

  if ((sk = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("Problem creating socket\n");
    exit(1);
  }

  server.sin_family = AF_INET;
  hp = gethostbyname("localhost");
  //输出ip地址
  //char *ip;  
  //ip = inet_ntoa(*(struct in_addr *)*hp->h_addr_list);
  //printf("%s",ip);

  /* copy the IP address into the sockaddr
     It is already in network byte order
  */
  memcpy(&server.sin_addr.s_addr, hp->h_addr, hp->h_length);

  /* establish the server port number - we must use network byte order! */
  server.sin_port = htons(7891);
  while(1)
  {
      /* read everything possible */
     // printf("Please input a string...\n");
      //fgets(buf, MAXBUF, stdin);
      size_t buf_len = strlen(buf);

      /* send it to the echo server */
  
      int n_sent = sendto(sk, buf, buf_len, 0,
                  (struct sockaddr*) &server, sizeof(server));
      if (n_sent < 0) {
          perror("Problem sending data");
          exit(1);
      }

      if (n_sent != buf_len) {
          printf("Sendto sent %d bytes\n", n_sent);
      }

      /* Wait for a reply (from anyone) */
      int n_read = recvfrom(sk, buf, MAXBUF, 0, NULL, NULL);
      if (n_read < 0) {
          perror("Problem in recvfrom");
          exit(1);
      }

      /* send what we got back to stdout */
      if (write(STDOUT_FILENO, buf, n_read) < 0) {
          perror("Problem writing to stdout");
          exit(1);
      }
      strcpy(buf,"hello\n");
      if(count>=50)
          break;
      count++;
      //printf("Received from server:%s\n",buf);
  }
  return 0;
}
