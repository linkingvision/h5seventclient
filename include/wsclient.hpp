
#ifndef __WS_CLIENT__
#define __WS_CLIENT__

#include <cstdlib>
#include <thread>
#include <mutex>
#include <string.h>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <map>

#include "civetweb.h"
#define WS_EBUF_LEN 4096

class WSClient
{
public:
	WSClient(std::string strHost, std::string strPort, std::string strPath);
	~WSClient();
public:
	bool Disconnect();
	bool StartKeepThread();
	
public:
	static int WSDataHandler(struct mg_connection *conn,
                              int flags,
                              char *data,
                              size_t data_len,
                              void *user_data);
	/* handle the data */
	int WSDataHandler1(struct mg_connection *conn,
                              int flags,
                              char *data,
                              size_t data_len);

	static void WSCloseHandler(struct mg_connection *conn, void *user_data);
	/* handle close */
	void WSCloseHandler1(struct mg_connection *conn);
	bool Connect();
	bool SendMsg(std::string &strMsg);
	bool SendBinary(const char * pBuf, size_t nSize);
	bool SendPing(const char * pBuf, size_t nSize);
	
	bool Connected();
	int GetReConnectCnt();
	
	bool Logined();
	static bool Thread(void* pData);
	bool ThreadRun();

private:
	struct mg_connection *m_wsConn;
	bool m_bConnected;
	int m_nReconnectCnt;
	std::string m_strHost;
	std::string m_strPort;
	std::string m_strPath;
	std::string m_strUser;
	std::string m_strPasswd;
	char m_ebuf[WS_EBUF_LEN];
	bool m_bExit;
protected:
	std::mutex m_lock;
	long long m_msgId;
	bool m_bOnline;
	std::thread * m_pThread;
	std::mutex m_ConnectLock;
	bool m_bLogined;

	bool m_bSSL;
	
};


#endif /* __CLOUD_WS_CLIENT__ */
