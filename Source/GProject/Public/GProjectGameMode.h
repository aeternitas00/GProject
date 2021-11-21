// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GProject.h"
#include "GameFramework/GameMode.h"
#include "GProjectGameMode.generated.h"

UCLASS(minimalapi)
class AGProjectGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AGProjectGameMode();

	virtual void PostInitializeComponents() override;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override; //before AActor::PreInitializeComponents
	virtual void PostLogin(APlayerController* NewPlayer) override;
	//virtual void GenericPlayerInitialization(AController* C) override; //Handles all player initialization that is shared between the travel methods (for client)
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual void HandleMatchIsWaitingToStart() override;


	virtual void StartPlay() override;

	UFUNCTION(BlueprintCallable)
	void ProcessServerTravel(const FString& URL, bool bAbsolute);
	
	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<class AActor*>& ActorList) override;

	//virtual void StartToLeaveMap() override;

protected:

	/** Returns game session class to use */ //현재는 GameSessionClass를 셋해서 되고 있는데 이걸 이용하는듯.
	//virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;
};



