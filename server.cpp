// #include  <bits/stdc++.h>
#include <string>
#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <cstring>
#include "util.h"
#include <fcntl.h>

using namespace std;

static void fd_set_nb(int fd){
    errno = 0;
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags == -1){
        perror("fcntl");
        exit(1);
    }
    flags |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flags) == -1){
        perror("fcntl");
        exit(1);
    }
}

static int32_t one_request(int connfd){
    char rbuf[4 + k_max_msg + 1];
    errno = 0;
    int32_t err = read_full(connfd, rbuf, 4);
    if(err){
        if(errno == 0){
            perror("EOF");
        } else {
            perror("read");
        }
        return err;
    }
    uint32_t len = 0;
    memcpy(&len, rbuf, 4);
    if (len > k_max_msg){
        fprintf(stderr, "Message too long: %u \n", len);
        return -1;
    }
    err = read_full(connfd, &rbuf[4], len); // Request body
    if(err){
        perror("read() error");
        return err;
    }
    rbuf[4 + len] = '\0';
    printf("Received: %s \n", &rbuf[4]);
    const char reply[] = "world";
    uint32_t reply_len = strlen(reply);
    memcpy(rbuf, &reply_len, 4);
    memcpy(&rbuf[4], reply, reply_len);
    return write_all(connfd, rbuf, 4 + reply_len);
}


int main() {
    int val = 1;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0);
    int rv = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
        perror("bind");
        exit(1);
    }
    rv = listen(fd, SOMAXCONN);
    if (rv) {
        perror("listen()");
        exit(1);
    }
    cout << "Server started listening on port 1234 \n";
    while(true){
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
        if(connfd < 0){
            continue;
        }
        while(true){
            int32_t err = one_request(connfd);
            if(err){
                break;
            }
        }
        close(connfd);
    }
}
