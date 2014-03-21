MyH3C
=====

流程简述：
1. 主机向服务器（多播或广播地址）发送EAPOL-Start；
2. 服务器向主机发送EAP-REQUEST-ID要求验证身份的请求；
3. 主机向服务器发送EAP-RESPONSE-ID回应；
4. 服务器向主机发送EAP-REQUEST-MD5_Challenge要求验证密码的MD5校验值；
5. 主机向服务器发送EAP-RESPONSE-MD5_Challenge回应；
6. 服务器向主机发送EAP-Success;
7. 保持连接的通信。

并非基于IP协议通信，而是直接抓取以太网帧，所以套接字采用AF_PACKET和SOCK_RAW的方式，protocol为0x888E（参见RFC），直接bind到ethernet interface，发送以太网帧。
以太网帧格式如下：

EAPOL是Payload部分，EAP跟在EAPOL之后，就是我们与服务器通信的核心内容。

EAPOL如下：
	版本号（1字节） 报文类型（1字节） 长度（2字节）
报文类型有：
	EAP_PACKET		0x00
	EAP_START		0x01
	EAP_LOGOFF		0x02
长度是指其后EAP的长度。

EAP如下：
	通信类型（1字节） 通信ID（1字节） 数据长度（2字节） 协商类型（1字节）
通信类型：
	EAP_REQUEST			0x01
	EAP_RESPONSE		0x02
	EAP_SUCCESS			0x03
	EAP_FAILURE			0x04
协商类型：
	EAP_TYPE_ID			0x01
	EAP_TYPE_MD5		0x04

# TODO: 注意^的符号问题以及长度问题，还有字符串的某些表示
