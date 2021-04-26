// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// ----------------------------------------------------------------------------------------------------------------
// This header is for enums and structs used by classes and blueprints accross the game
// Collecting these in a single header helps avoid problems with recursive header includes
// It's also a good place to put things like data table row structs
// ----------------------------------------------------------------------------------------------------------------

#include "Engine/Datatable.h"
#include "GPMapTypes.generated.h"

USTRUCT(BlueprintType)
struct GPROJECT_API FGPMapConnectionData
{
	GENERATED_BODY()

	FGPMapConnectionData(){}

	/** The type of items that can go in this slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	FName ConnectionType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	TArray<FName> RestrictedMapType;

	/** The number of this slot, 0 indexed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	FVector OffsetFromCenterOfMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	FRotator RotationOfConnectionPoint;
};

USTRUCT(BlueprintType)
struct GPROJECT_API FGPMapData : public FTableRowBase
{
	GENERATED_BODY()

	FGPMapData() {}

	/** The type of items that can go in this slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	TArray<FGPMapConnectionData> MapConnections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	FName MapType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	int32 MinDifficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	int32 MaxDifficulty;

	/** The number of this slot, 0 indexed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	TSoftObjectPtr<UWorld> MapAsset;
};

USTRUCT(BlueprintType)
struct GPROJECT_API FGPLevelNode
{
	GENERATED_BODY()

	FGPLevelNode() {}

	/** The type of items that can go in this slot */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	//TArray<FGPMapConnectionData> MapConnections;

	/** The number of this slot, 0 indexed */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	//ULevelStreamingDynamic LevelObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	bool isLoaded;
};

