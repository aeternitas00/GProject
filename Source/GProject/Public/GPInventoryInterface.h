// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "UObject/Interface.h"
#include "GPInventoryInterface.generated.h"

/**
 * Interface for actors that provide a set of GPItems bound to ItemSlots
 * This exists so GPCharacterBase can query inventory without doing hacky player controller casts
 * It is designed only for use by native classes
 */
UINTERFACE(MinimalAPI)
class UGPInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GPROJECT_API IGPInventoryInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Returns the map of items to data */
	virtual const TMap<UGPItem*, FGPItemData>& GetInventoryDataMap() const = 0;

	/** Returns the map of slots to items */
	virtual const TMap<FGPItemSlot, UGPItem*>& GetSlottedItemMap() const = 0;

	/** Gets the delegate for inventory item changes */
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() = 0;

	/** Gets the delegate for inventory slot changes */
	virtual FOnSlottedItemChangedNative& GetSlottedItemChangedDelegate() = 0;

	/** Gets the delegate for when the inventory loads */
	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() = 0;
};
