// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "Types/GPTypes.h"
#include "Components/SceneComponent.h"
#include "GameplayEffect.h"
#include "GPWeaponActorBase.h"
#include "GPWAttachmentComponent.generated.h"


UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GPROJECT_API UGPWAttachmentComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGPWAttachmentComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	EWAttachmentType Type;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	TArray<TSubclassOf<UGameplayEffect>> PassiveEffects;

	TArray<FActiveGameplayEffectHandle> ActiveEffects;
public:	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CommitEffects();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveEffects();

	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
