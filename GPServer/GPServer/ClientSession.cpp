#include "stdafx.h"
#include "ClientSession.h"
#include "GPServerManager.h"
#include "MyLog.h"
#include "MyCommon.h"


#define DEF_LOG_LEN LOG_TIME_LEN + 128

ClientSession::ClientSession(SOCKET asock)
{
	memset(this, 0, sizeof(*this));

	mSocket = asock;

	//접속한 클라 소켓 주소 정보 저장
	size_t al = sizeof(mAddr);
	getpeername(asock, (SOCKADDR*)&mAddr, (int *) &al);

	OnConnect();
}

bool ClientSession::OnConnect()
{
	mConnected = true;

	//connection time
	char time[LOG_TIME_LEN];
	GetCTime(time, LOG_TIME_LEN);

	stringstream str;
	str << "[" << time << "] Client Connected : IP = " << inet_ntoa(mAddr.sin_addr) << ",  Port = " << ntohs(mAddr.sin_port) << endl;
	cout << str.rdbuf();
	//MyLog::GetSingletonPtr()->WriteLog(str.str());//TODO 비동기

	return true;
}

bool ClientSession::Send(const char * buf, int len)
{
	if (!IsConnected()) return false;

	int iResult;
	iResult = send(mSocket, buf, len, 0);
	if (iResult == SOCKET_ERROR)
	{
		cout << "send failed, code : " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

bool ClientSession::Recv()
{
	if (!IsConnected()) return false;

	int iResult;
	
	//WSARecv test
	DWORD dwFlags = 0;
	WSABUF wb;
	wb.buf = mBuf;
	wb.len = sizeof(mBuf);

	iResult = WSARecv(mSocket, &wb, 1, NULL, &dwFlags, this, NULL);
	if (iResult == SOCKET_ERROR)
	{
		int err = WSAGetLastError();

		if (err != WSA_IO_PENDING)
		{
			cout << "WSARecv failed, Error : " << err << endl; //throw?
			return false;
		}
	}
	
	return true;
}

bool ClientSession::ParsePacket()
{
	Packet* pckt = (Packet*)mBuf;
	GPPacketType type = pckt->header.type;
	
	switch (type)
	{
	case PT_USER_LOGIN:
		OnLogin();//
		break;
	case PT_USER_LOGOUT:
		OnLogout();//
		break;
	case PT_MSG:
		OnChat();//
		break;
	default:
		return false;
	}

	return true;
}

void ClientSession::Disconnect()
{
	if (!mConnected) return;

	// ?shutdown the send half of the connection since no more data will be sent
	int iResult = shutdown(mSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cout << "shutdown failed: " << WSAGetLastError() << endl;
		return;
	}
	closesocket(mSocket);

	mConnected = false;

	//disconnection time
	char time[LOG_TIME_LEN];
	GetCTime(time, LOG_TIME_LEN);

	stringstream str;
	str << "[" << time << "] Client disconnected: IP = " << inet_ntoa(mAddr.sin_addr) << ",  Port = " << ntohs(mAddr.sin_port) << endl;
	cout << str.rdbuf();
	//MyLog::GetSingletonPtr()->WriteLog(str.str()); //todo 비동기

	//send disconnection message to all.
	const char * msg = ", disconnectd.";
	char sendbuf[MAX_PKT_SIZ];
	Packet *pckt = (Packet*)sendbuf;
	pckt->header.type = PT_MSG;

	int i = sizeof(PacketH);
	int len = strlen(mID);
	memcpy(sendbuf + i, mID, len);
	i += len;
	len = strlen(msg);
	memcpy(sendbuf + i, msg, len + 1); // +1 for '\0'.

	pckt->header.size = i + len + 1; // +1 for '\0'.

	GPServerManager::getSingleton()->SendToAll(sendbuf, pckt->header.size);
}

void ClientSession::OnChat()
{
	Packet* pckt = (Packet*)mBuf;
	if (pckt->header.type != PT_MSG)
	{
		cout << "Invalid call for OnChat. Pakct type is not PT_MSG." << endl;
		return;
	}
	//char sendBuf[BUFSIZE];
	//int bufLen = strlen(mBuf)+1;
	//memcpy(sendBuf, mBuf, bufLen);//

	GPServerManager::getSingleton()->SendToAll(mBuf, pckt->header.size);
}

void ClientSession::OnLogin()
{
	Packet* pckt = (Packet*)mBuf;

	if (pckt->header.type != PT_USER_LOGIN)
	{
		cout << "Packet type is not PT_USER_LOGIN." << endl;
		return;
	}

	//set client ID
	char * id = (char*)&pckt->data;
	cout << id << ", logged in." << endl;
	strcpy_s(mID, id);

	char sendbuf[MAX_PKT_SIZ];
	memcpy(sendbuf, pckt, pckt->header.size);
	pckt = (Packet*)sendbuf;

	//add connection msg
	const char * msg = ", logged in.";
	char * pData = (char*)&pckt->data;
	strcat_s(pData, sizeof(sendbuf) - sizeof(PacketH), msg);
	//memcpy(sendbuf + pckt->header.size - 1, msg, strlen(msg) + 1);// +1 for '\0'
	//set packet's size
	pckt->header.size = sizeof(PacketH) + strlen(pData)+1; // again, +1 for '\0'
	pckt->header.type = PT_MSG;

	GPServerManager::getSingleton()->SendToAll(sendbuf, pckt->header.size);
}

void ClientSession::OnLogout()
{
	Packet* pckt = (Packet*)mBuf;
	if (pckt->header.type != PT_USER_LOGOUT)
	{
		cout << "Packcet type is not PT_USER_LOGOUT." << endl;
		return;
	}

	cout << mID << ", disconnecting." << endl;

	GPServerManager::getSingleton()->DeleteClient(this); //Disconnect();
}