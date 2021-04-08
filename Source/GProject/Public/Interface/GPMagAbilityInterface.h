// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "UObject/Interface.h"
#include "GPMagAbilityInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGPMagAbilityInterface : public UInterface
{
	GENERATED_BODY()
};

class UGPItem;
/**
 * 
 */
class GPROJECT_API IGPMagAbilityInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetMagSize() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetMagSize(float inMagSize);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UGPItem* GetAmmoItem() const;
};
