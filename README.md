# raw_socket_demo

一个简单的，基于 raw socket，自定义包头发送 IPv4 UDP 包的示例

## Usage

```
make build
sudo ./server
```

发出 8 个具有不同 TOS 段的 UDP 包，可使用 wireshark 进行捕获

## Note

Tested on Mac OS

目前写死的 1.2.3.4 作为收发的 ip

## REF

- [A brief programming tutorial in C for raw sockets](http://www.cs.binghamton.edu/~steflik/cs455/rawip.txt): 一个英文的介绍如何使用 C 发出 raw socket 包的文章，本示例的主要参考对象
- [用 Raw socket 自己构造数据包头部](https://blog.csdn.net/ExcaliburXK/article/details/7307324): 类似上面的中文文章，有多余的不必要内容
- [IPv4 wikipedia](https://en.wikipedia.org/wiki/IPv4): 对照 IPv4 结构方便理解 C 库的缩写
