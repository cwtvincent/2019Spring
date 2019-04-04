#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

void dns(char* portnum){
    int mysockid, clntsockid, web_addr_size, clntlen, queuelen, web_addr_bufsize = 100, i, count = 1;
    struct sockaddr_in myport, clntaddr;
    struct addrinfo *ip_addr;
    char clnt_addr[INET_ADDRSTRLEN], *my_addr, web_addr[web_addr_bufsize], web_ip_addr[100], *web_addr2, hostname[255];

    if((mysockid = socket(PF_INET, SOCK_STREAM, 0)) < 0) 
        printf("socket ceate error");
    myport.sin_family = AF_INET;
    myport.sin_port = htons(atoi(portnum));
    myport.sin_addr.s_addr = INADDR_ANY;
    if(bind(mysockid, (struct sockaddr *) &myport, sizeof(myport)) == -1) 
        printf("bind fail\n");
    if(listen(mysockid, queuelen) == -1) 
        printf("listen fail\n");
    while(1){
        clntlen = sizeof(clntaddr);
        if((clntsockid = accept(mysockid,(struct sockaddr *)&clntaddr, &clntlen)) < 0) 
            printf("accept fail\n");
        
        gethostname(hostname, 255);
        my_addr = inet_ntoa (*(struct in_addr *)*gethostbyname(hostname)->h_addr_list);
        inet_ntop(AF_INET, &(clntaddr.sin_addr), clnt_addr, INET_ADDRSTRLEN);
        printf("(%i) Incoming client connection from [%s:%s] to me [%s:%s]\n", count, clnt_addr, portnum, my_addr, portnum);
        
        if((web_addr_size = recv(clntsockid, web_addr, web_addr_bufsize, 0)) == -1) 
            printf("recv fail\n");
        web_addr2 = malloc(sizeof(char) *(web_addr_size-2));
        strncpy(web_addr2, web_addr, web_addr_size-2);
        printf("    REQ: %s\n\n", web_addr2);
        if((getaddrinfo(web_addr2, NULL, NULL, &ip_addr)) == -1){
            printf("getaddrinfo fail\n");
            if(send(clntsockid, "NO IP ADDRESSES FOUND\n", 22, 0) == -1)
                printf("send fail");
        }
        if(ip_addr){
            while(ip_addr){          
                char a[] = " IP = ";
                if(ip_addr->ai_family == 2){
                    if(inet_ntop(AF_INET, &((struct sockaddr_in *)ip_addr->ai_addr)->sin_addr, web_ip_addr, sizeof(web_ip_addr)) == NULL)
                        printf("inet_ntop fail\n");
                }
                else{
                    if(inet_ntop(AF_INET6, &((struct sockaddr_in6 *)ip_addr->ai_addr)->sin6_addr, web_ip_addr, sizeof(web_ip_addr)) == NULL)
                        printf("inet_ntop fail\n");
                }
                strcat(a, web_ip_addr);
                strcat(a, "\n");
                for(i = 0; i != sizeof(a) + sizeof(web_ip_addr) + 1; i++){
                    if(a[i] == '\n')
                        break;
                }
                if(send(clntsockid, a, i + 1, 0) == -1)
                    printf("send fail");
                ip_addr = ip_addr->ai_next;
            }  
        }  
        else{
            if(send(clntsockid, "NO IP ADDRESSES FOUND\n", 22, 0) == -1)
                printf("send fail");
        }          
        if(close(clntsockid) == -1)
            printf("socket close error\n");   
        count++; 
    }
    if(close(mysockid) == -1)
        printf("socket close error");
}

int main(int argc, char *argv[])
{
    dns(argv[1]);
} 
