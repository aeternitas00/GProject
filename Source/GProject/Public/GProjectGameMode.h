// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GProject.h"
#include "GameFramework/GameModeBase.h"
#include "GProjectGameMode.generated.h"

UCLASS(minimalapi)
class AGProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFullyLoadedDelegate);

public:
	AGProjectGameMode();

	UPROPERTY(BlueprintAssignable, Category = "Load")
	FFullyLoadedDelegate OnLoadCompleted;

protected:
	UFUNCTION(BlueprintCallable, Category = "Load")
	void LoadCharacterData(const TArray<UGPCharacterDataAsset*>& InData);

	int8 UnloadedDataNum;
	void OnCharacterDataLoaded();

	UFUNCTION()
	void FXWarmupSpawn(UObject* FXSys);
};



