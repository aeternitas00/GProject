// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GProject.h"
#include "GameFramework/GameModeBase.h"
#include "GProjectGameMode.generated.h"

UCLASS(minimalapi)
class AGProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGProjectGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override; //before AActor::PreInitializeComponents

	virtual void PostLogin(APlayerController* NewPlayer) override;

	//virtual void GenericPlayerInitialization(AController* C) override; //Handles all player initialization that is shared between the travel methods (for client)

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual void StartPlay() override;

	UFUNCTION(BlueprintCallable)
	void ProcessServerTravel(const FString& URL, bool bAbsolute);
	
	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<class AActor*>& ActorList) override;

	//virtual void StartToLeaveMap() override;
};



