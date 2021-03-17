// Fill out your copyright notice in the Description page of Project Settings.

#include "GPAssetManager.h"
#include "Item/GPItem.h"
#include "AbilitySystemGlobals.h"

const FPrimaryAssetType	UGPAssetManager::ConsumableItemType = TEXT("Consumable");
const FPrimaryAssetType	UGPAssetManager::SkillItemType = TEXT("Skill");
const FPrimaryAssetType	UGPAssetManager::TokenItemType = TEXT("Token");
const FPrimaryAssetType	UGPAssetManager::WeaponItemType = TEXT("Weapon");

UGPAssetManager& UGPAssetManager::Get()
{
	UGPAssetManager* This = Cast<UGPAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	else
	{
		UE_LOG(LogGProject, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be GPAssetManager!"));
		return *NewObject<UGPAssetManager>(); // never calls this
	}
}

void UGPAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}


UGPItem* UGPAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UGPItem* LoadedItem = Cast<UGPItem>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogGProject, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}