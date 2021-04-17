// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GPTraitLibrary.h"
#include "GameplayTagContainer.h"

UBehaviorTree* UGPTraitLibrary::FindBTByTrait(const FGameplayTag& inTag)
{
	return *TraitMap.Find(inTag);
}