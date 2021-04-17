// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GPAISense_Damage.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Perception/AIPerceptionListenerInterface.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "AI/GPAISenseEvent_Damage.h"

FGPAIDamageEvent::FGPAIDamageEvent():FAIDamageEvent()
{
}

FGPAIDamageEvent::FGPAIDamageEvent(AActor* InDamagedActor, AActor* InInstigator, float DamageAmount, const FVector& EventLocation, const FVector& InHitLocation)
	: FAIDamageEvent(InDamagedActor, InInstigator, DamageAmount, EventLocation, InHitLocation)
{
}

FGPAIDamageEvent::FGPAIDamageEvent(FName inTag, AActor* InDamagedActor, AActor* InInstigator, float DamageAmount, const FVector& EventLocation, const FVector& InHitLocation)
	: FAIDamageEvent(InDamagedActor, InInstigator, DamageAmount, EventLocation, InHitLocation),Tag(inTag)
{
}

void UGPAISense_Damage::ReportGPDamageEvent(UObject* WorldContextObject, AActor* DamagedActor, AActor* Instigator, float DamageAmount, FVector EventLocation, FVector HitLocation, FName Tag)
{
	UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(WorldContextObject);
	if (PerceptionSystem)
	{
		FGPAIDamageEvent Event(Tag, DamagedActor, Instigator, DamageAmount, EventLocation, HitLocation);
		PerceptionSystem->OnEvent(Event);
	}
}

void UGPAISense_Damage::RegisterEvent(const FGPAIDamageEvent& Event)
{
	if (Event.IsValid())
	{
		RegisteredGPEvents.Add(Event);

		RequestImmediateUpdate();
	}
}

void UGPAISense_Damage::RegisterWrappedEvent(UAISenseEvent& PerceptionEvent)
{
	UGPAISenseEvent_Damage* DamageEvent = Cast<UGPAISenseEvent_Damage>(&PerceptionEvent);
	ensure(DamageEvent);
	if (DamageEvent)
	{
		RegisterEvent(DamageEvent->GetDamageEvent());
	}
}

float UGPAISense_Damage::Update()
{
	AIPerception::FListenerMap& ListenersMap = *GetListeners();

	for (const FGPAIDamageEvent& Event : RegisteredGPEvents)
	{
		IAIPerceptionListenerInterface* PerceptionListener = Event.GetDamagedActorAsPerceptionListener();
		if (PerceptionListener != nullptr)
		{
			UAIPerceptionComponent* PerceptionComponent = PerceptionListener->GetPerceptionComponent();
			if (PerceptionComponent != nullptr && PerceptionComponent->GetListenerId().IsValid())
			{
				// this has to succeed, will assert a failure
				FPerceptionListener& Listener = ListenersMap[PerceptionComponent->GetListenerId()];

				if (Listener.HasSense(GetSenseID()))
				{
					Listener.RegisterStimulus(Event.Instigator, FAIStimulus(*this, Event.Amount, Event.Location, Event.HitLocation,FAIStimulus::SensingSucceeded,Event.Tag));
				}
			}
		}
	}

	RegisteredGPEvents.Reset();

	// return decides when next tick is going to happen
	return SuspendNextUpdate;
}
