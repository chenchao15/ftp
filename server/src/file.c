/**********************************************************
*Copyright(C),2017, ChenChao
All Rights Reserved

*FileName:   file.c
*Author:     ChenChao
*Version:    1.0
*Date:       2017-11-2
*Description: Providing some functions of file operation,
 * including downloading files,uploading files,deleting files,
 * renaming files and sending file list of current working
 * directory.
************************************************************/

#include"server.h"

extern char *tmp;

/********************************************************
 * send file to client when recieving "RETR"
 * @param connfd  socket
 * @param file_sockfd  file transfer socket
 * @param buffer "RETR ****(filename)"
 * @param bufsize the size of buffer
 * @return error code (451 or 426)
 */
int transfer_file(int connfd, int file_sockfd,char *buffer,int bufsize)
{
    char filename[MAXSIZE];
    int i;
    FILE* fd = NULL;
    size_t num_read;
    int codes = 226;
    char data[MAXSIZE];

    for(i=5;buffer[i]!='\r';i++){
        filename[i-5] = buffer[i];
    }
    filename[i-5] = '\0';
    char *paths = (char*)malloc(MAXSIZE);

    strcpy(paths,tmp);
    strcat(paths,filename);

    fd = fopen(paths, "rb+");
    if (!fd) {
        send_const_message(connfd, "551 No such file or dictionary.\r\n");
        return -1;
    }
    else{
        send_const_message(connfd, "150 Opening BINARY mode data connection.\r\n");
        fflush(stdin);
        do
        {
            num_read = fread(data, 1, MAXSIZE, fd);
            if (num_read < 0){
                codes = 451;
            }
            if (send(file_sockfd, data, num_read, 0) < 0){
                codes = 426;
                break;
            }
        }
        while (num_read > 0);
        fclose(fd);
        return codes;

    }
}


/*****************************************************
 * get file from client when recieving "STOR"
 * @param connfd  socket
 * @param file_sockfd  file transfer socket
 * @param buffer "STOR ***(filename)"
 */
int get_file(int connfd,int file_sockfd,char *buffer){
    char filepath[50];
    int i;
    for(i=5;buffer[i]!='\r';i++){
        filepath[i-5] = buffer[i];
    }
    filepath[i-5] = '\0';

    char *paths = (char*)malloc(MAXSIZE);
    strcpy(paths,tmp);
    strcat(paths,filepath);
    FILE *dataFile = fopen(paths, "wb+");
    if (!dataFile) {
        send_const_message(connfd, "552 the server had trouble saving the file to disk.\r\n");
    }
    else {
        send_const_message(connfd,"150 Opening BINARY mode data connection. \r\n");
        int datasize = 0;
        while ((datasize =get_message(file_sockfd,buffer,sizeof(buffer))) > 0) {
            fwrite(buffer, 1, datasize, dataFile);
        }
    }
    fclose(dataFile);
    send_const_message(connfd,"226 Transfer complete.\r\n");
    return 0;
}


/*****************************************************
 * send list of file directory when recieving "LIST"
 * @param sockfd socket
 * @param file_sock file transfer socket
 */
void transfer_list(int sockfd,int file_sock){
    char data[MAXSIZE];
    size_t read;
    FILE* fd;
    char command[MAXSIZE];
    strcpy(command,"ls ");
    strcat(command,tmp);
    strcat(command," -l | tail -n+2 > tmp.txt");

    int rs = system(command);
    if(rs < 0){
        exit(1);
    }

    fd = fopen("tmp.txt","r");
    if(!fd)
        exit(1);
    send_const_message(sockfd,"150 Opening BINARY mode data connection. \r\n");

    fseek(fd,SEEK_SET,0);

    memset(data,0,MAXSIZE);

    while((read = fread(data,1,MAXSIZE,fd))>0){
        if(send(file_sock,data,read,0)<0)
            perror("error");
        memset(data,0,MAXSIZE);
    }
    fclose(fd);
    send_const_message(sockfd,"226,Transfer complete.\r\n");
}


/*******************************************************
 * delete designated file when recieving "DELE"
 * @param connfd socket
 * @param buffer "DELE ***(filename)"
 */
int remove_file(int connfd,char buffer[MAXSIZE]){
    int i;
    char filename[MAXSIZE];
    char *path = (char*)malloc(MAXSIZE);
    for(i=5;buffer[i]!='\r'&&buffer[i]!='\n'&&buffer[i]!='\0';i++){
        filename[i-5] = buffer[i];
    }
    filename[i-5] = '\0';
    strcpy(path,tmp);
    strcat(path,filename);
    if(!remove(path)){
        send_const_message(connfd,"250 Okay.\r\n");
    } else
        send_const_message(connfd,"550 delete failed\r\n");
    return 0;
}


/*****************************************************
 * remember the file to be renamed when recieving "RNFR"
 * @param old_filename the old name of the file
 * @param connfd socket
 * @param buffer "RNFR ***(filename)"
 * @param file_rename_flag a flag that conclude whether
 * user can execute other operation or can only input
 * command RNTO to rename the desgnated file.
 * @return
 */
int get_rename_file(char old_filename[MAXSIZE], int connfd,char buffer[MAXSIZE],int *file_rename_flag){
    int i;
    char *path = (char*)malloc(MAXSIZE);
    for(i=5;buffer[i]!='\r'&&buffer[i]!='\n'&&buffer[i]!='\0';i++){
        path[i-5] = buffer[i];
    }
    path[i-5] = '\0';
    strcpy(old_filename,tmp);
    strcat(old_filename,path);
    if(fopen(old_filename,"rb+")) {
        send_const_message(connfd, "350 Okay,using RNTO to renaming the file.\r\n");
        *file_rename_flag = 1;
    }
    else
        send_const_message(connfd,"550 file not exist.\r\n");
    return 0;
}


/*******************************************************
 * renaming designated file when recieving "RFTO"
 * @param old_filename the old name of the file
 * @param connfd socket
 * @param buffer RNTO ***(filename)"
 * @param file_rename_flag a flag that conclude whether
 * user can execute other operation or can only input
 * command RNTO to rename the desgnated file.
 */
int change_rename_file(char old_filename[MAXSIZE],int connfd,char buffer[MAXSIZE],int *file_rename_flag){
    int i;
    char new_filename[MAXSIZE];
    char *path = (char*)malloc(MAXSIZE);
    for(i=5;buffer[i]!='\r'&&buffer[i]!='\n'&&buffer[i]!='\0';i++){
        path[i-5] = buffer[i];
    }
    path[i-5] = '\0';
    strcpy(new_filename,tmp);
    strcat(new_filename,path);
    if(old_filename[0] == '\0')
        send_const_message(connfd,"550 file not exist.\r\n");
    else if(file_rename_flag == 0)
        send_const_message(connfd,"550 using RNFR before RNTO.\r\n");
    else{
        if(rename(old_filename,new_filename) == 0)
            send_const_message(connfd,"250 0key.\r\n");
        else
            send_const_message(connfd,"550 renaming failed\r\n");
    }
    *file_rename_flag = 0;
    return 0;
}
