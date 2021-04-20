// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GPCharacterDataAsset.generated.h"

/**
 * 
 */
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loading")
	TArray<class UGPAbilityDataAsset*> AbilityData;

public:
	FLoadCompletedSignature OnLoadCompleted;

	virtual void PostLoad() override;

	UFUNCTION(BlueprintCallable)
	virtual void LoadResources();
	UFUNCTION()
	void LoadResourcesDeffered();

	FORCEINLINE bool IsInitialzied() const { return bIsInitialized; }

};
