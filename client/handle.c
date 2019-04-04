/**********************************************************
*Copyright(C),2017, ChenChao
All Rights Reserved

*FileName:   handle.c
*Author:     ChenChao
*Version:    1.0
*Date:       2017-11-2
*Description: some details and operations which are used
 * frequently such as send and recv, get ip and port,
 * string matching.
************************************************************/
#include "client.h"


extern char message1[MAXSIZE];
extern char message2[MAXSIZE];

/*****************************************************
 * analysis of string, concluding whether the string
 * is two message, aiming to avoid double messages
 * are recieved by one recv
 * @param buffer message recieved from server
 * @return the flag
 */
int check_two_message(char buffer[MAXSIZE]){
    memset(message1, 0, sizeof(message1));
    memset(message2, 0, sizeof(message2));
    int i,j;
    int flag = 0;
    for(i=0;buffer[i]!='\r';i++){}
    strncpy(message1,buffer,i+2);
    for(j=i+1;buffer[j]!='\0';j++){
        if(buffer[j] == '\r') {
            flag = 1;
            break;
        }
    }
    if(flag == 1)
        strncpy(message2,buffer+i+2,j+2-(i+2));
    return flag;
}

//analysis of string,geting port
int get_ip_port(char *buffer, char *ip){
    int i, j, k, t;
    char port[2][10];
    int len = strlen(buffer);

    for (i=len-1,j= 0,k=len-1;i>=0&&j<2;i--){
        if (buffer[i] == ','){
            strncpy(port[j], buffer+i+1, k-i);
            j++;
            k = i-1;
        }
    }
    strncpy(ip,buffer, i+1);
    for (t= 0;t<=i;t++){
        if (buffer[t] == ','){
            ip[t] = '.';
        }
    }
    return atoi(port[0]) + 256*atoi(port[1]);
}


//get message from server
int get_message(int sockfd,char *buf,int bufsize){
    size_t num_bytes;
    memset(buf, 0, bufsize);
    num_bytes = recv(sockfd, buf, bufsize, 0);
    if (num_bytes < 0)
        return -1;
    return num_bytes;
}

//send message to server
int send_message(int sockfd,char *buf,int len){
    if (send(sockfd,buf,len,0) < 0 ) {
        perror("error sending...\n");
        return -1;
    }
    return 0;
}


//matching string in const length
int check_string(char *string, const char *buf, int length ) {
    int bools = 1;
    if (length == 0)
        length = strlen(buf);
    for (int i = 0; i < length; i++) {
        if (string[i] != buf[i]) {
            bools = 0;
        }
    }
    return bools;
}


void get_ipstring(char *buffer, char *str){
    int i, j;
    for(i=0;buffer[i]!='(';i++){}
    for(j=i+1;buffer[j]!=')';j++)
        str[j-i-1] = buffer[j];
    str[j-i-1] = '\0';
    return;
}