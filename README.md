# EasyTCP
EasyTCP is an asynchronous tcp library implemented in c language, including server and client implementations.

## 状态
“又不是不能用”，应用在[SKCPTUN](https://github.com/xboss/skcptun)

## 环境
运行环境：Linux，MacOS

依赖库：[libev](https://github.com/enki/libev)

## 使用
客户端代码中需要实现etcp_cli_conf_t中的：
```
void (*on_recv)(int fd, char *buf, int len);
void (*on_close)(int fd);
```

服务端代码中需要实现etcp_serv_conf_t中的：
```
int (*on_accept)(int fd);
void (*on_recv)(int fd, char *buf, int len);
void (*on_close)(int fd);
```
编译测试代码：
```
cd EasyTCP
mkdir build
make
```
