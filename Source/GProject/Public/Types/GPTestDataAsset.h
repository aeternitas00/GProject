// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GPTestDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class GPROJECT_API UGPTestDataAsset : public UDataAsset
{
	GENERATED_BODY()

	DECLARE_EVENT(UGPTestDataAsset, FLoadCompletedSignature);

private:
	UPROPERTY(VisibleAnywhere)
	bool bIsInitialized;

protected:
	//////////////////////// Properties ////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loading")
	TArray<TSoftObjectPtr<class UObject>> LoadTarget;

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
