// Copyright Epic Games, Inc. All Rights Reserved.

#include "GProjectGameMode.h"
#include "GProjectPlayerController.h"
#include "GPGameInstanceBase.h"


AGProjectGameMode::AGProjectGameMode()
{
}

void AGProjectGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	//GP_LOG_C(Warning);
	Super::InitGame(MapName, Options, ErrorMessage);
	//GP_LOG_C(Warning);
}

void AGProjectGameMode::PostLogin(APlayerController* NewPlayer)
{
	GP_LOG_C(Warning);
	Super::PostLogin(NewPlayer);
	GP_LOG_C(Warning);
}

void AGProjectGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	const bool bCanStartPlayer = GetNetMode() != ENetMode::NM_Standalone || GetGameInstance<UGPGameInstanceBase>()->CanStartGPPlayer();
	if (!bCanStartPlayer) return;
	GP_LOG_C(Warning);
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

void AGProjectGameMode::StartPlay()
{
	//GP_LOG_C(Warning);
	Super::StartPlay();
	//GP_LOG_C(Warning);
}


