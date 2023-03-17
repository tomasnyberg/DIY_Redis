// #include  <bits/stdc++.h>
#include <string>
#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <cstring>
#include <fcntl.h>
#include <vector>
#include <poll.h>
#include "util.h"

using namespace std;

const size_t k_max_msg = 4096;

enum {
    STATE_REQ = 0,
    STATE_RES = 1,
    STATE_END = 2,
};

struct Conn {
    int fd = -1;
    uint32_t state = 0;
    size_t rbuf_size = 0;
    uint8_t rbuf[4 + k_max_msg];
    size_t wbuf_size = 0;
    size_t wbuf_sent = 0;
    uint8_t wbuf[4 + k_max_msg];
};

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

static void conn_put(vector<Conn*> &fd2conn, struct Conn *conn){
    if(fd2conn.size() <= (size_t) conn->fd){
        fd2conn.resize(conn->fd + 1);
    }
    fd2conn[conn->fd] = conn;
}

static int32_t accept_new_conn(vector<Conn*> &fd2conn, int fd){
    struct sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *) &client_addr, &socklen);
    if(connfd < 0){
        perror("accept");
        return -1;
    }
    fd_set_nb(connfd);
    struct Conn *conn = (struct Conn *) malloc(sizeof(struct Conn));
    if(!conn){
        close(connfd);
        return -1;
    }
    conn->fd = connfd;
    conn->state = STATE_REQ;
    conn->rbuf_size = 0;
    conn->wbuf_size = 0;
    conn->wbuf_sent = 0;
    conn_put(fd2conn, conn);
    return 0;
}

static void connection_io(Conn *conn){
    if(conn->state == STATE_REQ){
        // state_req(conn); // TODO
    } else if (conn->state == STATE_RES){
        // state_res(conn); // TODO
    } else {
        assert(0); // We shouldn't get here
    }
}

static void state_req(Conn *conn){
    // while(try_fill_buffer(conn)) {} // TODO
}

static bool try_fill_buffer(Conn *conn){
    assert(conn->rbuf_size < sizeof(conn->rbuf));
    ssize_t rv = 0;
    do {
        size_t cap = sizeof(conn->rbuf) - conn->rbuf_size;
        rv = read(conn->fd, &conn->rbuf[conn->rbuf_size], cap);
    } while(rv < 0 && errno == EINTR);
    if(rv < 0 && errno == EAGAIN){
        return false;
    }
    if(rv < 0){
        perror("read");
        conn->state = STATE_END;
        return false;
    }
    if(rv == 0){
        if(conn->rbuf_size > 0){
            fprintf(stderr, "EOF with %zu bytes in buffer", conn->rbuf_size);
        } else {
            fprintf(stderr, "EOF");
        }
        conn->state = STATE_END;
        return false;
    }
    conn->rbuf_size += (size_t) rv;
    assert(conn->rbuf_size <= sizeof(conn->rbuf) - conn-> rbuf_size);
    // while(try_one_request(conn)){} // TODO
    return (conn->state == STATE_REQ);
}

static bool try_one_request(Conn *conn){
    // Try to parse a request from buffer
    if(conn->rbuf_size < 4){
        return false;
    }
    uint32_t len = 0;
    memcpy(&len, conn->rbuf, 4);
    if(len > k_max_msg){
        fprintf(stderr, "Message too long: %u \n", len);
        conn->state = STATE_END;
        return false;
    }
    if(4+len > conn->rbuf_size){
        return false; // Not enough data, retry later
    }
    printf("client says: %.*s\n", len, &conn->rbuf[4]);
    // Echo response
    memcpy(&conn->wbuf[0], &len, 4);
    memcpy(&conn->wbuf[4], &conn->rbuf[4], len);
    conn->wbuf_size = 4 + len;

    // Remove the request from the buffer
    size_t remain = conn->rbuf_size - (4 + len);
    if(remain){
        memmove(conn->rbuf, &conn->rbuf[4+len], remain);
    }
    conn->rbuf_size = remain;
    conn->state = STATE_RES;
    // state_res(conn); // TODO
    return (conn->state == STATE_REQ);
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
    vector<Conn *> fd2conn;
    fd_set_nb(fd); // Set non-blocking on the fd (poll should be our only way to wait for events)
    vector<struct pollfd> poll_args;
    // cout << "Server started listening on port 1234 \n";
    while(true) {
        poll_args.clear();
        struct pollfd pfd = {fd, POLLIN, 0};
        poll_args.push_back(pfd);
        for (Conn *conn : fd2conn){
            if(!conn) continue;
            struct pollfd pfd = {};
            pfd.fd = conn->fd;
            pfd.events = (conn->state == STATE_REQ) ? POLLIN : POLLOUT;
            pfd.events |= POLLERR;
            poll_args.push_back(pfd);
        }
    }
    int rv = poll(poll_args.data(), poll_args.size(), 1000);
    if (rv < 0){
        perror("poll");
        exit(1);
    }
    // Process active connections
    for(size_t i = 1; i < poll_args.size(); i++){
        if(poll_args[i].revents){
            Conn *conn = fd2conn[poll_args[i].fd];
            // connection_io(conn); // TODO
            if(conn->state == STATE_END){
                fd2conn[conn->fd] = nullptr;
                close(conn->fd);
                free(conn);
            }
        }
    }
    if(poll_args[0].revents){
        // accept_new_conn(fd2conn, fd); // TODO
    }
    return 0;
    // while(true){
    //     struct sockaddr_in client_addr = {};
    //     socklen_t socklen = sizeof(client_addr);
    //     int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
    //     if(connfd < 0){
    //         continue;
    //     }
    //     while(true){
    //         int32_t err = one_request(connfd);
    //         if(err){
    //             break;
    //         }
    //     }
    //     close(connfd);
    // }
}
