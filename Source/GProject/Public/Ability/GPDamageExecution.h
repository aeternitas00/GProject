// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GPDamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class GPROJECT_API UGPDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	// Constructor and overrides
	UGPDamageExecution();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
