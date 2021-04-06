// Fill out your copyright notice in the Description page of Project Settings.

#include "GPGameInstanceBase.h"


UGPGameInstanceBase::UGPGameInstanceBase()
	//: SaveSlot(TEXT("SaveGame"))
	//, SaveUserIndex(0)
{
	//Socket = INVALID_SOCKET;
}

UGPGameInstanceBase::~UGPGameInstanceBase()
{
	/*closesocket(Socket);
	WSACleanup();*/
}

bool UGPGameInstanceBase::IsValidItemSlot(FGPItemSlot ItemSlot) const
{
	if (ItemSlot.IsValid())
	{
		const int32* FoundCount = ItemSlotsPerType.Find(ItemSlot.ItemType);

		if (FoundCount)
		{
			return ItemSlot.SlotNumber < *FoundCount;
		}
	}
	return false;
}


//bool UGPGameInstanceBase::InitClientSocket()
//{
//	WSADATA wsaData;
//	int iResult;
//
//	//클라 wsa 시작.
//	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);//2.2 
//	if (iResult)
//	{
//		UE_LOG(LogGProject, Warning, TEXT("WSAStartup failed, code: %d"), WSAGetLastError());
//		return false;
//	}
//
//	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//	if (Socket == INVALID_SOCKET)
//	{
//		UE_LOG(LogGProject, Warning, TEXT("Invalid socket, code : %d"), WSAGetLastError());
//		return false;
//	}
//
//	//UE_LOG(LogGProject, Warning, TEXT("WSAStartup!"));
//
//	SOCKADDR_IN sa;
//	sa.sin_family = AF_INET;
//	sa.sin_port = htons(9000);
//	sa.sin_addr.s_addr = inet_addr("127.0.0.1");
//
//	//connect to server
//	if (connect(Socket, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR)
//	{
//		UE_LOG(LogGProject, Warning, TEXT("connect failed, code : %d"), WSAGetLastError());
//		closesocket(Socket);
//		return false;
//	}
//	//GetQueuedCompletionStatus()
//
//	//HANDLE hThread = CreateThread(NULL, 0, RecvThread, (LPVOID)Socket, 0, NULL);
//
//	//if (hThread = NULL)
//	//	UE_LOG(LogGProject, Warning, TEXT("CreateThread faild, code : %d"), GetLastError());
//
//	UE_LOG(LogGProject, Warning, TEXT("Connected to server."));
//
//	return true;
//}