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

	// This connection pointer's world rotation in sublevel
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

	// Load target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	TSoftObjectPtr<UWorld> MapAsset;
};

USTRUCT(BlueprintType)
struct GPROJECT_API FGPLevelNode
{
	GENERATED_BODY()

	FGPLevelNode():ConnectedLevelIndex({}),LevelObject(nullptr), MapTableRowName(TEXT("")), isLoaded(false) {}

	// Index of where we have to go
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	TArray<int32> ConnectedLevelIndex;

	// Index of other level's connection point 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	TArray<int32> ConnectionPointIndex;

	// Loaded level object
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	ULevelStreamingDynamic* LevelObject;

	// Load target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	FName MapTableRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
	bool isLoaded;

	//UFUNCTION(BlueprintCallable, Category = "Load")
	//void NotifyLoaded(ULevelStreamingDynamic* inLevelDynamic);
};

