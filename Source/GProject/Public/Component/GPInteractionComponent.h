// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "UObject/ScriptInterface.h"
#include "Actor.h"
#include "Components/ActorComponent.h"
#include "Interface/GPInteractionInterface.h"
#include "GPInteractionComponent.generated.h"

//class USWorldUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GPROJECT_API UGPInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGPInteractionComponent();

	void PrimaryInteract();

protected:
	// Reliable - Will always arrive, eventually. Request will be re-sent unless an acknowledgment was received.
	// Unreliable - Not guaranteed, packet can get lost and won't retry.

	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* InFocus);

	void FindBestInteractable();

	virtual void BeginPlay() override;

	UPROPERTY()
	AActor* FocusedActor;

	UPROPERTY()
	AActor* PrevFocusedActor;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	//UPROPERTY(EditDefaultsOnly, Category = "UI")
	//TSubclassOf<USWorldUserWidget> DefaultWidgetClass;

	//UPROPERTY()
	//USWorldUserWidget* DefaultWidgetInstance;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};