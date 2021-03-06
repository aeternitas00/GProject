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
	* ������ Ÿ���� ��� �з��� ���ΰ� �ε��̳� �¾�(��ġ) �����Ͽ� ����. ������Ʈ ���� ���丮.
	*/
};
