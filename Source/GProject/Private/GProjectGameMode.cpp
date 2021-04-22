// Copyright Epic Games, Inc. All Rights Reserved.

#include "GProjectGameMode.h"
#include "GProjectPlayerController.h"
#include "UObject/ConstructorHelpers.h"

AGProjectGameMode::AGProjectGameMode()
{
}

void AGProjectGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	GP_LOG_C(Warning);
}

void AGProjectGameMode::StartPlay()
{
	Super::StartPlay();

	GP_LOG_C(Warning);
}
