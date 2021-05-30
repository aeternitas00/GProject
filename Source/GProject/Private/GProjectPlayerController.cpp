// Copyright Epic Games, Inc. All Rights Reserved.

#include "GProjectPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "GPSaveGame.h"
#include "Item/GPItem.h"
#include "UI/ChatWindow.h"
#include "GPClient.h"
#include "GPGameInstanceBase.h"
#include "GameFramework/PlayerState.h"
#include <sstream>

AGProjectPlayerController::AGProjectPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	GPClient = nullptr;

}

void AGProjectPlayerController::OnPossess(APawn* InPawn)
{
	GP_LOG(Warning, TEXT("%s"), *GetName());
	Super::OnPossess(InPawn);

	if (InPawn && GPClient)//
	{
		Cast<ACharacter>(InPawn)->OnCharacterMovementUpdated.AddDynamic(this, &AGProjectPlayerController::SendMovementInfo);
		//GPClient->CreateAsyncSendTask();
	}
}

void AGProjectPlayerController::BeginPlay()
{
	GP_LOG(Warning, TEXT("%s"), *GetName());

	Super::BeginPlay();

	//LoadoutCommit();

	if (IsLocalPlayerController()) //
	{
		GPClient = FGPClient::GetGPClient();
		GPClient->SetPlayerController(this);
		GPClient->SendHeader(PT_USER_READY);
		//if (Client->Login()) //test
		{
			//GetWorldTimerManager().SetTimer(SendTimer, this, &AGProjectPlayerController::SendData, 10.f, true);
		}
	}
}

bool AGProjectPlayerController::SaveInventory()
{
	UWorld* World = GetWorld();
	UGPGameInstanceBase* GameInstance = World ? World->GetGameInstance<UGPGameInstanceBase>() : nullptr;

	if (!GameInstance)
	{
		return false;
	}

	UGPSaveGame* CurrentSaveGame = GameInstance->GetCurrentSaveGame();
	if (CurrentSaveGame)
	{
		// Reset cached data in save game before writing to it
		CurrentSaveGame->InventoryData.Reset();
		CurrentSaveGame->SlottedItems.Reset();

		for (const TPair<UGPItem*, FGPItemData>& ItemPair : InventoryData)
		{
			FPrimaryAssetId AssetId;

			if (ItemPair.Key)
			{
				AssetId = ItemPair.Key->GetPrimaryAssetId();
				CurrentSaveGame->InventoryData.Add(AssetId, ItemPair.Value);
			}
		}

		for (const TPair<FGPItemSlot, UGPItem*>& SlotPair : SlottedItems)
		{
			FPrimaryAssetId AssetId;

			if (SlotPair.Value)
			{
				AssetId = SlotPair.Value->GetPrimaryAssetId();
			}
			CurrentSaveGame->SlottedItems.Add(SlotPair.Key, AssetId);
		}

		// Now that cache is updated, write to disk
		GameInstance->WriteSaveGame();
		return true;
	}
	return false;
}

