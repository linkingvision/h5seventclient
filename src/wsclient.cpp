#include "wsclient.hpp"

WSClient::WSClient(std::string strHost, std::string strPort, std::string strPath)
:m_wsConn(NULL), m_bConnected(false), m_strHost(strHost), m_strPort(strPort),
m_strPath(strPath), m_msgId(0), m_bOnline(false), m_pThread(NULL), m_bExit(false), 
m_nReconnectCnt(0)
{
	memset(m_ebuf, 0, WS_EBUF_LEN);

}
WSClient::~WSClient()
{
	Disconnect();
}

bool WSClient::StartKeepThread()
{
	/* start keepalive thread */
	m_pThread = new std::thread(WSClient::Thread, this);

	return true;
}

bool WSClient::Disconnect()
{

	if (m_bExit == false)
	{
		m_bExit = true;
		if (m_pThread)
		{
			m_pThread->join();
			delete m_pThread;
			m_pThread = NULL;
		}
		if (m_wsConn)
		{
			mg_close_connection(m_wsConn);
			m_wsConn = NULL;
		}
	}

	return true;
}


int WSClient::WSDataHandler(struct mg_connection *conn,
                          int flags,
                          char *data,
                          size_t data_len,
                          void *user_data)
{	
	WSClient* pWS = (WSClient*)user_data;

	if (pWS)
	{
	    return pWS->WSDataHandler1(conn, flags, data, data_len);
	}
	return true;
}

/* handle the data */
int WSClient::WSDataHandler1(struct mg_connection *conn,
                          int flags,
                          char *data,
                          size_t data_len)

{
	std::string strMessage;

	strMessage.append(data, data_len);
	printf("Recevice Alarm: \n %s\n",strMessage.c_str());
	return data_len;
}

void WSClient::WSCloseHandler(struct mg_connection *conn, void *user_data)
{	
	WSClient* pWS = (WSClient*)user_data;

	if (pWS)
	{
	    return pWS->WSCloseHandler1(conn);
	}
	
	return;
}
/* handle close */
void WSClient::WSCloseHandler1(struct mg_connection *conn)

{
	m_bConnected = false;
	m_bLogined = false;
	printf("Disconnected from server\n");
	
}
bool WSClient::Connect()
{
	std::lock_guard<std::mutex> guard(m_ConnectLock);
	if (m_bConnected == true)
	{
		return m_bConnected;
	}
	if (m_wsConn)
	{
		mg_close_connection(m_wsConn);
		m_wsConn = NULL;
	}

	m_wsConn = mg_connect_websocket_client(m_strHost.c_str(),
										   atoi(m_strPort.c_str()),
										   false,
										   m_ebuf,
										   sizeof(m_ebuf),
										   m_strPath.c_str(),
										   NULL,
										   (mg_websocket_data_handler)WSClient::WSDataHandler,
										   (mg_websocket_close_handler)WSClient::WSCloseHandler,
										   this);
	if (m_wsConn != NULL)
	{
		m_bConnected = true;
		m_nReconnectCnt = 0;
		printf("Connected to server\n");
	}else 
	{
		m_nReconnectCnt ++;

	}

	return m_bConnected;
}

bool WSClient::SendMsg(std::string &strMsg)
{
	std::lock_guard<std::mutex> guard(m_ConnectLock);
	if (m_bConnected == false)
	{
		return false;
	}
	int ret = mg_websocket_client_write(m_wsConn, 
		WEBSOCKET_OPCODE_TEXT, strMsg.c_str(), strMsg.length());
	
	if (ret == strMsg.length())
	{
		return true;
	}
	return false;
}

bool WSClient::SendBinary(const char * pBuf, size_t nSize)
{
	std::lock_guard<std::mutex> guard(m_ConnectLock);
	if (m_bConnected == false)
	{
		return false;
	}
	int ret = mg_websocket_client_write(m_wsConn, 
		WEBSOCKET_OPCODE_BINARY, pBuf, nSize);
	if (ret != nSize)
	{
		return false;
	}
	
	return true;	
}

bool WSClient::SendPing(const char * pBuf, size_t nSize)
{
	std::lock_guard<std::mutex> guard(m_ConnectLock);
	if (m_bConnected == false)
	{
		return false;
	}
	int ret = mg_websocket_client_write(m_wsConn, 
		WEBSOCKET_OPCODE_PING, pBuf, nSize);
	if (ret != nSize)
	{
		return false;
	}
	
	return true;	
}


bool WSClient::Connected()
{
	std::lock_guard<std::mutex> guard(m_ConnectLock);
	return m_bConnected;
}

int WSClient::GetReConnectCnt()
{
	std::lock_guard<std::mutex> guard(m_ConnectLock);
	return m_nReconnectCnt;
}

bool WSClient::Thread(void* pData)
{
	WSClient * pThread = (WSClient *)pData;

	if (pThread)
	{
		return pThread->ThreadRun();
	}
	return false;		
}
bool WSClient::ThreadRun()
{
	while(m_bExit == false)
	{
		
		/* loop to send keepalive & reconnect */
		{
			if (Connected() == false)
			{
				if (Connect() == true)
				{
				}
			}
			char ping[1];
			ping[0] = 0x51;
			SendPing(ping, 1);
		}
		/* each 3 second keep alive */
		int i = 60;
		while(m_bExit == false && i > 0)
		{
			i --;
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}

	return true;

}

