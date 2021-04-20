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
	mConnected = true;//

	//connection time
	char time[LOG_TIME_LEN];
	GetCTime(time, LOG_TIME_LEN);

	stringstream ss;
	ss << "[" << time << "] Client connected : IP = " << inet_ntoa(mAddr.sin_addr) << ",  Port = " << ntohs(mAddr.sin_port) << endl;
	cout << ss.rdbuf();
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
	if (!IsConnected()) return false; //

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

GPPacketType ClientSession::ParsePacket()
{
	Packet* pckt = (Packet*)mBuf;
	GPPacketType type = pckt->header.type;
	
	return type;
}

void ClientSession::Disconnect()
{
	if (!mConnected) return;

	// shutdown the send half of the connection since no more data will be sent
	int iResult = shutdown(mSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cout << "shutdown failed: " << WSAGetLastError() << endl;
		return;
	}
	closesocket(mSocket);

	mConnected = false;

	//disconnection time //getlocaltime
	char time[LOG_TIME_LEN];
	GetCTime(time, LOG_TIME_LEN);

	stringstream str;
	str << "[" << time << "] Client disconnected: IP = " << inet_ntoa(mAddr.sin_addr) << ",  Port = " << ntohs(mAddr.sin_port) << endl;
	cout << str.rdbuf();
	//MyLog::GetSingletonPtr()->WriteLog(str.str()); //todo 비동기

}