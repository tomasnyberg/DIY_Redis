#ifndef UTIL_H
#define UTIL_H

#include <arpa/inet.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

int32_t read_full(int fd, char *buf, size_t n);
int32_t write_all(int fd, const char *buf, size_t n);

#endif