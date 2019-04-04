/**********************************************************
*Copyright(C),2017, ChenChao
All Rights Reserved

*FileName:   directory.c
*Author:     ChenChao
*Version:    1.0
*Date:       2017-11-2
*Description: Providing some functions of dircetory operation,
 * including creating directories,deleting directories,changing
 * the current working directory of the server
************************************************************/
#include"server.h"

extern char *tmp;


/**************************************************
 * create a directory when recieving "MKD"
 * @param connfd socket
 * @param buffer "MKD ***(directory)"
 */
int create_dir(int connfd,char buffer[MAXSIZE]){
    int i;
    char dirname[MAXSIZE];
    char *path = (char *)malloc(MAXSIZE);
    char *message = (char *)malloc(MAXSIZE);
    for(i=4;buffer[i]!='\0';i++){
        dirname[i-4] = buffer[i];
    }
    dirname[i-4] = '\0';
    strcpy(path,tmp);
    strcat(path,dirname);

    strcpy(message,"257 build at ");
    strcat(message,path);
    if(mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)<0)
        send_const_message(connfd,"550 File can't be estabished.\r\n");
    else
        send_const_message(connfd,"250 Create successful.\r\n");
    return 0;
}


/**************************************************
 * delete a directory when recieving "RMD"
 * @param connfd socket
 * @param buffer "RMD ***(directory)"
 */
int remove_dir(int connfd,char buffer[MAXSIZE]){
    int i;
    char dirname[MAXSIZE];
    char *path = (char*)malloc(MAXSIZE);
    for(i=4;buffer[i]!='\0';i++){
        dirname[i-4] = buffer[i];
    }
    dirname[i-4] = '\0';
    strcpy(path,tmp);
    strcat(path,dirname);
    if(rmdir(path)<0)
        send_const_message(connfd,"550 File can't be deleted.\r\n");
    else
        send_const_message(connfd,"250 Delete successful.\r\n");
    return 0;
}


/********************************************************
 * change the current working path when recieving "CWD"
 * @param connfd socket
 * @param buffer "CWD ***(path)"
 */
int change_working_path(int connfd,char buffer[MAXSIZE]){
    int i;
    char dirpath[MAXSIZE];
    for(i=4;buffer[i]!='\0'&&buffer[i]!='\r'&&buffer[i]!='\n';i++){
        dirpath[i-4] = buffer[i];
    }
    dirpath[i-4] = '\0';
    int t = access(dirpath,0);
    if(t<0)
        send_const_message(connfd,"550 No such file or directory.\r\n");
    else {
        memset(tmp,0,strlen(tmp));
        strcpy(tmp,dirpath);
        tmp[strlen(tmp)] = '/';
        tmp[strlen(tmp)] = '\0';
        send_const_message(connfd,"250 Okay.\r\n");
    }
    return 0;
}