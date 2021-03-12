// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "..\GProject.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GPAbilityTypes.h"
#include "GPTargetType.generated.h"

class AGPCharacterBase;
class AActor;
struct FGameplayEventData;

/**
 * Class that is used to determine targeting for abilities
 * It is meant to be blueprinted to run target logic
 * This does not subclass GameplayAbilityTargetActor because this class is never instanced into the world
 * This can be used as a basis for a game-specific targeting blueprint
 * If your targeting is more complicated you may need to instance into the world once or as a pooled actor
 */
UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class GPROJECT_API UGPTargetType : public UObject
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGPTargetType() {}

	/** Called to determine targets to apply gameplay effects to */
	UFUNCTION(BlueprintNativeEvent)
		void GetTargets(AGPCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
};

/** Trivial target type that uses the owner */
UCLASS(NotBlueprintable)
class GPROJECT_API UGPTargetType_UseOwner : public UGPTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGPTargetType_UseOwner() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(AGPCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

/** Trivial target type that pulls the target out of the event data */
UCLASS(NotBlueprintable)
class GPROJECT_API UGPTargetType_UseEventData : public UGPTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGPTargetType_UseEventData() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(AGPCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};
