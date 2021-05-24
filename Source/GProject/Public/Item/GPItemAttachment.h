// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/GPItem.h"
#include "Component/GPWAttachmentComponent.h"
#include "GPItemAttachment.generated.h"

/**
 * 
 */
UCLASS()
class GPROJECT_API UGPItemAttachment : public UGPItem
{
	GENERATED_BODY()

public:
	UGPItemAttachment()
	{
		ItemType = UGPAssetManager::AttachmentItemType;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attachment)
	TSubclassOf<UGPWAttachmentComponent> AttachmentClass;
};
