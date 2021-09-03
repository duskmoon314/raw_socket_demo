# socket_demo

一个简单的，基于 setsockopt ，自定义包头 TOS 段发送 IPv4 UDP 包的示例

## Usage

```
make run
```

发出 8 个具有不同 TOS 段的 UDP 包，可使用 wireshark 进行捕获

## Note

Tested on Mac OS

### server client

在 sys/socket.h 中提供 setsockopt 方法，可以修改当前 socket 发出包时包头中的字段。可配置字段在 netinet/in.h 中有列出，不同协议可以配置的部分不一样，所以需要参考协议标准和源码进行选择。

对于 IPv4 + UDP 的使用情况，仅需要配置 TOS 字段，其余部分和基础的使用 UDP 一致。只需加上如下几行配置 TOS 即可：

```c
if (setsockopt(socket_fd, IPPROTO_IP, IP_TOS, &t, sizeof(int)) < 0)
      printf("Warning: Cannot set TOS!\n");
```

其中 `t` 为 int 类型变量，存的是要写入 tos 字段的值。

仓库中的 `opt_server` 和 `opt_client` 实现了一个基本示例。client 会向 server 发出两个数字，server 计算并映射为 tos，发回给 client 的包中使用计算出的 tos。使用 tshark 抓包结果类似：

```
User Datagram Protocol, Src Port: 57950, Dst Port: 9000 0
User Datagram Protocol, Src Port: 9000, Dst Port: 57950 0
User Datagram Protocol, Src Port: 57950, Dst Port: 9000 0
User Datagram Protocol, Src Port: 9000, Dst Port: 57950 8
User Datagram Protocol, Src Port: 57950, Dst Port: 9000 0
User Datagram Protocol, Src Port: 9000, Dst Port: 57950 16
User Datagram Protocol, Src Port: 57950, Dst Port: 9000 0
User Datagram Protocol, Src Port: 9000, Dst Port: 57950 24
User Datagram Protocol, Src Port: 57950, Dst Port: 9000 0
User Datagram Protocol, Src Port: 9000, Dst Port: 57950 32
User Datagram Protocol, Src Port: 57950, Dst Port: 9000 0
User Datagram Protocol, Src Port: 9000, Dst Port: 57950 40
User Datagram Protocol, Src Port: 57950, Dst Port: 9000 0
User Datagram Protocol, Src Port: 9000, Dst Port: 57950 48
User Datagram Protocol, Src Port: 57950, Dst Port: 9000 0
User Datagram Protocol, Src Port: 9000, Dst Port: 57950 56
```

从 9000 端口发出的是 server 发的包，最后显示的数字对应 TOS 段的高 6bit

## REF

- [A brief programming tutorial in C for raw sockets](http://www.cs.binghamton.edu/~steflik/cs455/rawip.txt): 一个英文的介绍如何使用 C 发出 raw socket 包的文章，本示例的主要参考对象
- [用 Raw socket 自己构造数据包头部](https://blog.csdn.net/ExcaliburXK/article/details/7307324): 类似上面的中文文章，有多余的不必要内容
- [IPv4 wikipedia](https://en.wikipedia.org/wiki/IPv4): 对照 IPv4 结构方便理解 C 库的缩写
- [Advanced programming in the UNIX environment](https://en.wikipedia.org/wiki/Advanced_Programming_in_the_Unix_Environment): 经典的 Unix 环境 C 编程指南，对 socket 各种 api 都有所介绍，对照源码进行学习收获颇丰
