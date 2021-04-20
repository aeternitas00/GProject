// Fill out your copyright notice in the Description page of Project Settings.


#include "Types/GPAbilityDataAsset.h"
#include "Engine/AssetManager.h"
#include "Particles/ParticleSystem.h"


void UGPAbilityDataAsset::PostLoad()
{
	Super::PostLoad();

	bIsInitialized = false;
}

void UGPAbilityDataAsset::LoadResources()
{
	if (!bIsInitialized)
	{
		auto& AssetLoader = UAssetManager::GetStreamableManager();

		TArray<FSoftObjectPath> AssetsToLoad;

		for (const auto& SoftObj : FX)
		{
			if (SoftObj.IsPending())
			{
				AssetsToLoad.AddUnique(SoftObj.ToSoftObjectPath());
			}
		}

		for (const auto& SoftObj : Misc)
		{
			if (SoftObj.IsPending())
			{
				AssetsToLoad.AddUnique(SoftObj.ToSoftObjectPath());
			}
		}

		if (AssetsToLoad.Num() > 0)
		{
			AssetLoader.RequestAsyncLoad(AssetsToLoad, FStreamableDelegate::CreateUObject(this, &UGPAbilityDataAsset::LoadResourcesDeffered));
		}
		else
		{
			LoadResourcesDeffered();
		}
	}
}

void UGPAbilityDataAsset::LoadResourcesDeffered()
{
	for (const auto& SoftObj : FX)
	{
		if (SoftObj.IsValid())
		{
			//GET_MEMBER_NAME_STRING_CHECKED()
			//SoftObj.Get()->GetClass()->FindPropertyByName();
			HardRef.Add(SoftObj.Get());
		}
	}

	for (const auto& SoftObj : Misc)
	{
		if (SoftObj.IsValid())
		{
			HardRef.Add(SoftObj.Get());
		}
	}

	bIsInitialized = true;
	//OnLoadCompleted.Broadcast();
}
