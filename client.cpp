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
using namespace std;

const size_t k_max_msg = 4096;

static int32_t query(int fd, const char *text){
    uint32_t len = (uint32_t) strlen(text);
    if (len > k_max_msg){
        return -1;
    }
    char wbuf[4 + k_max_msg];
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], text, len);
    if(int32_t err = write_all(fd, wbuf, 4 + len)){
        return err;
    }
    char rbuf[4 + k_max_msg + 1];
    errno = 0;
    int32_t err = read_full(fd, rbuf, 4);
    if(err){
        if(errno == 0){
            perror("EOF");
        } else {
            perror("read");
        }
        return err;
    }
    memcpy(&len, rbuf, 4);
    if(len > k_max_msg){
        fprintf(stderr, "Message too long: %u \n", len);
        return -1;
    }
    err = read_full(fd, &rbuf[4], len); // Request body
    if(err){
        perror("read() error");
        return err;
    }
    rbuf[4 + len] = '\0';
    printf("Server says: %s \n", &rbuf[4]);
    return 0;
}

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);
    int rv = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
        perror("connect");
        exit(1);
    }
    query(fd, "hello");
}
