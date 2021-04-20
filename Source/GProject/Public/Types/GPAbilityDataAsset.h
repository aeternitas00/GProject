// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "Engine/DataAsset.h"
#include "GPAbilityDataAsset.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GPROJECT_API UGPAbilityDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
	DECLARE_DELEGATE(FLoadCompletedSignature);

private:
	UPROPERTY(VisibleAnywhere)
	bool bIsInitialized;

protected:
	//////////////////////// Properties ////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loading")
	TArray<TSoftObjectPtr<class UFXSystemAsset>> FX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loading")
	TArray<TSoftObjectPtr<class UObject>> Misc;

	UPROPERTY(Transient)
	TArray<UObject*> HardRef;

public:
	FLoadCompletedSignature OnLoadCompleted;

	virtual void PostLoad() override;

	UFUNCTION(BlueprintCallable)
	virtual void LoadResources();
	UFUNCTION()
	void LoadResourcesDeffered();

	FORCEINLINE bool IsInitialzied() const { return bIsInitialized; }

};