bool AGProjectPlayerController::LoadInventory()
{
	InventoryData.Reset();
	SlottedItems.Reset();

	// Fill in slots from game instance
	UWorld* World = GetWorld();
	UGPGameInstanceBase* GameInstance = World ? World->GetGameInstance<UGPGameInstanceBase>() : nullptr;

	if (!GameInstance)
	{
		return false;
	}

	// Bind to loaded callback if not already bound
	if (!GameInstance->OnSaveGameLoadedNative.IsBoundToObject(this))
	{
		GameInstance->OnSaveGameLoadedNative.AddUObject(this, &AGProjectPlayerController::HandleSaveGameLoaded);
	}

	for (const TPair<FPrimaryAssetType, int32>& Pair : GameInstance->ItemSlotsPerType)
	{
		for (int32 SlotNumber = 0; SlotNumber < Pair.Value; SlotNumber++)
		{
			SlottedItems.Add(FGPItemSlot(Pair.Key, SlotNumber), nullptr);
		}
	}

	UGPSaveGame* CurrentSaveGame = GameInstance->GetCurrentSaveGame();
	UGPAssetManager& AssetManager = UGPAssetManager::Get();
	if (CurrentSaveGame)
	{
		// Copy from save game into controller data
		bool bFoundAnySlots = false;
		for (const TPair<FPrimaryAssetId, FGPItemData>& ItemPair : CurrentSaveGame->InventoryData)
		{
			UGPItem* LoadedItem = AssetManager.ForceLoadItem(ItemPair.Key);

			if (LoadedItem != nullptr)
			{
				InventoryData.Add(LoadedItem, ItemPair.Value);
				//NotifyInventoryItemChanged(LoadedItem, ItemPair.Value);
			}
		}

		for (const TPair<FGPItemSlot, FPrimaryAssetId>& SlotPair : CurrentSaveGame->SlottedItems)
		{
			if (SlotPair.Value.IsValid())
			{
				UGPItem* LoadedItem = AssetManager.ForceLoadItem(SlotPair.Value);
				if (GameInstance->IsValidItemSlot(SlotPair.Key) && LoadedItem)
				{
					SlottedItems.Add(SlotPair.Key, LoadedItem);
					//NotifySlottedItemChanged(SlotPair.Key, LoadedItem);
					bFoundAnySlots = true;
				}
			}
		}

		if (!bFoundAnySlots)
		{
			// Auto slot items as no slots were saved
			FillEmptySlots();
		}

		NotifyInventoryLoaded();

		return true;
	}

	// Load failed but we reset inventory, so need to notify UI
	NotifyInventoryLoaded();

	return false;
}

void AGProjectPlayerController::NotifyInventoryLoaded()
{
	// Notify native before blueprint
	OnInventoryLoadedNative.Broadcast();
	OnInventoryLoaded.Broadcast();

	InventoryLoaded();
}

void AGProjectPlayerController::HandleSaveGameLoaded(UGPSaveGame* NewSaveGame)
{
	LoadInventory();
}

void AGProjectPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

}

void AGProjectPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AGProjectPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();
}

bool AGProjectPlayerController::AddInventoryItem(UGPItem* NewItem, int32 ItemCount, int32 ItemLevel, bool bAutoSlot)
{
	bool bChanged = false;
	if (!NewItem)
	{
		GP_LOG(Warning, TEXT("AddInventoryItem: Failed trying to add null item!"));
		return false;
	}

	if (ItemCount <= 0 || ItemLevel <= 0)
	{
		GP_LOG(Warning, TEXT("AddInventoryItem: Failed trying to add item %s with negative count or level!"), *NewItem->GetName());
		return false;
	}

	// Find current item data, which may be empty
	FGPItemData OldData;
	GetInventoryItemData(NewItem, OldData);

	// Find modified data
	FGPItemData NewData = OldData;
	NewData.UpdateItemData(FGPItemData(ItemCount, ItemLevel), NewItem->MaxCount, NewItem->MaxLevel);

	if (OldData != NewData)
	{
		// If data changed, need to update storage and call callback
		InventoryData.Add(NewItem, NewData);
		NotifyInventoryItemChanged(NewItem, NewData);
		bChanged = true;
	}

	if (bAutoSlot)
	{
		// Slot item if required
		bChanged |= FillEmptySlotWithItem(NewItem);
	}

	if (bChanged)
	{
		// If anything changed, write to save game
		//SaveInventory();
		return true;
	}
	return false;
}

//bool AddInventoryItemWithData(UGPItem* NewItem, FGPItemData& ItemData, bool bAutoSlot = true) 
//{
//}

