/**********************************************************
*Copyright(C),2017, ChenChao
All Rights Reserved

*FileName:   server.c
*Author:     ChenChao
*Version:    1.0
*Date:       2017-11-1
*Description: the main entrance of server running,including
 * basic server operation such as USER,PASS and other
 * commands when an user is logged in.
************************************************************/
#include "server.h"
#define MAXSIZE 1024

char *tmp = (char*)"/tmp";
int m_port = 21;

int Port_sockfd = -1;
int Pasv_sockfd = -1;
int Pasv_listenfd = -1;

struct sockaddr_in port_addr;

int main(int argc, char **argv) {
    int listenfd, connfd,pid;
    struct sockaddr_in addr;
    //get working directory and port from console
    tmp = (char*)malloc(MAXSIZE);
    for(int i=0;i<argc;i++){
        if(strcmp(argv[i],"-root")==0){
            strcpy(tmp,argv[i+1]);
        }
        if(strcmp(argv[i],"-port")==0){
            m_port = atoi(argv[i+1]);
        }
    }
    if(strlen(tmp) == 0)
        strcpy(tmp,"/tmp");
    tmp[strlen(tmp)] = '/';
    tmp[strlen(tmp)] = '\0';

    if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        printf("Error socket(): %s(%d)\n", strerror(errno), errno);
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        printf("Error bind(): %s(%d)\n", strerror(errno), errno);
        return 1;
    }

    if (listen(listenfd, 10) == -1) {
        printf("Error listen(): %s(%d)\n", strerror(errno), errno);
        return 1;
    }
    //Accepting requests from clients Circularly
    while (1){
        if ((connfd = accept(listenfd, NULL, NULL)) == -1) {
            printf("Error accept(): %s(%d)\n", strerror(errno), errno);
            continue;
        }
        if ((pid = fork()) < 0)
            perror("Error forking child process");
        else if (pid == 0){
            close(listenfd);
            serve_process(connfd);
            close(connfd);
            exit(0);
        }
        close(connfd);
    }
    close(listenfd);
}

/**************************************************
 *user login
 *check USER and PASS
 * @param connfd socket
 */
int serve_login(int connfd){
    char buffer[MAXSIZE];
    size_t bytes;
    char password[MAXSIZE];
    char str[MAXSIZE];
    //get username
    while(1){
        char temp_str[MAXSIZE] = {""};
        char temp_str2[MAXSIZE] = {""};
        bytes = get_message(connfd,buffer,sizeof(buffer));
        if(bytes<0)
            break;
        strcpy(str,buffer);
        strncpy(temp_str,str,4);
        strncpy(temp_str2,str,14);
        if(strcmp(temp_str, "USER") == 0){
            if(strcmp(temp_str2,"USER anonymous")==0){
                send_const_message(connfd,"331 Guest login ok, send your complete e-mail address as password.\r\n");
                break;
            }
            else{
                send_const_message(connfd,"553 you should be anonymous.\r\n");
                break;
            }
        }
    }

    while(1){
        //get password
        char temp_str[MAXSIZE] = {""};
        bytes = get_message(connfd,buffer,sizeof(buffer));
        strcpy(password,buffer);
        password[bytes] = '\0';

        //checking the password
        strncpy(temp_str,password,4);
        if(strcmp(temp_str, "PASS") == 0){
            send_const_message(connfd,"230-\r\n"
                    "230-Welcome to\r\n"
                    "230- School of Software\r\n"
                    "230- FTP Archives at ftp.ssast.org\r\n"
                    "230-\r\n"
                    "230-This site is provided as a public service by School of\r\n"
                    "230-Software. Use in violation of any applicable laws is strictly\r\n"
                    "230-prohibited. We make no guarantees, explicit or implicit, about the\r\n"
                    "230-contents of this site. Use at your own risk.\r\n"
                    "230-\r\n"
                    "230 Guest login ok, access restrictions apply.\r\n");
            break;
        }
        else{
            send_const_message(connfd,"430 this is invaid password.\r\n");
            continue;
        }
    }
    return 0;
}


/************************************************8
 * main process of server operation
 * @param connfd socket
 */
