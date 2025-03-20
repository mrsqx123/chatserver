#本项目是一个支持集群部署的高性能聊天服务器系统，基于 Muduo 网络库 提供高并发网络 I/O 服务，结合 Nginx TCP 负载均衡 实现横向扩展，利用 Redis 发布订阅 解决跨服务器消息同步问题，通过 MySQL 实现用户数据持久化。

#配套工具：
  Ubuntu20.04操作系统
  CMAKE3.30.5
  Nginx1.16.1
  Redis
  MySQL8.0.41

#编译流程： 
  ./make.sh    #输出文件在bin文件夹下
