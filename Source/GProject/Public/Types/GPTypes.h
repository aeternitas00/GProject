// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// ----------------------------------------------------------------------------------------------------------------
// This header is for enums and structs used by classes and blueprints accross the game
// Collecting these in a single header helps avoid problems with recursive header includes
// It's also a good place to put things like data table row structs
// ----------------------------------------------------------------------------------------------------------------

#include "UObject/PrimaryAssetId.h"
#include "Engine/Datatable.h"
//#include "GPCharacterDataAsset.h"
#include "GPTypes.generated.h"

class UGPItem;
//class UGPSaveGame;

/** Struct representing a slot for an item, shown in the UI */
USTRUCT(BlueprintType)
struct GPROJECT_API FGPItemSlot
{
	GENERATED_BODY()

		/** Constructor, -1 means an invalid slot */
	FGPItemSlot()
	: SlotNumber(-1)
	{}

	FGPItemSlot(const FPrimaryAssetType& InItemType, int32 InSlotNumber)
	: ItemType(InItemType)
	, SlotNumber(InSlotNumber)
	{}

	/** The type of items that can go in this slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FPrimaryAssetType ItemType;

	/** The number of this slot, 0 indexed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 SlotNumber;

	/** Equality operators */
	bool operator==(const FGPItemSlot& Other) const
	{
		return ItemType == Other.ItemType && SlotNumber == Other.SlotNumber;
	}
	bool operator!=(const FGPItemSlot& Other) const
	{
		return !(*this == Other);
	}

	/** Implemented so it can be used in Maps/Sets */
	friend inline uint32 GetTypeHash(const FGPItemSlot& Key)
	{
		uint32 Hash = 0;

		Hash = HashCombine(Hash, GetTypeHash(Key.ItemType));
		Hash = HashCombine(Hash, (uint32)Key.SlotNumber);
		return Hash;
	}

	/** Returns true if slot is valid */
	bool IsValid() const
	{
		return ItemType.IsValid() && SlotNumber >= 0;
	}
};


/** Extra information about a UGPItem that is in a player's inventory */
USTRUCT(BlueprintType)
struct GPROJECT_API FGPItemData
{
	GENERATED_BODY()

	/** Constructor, default to count/level 1 so declaring them in blueprints gives you the expected behavior */
	FGPItemData()
	: ItemCount(1)
	, ItemLevel(1)
	{}

	FGPItemData(int32 InItemCount, int32 InItemLevel)
	: ItemCount(InItemCount)
	, ItemLevel(InItemLevel)
	{}

	/** The number of instances of this item in the inventory, can never be below 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 ItemCount;

	/** The level of this item. This level is shared for all instances, can never be below 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 ItemLevel;

	/** Equality operators */
	bool operator==(const FGPItemData& Other) const
	{
		return ItemCount == Other.ItemCount && ItemLevel == Other.ItemLevel;
	}
	bool operator!=(const FGPItemData& Other) const
	{
		return !(*this == Other);
	}

	/** Returns true if count is greater than 0 */
	bool IsValid() const
	{
		return ItemCount > 0;
	}

	/** Append an item data, this adds the count and overrides everything else */
	void UpdateItemData(const FGPItemData& Other, int32 MaxCount, int32 MaxLevel)
	{
		if (MaxCount <= 0)
		{
			MaxCount = MAX_int32;
		}

		if (MaxLevel <= 0)
		{
			MaxLevel = MAX_int32;
		}

		ItemCount = FMath::Clamp(ItemCount + Other.ItemCount, 1, MaxCount);
		ItemLevel = FMath::Clamp(Other.ItemLevel, 1, MaxLevel);
	}
};

class AGPCharacterBase;

USTRUCT(BlueprintType)
struct GPROJECT_API FGPCharacterAssetStruct : public FTableRowBase
{
	GENERATED_BODY()

	FGPCharacterAssetStruct():EnemyAssetData(nullptr), MinDifficulty(0), MaxDifficulty(0), CharacterClass(nullptr)
	{}

	/** The type of items that can go in this slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	class UGPCharacterDataAsset* EnemyAssetData;

	/** The number of this slot, 0 indexed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	int32 MinDifficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	int32 MaxDifficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	TSubclassOf<AGPCharacterBase> CharacterClass;
};

UENUM(BlueprintType)
enum class EWAttachmentType : uint8
{
	WA_Magazine UMETA(DisplayName = "Magazine"),
	WA_Barrel UMETA(DisplayName = "Barrel"),
	WA_Stock UMETA(DisplayName = "Stock"),
	WA_Optic UMETA(DisplayName = "Optic"),
	WA_Custom1 UMETA(DisplayName = "Custom1"),
	WA_Custom2 UMETA(DisplayName = "Custom2")
};

UENUM(BlueprintType)
enum class EWFireType : uint8
{
	WF_Primary UMETA(DisplayName = "Primary"),
	WF_Sub UMETA(DisplayName = "Sub"),
	WF_Special UMETA(DisplayName = "Special"),
	WF_Custom1 UMETA(DisplayName = "Custom1"),
	WF_Custom2 UMETA(DisplayName = "Custom2")
};

UENUM(BlueprintType)
enum class EWFiringMode : uint8
{
	WM_Single UMETA(DisplayName = "Single"),
	WM_Burst UMETA(DisplayName = "Burst"),
	WM_Auto UMETA(DisplayName = "Auto"),
	WM_Custom1 UMETA(DisplayName = "Custom1"),
	WM_Custom2 UMETA(DisplayName = "Custom2")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_AR		UMETA(DisplayName = "Assault Rifle"),
	WT_Pistol	UMETA(DisplayName = "Pistol"),
	WT_SMG		UMETA(DisplayName = "SMG"),
	WT_SR		UMETA(DisplayName = "SniperRifle"),
	WT_Launcher UMETA(DisplayName = "Launcher"),
	WT_Shotgun	UMETA(DisplayName = "Shotgun"),
	WT_DMR		UMETA(DisplayName = "DMR"),
	WT_Custom1	UMETA(DisplayName = "Custom1"),
	WT_Custom2	UMETA(DisplayName = "Custom2")
}; 

/** Delegate called when an inventory item changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChanged, UGPItem*, Item, FGPItemData, ItemData);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChangedNative, UGPItem*, FGPItemData);

/** Delegate called when the contents of an inventory slot change */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlottedItemChanged, FGPItemSlot, ItemSlot, UGPItem*, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSlottedItemChangedNative, FGPItemSlot, UGPItem*);

/** Delegate called when the entire inventory has been loaded, all items may have been replaced */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryLoaded);
DECLARE_MULTICAST_DELEGATE(FOnInventoryLoadedNative);

/** Delegate called when the save game has been loaded/reset */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameLoaded, UGPSaveGame*, SaveGame);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSaveGameLoadedNative, UGPSaveGame*);
