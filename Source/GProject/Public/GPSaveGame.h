// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "Item/GPItem.h"
#include "Types/GPMapTypes.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	TMap<FPrimaryAssetId, EItemReplicateFlag> ReplicableItems;

	/** Map of slotted items */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	TMap<FGPItemSlot, FPrimaryAssetId> SlottedItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	TMap<FGPItemSlot, FGPWAttachmentedData> AttachmentData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	TArray<FGPStageNode> SavedStageNodes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	FGPStageNode SavedCurrentStageNode;

	/** User's unique id */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	FString UserId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SaveGame)
	TArray<FGPMessageInfo> SavedMessages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SaveGame)
	EGPGameDifficulty GameDifficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SaveGame)
	float SavedAttentionLevel;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void ResetSavedData();

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
