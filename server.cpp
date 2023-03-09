// #include  <bits/stdc++.h>
#include <string>
#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>

using namespace std;

static void do_something(int connfd){
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if(n < 0){
        perror("read");
        return;
    }
    printf("Client says: %s \n", rbuf);

    char wbuf[] = "world";
    n = write(connfd, wbuf, sizeof(wbuf));
    if(n < 0){
        perror("write");
        return;
    }
}

static int32_t read_full(int fd, char *buf, size_t n){
    while (n > 0){
        ssize_t rv = read(fd, buf, n);
        if (rv <= 0){
            return -1;
        }
        assert((size_t)rv <= n);
        n -= (size_t) rv;
        buf += rv;
    }
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
    // cout << "Server stared listening on port 1234 \n";
    while(true){
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
        if(connfd < 0){
            continue;
        }
        do_something(connfd);
        close(connfd);
    }
}
