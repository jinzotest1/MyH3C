MyH3C
=====

### 基本原理
并非基于IP协议通信，而是直接抓取以太网帧，所以套接字采用`AF_PACKET`和`SOCK_RAW`的方式，`protocol`为`0x888E`（参见RFC），直接`bind`到`ethernet interface`（通常是eth0，不过有一些比较奇葩不一样），发送以太网帧。
以太网帧格式如下：

* EAPOL是Payload部分，EAP跟在EAPOL之后，就是我们与服务器通信的核心内容。
* EAPOL如下：
	`版本号（1字节）` `报文类型（1字节）` `长度（2字节）`
* 报文类型有：
	* EAP_PACKET 0x00
	* EAP_START 0x01
	* EAP_LOGOFF 0x02
* EAP如下：
	`通信类型（1字节）` `通信ID（1字节）` ` 数据长度（2字节） ` `协商类型（1字节）`
* 通信类型：
	* EAP_REQUEST 0x01
	* EAP_RESPONSE 0x02
	* EAP_SUCCESS 0x03
	* EAP_FAILURE 0x04
* 协商类型：
	* EAP_TYPE_ID 0x01
	* EAP_TYPE_MD5 0x04

### 流程简述
1. 主机向服务器（多播或广播地址）发送EAPOL-Start；
2. 服务器向主机发送`EAP-REQUEST-ID`要求验证身份的请求；
3. 主机向服务器发送`EAP-RESPONSE-ID`回应；
4. 服务器向主机发送`EAP-REQUEST-MD5_Challenge`要求验证密码的MD5校验值；
5. 主机向服务器发送`EAP-RESPONSE-MD5_Challenge`回应（其实和MD5毫无关系，= =）；
6. 服务器向主机发送`EAP-Success`;
7. 通过Identity-response保持心跳。

### TODOs
1. 注意^的符号问题以及长度问题，还有字符串的某些表示
2. 待机处理问题
