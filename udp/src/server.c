#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>     /* defines STDIN_FILENO, system calls,etc */
#include <sys/types.h>  /* system data type definitions */
#include <sys/socket.h> /* socket specific definitions */
#include <netinet/in.h> /* INET constants and stuff */
#include <arpa/inet.h>  /* IP address conversion stuff */
#include <netdb.h>      /* gethostbyname */
#include<malloc.h>
#include<string.h>

#define MAXBUF 1024*1024

int count = 0;

char *str[52];

void uppercase(char *p) {
  for ( ; *p; ++p) *p = toupper(*p);
}

int getstring(char *s,int count,char *bufin,int n){
   int j;
   if (count <=9){
       s[0] = count + 48;
       s[1] = ' ';
       for(j=0;j<n;j++)
            s[j+2] = bufin[j];
       s[j+2] = '\0';
       return n+2;
   }
   else{
       int first = count%10;
       int second = count/10;
       s[0] = second + 48;
       s[1] = first + 48;
       s[2] = ' ';
       for(j=0;j<n;j++)
            s[j+3] = bufin[j];
       s[j+3] = '\0';
       return n+3;
   }
}

int is_number(char *s,int n){
    int num = 0;
    for(int i=0;i<n-1;i++){
        if(s[i]>=48 && s[i]<='9'){
            num = num * 10 + s[i]-48;
            continue;
        }
        else
            return -1;
    }
    if(num > 50)
        return -1;
    return num;
}

void echo(int sd) {
    char bufin[MAXBUF];
    struct sockaddr_in remote;
    char *p;
    /* need to know how big address struct is, len must be set before the
       call to recvfrom!!! */
    socklen_t len = sizeof(remote); 
    while (1) {
      /* read a datagram from the socket (put result in bufin) */
      int n = recvfrom(sd, bufin, MAXBUF, 0, (struct sockaddr *) &remote, &len);
      if (n < 0) {
        perror("Error receiving data");
      } else {
        int number = is_number(bufin,n);
        if(number >= 0 && number < count){
            sendto(sd, str[number],strlen(str[number]), 0, (struct sockaddr *)&remote, len);
        }
        else{
            p = (char *)malloc(n+1);
            strcpy(p,bufin);
            str[count] = p;
            str[count][n] = '\0';
            char *s = (char *)malloc(n+7);
            int length = getstring(s,count,bufin,n);
            /* Got something, just send it back */
            sendto(sd, s, length, 0, (struct sockaddr *)&remote, len);
            count++;
            free(s);
        }
        if(count == 51)
            count = 0;
      }
    }
}

/* server main routine */

int main() {
  int ld;
  char buffer[MAXBUF];
  struct sockaddr_in skaddr;
  socklen_t length;

  /* create a socket
     IP protocol family (PF_INET)
     UDP protocol (SOCK_DGRAM)
  */

  if ((ld = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("Problem creating socket\n");
    exit(1);
  }

  /* establish our address
     address family is AF_INET
     our IP address is INADDR_ANY (any of our IP addresses)
     the port number is 9876
  */

  skaddr.sin_family = AF_INET;
  skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  skaddr.sin_port = htons(7891);
  memset(skaddr.sin_zero,'\0',sizeof skaddr.sin_zero);

  if (bind(ld, (struct sockaddr *) &skaddr, sizeof(skaddr)) < 0) {
    printf("Problem binding\n");
    exit(0);
  }

  /* find out what port we were assigned and print it out */

  length = sizeof(skaddr);
  if (getsockname(ld, (struct sockaddr *) &skaddr, &length) < 0) {
    printf("Error getsockname\n");
    exit(1);
  }
  echo(ld);
  return 0;
}
