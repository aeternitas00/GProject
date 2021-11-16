// Fill out your copyright notice in the Description page of Project Settings.


#include "GPGameSession.h"
#include "Online.h"//big OnlineSubSystem header for now

void AGPGameSession::RegisterServer()
{
	GP_LOG(Warning, TEXT("StartRegisterServer"))
	/*IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld());
	if (OnlineSub)*/
	IOnlineSessionPtr SessionInt = Online::GetSessionInterface();

	FOnlineSessionSettings Settings;
	Settings.NumPublicConnections = 4;
	Settings.bShouldAdvertise = true;
	Settings.bAllowJoinInProgress = true;
	Settings.bIsLANMatch = true;
	Settings.bUsesPresence = true;
	Settings.bAllowJoinViaPresence = true;
	Settings.Set(SETTING_MAPNAME, FString(TEXT("test")), EOnlineDataAdvertisementType::ViaOnlineService);


	SessionInt->CreateSession(0, GameSessionName, Settings);
}
