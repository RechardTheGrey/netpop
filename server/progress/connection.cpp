#include "../util/sysDefine.h"
#include "connection.h"
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
using namespace std;

static void getConnect (struct connection** head, const char* path)
{
	ifstream in (path);
	string line;

	struct connection* ct = *head;

	// 读取第一行的名称
	getline (in, line);
	
	struct sockaddr_in localaddr, remoteaddr;
	while (getline (in, line)) {
		char local[MAX_LINE], remote[MAX_LINE];
		int inode;
		unsigned int port, remotePort;
		if (sscanf (line.c_str (), "%*s %[^:]:%X %[^:]:%X %*s %*s %*s %*s %*s %*s %d %*s", local, &port, remote, &remotePort, &inode) != 5) {
			NET_POP_EXCEPTION ("sscanf %s failed, %s, %d", line.c_str (), __FILE__, __LINE__);
		}
		
		// 此处ip地址是按照网络字节序排列的
		struct connection* temp = new struct connection;
		sscanf (local, "%X", &((struct sockaddr_in*)&localaddr)->sin_addr.s_addr);
		inet_ntop (AF_INET, &localaddr.sin_addr, local, sizeof (local));
		sscanf (remote, "%X", &((struct sockaddr_in*)&remoteaddr)->sin_addr.s_addr);
		inet_ntop (AF_INET, &remoteaddr.sin_addr, remote, sizeof (remote));

		temp->_inode = inode;
		temp->_ip = local;
		temp->_remIp = remote;

		// 此处端口是按照机器字节序排列的
		temp->_port = port;
		temp->_remPort = remotePort;
		temp->_next = NULL;

		if (*head == NULL) {
			*head = temp;
			ct = *head;		
		}
		else {
			while (ct->_next != NULL) {
				ct = ct->_next;			
			}
			ct->_next = temp;
		}
	}	
}

void getTcpCt (struct connection** head)
{
	getConnect (head, TCP_CONNECTION_FILE);
}

void getUdpCt (struct connection** head)
{
	getConnect (head, UDP_CONNECTION_FILE);
}
