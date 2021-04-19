// Fill out your copyright notice in the Description page of Project Settings.


#include "Types/GPTestDataAsset.h"
#include "Engine/AssetManager.h"

void UGPTestDataAsset::PostLoad()
{
	Super::PostLoad();

	bIsInitialized = false;
}

void UGPTestDataAsset::LoadResources()
{
	if (!bIsInitialized)
	{
		auto& AssetLoader = UAssetManager::GetStreamableManager();

		TArray<FSoftObjectPath> AssetsToLoad;

		for (const auto& SoftObj : LoadTarget)
		{
			if (SoftObj.IsPending())
			{
				AssetsToLoad.AddUnique(SoftObj.ToSoftObjectPath());
			}
		}

		if (AssetsToLoad.Num() > 0)
		{
			AssetLoader.RequestAsyncLoad(AssetsToLoad, FStreamableDelegate::CreateUObject(this, &UGPTestDataAsset::LoadResourcesDeffered));
		}
		else
		{
			LoadResourcesDeffered();
		}
	}
}

void UGPTestDataAsset::LoadResourcesDeffered()
{
	for (const auto& SoftObj : LoadTarget)
	{
		if (SoftObj.IsValid())
		{
			HardRef.Add(SoftObj.Get());
		}
	}

	bIsInitialized = true;
	OnLoadCompleted.Broadcast();
}
