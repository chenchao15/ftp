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
#include"server.h"


/*****************************************************
 * matching string in designated length
 * @param string message recieved from client
 * @param buf "USER","PASS" and so on
 * @param length 4 or 3
 * @return whether matching successfully or not
 */
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


//recieving message from client
int get_message(int sockfd,char *buf,int bufsize){
    size_t num_bytes;
    memset(buf, 0, bufsize);
    num_bytes = recv(sockfd, buf, bufsize, 0);
    if (num_bytes < 0)
        return -1;
    return num_bytes;
}


//send const content to client
int send_const_message(int sockfd,const char buf[MAXSIZE]){
    int len = strlen(buf);
    if (send(sockfd,buf,len,0) < 0 ) {
        perror("error sending...\n");
        return -1;
    }
    return 0;
}

//send message to client
int send_message(int sockfd, char *buf) {
    if (write(sockfd,buf, strlen(buf)) < 0) {
        printf("Error sending(): %s(%d)\n", strerror(errno), errno);
        return -1;
    }
    return 0;
}

/***************************************************
 * analysis of string,geting ip and port
 * @param str "PORT **,**,**,**,**,**"
 * @param ip ip
 * @param port port
 */
void get_ip_port(char *str,char *ip, int *port) {
    int num = 0;
    int i;
    int flag = 0;
    for (i = 5; i<(int)strlen(str); i++) {
        if (isdigit(str[i])) {
            ip[i-5] = str[i];
            num = num * 10 + str[i] - 48;
        }
        else if (str[i] == ','){
            flag++;
            if (flag == 4) {
                ip[i-5] = '\0';
                break;
            }
            ip[i-5] = '.';
            num = 0;
        }
    }
    num = 0;
    for (int j = i; j<(int)strlen(str); j++) {
        if (isdigit(str[j])) {
            *port = *port * 10 + str[j] - 48;
        }
        else if (str[j] == ','){
            num = *port;
            *port = 0;
        }
    }
    *port += num * 256;
}
