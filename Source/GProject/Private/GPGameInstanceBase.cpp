// Fill out your copyright notice in the Description page of Project Settings.

#include "GPGameInstanceBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Types/GPCharacterDataAsset.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraComponent.h" 
#include "NiagaraFunctionLibrary.h"


UGPGameInstanceBase::UGPGameInstanceBase()
	//: SaveSlot(TEXT("SaveGame"))
	//, SaveUserIndex(0)
	: GPClient(nullptr)
{
}

UGPGameInstanceBase::~UGPGameInstanceBase()
{
}

bool UGPGameInstanceBase::IsValidItemSlot(FGPItemSlot ItemSlot) const
{
	if (ItemSlot.IsValid())
	{
		const int32* FoundCount = ItemSlotsPerType.Find(ItemSlot.ItemType);

		if (FoundCount)
		{
			return ItemSlot.SlotNumber < *FoundCount;
		}
	}
	return false;
}

void UGPGameInstanceBase::Init()
{
	Super::Init();
}

void UGPGameInstanceBase::Shutdown()
{
	Super::Shutdown();
	if (GPClient)
		GPClient->Shutdown();
}

void UGPGameInstanceBase::OnStart()
{
	//Super::OnStart(); //blank.
	GP_LOG_C(Warning);

	GPClient = FGPClient::InitClient();
}

bool UGPGameInstanceBase::Send(FString buf)
{
	if (!GPClient) return false;
	
	return GPClient->SendChat(buf);
}

bool UGPGameInstanceBase::Connect()
{
	if (!GPClient) return false;

	return GPClient->Connect();
}


void UGPGameInstanceBase::LoadCharacterData(const TArray<UGPCharacterDataAsset*>& InDataArr)
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
			CharacterData->OnLoadCompleted.AddUObject(this, &UGPGameInstanceBase::OnCharacterDataLoaded);
			CharacterData->FXSpawnCall.AddDynamic(this, &UGPGameInstanceBase::FXWarmupSpawn);
		}

		CharacterData->LoadResources();
	}

	//로드할 것이 없을 때 완료.
	if (AlreadyLoadedNum == InDataArr.Num())
	{
		//GP_LOG(Warning, TEXT("No data to load"));
		OnLoadCompleted.Broadcast();
	}
}

//Common이 로드될 때마다 전부 완료됬는지 체크.
void UGPGameInstanceBase::OnCharacterDataLoaded()
{
	//GP_LOG(Warning, TEXT("Loading remain : %d"), UnloadedDataNum - 1);
	if (--UnloadedDataNum == 0)
	{
		//GP_LOG(Warning, TEXT("No data to load"));
		OnLoadCompleted.Broadcast();
	}
}

void UGPGameInstanceBase::FXWarmupSpawn(UObject* FXSys)
{
	const FTransform Trsf(FVector(5000.0f, 5000.0f, -900.0f));
	UWorld* World = GetWorld();
	AActor* Temp = World->SpawnActor<AActor>(AActor::StaticClass(), Trsf);
	Temp->SetLifeSpan(2.0f);

	//GP_LOG(Warning, TEXT("FXWarmpUpStart"));
	if (UNiagaraSystem* NiagaraSys = Cast<UNiagaraSystem>(FXSys))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, NiagaraSys, Trsf.GetLocation(), Trsf.Rotator())->AttachToComponent(Temp->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	}
	else if (UParticleSystem* ParticleSys = Cast<UParticleSystem>(FXSys))
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, ParticleSys, Trsf)->AttachToComponent(Temp->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	}
}
