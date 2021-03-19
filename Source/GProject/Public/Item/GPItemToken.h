// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/GPItem.h"
#include "GPItemToken.generated.h"

/**
 * 
 */
UCLASS()
class GPROJECT_API UGPItemToken : public UGPItem
{
	GENERATED_BODY()
	
public:
	UGPItemToken()
	{
		ItemType = UGPAssetManager::TokenItemType;
	}

};
