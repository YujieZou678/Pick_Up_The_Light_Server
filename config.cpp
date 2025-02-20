/*
func: 全局变量的定义。
author: zouyujie
date: 2024.12.2
*/
#include "config.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>

#include <iomanip>
#include <chrono>
#include <sstream>

/* 函数的实现 */
int my_recv(int fd, void* buf, int length, int flags)
{
    int bytes_left;
    int bytes_read;

    bytes_left = length;
    char* ptr = static_cast<char*>(buf);

    while (bytes_left > 0) {
        bytes_read = recv(fd, ptr, bytes_left, flags);
        if (bytes_read < 0) {
            if (errno == EINTR) bytes_read = 0;
            else return (-1);
        } else if (bytes_read == 0) {
            break;
        }

        bytes_left -= bytes_read;
        ptr += bytes_read;
    }

    return (length-bytes_left);
}

int my_send(int fd, void* buf, int length, int flags)
{
    int bytes_left;
    int bytes_written;

    bytes_left = length;
    char* ptr = static_cast<char*>(buf);

    while (bytes_left > 0) {
        bytes_written = send(fd, ptr, bytes_left, flags);
        if (bytes_written < 0) {
            if (errno == EINTR) bytes_written = 0;
            else return (-1);
        } else if (bytes_written == 0) {
            break;
        }

        bytes_left -= bytes_written;
        ptr += bytes_written;
    }

    return (length-bytes_left);
}
