// Fill out your copyright notice in the Description page of Project Settings.

#include "GPGameInstanceBase.h"


UGPGameInstanceBase::UGPGameInstanceBase()
	//: SaveSlot(TEXT("SaveGame"))
	//, SaveUserIndex(0)
	: GPClient(nullptr)
{
}

UGPGameInstanceBase::~UGPGameInstanceBase()
{
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

void UGPGameInstanceBase::Init()
{
	Super::Init();
}

void UGPGameInstanceBase::Shutdown()
{
	Super::Shutdown();
	if (GPClient)
		GPClient->Shutdown();
}

void UGPGameInstanceBase::OnStart()
{
	//Super::OnStart(); //blank.
	GP_LOG_C(Warning);

	GPClient = FGPClient::InitClient();
}

bool UGPGameInstanceBase::Send(FString buf)
{
	if (!GPClient) return false;
	
	return GPClient->SendChat(buf);
}
