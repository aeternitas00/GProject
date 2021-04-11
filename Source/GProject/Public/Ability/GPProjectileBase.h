// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/GPAbilityTypes.h"
#include "GPProjectileBase.generated.h"

class UProjectileMovementComponent;

UCLASS()
class GPROJECT_API AGPProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGPProjectileBase();

	//PROPERTIES
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsWaitingToKill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProjectileMovementComponent* ProjMovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UArrowComponent* ArrowComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	FGPGameplayEffectContainerSpec EffectContainerSpec;

	TArray<AActor*> HitActors;
	// FUNC
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ProjectileHit(AActor* OverlappedActor, AActor* OtherActor);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
