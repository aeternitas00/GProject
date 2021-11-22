// Copyright Epic Games, Inc. All Rights Reserved.

#include "GProjectGameMode.h"
#include "GProjectPlayerController.h"
#include "GPGameInstanceBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/GameState.h"
#include "GPGameSession.h"


AGProjectGameMode::AGProjectGameMode()
{
    GameSessionClass = AGPGameSession::StaticClass();//

    //bDelayedStart = GetNetMode() != ENetMode::NM_Standalone; //host servers may wait for players.
}

void AGProjectGameMode::PostInitializeComponents()
{
    Super::PostInitializeComponents();
   // GP_LOG(Warning, TEXT("netmode: %d"), GetNetMode()); //아직 0(리턴 스탠드얼론임.
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
     //?플레이어와의 연결이 생겻을때 호출, 레벨이 바뀌어도 호출되지 않음.


	GP_LOG_C(Warning);
}

void AGProjectGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    GP_LOG_C(Warning);
	/*const bool bCanStartPlayer = GetNetMode() != ENetMode::NM_Standalone || GetGameInstance<UGPGameInstanceBase>()->CanStartGPPlayer();
	if (!bCanStartPlayer) return;*/
	GP_LOG_C(Warning);
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
    Cast<AGProjectPlayerController>(NewPlayer)->ClientJoinGP();

}

void AGProjectGameMode::HandleMatchIsWaitingToStart()
{
    Super::HandleMatchIsWaitingToStart();
    /*if (GameSession != nullptr)
    {
        GameSession->HandleMatchIsWaitingToStart();
    }*/
}

void AGProjectGameMode::StartPlay()
{
	//GP_LOG_C(Warning);

	Super::StartPlay();
	//GP_LOG_C(Warning);
}

void AGProjectGameMode::GetSeamlessTravelActorList(bool bToEntry, TArray<class AActor*>& ActorList)
{
    Super::GetSeamlessTravelActorList(bToEntry,ActorList);
    //bToEntry : true if we are going from old level to transition map, false if we are going from transition map to new level

    FString Str = bToEntry ? TEXT("true") : TEXT("false");

    for (AActor* ListedActor : ActorList)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Actor Name : %s"), *ListedActor->GetName()));
    }
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Entry is %s"), *Str));
}

void AGProjectGameMode::ProcessServerTravel(const FString& URL, bool bAbsolute)
{
	//GP_LOG_C(Warning);

    StartToLeaveMap();
  
    // Force an old style load screen if the server has been up for a long time so that TimeSeconds doesn't overflow and break everything
    bool bSeamless = (bUseSeamlessTravel && GetWorld()->TimeSeconds < 172800.0f); // 172800 seconds == 48 hours

    FString NextMap;
    if (URL.ToUpper().Contains(TEXT("?RESTART")))
    {
        NextMap = UWorld::RemovePIEPrefix(GetOutermost()->GetName());
    }
    else
    {
        int32 OptionStart = URL.Find(TEXT("?"));
        if (OptionStart == INDEX_NONE)
        {
            NextMap = URL;
        }
        else
        {
            NextMap = URL.Left(OptionStart);
        }
    }

    FGuid NextMapGuid = UEngine::GetPackageGuid(FName(*NextMap), GetWorld()->IsPlayInEditor());

    // Notify clients we're switching level and give them time to receive.
    FString URLMod = URL;
    APlayerController* LocalPlayer = ProcessClientTravel(URLMod, NextMapGuid, bSeamless, bAbsolute);

    UE_LOG(LogGameMode, Log, TEXT("ProcessServerTravel: %s"), *URL);
    UWorld* World = GetWorld();
    check(World);
    World->NextURL = URL;
    ENetMode NetMode = GetNetMode();

    if (bSeamless)
    {
        World->SeamlessTravel(World->NextURL, bAbsolute);
        World->NextURL = TEXT("");
    }
    // Switch immediately if not networking.
    else if (NetMode != NM_DedicatedServer && NetMode != NM_ListenServer)
    {
        World->NextSwitchCountdown = 0.0f;
    }
	
	//GP_LOG_C(Warning);
}