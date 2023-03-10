#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
static char addr[] = "127.0.0.1";
static uint16_t port = 8888;

static void on_recv(int fd, char *buf, int len) {
    char msg[10000] = {0};
    if (len > 0) {
        memcpy(msg, buf, len);
    }
    _LOG("client on_recv cid: %d len: %d  msg: %s", fd, len, msg);
}
static void on_close(int fd) { _LOG("server on_close cid: %d", fd); }

static void send_cb(struct ev_loop *loop, struct ev_timer *watcher, int revents) {
    if (EV_ERROR & revents) {
        _LOG("send_cb got invalid event");
        return;
    }
    int rt;

    // etcp_cli_conn_t *conn = skcp_get_conn(cli, g_cid);
    etcp_cli_t *cli = (etcp_cli_t *)watcher->data;

    if (cli_fd <= 0) {
        cli_fd = etcp_client_create_conn(cli, addr, port, NULL);
        assert(cli_fd > 0);
        return;
    }
    // connection alive
    char msg[10000] = {0};
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

    sprintf(msg, "hello %lu", clock());
    rt = etcp_client_send(cli, cli_fd, msg, strlen(msg));
    assert(rt >= 0);
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

    etcp_cli_conf_t *conf = malloc(sizeof(etcp_cli_conf_t));
    ETCP_CLI_DEF_CONF(conf);

    conf->on_close = on_close;
    conf->on_recv = on_recv;

    cli = etcp_init_client(conf, loop, NULL);
    assert(cli);

    send_watcher = malloc(sizeof(ev_timer));
    send_watcher->data = cli;
    ev_init(send_watcher, send_cb);
    ev_timer_set(send_watcher, 1, 2);
    ev_timer_start(cli->loop, send_watcher);

    ev_run(loop, 0);

    etcp_free_client(cli);
    free(conf);

    _LOG("test end...");
    return 0;
}
