// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "Engine/DataAsset.h"
#include "Styling/SlateBrush.h"
#include "AttributeSet.h"
#include "GPAssetManager.h"
#include <string>
#include <unordered_map>
#include "GPItem.generated.h"

struct FGameplayAttribute;
class UGPGameplayAbility;

UCLASS()
class GPROJECT_API UGPItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Constructor */
	UGPItem()
		: Price(0)
		, MaxCount(1)
		, MaxLevel(1)
		, AbilityLevel(1)
	{
	}

	/** Type of this item, set in native parent class */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	FPrimaryAssetType ItemType;

	/** User-visible short name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemName;

	/** User-visible short description */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemDescription;

	/** User-visible long description */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemDetailDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	TMap<FGameplayAttribute,float> ModsDescription;

	/** Icon to display */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FSlateBrush ItemIcon;

	/** Price in game */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	int32 Price;

	/** Maximum number of instances that can be in inventory at once, <= 0 means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
	int32 MaxCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
	int32 MaxStack;

	/** Pickup actor to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	TSubclassOf<AActor> SpawningActor;

	/** Returns if the item is consumable (MaxCount <= 0)*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Max)
	bool IsConsumable() const;

	/** Maximum level this item can be, <= 0 means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
	int32 MaxLevel;

	/** Ability to grant if this item is slotted */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TSubclassOf<UGPGameplayAbility> GrantedAbility;

	/** Ability level this item grants. <= 0 means the character level */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	int32 AbilityLevel;

	/** Returns the logical name, equivalent to the primary asset id */
	UFUNCTION(BlueprintCallable, Category = Item)
	FString GetIdentifierString() const;

	/** Overridden to use saved type */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	public:
	std::string TCHARToString(const TCHAR* ptsz) const
	{
		int len = wcslen((wchar_t*)ptsz);
		char* psz = new char[2 * len + 1];
		wcstombs(psz, (wchar_t*)ptsz, 2 * len + 1);
		std::string s = psz;
		delete[] psz;
		return s;
	}

public:
	FORCEINLINE bool operator== (const UGPItem& iItem) const 
	{
		return ItemName.EqualTo(iItem.ItemName);
	}

	FORCEINLINE bool operator< (const UGPItem& iItem) const
	{
		std::string OrgName = TCHARToString(ItemName.ToString().GetCharArray().GetData());
		std::string iName = TCHARToString(iItem.ItemName.ToString().GetCharArray().GetData());
		std::hash<std::string> Hasher;
		size_t OrgHash = Hasher(OrgName);
		size_t iHash = Hasher(iName);
		return (OrgHash < iHash);
	}
};
