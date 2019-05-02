//
// Copyright(c) 2019 linkingvision.com
// Distributed under Commercial license info@linkingvision.com
//
#include <thread>
#include <mutex>
#include <chrono>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h> /* DTL add for SO_EXCLUSIVE */
#include <ws2tcpip.h>
#endif
#include "wsclient.hpp"

/*
 This device demo will connect to url as a channel, the chnum is the channel of this device, 
 this device will connect chnum times to the url.
*/

void Usage()
{
	printf("h5seventclient ip(192.168.100.101) port(8080) \n");
	return;
}

int main(int argc, char *argv[])
{
#ifdef _WIN32
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
#endif

	if (argc < 3)
	{
		Usage();
		return 0;
	}

	std::string strIp = argv[1];
	std::string strPort = argv[2];
	std::string strSession = "c1782caf-b670-42d8-ba90-2244d0b0ee83";
	std::string strPath = "/api/v1/h5seventapi?session=" + strSession;

	WSClient *pClient = new WSClient(strIp, strPort, strPath);

	pClient->StartKeepThread();

	int j = 0;
	while (1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		j ++;

	}
		

}
