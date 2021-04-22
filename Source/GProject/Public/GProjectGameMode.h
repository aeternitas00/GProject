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

<<<<<<< HEAD
	//virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override; //before AActor::PreInitializeComponents

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void StartPlay() override;

	//virtual void StartToLeaveMap() override;
=======
	UPROPERTY(BlueprintAssignable, Category = "Load")
	FFullyLoadedDelegate OnLoadCompleted;

protected:
	UFUNCTION(BlueprintCallable, Category = "Load")
	void LoadCharacterData(const TArray<UGPCharacterDataAsset*>& InData);

	int8 UnloadedDataNum;
	void OnCharacterDataLoaded();

	UFUNCTION()
	void FXWarmupSpawn(UObject* FXSys);
>>>>>>> f5ed9af0209248da7e78c3445368ab9a204376ca
};



