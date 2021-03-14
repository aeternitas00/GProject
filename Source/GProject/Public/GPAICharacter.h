// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "GPCharacterBase.h"
#include "GameFramework/Character.h"
#include "GPAICharacter.generated.h"

UCLASS(Blueprintable)
class GPROJECT_API AGPAICharacter : public AGPCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGPAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
