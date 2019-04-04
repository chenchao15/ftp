/**********************************************************
*Copyright(C),2017, ChenChao
All Rights Reserved

*FileName:   client.c
*Author:     ChenChao
*Version:    1.0
*Date:       2017-11-2
*Description: the main entrance of client running,including
 * basic server operation such as USER,PASS and other
 * commands when an user is logged in.
************************************************************/
#include "client.h"

int m_port = 5977;
char m_ip[MAXSIZE] = "127.0.0.1";
char message1[MAXSIZE],message2[MAXSIZE];

int main(int argc, char **argv) {
    int sockfd;
    char res[MAXSIZE];
    char buffer[MAXSIZE];
    char ip[50];
    int len;
    int mode = UNLOGIN;
    int port;
    int listenfd = 0,connfd = 0;
    int flag = 0;               //flag used to conclude whether to print "cc_ftp>"
    int currentflag = 0;        //currentflag used to conclude whether the command has been executed

    for(int i=0;i<argc;i++){
        if(strcmp(argv[i],"-ip")==0){
            strcpy(m_ip,argv[i+1]);
        }
        if(strcmp(argv[i],"-port")==0){
            m_port = atoi(argv[i+1]);
        }
    }

    sockfd = socket_create(m_port, m_ip);
    if (sockfd < 0){
        return -1;
    }

    get_message(sockfd,buffer,sizeof(buffer));
    printf("%s\n", buffer);
    memset(buffer, 0, sizeof(buffer));

    while(1){
        memset(res, 0, sizeof(res));
        memset(buffer, 0, sizeof(buffer));
        char temp_str[20] = {""};
        if(flag == 1)
            printf("cc_ftp>");
        fgets(res, 4096, stdin);
        len = strlen(res);
        res[len-1] = '\0';
        if (strcmp(res, "\0") == 0)
            continue;
        strcat(res, "\r\n");

        if (check_string(res, "USER", 4)){
            send_message(sockfd,res,len+1);
            get_message(sockfd,buffer,sizeof(buffer));
            printf("%s\n", buffer);
            strncpy(temp_str, buffer, 3);
            if (strcmp(temp_str, "331") == 0){
                mode = NEEDPASS;
            }
            currentflag = 1;  //the "USER" command has been executed
        }
        else if (check_string(res, "QUIT", 4)){
            send_message(sockfd,res,len+1);
            get_message(sockfd,buffer,sizeof(buffer));
            printf("%s", buffer);
            strncpy(temp_str, buffer, 3);
            if (strcmp(temp_str, "221") == 0){
                break;
            }
            currentflag = 1;
        }
        else if(check_string(res, "PASS", 4) && mode == NEEDPASS){
            send_message(sockfd,res,len+1);
            get_message(sockfd,buffer,sizeof(buffer));
            printf("%s", buffer);
            strncpy(temp_str, buffer, 3);
            if (strcmp(temp_str, "230") == 0){
                mode = LOGIN;
                flag = 1;
            }
            currentflag = 1;
        }
        else if(check_string(res, "TYPE", 4) || check_string(res, "SYST", 4)) {
            send_message(sockfd, res, len + 1);
            get_message(sockfd, buffer, sizeof(buffer));
            printf("%s", buffer);
            currentflag = 1;
        }
        if (mode >= LOGIN){   //if user login
            if(check_string(res, "PASV", 4)){
                send_message(sockfd,res,len+1);
                get_message(sockfd,buffer,sizeof(buffer));
                printf("%s", buffer);
                strncpy(temp_str, buffer, 3);
                if (strcmp(temp_str, "227") == 0){
                    char str[100];
                    get_ipstring(buffer, str);
                    port = get_ip_port(str, ip);
                    connfd = run_pasv_mode(port, ip);
                    mode = PASV;
                }
            }
            else if(check_string(res, "PORT", 4)){
                send_message(sockfd,res,len+1);
                get_message(sockfd,buffer,sizeof(buffer));
                printf("%s", buffer);
                strncpy(temp_str, buffer, 3);
                if (strcmp(temp_str, "200") == 0){
                    int len = strlen(res);
                    res[len-2] = '\0';
                    port = get_ip_port(res+5, ip);
                    listenfd = run_port_mode(port,ip);
                    if (listenfd != -1){
                        mode = PORT;
                    }
                }
            }
            else if(check_string(res, "RETR", 4)){
                send_message(sockfd,res,len+1);
                get_message(sockfd,buffer,sizeof(buffer));
                int check = check_two_message(buffer);
                if(check == 0)
                    printf("%s", buffer);
                else
                    printf("%s",message1);
                strncpy(temp_str, buffer, 3);
                if (strcmp(temp_str, "150") == 0){
                    if(mode == PORT){  //if use port mode
                        connfd = accept(listenfd, NULL, NULL);
                    }
                    get_file(res,connfd);
                    close(connfd);
                    if(mode == PORT){
                        close(listenfd);
                        connfd = 0;
                    }
                    if(check == 0) {
                        get_message(sockfd, buffer, sizeof(buffer));
                        printf("%s", buffer);
                    }
                    else
                        printf("%s", message2);
                    mode = LOGIN;
                }
            }
            else if(check_string(res, "STOR", 4)){
                char filename[100];
                strcpy(filename, res+5);
                filename[strlen(filename)-2] = '\0';
                char data[MAXSIZE];
                FILE *fp = fopen(filename, "rb");
                if (fp == NULL){
                    printf("550 file not exist.\r\n");
                }
                else{
                    send_message(sockfd,res,len+1);
                    get_message(sockfd,buffer,sizeof(buffer));
                    int check = check_two_message(buffer);
                    if(check == 0)
                        printf("%s", buffer);
                    else
                        printf("%s",message1);
                    if (mode == PORT){
                        connfd = accept(listenfd, NULL, NULL);
                    }
                    strncpy(temp_str, buffer, 3);
                    if (strcmp(temp_str, "150") == 0){
                        int fsize = 0;
                        while(!feof(fp)){
                            fsize = fread(data, sizeof(char), sizeof(data), fp);
                            if (send(connfd,data,fsize, 0) < 0 ) {
                                printf("send message error\n");
                                break;
                            }
                            memset(data, 0, sizeof(data));
                        }
                        fclose(fp);
                        close(connfd);

                        if(mode == PORT){
                            close(listenfd);
                            connfd = 0;
                        }
                        if(check == 0) {
                            get_message(sockfd, buffer, sizeof(buffer));
                            printf("%s", buffer);
                        }
                        else
                            printf("%s", message2);
                        mode = LOGIN;
                    }
                }
            }
            else if(check_string(res, "LIST", 4)){
                send_message(sockfd,res,len+1);
                get_message(sockfd,buffer,sizeof(buffer));
                int check = check_two_message(buffer);
                if(check == 0)
                    printf("%s", buffer);
                else
                    printf("%s",message1);
                strncpy(temp_str, buffer, 3);
                if (strcmp(temp_str, "150") == 0){
                    if(mode == PORT){
                        connfd = accept(listenfd, NULL, NULL);
                    }
                    recv_list(connfd);
                    close(connfd);
                    if(mode == PORT){
                        close(listenfd);
                        connfd = 0;
                    }
                    if(check == 0) {
                        get_message(sockfd, buffer, sizeof(buffer));
                        printf("%s", buffer);
                    }
                    else
                        printf("%s", message2);
                    mode = LOGIN;
                }

            }
            else if(currentflag == 0){  //if user login and the command hasn't been executed
                send_message(sockfd, res, len + 1);
                get_message(sockfd, buffer, sizeof(buffer));
                printf("%s", buffer);
            }
        }
        else if(currentflag == 0){   //if user not login and the command hasn't been executed
            send_message(sockfd, res, len + 1);
            get_message(sockfd, buffer, sizeof(buffer));
            printf("%s", buffer);
        }
        currentflag = 0;
    }

    close(sockfd);
    if(listenfd != 0){
        close(listenfd);
    }

    return 0;
}


/*****************************************************
 * create a socket
 * @param port port
 * @param ip ip
 */
int socket_create(int port,const char *ip){
    int sockfd;
    struct sockaddr_in addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        printf("Error socket(): %s(%d)\n", strerror(errno), errno);
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &addr.sin_addr) < 0) {
        printf("Error inet_pton(): %s(%d)\n", strerror(errno), errno);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Error connect(): %s(%d)\n", strerror(errno), errno);
        return -1;
    }
    else{
        printf("connect to server successfully\n");
    }
    return sockfd;
}
