# socket_demo

一个简单的，基于 setsockopt ，自定义包头 TOS 段发送 IPv4 UDP 包的示例

## Usage

```
mkdir dist
mkdir logs
make gen
make test
```

`make gen` 会调用 `gen_trace.py` 在根目录下生成 `trace.txt`，其中有 100 条格式如下的 DTP traces：

```
<send_time_gap ([0, 1] 间浮点数，demo中未使用)>    <deadline (ms 整型，小于 0x5265C00 一天)>    <block_size (Byte 整型，小于 0xFFFF)>    <priority (整型，小于 10000)>
```

> 使用 [simonkorl/dtp_utils](https://github.com/simonkorl/dtp_utils) 的 C 代码对 trace 进行读取。

`make test` 会编译 `dtp_server.c` 和 `dtp_client.c` 并运行。两个的输出结果重定向到 `logs/server.log` 和 `logs/client.log`。

`dtp_server` 读取 trace 并在 client 发送来一条数据触发后根据 trace 发若干个长度不超过 1350 的 UDP 包，其中 TOS 字段根据 trace 进行设置。`dtp_server` 对每条 trace 的处理与发送过程中会输出类似如下的内容：

```
================
Client msg: 256, 2566, 268435456
trace 32151680 481 9440 4
Server message sent.
Server message sent.
Server message sent.
Server message sent.
Server message sent.
Server message sent.
Server message sent.
```

内容包含：与前一条 trace 的分割线、来自 client 的触发用信息、本条 trace 的各项（ddl prio blk_size）和计算得到的优先级、每次发包后输出的通知信息。

`dtp_client` 同样读取 trace，并发送一个包用于触发 `dtp_server` 的一轮数据发送。在每轮接收到第一个包后，将包中内容取出，和读取的 trace 进行比对，同时计算要收并扔弃的剩下包的数量。其输出内容如下：

```
================
Client message sent.
Server : 1350 Server Msg32151680 481 9440 4###
correct
dump1 dump2 dump3 dump4 dump5 dump6
```

内容包含：与前一条 trace 的分割线、发送触发用包后的通知信息、来自 server 的第一个包内容、与 trace 比对是否正确、收并扔弃剩下包的通知信息。

在 `dtp_client` 最后会输出 100 条 trace 的比对数据（本机测试应该不太可能会有错）

> 现在使用 UDP 进行收发包，似乎 libc 不存在接口直接获取收到的 IPv4 包中 TOS 字段，所以将内容放在 data 段进行收发。这或许非常不合理，不能取到真正正确的，尽是权宜之计。更准确的方法是使用 wireshark，不过我尚不太会用。

## Note

Tested on Mac OS

### server client

在 sys/socket.h 中提供 setsockopt 方法，可以修改当前 socket 发出包时包头中的字段。可配置字段在 netinet/in.h 中有列出，不同协议可以配置的部分不一样，所以需要参考协议标准和源码进行选择。

```c
// netinet/in.h
/*
 * Options for use with [gs]etsockopt at the IP level.
 * First word of comment is data type; bool is stored in int.
 */
#define IP_OPTIONS              1    /* buf/ip_opts; set/get IP options */
#define IP_HDRINCL              2    /* int; header is included with data */
#define IP_TOS                  3    /* int; IP type of service and preced. */
#define IP_TTL                  4    /* int; IP time to live */
...
```

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

###

## REF

- [A brief programming tutorial in C for raw sockets](http://www.cs.binghamton.edu/~steflik/cs455/rawip.txt): 一个英文的介绍如何使用 C 发出 raw socket 包的文章，本示例的主要参考对象
- [用 Raw socket 自己构造数据包头部](https://blog.csdn.net/ExcaliburXK/article/details/7307324): 类似上面的中文文章，有多余的不必要内容
- [IPv4 wikipedia](https://en.wikipedia.org/wiki/IPv4): 对照 IPv4 结构方便理解 C 库的缩写
- [Advanced programming in the UNIX environment](https://en.wikipedia.org/wiki/Advanced_Programming_in_the_Unix_Environment): 经典的 Unix 环境 C 编程指南，对 socket 各种 api 都有所介绍，对照源码进行学习收获颇丰
