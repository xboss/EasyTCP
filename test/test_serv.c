#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "easy_tcp.h"

#define _LOG(fmt, args...)   \
    do {                     \
        printf(fmt, ##args); \
        printf("\n");        \
    } while (0)

static etcp_serv_t *serv = NULL;

static int on_accept(int fd) {
    _LOG("server accept fd: %d", fd);
    return 0;
}
static void on_recv(int fd, char *buf, int len) {
    char msg[10000] = {0};
    if (len > 0) {
        memcpy(msg, buf, len);
    }
    _LOG("server on_recv fd: %d len: %d  msg: %s", fd, len, msg);
    int rt = etcp_server_send(serv, fd, msg, strlen(msg));
    assert(rt >= 0);
}
static void on_close(int fd) { _LOG("server on_close fd: %d", fd); }

/* -------------------------------------------------------------------------- */
/*                                    main                                    */
/* -------------------------------------------------------------------------- */
int main(int argc, char const *argv[]) {
    _LOG("test start...");

#if (defined(__linux__) || defined(__linux))
    struct ev_loop *loop = ev_loop_new(EVBACKEND_EPOLL);
#elif defined(__APPLE__)
    struct ev_loop *loop = ev_loop_new(EVBACKEND_KQUEUE);
#else
    struct ev_loop *loop = ev_default_loop(0);
#endif

    etcp_serv_conf_t *conf = malloc(sizeof(etcp_serv_conf_t));
    ETCP_SER_DEF_CONF(conf);
    conf->serv_addr = "127.0.0.1";
    conf->on_accept = on_accept;
    conf->on_recv = on_recv;
    conf->on_close = on_close;
    conf->r_buf_size = 128;

    if (argc == 3) {
        if (argv[1]) {
            conf->serv_addr = (char *)argv[1];
        }
        if (argv[2]) {
            conf->serv_port = atoi(argv[2]);
        }
    }

    serv = etcp_init_server(conf, loop, NULL);
    assert(serv);

    ev_run(loop, 0);

    etcp_free_server(serv);
    free(conf);

    _LOG("test end...");
    return 0;
}
