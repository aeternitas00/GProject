// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "UObject/ObjectMacros.h"
#include "Perception/AISenseEvent.h"
#include "AI/GPAISense_Damage.h"
#include "GPAISenseEvent_Damage.generated.h"

/**
 * 
 */
UCLASS()
class GPROJECT_API UGPAISenseEvent_Damage : public UAISenseEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
	FGPAIDamageEvent Event;

	virtual FAISenseID GetSenseID() const override;

	FORCEINLINE FGPAIDamageEvent GetDamageEvent()
	{
		Event.Compile();
		return Event;
	}
};
