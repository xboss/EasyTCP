#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "easy_tcp.h"

#define _LOG(fmt, args...)   \
    do {                     \
        printf(fmt, ##args); \
        printf("\n");        \
    } while (0)

static struct ev_timer *send_watcher = NULL;
static etcp_cli_t *cli = NULL;
static int cli_fd = 0;
static char addr[64] = "127.0.0.1";
static uint16_t port = 8888;

static uint64_t getmillisecond() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t millisecond = (tv.tv_sec * 1000000l + tv.tv_usec) / 1000l;
    return millisecond;
}

static void on_recv(int fd, char *buf, int len) {
    char msg[1024] = {0};
    if (len > 0) {
        memcpy(msg, buf, len);
    }
    char tm[32] = {0};
    int tm_len = strlen(msg) > 31 ? 31 : strlen(msg);
    sprintf(tm, msg, tm_len);
    uint64_t tmi = atoll(tm);
    uint64_t now = getmillisecond();
    _LOG("rtt:%llu", now - tmi);
    // _LOG("client on_recv fd: %d len: %d  msg: %s", fd, len, msg);
}
static void on_close(int fd) {
    _LOG("client on_close fd: %d", fd);
    cli_fd = 0;
}

static void send_cb(struct ev_loop *loop, struct ev_timer *watcher, int revents) {
    if (EV_ERROR & revents) {
        _LOG("send_cb got invalid event");
        return;
    }
    int rt;

    etcp_cli_t *cli = (etcp_cli_t *)watcher->data;

    if (cli_fd <= 0) {
        cli_fd = etcp_client_create_conn(cli, addr, port, NULL);
        assert(cli_fd > 0);
        // return;
        // ev_sleep(10);
    }
    // connection alive
    char msg[1024] = {0};
    // int i = 0;
    // for (; i < 1000; i++) {
    //     msg[i] = 'a';
    // }
    // for (; i < 2000; i++) {
    //     msg[i] = 'b';
    // }
    // for (; i < 3000; i++) {
    //     msg[i] = 'd';
    // }
    // for (; i < 4000; i++) {
    //     msg[i] = 'e';
    // }
    // for (; i < 5000; i++) {
    //     msg[i] = 'f';
    // }
    // for (; i < 6000; i++) {
    //     msg[i] = 'g';
    // }

    sprintf(msg, "%llu", getmillisecond());
    rt = etcp_client_send(cli, cli_fd, msg, strlen(msg));
    assert(rt >= 0);
    // _LOG("msg: %s", msg);
}

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

    if (argc == 3) {
        if (argv[1]) {
            memcpy(addr, argv[1], strlen(argv[1]));
            addr[strlen(argv[1])] = '\0';
        }
        if (argv[2]) {
            port = atoi(argv[2]);
        }
    }

    etcp_cli_conf_t *conf = malloc(sizeof(etcp_cli_conf_t));
    ETCP_CLI_DEF_CONF(conf);

    conf->on_close = on_close;
    conf->on_recv = on_recv;
    conf->r_buf_size = 13;

    cli = etcp_init_client(conf, loop, NULL);
    assert(cli);

    send_watcher = malloc(sizeof(ev_timer));
    send_watcher->data = cli;
    ev_init(send_watcher, send_cb);
    ev_timer_set(send_watcher, 1, 0.01);
    ev_timer_start(cli->loop, send_watcher);

    ev_run(loop, 0);

    etcp_free_client(cli);
    free(conf);

    _LOG("test end...");
    return 0;
}
