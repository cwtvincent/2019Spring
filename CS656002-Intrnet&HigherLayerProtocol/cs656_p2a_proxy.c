#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>

const int HTML_SIZE = 70000;
const int MAX_BUF_SIZE = 65535;
const int IP_BUF_SIZE = 32;
const int HTTP_BUF_SIZE = 512;

int strsize(char* a){
    int i;
    for(i = 0; i != 100; i++){
        if(a[i] == '\0')
            return i;
    }
    return -1;
}
int doParse(int clntsockid, char* browser_req){
    int i, browser_req_size;
    char protocol[12] = "GET http://\0", protocol2[9] = "HTTP/1.1\0";

    if((browser_req_size = recv(clntsockid, browser_req, MAX_BUF_SIZE, 0)) < 0) {perror("socket error"); return -1;}
    if(browser_req_size > MAX_BUF_SIZE){return -1;}
    if(browser_req_size < 11) return -1;
    for(i = 0; i != 11; i++){
        if(browser_req[i] != protocol[i])
            return -1;
    }
    for(i = 0; i != 8; i++){
        if((strchr(browser_req, '\n') - 9)[i] != protocol2[i])
            return -1;
    }
    return browser_req_size;
}

long long preferTime(char* ip, int ai_family){
    struct timeval start_t, end_t;
    struct sockaddr_in addr;
    int sockid;
    if(ai_family != 2) return -1;
    if((sockid = socket(PF_INET, SOCK_STREAM, 0)) < 0) {perror("socket error"); exit(1);}
    gettimeofday(&start_t, NULL);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr(ip);
    if(connect(sockid, (struct sockaddr *) &addr, sizeof(addr)) < 0){perror("socket error"); return -1; exit(1);} 
    gettimeofday(&end_t, NULL); 
    if(close(sockid) < 0) {perror("socket error"); exit(1);}
    if(end_t.tv_usec - start_t.tv_usec < 0)
        return 10000000 - start_t.tv_usec + end_t.tv_usec;
    return end_t.tv_usec - start_t.tv_usec;
}

int dns(char *browser_req ,char* prefer_ip){
    long long min_t = -1, t;
    struct addrinfo *ip_addr;
    char web_ip_addr[IP_BUF_SIZE], na[100];

    if((getaddrinfo(browser_req, NULL, NULL, &ip_addr)) < 0){perror("getaddrinfo");return -1;}
    if(getnameinfo(ip_addr->ai_addr, sizeof(struct sockaddr), na, 100, NULL, 0, 0) < 0){perror("getnameinfo");return -1;};

    if(strcmp(na, browser_req) == 0)
        return -1;
    if(ip_addr){
        while(ip_addr){    
            memset(web_ip_addr, 0, IP_BUF_SIZE);;
            if(ip_addr->ai_family == 2){
                if(inet_ntop(AF_INET, &((struct sockaddr_in *)ip_addr->ai_addr)->sin_addr, web_ip_addr, IP_BUF_SIZE) == NULL){perror("inet_ntop"); return -1;}
            }
            else{
                if(inet_ntop(AF_INET6, &((struct sockaddr_in6 *)ip_addr->ai_addr)->sin6_addr, web_ip_addr, IP_BUF_SIZE) == NULL){perror("inet_ntop"); return -1;}
            }     
            t = preferTime(web_ip_addr, ip_addr->ai_family); 
            if(t != -1 && (min_t == -1 || min_t > t)){
                strcpy(prefer_ip, web_ip_addr);
                min_t = t;
            }
            ip_addr = ip_addr->ai_next;
        }  
    }  
    else{return -1; exit(1);}
    return 0;    
}

