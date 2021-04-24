// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWindow.generated.h"

/**
 * 
 */
UCLASS()
class GPROJECT_API UChatWindow : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void AddChat(const FString& ChatMsg);
	
};
