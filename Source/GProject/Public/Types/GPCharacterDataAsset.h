// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Abilities/GameplayAbility.h"

#include "GPCharacterDataAsset.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFXSpawnDelegate, UObject*, FXSys);

UCLASS(Blueprintable)
class GPROJECT_API UGPCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

	DECLARE_EVENT(UGPCharacterDataAsset, FLoadCompletedSignature);



private:
	UPROPERTY(VisibleAnywhere)
	bool bIsInitialized;

protected:
	//////////////////////// Properties ////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loading")
	TArray<TSoftObjectPtr<class UObject>> LoadTarget;

	UPROPERTY(Transient)
	TArray<UObject*> HardRef;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loading")
	//TArray<class UGPAbilityDataAsset*> AbilityData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loading")
	TArray<TSoftClassPtr<UGameplayAbility>> AbilityClasses;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loading")
	//TSoftClassPtr<TSubclassOf<UGameplayAbility>> AbilityClasse;


public:
	FLoadCompletedSignature OnLoadCompleted;
	FFXSpawnDelegate FXSpawnCall;

	virtual void PostLoad() override;

	UFUNCTION(BlueprintCallable)
	virtual void LoadResources();
	UFUNCTION()
	void LoadResourcesDeffered();

	void RecursiveLoadCheck(UClass* inClass);

	//void FXWarmupSpawn(UObject* FXSys);

	FORCEINLINE bool IsInitialzied() const { return bIsInitialized; }

};
