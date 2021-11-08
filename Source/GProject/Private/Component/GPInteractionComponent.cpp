// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GPInteractionComponent.h"
//#include "SWorldUserWidget.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGPInteractionComponent::UGPInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceRadius = 30.0f;
	TraceDistance = 50.0f;
	CollisionChannel = ECC_WorldDynamic;
}


void UGPInteractionComponent::PrimaryInteract()
{
	ServerInteract(FocusedActor);
}

void UGPInteractionComponent::FindBestInteractable()
{
//	bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	APawn* MyPawn = Cast<APawn>(GetOwner());

	FVector EyeLocation;
	FRotator EyeRotation;

	EyeLocation = MyPawn->GetActorLocation();
	EyeRotation = MyPawn->GetBaseAimRotation();
	
	FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);

	TArray<FHitResult> Hits;

	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);

	bool bBlockingHit = 
	GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);

	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	// Clear ref before trying to fill
	PrevFocusedActor = FocusedActor;
	FocusedActor = nullptr;

	for (FHitResult& Hit : Hits)
	{
		//if (bDebugDraw)
		//DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TraceRadius, 32, LineColor, false, 0.0f);

		AActor* HitActor = Hit.GetActor();

		if (HitActor->Implements<UGPInteractionInterface>())
		{
			FocusedActor = HitActor;
			if (PrevFocusedActor != FocusedActor)
			{
				if(PrevFocusedActor != nullptr)	
				{
					IGPInteractionInterface::Execute_HideInteractUI(PrevFocusedActor,MyPawn);
				}

				IGPInteractionInterface::Execute_ShowInteractUI(HitActor,MyPawn);
			}
			break;
		}
	}
	
	if (FocusedActor != PrevFocusedActor && PrevFocusedActor != nullptr)
	{
		IGPInteractionInterface::Execute_HideInteractUI(PrevFocusedActor, MyPawn);
	}

	//if (bDebugDraw)
	//DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 0.0f);
}

// Called when the game starts
void UGPInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UGPInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APawn* MyPawn = Cast<APawn>(GetOwner());
	if (MyPawn->IsLocallyControlled())
	{
		FindBestInteractable();
	}
}

void UGPInteractionComponent::ServerInteract_Implementation(AActor* InFocus)
{
	if (InFocus == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No Focus Actor to interact.");
		return;
	}

	APawn* MyPawn = Cast<APawn>(GetOwner());
	IGPInteractionInterface::Execute_Interact(InFocus, MyPawn);
}