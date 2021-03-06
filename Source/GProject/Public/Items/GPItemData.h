// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GPItemData.generated.h"

/**
 * 
 */
UCLASS()
class GPROJECT_API UGPItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** Type of this item, set in native parent class */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	FPrimaryAssetType ItemType;

	/** Overridden to use saved type */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	/*
	* 데이터 타입을 어떻게 분류할 것인가 로딩이나 셋업(배치) 관련하여 생각. 프로젝트 세팅 디렉토리.
	*/
};
