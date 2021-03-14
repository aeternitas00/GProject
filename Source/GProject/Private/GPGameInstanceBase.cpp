// Fill out your copyright notice in the Description page of Project Settings.

#include "GPGameInstanceBase.h"

UGPGameInstanceBase::UGPGameInstanceBase()
	//: SaveSlot(TEXT("SaveGame"))
	//, SaveUserIndex(0)
{}

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
