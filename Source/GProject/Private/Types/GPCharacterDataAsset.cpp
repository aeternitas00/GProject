// Fill out your copyright notice in the Description page of Project Settings.


#include "Types/GPCharacterDataAsset.h"
#include "Engine/AssetManager.h"
#include "Types/GPAbilityDataAsset.h"

void UGPCharacterDataAsset::PostLoad()
{
	Super::PostLoad();

	bIsInitialized = false;
}

void UGPCharacterDataAsset::LoadResources()
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
			AssetLoader.RequestAsyncLoad(AssetsToLoad, FStreamableDelegate::CreateUObject(this, &UGPCharacterDataAsset::LoadResourcesDeffered));
		}

		else
		{
			LoadResourcesDeffered();
		}

		//UGameplayAbility* Rv = FoundSpec->Ability;

		//UClass* ClassInfo = Rv->GetClass();

		//for (TFieldIterator<FProperty> It(ClassInfo); It; ++It)
		//{
		//	GP_LOG(Warning, TEXT("Field : %s , PropertyClass : %s"), *It->GetName(), *It->GetClass()->GetName());

		//	// 한 클래스의 오브젝트형 필드의 정보를 담는 FObjectProperty
		//	FSoftObjectProperty* ObjProperty = FindFProperty<FSoftObjectProperty>(ClassInfo, *It->GetName());
		
			
		//	if (ObjProperty)
		//	{
		//		GP_LOG(Warning, TEXT("UClass : %s"), *ObjProperty->PropertyClass->GetName());

		//		if (ObjProperty->GetPropertyValue(Rv)->IsValidLowLevel())
		//		{
		//			GP_LOG(Warning, TEXT("Value objects name : %s"), *ObjProperty->GetPropertyValue(Rv)->GetName());
		//		}
		//	}
		//}

	}
}

void UGPCharacterDataAsset::LoadResourcesDeffered()
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
