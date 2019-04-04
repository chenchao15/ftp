/****************************************************
*Copyright(C),2017, ChenChao
All Rights Reserved

*FileName:   server.h
*Author:     ChenChao
*Version:    1.0
*Date:       2017-11-1
*Description: Provide the main functions' declaration
 * of this program, including starting PASV or PORT mode,
recieving or send message, file transfer and so on
******************************************************/
#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<ctype.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include<malloc.h>
#include <netdb.h>
#include <sys/utsname.h>
#include <time.h>

#define MAXSIZE 1024

//string matching
int check_string(char *string, const char *buf, int length );
//get ip and port
void get_ip_port(char *str,char *ip, int *port);
//recieve message from client
int get_message(int sockfd,char *buf,int bufsize);
//send const string
int send_const_message(int sockfd,const char buf[MAXSIZE]);
//send string
int send_message(int sockfd, char *buf);
//pasv mode
int pasv_accept();
//port mode
int port_connect();
//run port mode
int run_port_mode(char *buffer);
//run pasv mode
int run_pasv_mode(int connfd);
//send file
int transfer_file(int connfd, int file_sockfd,char *buffer,int bufsize);
//download file
int get_file(int connfd,int file_sockfd,char *buffer);
//send list of file directory
void transfer_list(int sockfd,int file_sock);
//main operating process
void serve_process(int connfd);
//create a directory
int create_dir(int connfd,char buffer[MAXSIZE]);
//remove a directory
int remove_dir(int connfd,char buffer[MAXSIZE]);
//change the current working path of the server
int change_working_path(int connfd,char buffer[MAXSIZE]);
//remove a file
int remove_file(int connfd,char buffer[MAXSIZE]);
//get the filename of file to be renamed
int get_rename_file(char old_filename[MAXSIZE], int connfd,char buffer[MAXSIZE],int *file_rename_flag);
//rename the file
int change_rename_file(char old_filename[MAXSIZE],int connfd,char buffer[MAXSIZE],int *file_rename_flag);

#endif