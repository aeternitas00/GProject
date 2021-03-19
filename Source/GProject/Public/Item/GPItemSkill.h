// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/GPItem.h"
#include "GPItemSkill.generated.h"

/**
 * 
 */
UCLASS()
class GPROJECT_API UGPItemSkill : public UGPItem
{
	GENERATED_BODY()
	
public:
	UGPItemSkill()
	{
		ItemType = UGPAssetManager::SkillItemType;
	}

};