void serve_process(int connfd)
{
    //ftp connect message
    char buffer[MAXSIZE];
    int mode = 0;
    char old_filename[MAXSIZE];
    int file_rename_flag = 0;
    //welcome login message
    send_const_message(connfd,"220 Anonymous FTP server ready.\r\n");

    serve_login(connfd);

    while(1){
        get_message(connfd,buffer,sizeof(buffer));
        if(file_rename_flag == 1 && !check_string(buffer,"RNTO",4)) {
            send_const_message(connfd, "503 you should input RNTO\r\n");
            continue;
        }

        if(check_string(buffer, "SYST", 4)){
            send_const_message(connfd,"215 UNIX Type: L8\r\n");
        }
        else if(check_string(buffer, "QUIT", 4)){
            send_const_message(connfd,"221 Goodbye.\r\n");
            break;
        }
        else if (check_string(buffer, "TYPE I", 6)) {
            send_const_message(connfd, "200 Type set to I.\r\n");
        }
        else if(check_string(buffer, "PORT", 4)){
            if (run_port_mode(buffer) < 0) {
                send_const_message(connfd, "500 error PORTcommand.\r\n");
                continue;
            }
            else {
                mode = 1;
                send_const_message(connfd, "200 PORT command successful.\r\n");
            }
        }
        else if(check_string(buffer, "PASV", 4)){
            if (run_pasv_mode(connfd) < 0) {
                send_const_message(connfd, "500 error PASVcommand.\r\n");
                break;
            }
            else {
                mode = 2;
            }
        }
        else if(check_string(buffer, "RETR", 4)) {
            if(mode == 1) {
                if (port_connect() < 0) {
                    send_const_message(connfd, "425 No connect was estabished.\r\n");
                    close(Port_sockfd);
                    Port_sockfd = -1;
                    send_const_message(connfd, "426 connection was broken or network failure.\r\n");
                    break;
                }
                int t = transfer_file(connfd, Port_sockfd, buffer, sizeof(buffer));
                if (t == 226)
                    send_const_message(connfd,"226 transfer complete.\r\n");
                mode = 0;
                close(Port_sockfd);
                Port_sockfd = -1;
            }
            else if(mode == 2){
                if (pasv_accept() < 0) {
                    send_const_message(connfd, "426 connection was broken or network failure.\r\n");
                    close(Pasv_sockfd);
                    Pasv_sockfd = -1;
                    close(Pasv_listenfd);
                    Pasv_listenfd = -1;
                    break;
                }
                int t = transfer_file(connfd, Pasv_sockfd, buffer, sizeof(buffer));
                if (t == 226)
                    send_const_message(connfd, "226 transfer complete.\r\n");
                close(Pasv_sockfd);
                Pasv_sockfd = -1;
                close(Pasv_listenfd);
                Pasv_listenfd = -1;
                mode = 0;
            }
            else
                send_const_message(connfd,"425 no TCP connection established.\r\n");
        }
        else if(check_string(buffer, "STOR", 4)){
            if(mode == 0)
                send_const_message(connfd,"425 no TCP connection established.\r\n");
            else{
                if(mode == 1) {
                    if (port_connect() < 0) {
                        send_const_message(connfd, "425 No connect was estabished.\r\n");
                        close(Port_sockfd);
                        Port_sockfd = -1;
                        send_const_message(connfd, "426 connection was broken or network failure.\r\n");
                        break;
                    }
                    get_file(connfd,Port_sockfd,buffer);
                    mode = 0;
                    close(Port_sockfd);
                    Port_sockfd = -1;
                }
                else if(mode ==2){
                    if (pasv_accept() < 0) {
                        send_const_message(connfd, "425 No connect was estabished.\r\n");
                        close(Pasv_sockfd);
                        Pasv_sockfd = -1;
                        close(Pasv_listenfd);
                        Pasv_listenfd = -1;
                        break;
                    }
                    get_file(connfd,Pasv_sockfd,buffer);
                    close(Pasv_sockfd);
                    Pasv_sockfd = -1;
                    close(Pasv_listenfd);
                    Pasv_listenfd = -1;
                    mode = 0;
                }

            }
        }

        else if(check_string(buffer, "LIST", 4)){
            if(mode == 0)
                send_const_message(connfd,"503 No mode is estabished(PORT/PASV).\r\n");
            else {
                if (mode == 1) {
                    if (port_connect() < 0) {
                        send_const_message(connfd, "425 No connect was estabished.\r\n");
                        close(Port_sockfd);
                        Port_sockfd = -1;
                        send_const_message(connfd, "426 connection was broken or network failure.\r\n");
                        break;
                    }
                    transfer_list(connfd,Port_sockfd);
                    mode = 0;
                    close(Port_sockfd);
                    Port_sockfd = -1;
                }
                else if (mode == 2) {
                    if (pasv_accept() < 0) {
                        send_const_message(connfd, "425 No connect was estabished.\r\n");
                        close(Pasv_sockfd);
                        Pasv_sockfd = -1;
                        close(Pasv_listenfd);
                        Pasv_listenfd = -1;
                        break;
                    }
                    transfer_list(connfd,Pasv_sockfd);
                    close(Pasv_sockfd);
                    Pasv_sockfd = -1;
                    close(Pasv_listenfd);
                    Pasv_listenfd = -1;
                    mode = 0;
                }
            }
        }
        else if(check_string(buffer, "MKD", 3)){
            create_dir(connfd,buffer);
        }
        else if(check_string(buffer, "RMD", 3)){
            remove_dir(connfd,buffer);
        }
        else if(check_string(buffer, "CWD", 3)){
            change_working_path(connfd,buffer);
        }
        else if(check_string(buffer,"DELE",4)){
            remove_file(connfd,buffer);
        }
        else if(check_string(buffer,"RNFR",4)){
            get_rename_file(old_filename,connfd,buffer,&file_rename_flag);
        }
        else if(check_string(buffer,"RNTO",4)){
            change_rename_file(old_filename,connfd,buffer,&file_rename_flag);
            memset(old_filename,0,sizeof(old_filename));
        }
        else if (check_string(buffer, "null", 4)) {
            send_const_message(connfd, "500 null");
        }
        else {
            send_const_message(connfd, "500 error command.\r\n");
        }
    }
};