int32 AGProjectPlayerController::RemoveInventoryItem(UGPItem* RemovedItem, int32 RemoveCount)
{
	if (!RemovedItem)
	{
		//UE_LOG(GProject, Warning, TEXT("RemoveInventoryItem: Failed trying to remove null item!"));
		return -1;
	}

	// Find current item data, which may be empty
	FGPItemData NewData;
	GetInventoryItemData(RemovedItem, NewData);

	float BeforeRemoveNum = NewData.ItemCount;

	if (!NewData.IsValid())
	{
		// Wasn't found
		return -1;
	}

	// If RemoveCount <= 0, delete all
	if (RemoveCount <= 0)
	{
		NewData.ItemCount = 0;
	}
	else
	{
		NewData.ItemCount -= RemoveCount;
	}

	if (NewData.ItemCount > 0)
	{
		// Update data with new count
		InventoryData.Add(RemovedItem, NewData);
	}
	else
	{
		NewData.ItemCount = 0;
		// Remove item entirely, make sure it is unslotted
		InventoryData.Remove(RemovedItem);

		for (TPair<FGPItemSlot, UGPItem*>& Pair : SlottedItems)
		{
			if (Pair.Value == RemovedItem)
			{
				Pair.Value = nullptr;
				NotifySlottedItemChanged(Pair.Key, Pair.Value);
			}
		}
	}

	float AfterRemoveNum = NewData.ItemCount;

	// If we got this far, there is a change so notify and save
	NotifyInventoryItemChanged(RemovedItem, NewData);

	//SaveInventory();
	return BeforeRemoveNum - AfterRemoveNum;
}

void AGProjectPlayerController::GetInventoryItems(TArray<UGPItem*>& Items, FPrimaryAssetType ItemType)
{
	for (const TPair<UGPItem*, FGPItemData>& Pair : InventoryData)
	{
		if (Pair.Key)
		{
			FPrimaryAssetId AssetId = Pair.Key->GetPrimaryAssetId();

			// Filters based on item type
			if (AssetId.PrimaryAssetType == ItemType || !ItemType.IsValid())
			{
				Items.Add(Pair.Key);
			}
		}
	}
}

bool AGProjectPlayerController::SetSlottedItem(FGPItemSlot ItemSlot, UGPItem* Item)
{
	// Iterate entire inventory because we need to remove from old slot
	bool bFound = false;
	for (TPair<FGPItemSlot, UGPItem*>& Pair : SlottedItems)
	{
		if (Pair.Key == ItemSlot)
		{
			// Add to new slot
			bFound = true;
			Pair.Value = Item;
			NotifySlottedItemChanged(Pair.Key, Pair.Value);
		}
		else if (Item != nullptr && Pair.Value == Item)
		{
			// If this item was found in another slot, remove it
			Pair.Value = nullptr;
			NotifySlottedItemChanged(Pair.Key, Pair.Value);
		}
	}

	if (bFound)
	{
		//SaveInventory();
		return true;
	}

	return false;
}

int32 AGProjectPlayerController::GetInventoryItemCount(UGPItem* Item) const
{
	const FGPItemData* FoundItem = InventoryData.Find(Item);

	if (FoundItem)
	{
		return FoundItem->ItemCount;
	}
	return 0;
}

bool AGProjectPlayerController::GetInventoryItemData(UGPItem* Item, FGPItemData& ItemData) const
{
	const FGPItemData* FoundItem = InventoryData.Find(Item);

	if (FoundItem)
	{
		ItemData = *FoundItem;
		return true;
	}
	ItemData = FGPItemData(0, 0);
	return false;
}

UGPItem* AGProjectPlayerController::GetSlottedItem(FGPItemSlot ItemSlot) const
{
	UGPItem* const* FoundItem = SlottedItems.Find(ItemSlot);

	if (FoundItem)
	{
		return *FoundItem;
	}
	return nullptr;
}

void AGProjectPlayerController::GetSlottedItems(TArray<UGPItem*>& Items, FPrimaryAssetType ItemType, bool bOutputEmptyIndexes)
{
	for (TPair<FGPItemSlot, UGPItem*>& Pair : SlottedItems)
	{
		if (Pair.Key.ItemType == ItemType || !ItemType.IsValid())
		{
			Items.Add(Pair.Value);
		}
	}
}

void AGProjectPlayerController::FillEmptySlots()
{
	bool bShouldSave = false;
	for (const TPair<UGPItem*, FGPItemData>& Pair : InventoryData)
	{
		bShouldSave |= FillEmptySlotWithItem(Pair.Key);
	}

	if (bShouldSave)
	{
		//SaveInventory();
	}
}

