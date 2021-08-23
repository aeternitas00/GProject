// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "Perception/AISense_Damage.h"
#include "GPAISense_Damage.generated.h"

USTRUCT(BlueprintType)
struct GPROJECT_API FGPAIDamageEvent : public FAIDamageEvent
{
	GENERATED_USTRUCT_BODY()

	typedef class UGPAISense_Damage FSenseClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
	FName SenseTag;

	FGPAIDamageEvent();
	FGPAIDamageEvent(AActor* InDamagedActor, AActor* InInstigator, float DamageAmount, const FVector& EventLocation, const FVector& InHitLocation = FAISystem::InvalidLocation);
	FGPAIDamageEvent(FName Tag, AActor* InDamagedActor, AActor* InInstigator, float DamageAmount, const FVector& EventLocation, const FVector& InHitLocation = FAISystem::InvalidLocation);

};

UCLASS()
class GPROJECT_API UGPAISense_Damage : public UAISense
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FGPAIDamageEvent> RegisteredGPEvents;

public:
	void RegisterEvent(const FGPAIDamageEvent& Event);
	virtual void RegisterWrappedEvent(UAISenseEvent& PerceptionEvent) override;

	/** EventLocation will be reported as Instigator's location at the moment of event happening*/
	UFUNCTION(BlueprintCallable, Category = "AI|Perception", meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "HitLocation"))
	static void ReportGPDamageEvent(UObject* WorldContextObject, AActor* DamagedActor, AActor* Instigator, float DamageAmount, FVector EventLocation, FVector HitLocation, FName Tag);

protected:
	virtual float Update() override;
};
