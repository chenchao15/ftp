/**********************************************************
*Copyright(C),2017, ChenChao
All Rights Reserved

*FileName:   mode.c
*Author:     ChenChao
*Version:    1.0
*Date:       2017-11-2
*Description: the realization of two modes:PASV and PORT
************************************************************/
#include"server.h"

extern int Port_sockfd;
extern int Pasv_sockfd;
extern int Pasv_listenfd;
extern struct sockaddr_in port_addr;


/****************************************************
 * starting port mode when recieving "PORT"
 * @param buffer "PORT **,**,**,**,**,**"
 */
int run_port_mode(char *buffer) {
    char ip[20];
    int port;
    get_ip_port(buffer,ip,&port);
    memset(&port_addr, 0, sizeof(port_addr));
    port_addr.sin_family = AF_INET;
    port_addr.sin_port = htons(port);

    if (inet_pton(AF_INET,ip,&port_addr.sin_addr) <= 0) {
        printf("Error inet_pton(): %s(%d)\n", strerror(errno), errno);
        return -1;
    }

    if (Port_sockfd >= 0) {
        close(Port_sockfd);
        Port_sockfd = -1;
    }
    if ((Port_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        printf("Error socket(): %s(%d)\n", strerror(errno), errno);
        return -1;
    }
    return 0;
}
/***************************************************
 * starting pasv mode when recieving "PASV"
 * @param connfd socket
 */
int run_pasv_mode(int connfd) {
    struct sockaddr_in pasv_addr;
    socklen_t len = sizeof(pasv_addr);
    int port;
    char message[50] = "227 Entering Passive Mode (";
    char str[20];

    srand((unsigned)time(NULL));
    port = rand() % (65535 - 20000 + 1) + 20000;
    if (Pasv_listenfd >= 0) {
        close(Pasv_listenfd);
        Pasv_listenfd = -1;
    }
    if ((Pasv_listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        printf("Error socket(): %s(%d)\n", strerror(errno), errno);
        return -1;
    }

    getsockname(connfd, (struct sockaddr*)&pasv_addr, &len);
    pasv_addr.sin_port = htons(port);

    if (bind(Pasv_listenfd, (struct sockaddr*)&pasv_addr, sizeof(pasv_addr)) == -1) {
        printf("Error bind(): %s(%d)\n", strerror(errno), errno);
        close(Pasv_listenfd);
        Pasv_listenfd = -1;
        return -1;
    }
    if (listen(Pasv_listenfd, 10) == -1) {
        printf("Error listen(): %s(%d)\n", strerror(errno), errno);
        close(Pasv_listenfd);
        Pasv_listenfd = -1;
        return -1;
    }
    //concat ip and port to message
    len = sizeof(pasv_addr);
    getsockname(Pasv_listenfd, (struct sockaddr*)&pasv_addr, &len);
    strcat(message, inet_ntoa(pasv_addr.sin_addr));
    len = strlen(message);
    message[len++] = '.';
    message[len] = '\0';
    sprintf(str,"%d",port/256);
    strcat(message, str);
    len = strlen(message);
    message[len++] = '.';
    message[len] = '\0';
    sprintf(str,"%d",port%256);
    strcat(message, str);
    len = strlen(message);
    message[len++] = ')';
    message[len++] = '\r';
    message[len++] = '\n';
    message[len] = '\0';
    for (int i = 0; i < (int)strlen(message); i++) {
        if (message[i] == '.') {
            message[i] = ',';
        }
    }
    send_message(connfd, message);
    return 0;
}


int pasv_accept() {
    if (Pasv_sockfd >= 0) {
        close(Pasv_sockfd);
        Pasv_sockfd = -1;
    }
    if ((Pasv_sockfd = accept(Pasv_listenfd, NULL, NULL)) == -1) {
        printf("Error accept(): %s(%d)\n", strerror(errno), errno);
        return -1;
    }
    return 0;
}


int port_connect() {
    if (connect(Port_sockfd, (struct sockaddr*)&port_addr, sizeof(port_addr)) < 0) {
        printf("Error connect(): %s(%d)\n", strerror(errno), errno);
        if (Port_sockfd >= 0) {
            close(Port_sockfd);
            Port_sockfd = -1;
        }
        return -1;
    }
    return 0;
}