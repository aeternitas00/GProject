// Fill out your copyright notice in the Description page of Project Settings.

#include "GPGameInstanceBase.h"
#include "GProjectGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Types/GPCharacterDataAsset.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraComponent.h" 
#include "NiagaraFunctionLibrary.h"
#include <sstream>


UGPGameInstanceBase::UGPGameInstanceBase()
	: SaveSlot(TEXT("SaveGame"))
	, SaveUserIndex(0)
	, GPClient(nullptr)
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
	{
		GPClient->Shutdown();
	}
}

AGameModeBase* UGPGameInstanceBase::CreateGameModeForURL(FURL InURL, UWorld* InWorld)
{
	if (!IsConnected())
	{
		return Super::CreateGameModeForURL(InURL, InWorld);
	}

	if (GetWorld() && GetEngine()->GetNetMode(GetWorld()) == ENetMode::NM_Standalone)
	{
		for (auto Level : GetWorld()->GetLevels())//
		{
			for (auto Actor : Level->Actors)//
			{
				if (Actor && !Actor->bNetLoadOnClient)
				{
					//GP_LOG(Display, TEXT("%s"), *Actor->GetName())
					GPGameObjects.Add(Actor);
					//need to be destroyed if not host
				}
			}
		}
	}

	AGameModeBase* GM = Super::CreateGameModeForURL(InURL, InWorld);
	GPClient->SetGameMode(Cast<AGProjectGameMode>(GM));
	return GM;
}

void UGPGameInstanceBase::OnStart()
{
	//Super::OnStart(); //blank.
	GP_LOG_C(Warning)
}

bool UGPGameInstanceBase::Send(FString buf)
{
	if (!GPClient) return false;
	
	return GPClient->SendChat(buf);
}

bool UGPGameInstanceBase::Connect()
{
	GPClient = FGPClient::GetGPClient();
	if (!GPClient) return false;

	GPClient->SetGameInstance(this);
	//FullyLoadedEvent.AddLambda([this](){GPClient->SendHeader(PT_USER_READY); });
	return 	true;
	//return GPClient->Connect();//
}

void UGPGameInstanceBase::BeGPHost()
{
	if (!IsConnected() || !GetWorld()) return; //

	/*AGProjectGameMode* GM = GetWorld()->GetAuthGameMode<AGProjectGameMode>();
	if (!GM) return;*/

	//todo sync update

	bIsGPHost = true;

	std::stringstream ss;
	for (auto Actor : GPGameObjects)//
	{
		//GP_LOG(Display, TEXT("%s"), *Actor->GetName())
		const FVector& Location = Actor->GetActorLocation();
		ss /*<< i++ << " " */ << Location.X << " " << Location.Y << " " << Location.Z << '\n'
			<< "0 0 0\n";
	}
	GP_LOG(Warning, TEXT("go tellp: %d"), (int)ss.tellp());
	GPClient->CreateAsyncSendTask(ss, PT_GAME);
}

bool UGPGameInstanceBase::IsConnected() const
{
	return GPClient ? true : false;//
}

void UGPGameInstanceBase::SaveDefaults(UGPSaveGame* SaveGame, bool WriteAfterSave)
{
	SaveGame->InventoryData.Reset();
	SaveGame->SlottedItems.Reset();
	SaveGame->AttachmentData.Reset();
	SaveGame->SavedStageNodes.Reset();

	// Now add the default inventory, this only adds if not already in hte inventory
	for (const TPair<FPrimaryAssetId, FGPItemData>& Pair : DefaultInventory)
	{
		SaveGame->InventoryData.Add(Pair.Key, Pair.Value);
	}

	for (const TPair<FGPItemSlot, FPrimaryAssetId>& SlotPair : DefaultSlottedItems)
	{
		if (SlotPair.Value.IsValid())
		{
			SaveGame->SlottedItems.Add(SlotPair.Key, SlotPair.Value);
		}
	}

	for (const TPair<FGPItemSlot, FGPWAttachmentedData>& SlotPair : AttachedItems)
	{
		//if (SlotPair.Value.IsValid())
		//{
			SaveGame->AttachmentData.Add(SlotPair.Key, SlotPair.Value);
		//}
	}

	for (const FGPStageNode& StageNode : StageNodes)
	{
		SaveGame->SavedStageNodes.Add(StageNode);
	}

	SaveGame->GameDifficulty = CurrentGameDifficulty;

	if (WriteAfterSave)
	{
		WriteSaveGame();
	}
}

void UGPGameInstanceBase::LoadDefaults(UGPSaveGame* SaveGame)
{
	DefaultInventory.Reset();
	DefaultSlottedItems.Reset();
	AttachedItems.Reset();
	StageNodes.Reset();


	for (const TPair<FPrimaryAssetId, FGPItemData>& Pair : SaveGame->InventoryData)
	{
		DefaultInventory.Add(Pair.Key, Pair.Value);
	}

	for (const TPair<FGPItemSlot, FPrimaryAssetId>& SlotPair : SaveGame->SlottedItems)
	{
		if (SlotPair.Value.IsValid())
		{
			DefaultSlottedItems.Add(SlotPair.Key, SlotPair.Value);
		}
	}

	for (const TPair<FGPItemSlot, FGPWAttachmentedData>& SlotPair : SaveGame->AttachmentData)
	{
		//if (SlotPair.Value.IsValid())
		//{
		AttachedItems.Add(SlotPair.Key, SlotPair.Value);
		//}
	}

	for (const FGPStageNode& StageNode : SaveGame->SavedStageNodes)
	{
		StageNodes.Add(StageNode);
	}

	CurrentGameDifficulty = SaveGame->GameDifficulty;
}