bool AGProjectPlayerController::IsThereEmptySlotWithType(FPrimaryAssetType inType)
{
	for (TPair<FGPItemSlot, UGPItem*>& Pair : SlottedItems)
	{
		if (Pair.Key.ItemType == inType && Pair.Value == nullptr)	return true;
	}
	return false;
}

bool AGProjectPlayerController::FillEmptySlotWithItem(UGPItem* NewItem)
{
	// Look for an empty item slot to fill with this item
	FPrimaryAssetType NewItemType = NewItem->GetPrimaryAssetId().PrimaryAssetType;
	FGPItemSlot EmptySlot;
	for (TPair<FGPItemSlot, UGPItem*>& Pair : SlottedItems)
	{
		if (Pair.Key.ItemType == NewItemType)
		{
			if (Pair.Value == NewItem)
			{
				// Item is already slotted
				return false;
			}
			else if (Pair.Value == nullptr && (!EmptySlot.IsValid() || EmptySlot.SlotNumber > Pair.Key.SlotNumber))
			{
				// We found an empty slot worth filling
				EmptySlot = Pair.Key;
			}
		}
	}

	if (EmptySlot.IsValid())
	{
		SlottedItems[EmptySlot] = NewItem;
		NotifySlottedItemChanged(EmptySlot, NewItem);
		return true;
	}

	return false;
}

void AGProjectPlayerController::NotifyInventoryItemChanged(UGPItem* Item, FGPItemData ItemData)
{
	// Notify native before blueprint
	OnInventoryItemChangedNative.Broadcast(Item, ItemData);
	OnInventoryItemChanged.Broadcast(Item, ItemData);

	// Call BP update event
	InventoryItemChanged(Item, ItemData);
}

void AGProjectPlayerController::NotifySlottedItemChanged(FGPItemSlot ItemSlot, UGPItem* Item)
{
	// Notify native before blueprint
	OnSlottedItemChangedNative.Broadcast(ItemSlot, Item);
	OnSlottedItemChanged.Broadcast(ItemSlot, Item);

	// Call BP update event
	SlottedItemChanged(ItemSlot, Item);
}

void AGProjectPlayerController::SendMovementInfo(float DeltaSeconds, FVector OldLocation, FVector OldVelocity)
{
	if (OldVelocity.IsNearlyZero() && GetPawn()->GetVelocity().IsNearlyZero()) return;
	//GP_LOG(Display, TEXT("old: %s, %s, cur: %s, %s"), *OldLocation.ToString(), *OldVelocity.ToString(), *GetPawn()->GetActorLocation().ToString(), *GetPawn()->GetVelocity().ToString())
	
	SendData();
}

void AGProjectPlayerController::SendData()
{
	if (GetPawn() && GPClient)
	{
		std::stringstream ss;
		const FVector& Location = GetPawn()->GetActorLocation();
		const FVector& Velocity = GetPawn()->GetVelocity();
		//const FRotator& Rotation = GetPawn()->GetActorRotation();
		ss << Location.X << " " << Location.Y << " " << Location.Z << std::endl;
		ss << Velocity.X << " " << Velocity.Y << " " << Velocity.Z << std::endl;
		//ss << Rotation.Pitch << " " << Rotation.Yaw << " " << Rotation.Roll << std::endl;
		//GP_LOG(Display, TEXT("%s %d %d"), ANSI_TO_TCHAR(ss.str().c_str()), ss.str().length(),(int)ss.tellp())

		//test
		//GPClient->CreateAsyncSendTask(ss, PT_PLAYER_UPDATE);
	}
}

void AGProjectPlayerController::UpdateChat()
{
	if (ChatWindow)
	{
		FString Chat;

		while (ChatMessages.Dequeue(Chat))
		{
			ChatWindow->AddChat(Chat);
		}
	}
}

void AGProjectPlayerController::AddChat(const FString& ChatMsg)
{
	if (ChatWindow)
	{
		ChatWindow->AddChat(ChatMsg);
	}
	else
	{
		ChatMessages.Enqueue(ChatMsg);
		//bShouldUpdateChat = true;
	}
}

void AGProjectPlayerController::GetSeamlessTravelActorList(bool bToEntry, TArray<class AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);

	ActorList.Add(PlayerState);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("PC ActorList")));

}
