// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "Perception/AISenseConfig.h"
#include "Templates/SubclassOf.h"
#include "Perception/AISense.h"
#include "AI/GPAISense_Damage.h"
#include "GPAISenseConfig_Damage.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "GP AI Damage sense config"))
class GPROJECT_API UGPAISenseConfig_Damage : public UAISenseConfig
{
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", NoClear, config)
	TSubclassOf<UGPAISense_Damage> Implementation;

	virtual TSubclassOf<UAISense> GetSenseImplementation() const override;

};
