// Copyright Epic Games, Inc. All Rights Reserved.

#include "GProjectGameMode.h"
#include "GProjectPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Types/GPCharacterDataAsset.h"
#include "Particles/ParticleSystem.h"
//#include "NiagaraSystem.h"
//#include "NiagaraFunctionLibrary.h"


AGProjectGameMode::AGProjectGameMode()
{
}

void AGProjectGameMode::LoadCharacterData(const TArray<UGPCharacterDataAsset*>& InDataArr)
{
	UnloadedDataNum = InDataArr.Num();
	int AlreadyLoadedNum = 0;

	for (const auto& CharacterData : InDataArr)
	{
		if (CharacterData->IsInitialzied())
		{
			UnloadedDataNum--;
			AlreadyLoadedNum++;
		}
		else
		{
			CharacterData->OnLoadCompleted.AddUObject(this, &AGProjectGameMode::OnCharacterDataLoaded);
			CharacterData->FXSpawnCall.AddDynamic(this, &AGProjectGameMode::FXWarmupSpawn);
		}

		CharacterData->LoadResources();
	}

	//로드할 것이 없을 때 완료.
	if (AlreadyLoadedNum == InDataArr.Num())
	{
		GP_LOG(Warning, TEXT("No data to load"));
		OnLoadCompleted.Broadcast();
	}
}

//Common이 로드될 때마다 전부 완료됬는지 체크.
void AGProjectGameMode::OnCharacterDataLoaded()
{
	GP_LOG(Warning, TEXT("Loading remain : %d"), UnloadedDataNum-1);
	if (--UnloadedDataNum == 0)
	{
		GP_LOG(Warning, TEXT("No data to load"));
		OnLoadCompleted.Broadcast();
	}
}

void AGProjectGameMode::FXWarmupSpawn(UObject* FXSys)
{
	GP_LOG(Warning, TEXT("FXWarmpUpStart"));
/*	if(UNiagaraSystem* NiagaraSys = Cast<UNiagaraSystem>(FXSys))
	{
		const FTransform Trsf(FVector(0, 0, -500.0f));
		AActor* Temp = GEngine->GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), Trsf);
		Temp->SetLifeSpan(2.0f);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GEngine->GetWorld(), NiagaraSys, Trsf.GetLocation(),Trsf.Rotator())->AttachTo(Temp->GetRootComponent());
	}
	else */if (UParticleSystem* ParticleSys = Cast<UParticleSystem>(FXSys))
	{
		/*	GEngine->GetWorldFromContextObject()*/
		const FTransform Trsf(FVector(0, 0, -500.0f));
		UWorld* World = GetWorld();
		AActor* Temp = World->SpawnActor<AActor>(AActor::StaticClass(), Trsf);
		Temp->SetLifeSpan(2.0f);
		UGameplayStatics::SpawnEmitterAtLocation(World, ParticleSys, Trsf)->AttachTo(Temp->GetRootComponent());
	}
}
