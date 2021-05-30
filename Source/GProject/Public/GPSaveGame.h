// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "Item/GPItem.h"
#include "GameFramework/SaveGame.h"
#include "GPSaveGame.generated.h"

/**
 * 
 */
 /** List of versions, native code will handle fixups for any old versions */
namespace EGPSaveGameVersion
{
	enum type
	{
		// Initial version
		Initial,
		// Added Inventory
		AddedInventory,
		// Added ItemData to store count/level
		AddedItemData,

		// -----<new versions must be added before this line>-------------------------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};
}

UENUM(BlueprintType)
enum class EGPStageType : uint8
{
	ST_T1		UMETA(DisplayName = "Stage Type 1 (Ice)"),
	ST_T2		UMETA(DisplayName = "Stage Type 2 ()"),
	ST_T3		UMETA(DisplayName = "Stage Type 3 ()"),
	ST_T4		UMETA(DisplayName = "Stage Type 4 ()")
};

UENUM(BlueprintType)
enum class EGPStageStatus : uint8
{
	SS_NoInfo		UMETA(DisplayName = "Can't see info of stage"),
	SS_Info			UMETA(DisplayName = "Can see info but not visited"),
	SS_Cleared		UMETA(DisplayName = "Initialized and Cleared")
};

USTRUCT(BlueprintType)
struct GPROJECT_API FGPStageInfo
{
	GENERATED_BODY()

	FGPStageInfo() : StageLevel(0) , StageType(EGPStageType::ST_T1){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	int32 StageLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	EGPStageType StageType; // Tileset

	// Faction
};

USTRUCT(BlueprintType)
struct GPROJECT_API FGPStageNode
{
	GENERATED_BODY()

	FGPStageNode() :ConnectedStageNodeIdx({}), StageInfo(), StageStatus(EGPStageStatus::SS_NoInfo) {}

	// Index of where we have to go
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	TArray<int32> ConnectedStageNodeIdx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	FGPStageInfo StageInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	EGPStageStatus StageStatus;
};


/** Object that is written to and read from the save game archive, with a data version */
UCLASS(BlueprintType)
class GPROJECT_API UGPSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/** Constructor */
	UGPSaveGame()
	{
		// Set to current version, this will get overwritten during serialization when loading
		SavedDataVersion = EGPSaveGameVersion::LatestVersion;
	}

	/** Map of items to item data */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	TMap<FPrimaryAssetId, FGPItemData> InventoryData;

	/** Map of slotted items */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	TMap<FGPItemSlot, FPrimaryAssetId> SlottedItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	TMap<FGPItemSlot, FGPWAttachmentedData> AttachmentData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	TArray<FGPStageNode> SavedStageNodes;

	/** User's unique id */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	FString UserId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SaveGame)
	EGPGameDifficulty GameDifficulty;

protected:
	/** Deprecated way of storing items, this is read in but not saved out */
	UPROPERTY()
	TArray<FPrimaryAssetId> InventoryItems_DEPRECATED;

	/** What LatestVersion was when the archive was saved */
	UPROPERTY()
	int32 SavedDataVersion;

	/** Overridden to allow version fixups */
	virtual void Serialize(FArchive& Ar) override;
};
