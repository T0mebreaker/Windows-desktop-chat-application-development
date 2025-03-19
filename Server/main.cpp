#include<iostream>
#include<map>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/errno.h>
#include<sys/stat.h>
#include<arpa/inet.h>
#include<pthread.h>
using namespace std;

map<int, string> m_clients; //存储socket和名称的映射关系

void* RecvMSG(void* param) {
	pthread_detach(pthread_self());
	long long cli = (long long)param;
	//通知所有客户端
	for (auto i : m_clients) {
		if (i.first == cli) continue;
		string tm = "1:";
		tm += (m_clients[cli] + ":加入聊天室");
		send(i.first, tm.data(), tm.size(), 0);
	}
	//向新客户端发送已有用户
	string tn = "4:";
	for (auto i : m_clients) {
		if (i.first == cli) continue;
		tn += (i.second + ":");
	}
	send(cli, tn.data(), tn.size(), 0);

	for (auto i : m_clients) {
		if (i.first == cli) continue;
		int len = send(cli, i.second.data(), i.second.size(), 0);
		if (len != i.second.size()) {
			cout << i.second << ":发送出错" << endl;
		}
	}
	char msg[0xFF];
	while (1) {
		int len = recv(cli, msg, sizeof(msg), 0);
		//正常接收，转发消息
		if (len > 0) {
			for (auto i : m_clients) {
				if (i.first == cli) continue;
				string tm = "3:" + m_clients[cli] + ':';
				tm += msg;
				send(i.first, tm.data(), tm.size(), 0);
			}
			continue;
		}
		//客户端断连，通知
		for (auto i : m_clients) {
			if (i.first == cli) continue;
			string exitMsg = "2:";
			exitMsg += (m_clients[cli] + ":退出聊天室");
			send(i.first, exitMsg.data(), exitMsg.size(), 0);

		}
		cout << m_clients[cli] << ":退出聊天室" << endl;
		m_clients.erase(cli);
		close(cli);
		break;
	}
	pthread_exit(0);
	return 0;
}

int main() {
	int sockSev = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addrSev;
	addrSev.sin_family = AF_INET;
	addrSev.sin_port = htons(9999);
	addrSev.sin_addr.s_addr = INADDR_ANY;
	bind(sockSev, (sockaddr*)&addrSev, sizeof(addrSev));
	listen(sockSev, 5);
	cout << "服务器启动成功！" << endl;
	while (true) {
		sockaddr_in addrCli;
		socklen_t len = sizeof(addrCli);
		int sockCli = accept(sockSev, (sockaddr*)&addrCli, &len);
		if (sockCli == -1) {
			cout << inet_ntoa(addrCli.sin_addr) << ":连接失败！" << endl;
			continue;
		}
		char msg[20];
		len = recv(sockCli, msg, 20, 0);
		if (len <= 0) {
			close(sockCli);
			cout << inet_ntoa(addrCli.sin_addr) << ":接收数据失败！" << endl;
		}
		m_clients.insert(pair<int, string>(sockCli, msg));
		cout << msg << ":进入聊天室！" << endl;

		//为新客户端开启线程接收信息
		pthread_t tid;
		pthread_create(&tid, 0, RecvMSG, (void*)sockCli);
	}
	close(sockSev);
}