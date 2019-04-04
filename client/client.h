/****************************************************
*Copyright(C),2017, ChenChao
All Rights Reserved

*FileName:   client.h
*Author:     ChenChao
*Version:    2.0
*Date:       2017-11-3
*Description: Provide the main functions of this
program, including starting PASV or PORT mode,
recieving or send message, file transfer and so on
******************************************************/
#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include<sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<ctype.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 8192

/*the state of user*/
#define UNLOGIN 0
#define NEEDPASS 1
#define LOGIN 2
#define PASV 3
#define PORT 4

//create a socket
int socket_create(int port,const char *ip);
//start port mode
int run_port_mode(int port, const char *ip);
//start pasv mode
int run_pasv_mode(int port, const char *ip);
//receive file from server
int get_file(char *res,int connfd);
//receive messge from server
int get_message(int sockfd,char *buf,int bufsize);
//send message to server
int send_message(int sockfd,char *buf,int len);
//get ip from message sent by server
void get_ipstring(char *buffer, char *str);
//get port
int get_ip_port(char *buffer, char *ip);
//matching string
int check_string(char *string, const char *buf, int length);
//recieving list of file directory
int recv_list(int file_sockfd);
//check the message from server
int check_two_message(char buffer[MAXSIZE]);

#endif
