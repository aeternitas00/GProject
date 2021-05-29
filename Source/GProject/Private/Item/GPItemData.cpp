// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GPItemData.h"

/** Append an item data, this adds the count and overrides everything else */

inline void UGPItemData::UpdateItemData(const UGPItemData* Other, const UGPItem* Item)
{
	if (!Other || !Item) return;

	//if (Item->MaxCount <= 0)
	//{
	//	MaxCount = MAX_int32;
	//}

	//if (Item->MaxLevel <= 0)
	//{
	//	MaxLevel = MAX_int32;
	//}

	ItemCount = FMath::Clamp(ItemCount + Other->ItemCount, 1, Item->MaxCount <= 0 ? MAX_int32 : Item->MaxCount);
	ItemLevel = FMath::Clamp(Other->ItemLevel, 1, Item->MaxLevel <= 0 ? MAX_int32 : Item->MaxLevel);
}
