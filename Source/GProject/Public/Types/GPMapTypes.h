// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// ----------------------------------------------------------------------------------------------------------------
// This header is for enums and structs used by classes and blueprints accross the game
// Collecting these in a single header helps avoid problems with recursive header includes
// It's also a good place to put things like data table row structs
// ----------------------------------------------------------------------------------------------------------------

#include "GProject.h"
#include "Engine/Datatable.h"
#include "Engine/LevelStreamingDynamic.h"
#include "GPMapTypes.generated.h"

//UENUM(BlueprintType)
//enum class EGPStageType : uint8
//{
//	ST_T1		UMETA(DisplayName = "Stage Type 1 (Ice)"),
//	ST_T2		UMETA(DisplayName = "Stage Type 2 ()"),
//	ST_T3		UMETA(DisplayName = "Stage Type 3 ()"),
//	ST_T4		UMETA(DisplayName = "Stage Type 4 ()")
//};

UENUM(BlueprintType)
enum class EGPStageStatus : uint8
{
	SS_NoInfo		UMETA(DisplayName = "Can't see info of stage"),
	SS_Info			UMETA(DisplayName = "Can see info but not visited"),
	SS_Cleared		UMETA(DisplayName = "Initialized and Cleared")
};

USTRUCT(BlueprintType)
struct GPROJECT_API FGPMapConnectionData
{
	GENERATED_BODY()

	FGPMapConnectionData():ConnectionType(TEXT("")), PossibleConnectionType({}), PossibleMapType({}), OffsetFromCenterOfMap(FVector::ZeroVector), RotationOfConnectionPoint(FRotator::ZeroRotator){}

	// Type of connection point
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	FName ConnectionType;

	// This connection point can be connected with other connection point with these types of one
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	TArray<FName> PossibleConnectionType;

	// This connection point cannot be connected with other connection point with these types of one
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	TArray<FName> PossibleMapType;

	// This connection pointer's world location in sublevel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	FVector OffsetFromCenterOfMap;

	// This connection pointer's world rotation in sublevel , ** ROTATION FROM X AXIS VECTOR **
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	FRotator RotationOfConnectionPoint;
};

USTRUCT(BlueprintType)
struct GPROJECT_API FGPMapData : public FTableRowBase
{
	GENERATED_BODY()

	FGPMapData():MapConnections({}),MapType(TEXT("")), MinDifficulty(0), MaxDifficulty(0), MapAsset(nullptr){}

	// Data of connection point
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	TArray<FGPMapConnectionData> MapConnections;

	// Type of map , maybe tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	FName MapType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	int32 MinDifficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	int32 MaxDifficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	TArray<FName> AvailableTileset;

	// Load target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	TSoftObjectPtr<UWorld> MapAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	TArray<FVector> MinPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	TArray<FVector> MaxPoint;

};

USTRUCT(BlueprintType)
struct GPROJECT_API FGPLevelNode
{
	GENERATED_BODY()

	FGPLevelNode():ConnectedLevelIndex({}),LevelObject(nullptr), isLoaded(false) {}

	// Index of where we have to go
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	TArray<int32> ConnectedLevelIndex;

	// Index of other level's connection point 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	TArray<int32> ConnectionPointIndex;

	// Loaded level object
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	ULevelStreamingDynamic* LevelObject;

	// Load target, DEPRECATED
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	//FName MapTableRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	FGPMapData MapData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	bool isLoaded;
};

USTRUCT(BlueprintType)
struct GPROJECT_API FGPStageInfo
{
	GENERATED_BODY()

	FGPStageInfo() : StageLevel(0), StageTileset(NAME_None), StageFaction(NAME_None){}
	FGPStageInfo(const FGPStageInfo& copyInfo) : 
	StageLevel(copyInfo.StageLevel), StageTileset(copyInfo.StageTileset), StageFaction(copyInfo.StageFaction) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	int32 StageLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	FName StageTileset; // Tileset

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	FName StageFaction;	// Faction

	FORCEINLINE	bool operator==(const FGPStageInfo& inInfo) const {
		return ( 
			( this->StageLevel == inInfo.StageLevel ) && 
			( this->StageTileset.Compare(inInfo.StageTileset) == 0 || this->StageTileset.IsNone() || inInfo.StageTileset.IsNone() ) &&
			( this->StageFaction.Compare(inInfo.StageFaction) == 0 || this->StageFaction.IsNone() || inInfo.StageFaction.IsNone() )
		);
	}
};

USTRUCT(BlueprintType)
struct GPROJECT_API FGPStageNode
{
	GENERATED_BODY()

	FGPStageNode() : StageInfo(), StageStatus(EGPStageStatus::SS_NoInfo) {}
	FGPStageNode(const FGPStageNode& copyNode) : 
	StageLoc(copyNode.StageLoc), StageInfo(copyNode.StageInfo), StageStatus(copyNode.StageStatus) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stage)
	FVector2D StageLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stage)
	FGPStageInfo StageInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stage)
	EGPStageStatus StageStatus;

	FORCEINLINE	bool operator==(const FGPStageNode& inNode) const {
		return (
			this->StageLoc == inNode.StageLoc && 
			this->StageInfo == inNode.StageInfo	&& 
			this->StageStatus == inNode.StageStatus
		);
	}

	FORCEINLINE	bool operator!=(const FGPStageNode& inNode) const {
		return !operator==(inNode);
	}

};


USTRUCT(BlueprintType)
struct GPROJECT_API FGPMessageInfo
{
	GENERATED_BODY()

	FGPMessageInfo():MessageIdx(-1), bIsRead(false){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Message)
	int32 MessageIdx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Message)
	bool bIsRead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Message)
	FName MessageSenderName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Message)
	FText MessageText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Message)
	TMap<FPrimaryAssetId, FGPItemData> MessageItems;


	FORCEINLINE	bool operator==(const FGPMessageInfo& inMsg) const {
		return (			
			this->MessageSenderName == inMsg.MessageSenderName &&
			this->MessageIdx == inMsg.MessageIdx
			);
	}

	FORCEINLINE	bool operator!=(const FGPMessageInfo& inMsg) const {
		return !operator==(inMsg);
	}

};