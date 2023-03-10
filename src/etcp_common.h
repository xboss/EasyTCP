#ifndef _ETCP_COMMON_H
#define _ETCP_COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define _ALLOC(v_type, v_element_size) (v_type *)calloc(1, v_element_size)

#define _FREEIF(p)    \
    do {              \
        if (p) {      \
            free(p);  \
            p = NULL; \
        }             \
    } while (0)

#define _LOG(fmt, args...)   \
    do {                     \
        printf(fmt, ##args); \
        printf("\n");        \
    } while (0)

uint64_t getmillisecond();
int setnonblock(int fd);
int setreuseaddr(int fd);
void set_recv_timeout(int fd, time_t sec);
void set_send_timeout(int fd, time_t sec);

#endif  // ETCP_COMMON_H