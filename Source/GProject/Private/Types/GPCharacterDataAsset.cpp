// Fill out your copyright notice in the Description page of Project Settings.


#include "Types/GPCharacterDataAsset.h"
#include "Engine/AssetManager.h"
#include "Ability/GPGameplayAbility.h"
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

		for (const auto& SoftClass : AbilityClasses)
		{
			if (SoftClass.IsPending())
			{
				AssetsToLoad.AddUnique(SoftClass.ToSoftObjectPath());
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

	for (const auto& SoftClass : AbilityClasses)
	{
		if (SoftClass.IsValid())
		{
			UClass* AbilityClass = SoftClass.Get();
			HardRef.Add(AbilityClass);
			
			RecursiveLoadCheck(AbilityClass);


			//UObject* AbilityCDO = AbilityClass->GetDefaultObject();

			//for (TFieldIterator<FProperty> It(AbilityClass); It; ++It)
			//{
			//	//GP_LOG(Warning, TEXT("Field : %s , PropertyClass : %s"), *It->GetName(), *It->GetClass()->GetName());

			//	// 한 클래스의 오브젝트형 필드의 정보를 담는 FObjectProperty
			//	FObjectProperty* ObjProperty = FindFProperty<FObjectProperty>(AbilityClass, *It->GetName());
			//	
			//	if (!ObjProperty) continue;
			//	
			//	GP_LOG(Warning, TEXT("Name : %s / UClass : %s"), *It->GetName(), *ObjProperty->PropertyClass->GetName());

			//	if (ObjProperty->PropertyClass->IsChildOf<UClass>())
			//	{
			//		GP_LOG(Warning, TEXT("Class Type"));

			//		UClass* ObjValue = Cast<UClass>(ObjProperty->GetPropertyValue_InContainer(AbilityCDO));

			//		if (ObjValue->IsValidLowLevel()) continue;
			//
			//		GP_LOG(Warning, TEXT("Value objects name : %s"), *ObjValue->GetName());

			//		if ( ObjValue->IsChildOf<AActor>())
			//		{ }
			//		else if (ObjValue->IsChildOf<UFXSystemAsset>())
			//		{ }
			//	}
			//}
		}
	}

	bIsInitialized = true;
	OnLoadCompleted.Broadcast();
}

void UGPCharacterDataAsset::RecursiveLoadCheck(UClass* inClass)
{
	UObject* AbilityCDO = inClass->GetDefaultObject();

	for (TFieldIterator<FProperty> It(inClass); It; ++It)
	{
		//GP_LOG(Warning, TEXT("Field : %s , PropertyClass : %s"), *It->GetName(), *It->GetClass()->GetName());

		// 한 클래스의 오브젝트형 필드의 정보를 담는 FObjectProperty
		FObjectProperty* ObjProperty = FindFProperty<FObjectProperty>(inClass, *It->GetName());

		if (!ObjProperty) continue;

		//GP_LOG(Warning, TEXT("Name : %s / UClass : %s"), *It->GetName(), *ObjProperty->PropertyClass->GetName());

		UObject* ObjectValue = ObjProperty->GetPropertyValue_InContainer(AbilityCDO);

		if (!ObjectValue->IsValidLowLevel()) continue;

		if (ObjProperty->PropertyClass->IsChildOf<UClass>())
		{
			UClass* ClassValue = Cast<UClass>(ObjectValue);

			GP_LOG(Warning, TEXT("Class Type"));
			GP_LOG(Warning, TEXT("Class name : %s"), *ClassValue->GetName());

			if (ClassValue->IsChildOf<AActor>())
			{
				RecursiveLoadCheck(ClassValue);
			}
		}
		else if (ObjProperty->PropertyClass->IsChildOf<UFXSystemAsset>())
		{
			GP_LOG(Warning, TEXT("FX Type"));
			GP_LOG(Warning, TEXT("Class name : %s"), *ObjectValue->GetName());

			
			FXSpawnCall.Broadcast(ObjectValue);
			//FX.AddUnique(ObjectValue);
			//FXWarmupSpawn(ObjectValue);
		}
	}
}

