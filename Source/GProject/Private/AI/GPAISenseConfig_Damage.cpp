// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GPAISenseConfig_Damage.h"

UGPAISenseConfig_Damage::UGPAISenseConfig_Damage(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DebugColor = FColor::Red;
}

TSubclassOf<UAISense> UGPAISenseConfig_Damage::GetSenseImplementation() const
{
	return Implementation;
}
