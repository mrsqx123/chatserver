项目描述：开发了一个嵌入式平台和高性能聊天服务器的通信系统，独立完成了嵌入式系统的移植与优化，包括 Uboot 引
导程序、Linux 内核、根文件系统的移植，以及 SDIO RTL8189 WiFi 模块驱动的适配与配置，确保系统在嵌入式平台上稳
定运行并实现高效的无线网络通信。基于 Muduo 网络库实现高并发网络 I/O 服务，结合 Nginx TCP 负载均衡实现横向
扩展，利用 Redis 发布订阅机制解决跨服务器消息同步问题，并通过 MySQL 实现用户数据持久化，支持多用户实时通
信，最终实现了嵌入式平台与聊天服务器集群的无缝集成。

#CMakeLists.txt可以实现对server，client的编译，如果需要交叉编译，需要使用项目中toolchain.cmake文件，配置交叉编译环境。

#使用的工具版本：
  操作系统UBUNTU22.04 MYSQL8 nginx1.6 UBOOT为2020.04  linux版本为5.4.3 builtroot工具为2022.02.8

在sdk_imx6ull中已经配置好makefile编译环境和流程，使用
make uboot  make linux   make buildroot可以直接编译好uboot，linux以及对应的rootfs
镜像文件会存放到work和images中，配置文件会在conf中进行备份。
make buildroot会直接帮你把新加入的模块添加到nfs/rootfs的文件系统中。

make uboot_menuconfig
make linux_menuconfig
make buildroot_menuconfig  
make busybox 
可以自定义配置，并且自动替换配置文件，然后直接编译即可。

这个项目部署是需要把编译好的chatclient放到imx6ull开发板上执行，通过sdio wifi和主机运行的服务器进行tcp通信，这样客户端就可以运行在这个linux开发板上和server通信执行业务。
