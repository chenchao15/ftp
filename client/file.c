/**********************************************************
*Copyright(C),2017, ChenChao
All Rights Reserved

*FileName:   file.c
*Author:     ChenChao
*Version:    1.0
*Date:       2017-11-2
*Description: Providing some functions of file operation,
 * including downloading files,deleting files and recieving
 * file list of current working directory of server.
************************************************************/
#include "client.h"

/*********************************************************
 * download file from server when user put command "RETR"
 * @param res "RETR ***(filename)"
 * @param connfd socket
 */
int get_file(char *res,int connfd){
    char filename[100];
    strcpy(filename, res+5);
    filename[strlen(filename)-2] = '\0';
    char data[MAXSIZE];

    FILE *fp = fopen(filename,"wb+");
    int fsize = 0;

    while(1){
        fsize = recv(connfd, data, sizeof(data), 0);
        if(fsize == 0){
            break;
        }
        fwrite(data, sizeof(char), fsize, fp);
    }
    fclose(fp);
}


/*********************************************************
 * get list of the current working directory of the server
 * @param file_sockfd file transfer socket
 */
int recv_list(int file_sockfd){
    size_t recvd;
    char data[MAXSIZE];
    int tmp = 0;
    memset(data,0,sizeof(data));

    while((recvd = recv(file_sockfd,data,MAXSIZE,0))>0){
        printf("%s",data);
        memset(data,0,sizeof(data));
    }
    if(recvd<0)
        perror("error");
    return 0;
}