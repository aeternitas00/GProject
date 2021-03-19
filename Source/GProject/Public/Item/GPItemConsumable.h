// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/GPItem.h"
#include "GPItemConsumable.generated.h"

/**
 * 
 */
UCLASS()
class GPROJECT_API UGPItemConsumable : public UGPItem
{
	GENERATED_BODY()
	
public:
	UGPItemConsumable()
	{
		ItemType = UGPAssetManager::ConsumableItemType;
	}
};
