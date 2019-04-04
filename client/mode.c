/**********************************************************
*Copyright(C),2017, ChenChao
All Rights Reserved

*FileName:   mode.c
*Author:     ChenChao
*Version:    1.0
*Date:       2017-11-2
*Description: the realization of two modes:PASV and PORT
************************************************************/
#include "client.h"

/****************************************************
 * starting pasv mode when putting "PASV"
 * @param ip ip
 * @param port port
 */
int run_pasv_mode(int port, const char *ip){
    int pasv_sockfd;
    struct sockaddr_in pasv_addr;

    if ((pasv_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        printf("Error socket(): %s(%d)\n", strerror(errno), errno);
        return -1;
    }

    memset(&pasv_addr, 0, sizeof(pasv_addr));
    pasv_addr.sin_family = AF_INET;
    pasv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &pasv_addr.sin_addr) < 0) {
        printf("Error inet_pton(): %s(%d)\n", strerror(errno), errno);
        return -1;
    }

    if (connect(pasv_sockfd, (struct sockaddr*)&pasv_addr, sizeof(pasv_addr)) < 0) {
        printf("Error connect(): %s(%d)\n", strerror(errno), errno);
        return -1;
    }
    return pasv_sockfd;
}


/****************************************************
 * starting port mode when putting "PORT"
 * @param ip ip
 * @param port port
 */
int run_port_mode(int port, const char *ip){
    int port_listenfd;
    struct sockaddr_in port_addr;

    if ((port_listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        printf("Error socket(): %s(%d)\n", strerror(errno), errno);
        return -1;
    }

    memset(&port_addr, 0, sizeof(port_addr));

    port_addr.sin_family = AF_INET;
    port_addr.sin_port = htons(port);
    port_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(port_listenfd, (struct sockaddr*)&port_addr, sizeof(port_addr)) == -1) {
        printf("Error bind(): %s(%d)\n", strerror(errno), errno);
        return -1;
    }

    if (listen(port_listenfd, 10) == -1) {
        printf("Error listen(): %s(%d)\n", strerror(errno), errno);
        return -1;
    }

    return port_listenfd;
}