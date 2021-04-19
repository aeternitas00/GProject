// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GPTraitLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GPROJECT_API UGPTraitLibrary : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UGPTraitLibrary(){}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	TMap<FGameplayTag, UBehaviorTree* > TraitMap;

	/** User-visible long description */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	//UBehaviorTree* SubBehaviorTree;

	UFUNCTION(BlueprintCallable, Category = AI)
	UBehaviorTree* FindBTByTrait(const FGameplayTag& inTag);
};
