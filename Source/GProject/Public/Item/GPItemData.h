// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "GPItem.h"
#include "UObject/NoExportTypes.h"
#include "GPItemData.generated.h"

/**
 * 
 */
 /** Extra information about a UGPItem that is in a player's inventory */
UCLASS(Blueprintable)
class GPROJECT_API UGPItemData : public UObject
{
	GENERATED_BODY()
public:
	/** Constructor, default to count/level 1 so declaring them in blueprints gives you the expected behavior */
	UGPItemData()
		: ItemCount(0)
		, ItemLevel(1)
	{}

	/** The number of instances of this item in the inventory, can never be below 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item, meta = (ExposeOnSpawn = "true"))
	int32 ItemCount;

	/** The level of this item. This level is shared for all instances, can never be below 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item, meta = (ExposeOnSpawn = "true"))
	int32 ItemLevel;

	/** Equality operators */
	bool operator==(const UGPItemData*& Other) const
	{
		return ItemCount == Other->ItemCount && ItemLevel == Other->ItemLevel;
	}
	bool operator!=(const UGPItemData*& Other) const
	{
		return !(*this == Other);
	}

	/** Returns true if count is greater than 0 */
	bool IsValid() const
	{
		return ItemCount > 0;
	}

	/** Append an item data, this adds the count and overrides everything else */
	virtual void UpdateItemData(const UGPItemData* Other, const UGPItem* Item);
};
