// #include  <bits/stdc++.h>
#include <string>
#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

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
    char rbuf[64] = {};
    ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
    if(n < 0){
        perror("read");
        return 0;
    }
    printf("Server says: %s \n", rbuf);
}