void UGPGameInstanceBase::CleanupDefaultInventory()
{	
	DefaultInventory.Reset();
	DefaultSlottedItems.Reset();
	AttachedItems.Reset();
	//ItemSlotsPerType.Reset();
}


UGPSaveGame* UGPGameInstanceBase::GetCurrentSaveGame()
{
	return CurrentSaveGame;
}

void UGPGameInstanceBase::SetSavingEnabled(bool bEnabled)
{
	bSavingEnabled = bEnabled;
}

bool UGPGameInstanceBase::LoadOrCreateSaveGame()
{
	UGPSaveGame* LoadedSave = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(SaveSlot, SaveUserIndex) && bSavingEnabled)
	{
		LoadedSave = Cast<UGPSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlot, SaveUserIndex));
	}

	return HandleSaveGameLoaded(LoadedSave);
}

bool UGPGameInstanceBase::HandleSaveGameLoaded(USaveGame* SaveGameObject)
{
	bool bLoaded = false;

	if (!bSavingEnabled)
	{
		// If saving is disabled, ignore passed in object
		SaveGameObject = nullptr;
	}

	// Replace current save, old object will GC out
	CurrentSaveGame = Cast<UGPSaveGame>(SaveGameObject);

	if (CurrentSaveGame)
	{
		// Make sure it has any newly added default inventory
		//AddDefaultInventory(CurrentSaveGame, false);
		LoadDefaults(CurrentSaveGame);
		bLoaded = true;
	}
	else
	{
		// This creates it on demand
		
		// Cast doesn't work somehow so we use this way
		USaveGame* CreatedSaveGame = UGameplayStatics::CreateSaveGameObject(UGPSaveGame::StaticClass());
		CurrentSaveGame = (UGPSaveGame*)(CreatedSaveGame);
		
		// Override defaults to null
		LoadDefaults(CurrentSaveGame);
		GenerateStageNodes();
	}

	OnSaveGameLoaded.Broadcast(CurrentSaveGame);
	OnSaveGameLoadedNative.Broadcast(CurrentSaveGame);

	return bLoaded;
}

void UGPGameInstanceBase::GenerateStageNodes(const int32& Legnth)
{
	StageNodes.Reserve(Legnth);
	// Generate Stage Nodes
	for (int i = 0; i < Legnth; i++)
	{
		FGPStageNode NewNode;
		NewNode.ConnectedStageNodeIdx.Add(i + 1);
		NewNode.StageInfo.StageLevel = i+2;
		StageNodes.Add(NewNode);
	}
	StageNodes[0].StageStatus = EGPStageStatus::SS_Info;
	StageNodes[Legnth-1].ConnectedStageNodeIdx.Reset();

	CurrentSaveGame->SavedStageNodes.Reset();
	for (const FGPStageNode& StageNode : StageNodes)
	{
		CurrentSaveGame->SavedStageNodes.Add(StageNode);
	}
}


void UGPGameInstanceBase::GetSaveSlotInfo(FString& SlotName, int32& UserIndex) const
{
	SlotName = SaveSlot;
	UserIndex = SaveUserIndex;
}

// TODO :: Update only changed things? 
void UGPGameInstanceBase::UpdateCurrentSaveGame()
{
	SaveDefaults(CurrentSaveGame,true);
}

bool UGPGameInstanceBase::WriteSaveGame()
{
	if (bSavingEnabled)
	{
		if (bCurrentlySaving)
		{
			// Schedule another save to happen after current one finishes. We only queue one save
			bPendingSaveRequested = true;
			return true;
		}

		// Indicate that we're currently doing an async save
		bCurrentlySaving = true;

		// This goes off in the background
		UGameplayStatics::AsyncSaveGameToSlot(GetCurrentSaveGame(), SaveSlot, SaveUserIndex, FAsyncSaveGameToSlotDelegate::CreateUObject(this, &UGPGameInstanceBase::HandleAsyncSave));
		return true;
	}
	return false;
}

void UGPGameInstanceBase::ResetSaveGame()
{
	// Call handle function with no loaded save, this will reset the data
	HandleSaveGameLoaded(nullptr);
}

void UGPGameInstanceBase::HandleAsyncSave(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	ensure(bCurrentlySaving);
	bCurrentlySaving = false;

	if (bPendingSaveRequested)
	{
		// Start another save as we got a request while saving
		bPendingSaveRequested = false;
		WriteSaveGame();
	}
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

	if (AlreadyLoadedNum == InDataArr.Num())
	{
		//GP_LOG(Warning, TEXT("No data to load"));
		OnLoadCompleted.Broadcast();
	}
}

void UGPGameInstanceBase::OnCharacterDataLoaded()
{
	//GP_LOG(Warning, TEXT("Loading remain : %d"), UnloadedDataNum - 1);
	if (--UnloadedDataNum == 0)
	{
		//GP_LOG(Warning, TEXT("No data to load"));
		OnLoadCompleted.Broadcast();
		FullyLoadedEvent.Broadcast();
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
