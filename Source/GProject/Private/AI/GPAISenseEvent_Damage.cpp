// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GPAISenseEvent_Damage.h"
#include "AI/GPAISense_Damage.h"

FAISenseID UGPAISenseEvent_Damage::GetSenseID() const
{
	return UAISense::GetSenseID<UGPAISense_Damage>();
}