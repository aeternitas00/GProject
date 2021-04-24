// Copyright Epic Games, Inc. All Rights Reserved.

#include "GProjectPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Item/GPItem.h"
#include "UI/ChatWindow.h"
#include "GPClient.h"
//#include "Engine/World.h"

AGProjectPlayerController::AGProjectPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AGProjectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		FGPClient* Client = FGPClient::InitClient();
		Client->SetPlayerController(this);
		Client->Login();//test
	}
}

void AGProjectPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	UpdateChat();
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

void AGProjectPlayerController::UpdateChat()
{
	if (ChatWindow)
	{
		FString Chat;

		if (ChatMessages.Dequeue(Chat))
		{
			ChatWindow->AddChat(Chat);
		}
	}
}

void AGProjectPlayerController::AddChat(const FString& ChatMsg)
{
	ChatMessages.Enqueue(ChatMsg);

	//bShouldUpdateChat = true;
}