void doHTTP(char *browser_req, char *html_add_line, int clntsockid, char *p_ip){    
    int size = 0, html_size = 0, tmps = 0, i = 0;
    char full_html[HTML_SIZE], add_line[3][11] = {" HOSTIP = \0", " PORT = \0", " PATH = \0"};
    
    memset(full_html, 0, HTML_SIZE);
    strcat(full_html, "<!DOCTYPE html><html><head></head><body>");
    while(browser_req[html_size] != '\0'){ 
        strcat(full_html, "<p>");
        tmps = strchr(browser_req + html_size, '\n') - browser_req - html_size;
        strncat(full_html, browser_req + html_size, tmps-1);        
        strcat(full_html, "</p>");
        html_size += tmps + 1; 
    }
    //printf("full_html = %s\n", full_html);
    for(i = 0; i != 3; i++){ 
        strcat(full_html, "<font color=red><p>&nbsp;");
        strcat(full_html, add_line[i]);
        strcat(full_html, html_add_line + i * HTTP_BUF_SIZE);
        if(i == 0){
            strcat(full_html, " (");
            strcat(full_html, p_ip);
            strcat(full_html, ")");
            size += 3 + strsize(p_ip);
        }
        strcat(full_html, "</p>");        
    }
    strcat(full_html, "</body></html>");
    //printf("full_html2 = %s\n", full_html);
    for(i = strstr(full_html, "</html>") - full_html + 7; i < HTML_SIZE; i++) full_html[i] = ' ';
    if(send(clntsockid, full_html, HTML_SIZE, 0) < 0){perror("socket error");}
    if(close(clntsockid) < 0){perror("socket error");}
}

void getAddr(char* header, char *html_add_line){
    int size, size2, size3;
    char web[HTTP_BUF_SIZE], port[6] = "80";
    size = strchr(header, '\n') - 9 - header - 1 - 11;//9 "HTTP/1.1", 11 "GET http://"
    size2 = size;
    strncpy(web, strchr(header, ' ') + 8, size);
    size = strchr(web, '/') - web;
    size3 = strchr(web, ':') - web;
    if(size3 < size && size3 > 0){        
        strncpy(html_add_line, web, size3);
        strncpy(port, web+size3+1, size - size3 - 1);
    }
    else{
        strncpy(html_add_line, web, size);  
    }
    strcpy(html_add_line + HTTP_BUF_SIZE, port);
    strncpy(html_add_line + HTTP_BUF_SIZE * 2, web + size, size2 - size);
}

int main(int argc, char *argv[])
{
    char prefer_ip[IP_BUF_SIZE], browser_req[MAX_BUF_SIZE];
    char html_add_line[3][HTTP_BUF_SIZE];
    int browser_req_size;
    int mysockid, queuelen = 0, clntsockid;
    unsigned int clntlen = 0;
    int ip_found = 0;
    struct sockaddr_in myport, clntaddr;

    if(argc != 2){
        printf("You need to input port number only!");
    }
    else{
        if((mysockid = socket(PF_INET, SOCK_STREAM, 0)) < 0) {perror("socket error"); exit(1);}
        myport.sin_family = AF_INET;
        myport.sin_port = htons(atoi(argv[1]));
        myport.sin_addr.s_addr = INADDR_ANY;
        if(bind(mysockid, (struct sockaddr *) &myport, sizeof(myport)) < 0) {perror("socket error"); exit(1);}
        if(listen(mysockid, queuelen) < 0) {perror("socket error"); exit(1);}
        printf("listening on port (%s)\n", argv[1]); 
        while(1){
            memset(html_add_line[0], 0, HTTP_BUF_SIZE);
            memset(html_add_line[1], 0, HTTP_BUF_SIZE);
            memset(html_add_line[2], 0, HTTP_BUF_SIZE);
            memset(browser_req, 0, MAX_BUF_SIZE);
            memset(prefer_ip, 0, IP_BUF_SIZE);
            clntlen = sizeof(clntaddr);
            if((clntsockid = accept(mysockid,(struct sockaddr *)&clntaddr, &clntlen)) < 0) {perror("socket error"); exit(1);}
            browser_req_size = doParse(clntsockid, browser_req);
            if(browser_req_size == -1){
                if(send(clntsockid, "<html><body><p>BAD REQUEST</p></html></body>", 44, 0) < 0){perror("socket error"); exit(1);}
                if(close(clntsockid) < 0){perror("socket error"); exit(1);}
            }
            else{       
                getAddr(browser_req, (char *)html_add_line);
                ip_found = dns(html_add_line[0], prefer_ip);
                if(ip_found == -1) strcpy(prefer_ip, "NO IP FOUND");
                doHTTP(browser_req, (char *)html_add_line, clntsockid, prefer_ip);
            }
        }
        if(close(mysockid) < 0){perror("socket error"); exit(1);}
    }
    return 0;
} 
