// Copyright Epic Games, Inc. All Rights Reserved.

#include "GProjectPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "GProjectCharacter.h"
#include "Item/GPItem.h"
//#include "Engine/World.h"

AGProjectPlayerController::AGProjectPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AGProjectPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);	
}

void AGProjectPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	//InputComponent->BindAction("SetDestination", IE_Pressed, this, &AGProjectPlayerController::OnSetDestinationPressed);
	//InputComponent->BindAction("SetDestination", IE_Released, this, &AGProjectPlayerController::OnSetDestinationReleased);

	// support touch devices 
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AGProjectPlayerController::MoveToTouchLocation);
	//InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AGProjectPlayerController::MoveToTouchLocation);

	//InputComponent->BindAction("ResetVR", IE_Pressed, this, &AGProjectPlayerController::OnResetVR);
}

//void AGProjectPlayerController::OnResetVR()
//{
	//Unuse HMD 
	//UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
//}

//void AGProjectPlayerController::MoveToMouseCursor()
//{
	//Unuse HMD
	//if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	//{
	//	if (AGProjectCharacter* MyPawn = Cast<AGProjectCharacter>(GetPawn()))
	//	{
	//		if (MyPawn->GetCursorToWorld())
	//		{
	//			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
	//		}
	//	}
	//}
	//else
	//{
	//	// Trace to see what is under the mouse cursor
	//	FHitResult Hit;
	//	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	//	if (Hit.bBlockingHit)
	//	{
	//		// We hit something, move there
	//		SetNewMoveDestination(Hit.ImpactPoint);
	//	}
	//}
//}

//void AGProjectPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	FVector2D ScreenSpaceLocation(Location);
//
//	// Trace to see what is under the touch location
//	FHitResult HitResult;
//	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
//	if (HitResult.bBlockingHit)
//	{
//		// We hit something, move there
//		SetNewMoveDestination(HitResult.ImpactPoint);
//	}
//}

//void AGProjectPlayerController::SetNewMoveDestination(const FVector DestLocation)
//{
//	APawn* const MyPawn = GetPawn();
//	if (MyPawn)
//	{
//		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());
//
//		// We need to issue move command only if far enough in order for walk animation to play correctly
//		if ((Distance > 120.0f))
//		{
//			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
//		}
//	}
//}

//void AGProjectPlayerController::OnSetDestinationPressed()
//{
//	// set flag to keep updating destination until released
//	bMoveToMouseCursor = true;
//}
//
//void AGProjectPlayerController::OnSetDestinationReleased()
//{
//	// clear flag to indicate we should stop updating the destination
//	bMoveToMouseCursor = false;
//}

bool AGProjectPlayerController::AddInventoryItem(UGPItem* NewItem, int32 ItemCount, int32 ItemLevel, bool bAutoSlot)
{
	bool bChanged = false;
	if (!NewItem)
	{
		//UE_LOG(GProject, Warning, TEXT("AddInventoryItem: Failed trying to add null item!"));
		return false;
	}

	if (ItemCount <= 0 || ItemLevel <= 0)
	{
		//UE_LOG(GProject, Warning, TEXT("AddInventoryItem: Failed trying to add item %s with negative count or level!"), *NewItem->GetName());
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
		NotifyInventoryItemChanged(true, NewItem);
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

bool AGProjectPlayerController::RemoveInventoryItem(UGPItem* RemovedItem, int32 RemoveCount)
{
	if (!RemovedItem)
	{
		//UE_LOG(GProject, Warning, TEXT("RemoveInventoryItem: Failed trying to remove null item!"));
		return false;
	}

	// Find current item data, which may be empty
	FGPItemData NewData;
	GetInventoryItemData(RemovedItem, NewData);

	if (!NewData.IsValid())
	{
		// Wasn't found
		return false;
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

	// If we got this far, there is a change so notify and save
	NotifyInventoryItemChanged(false, RemovedItem);

	//SaveInventory();
	return true;
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

void AGProjectPlayerController::NotifyInventoryItemChanged(bool bAdded, UGPItem* Item)
{
	// Notify native before blueprint
	OnInventoryItemChangedNative.Broadcast(bAdded, Item);
	OnInventoryItemChanged.Broadcast(bAdded, Item);

	// Call BP update event
	InventoryItemChanged(bAdded, Item);
}

void AGProjectPlayerController::NotifySlottedItemChanged(FGPItemSlot ItemSlot, UGPItem* Item)
{
	// Notify native before blueprint
	OnSlottedItemChangedNative.Broadcast(ItemSlot, Item);
	OnSlottedItemChanged.Broadcast(ItemSlot, Item);

	// Call BP update event
	SlottedItemChanged(ItemSlot, Item);
}
