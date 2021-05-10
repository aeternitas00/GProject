// Copyright Epic Games, Inc. All Rights Reserved.

#include "GProjectGameMode.h"
#include "GProjectPlayerController.h"


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
	//GP_LOG_C(Warning);
	Super::PostLogin(NewPlayer);
	//GP_LOG_C(Warning);
}

void AGProjectGameMode::StartPlay()
{
	//GP_LOG_C(Warning);
	Super::StartPlay();
	//GP_LOG_C(Warning);
}


