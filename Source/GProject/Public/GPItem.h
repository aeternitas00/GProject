// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/GPItemData.h"
#include "GPItem.generated.h"


UCLASS()
class GPROJECT_API AGPItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGPItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Use();

	virtual void Loot(); //땅에서 인벤토리로 획득.
	virtual void Drop(); //인벤토리에서 다시 땅으로. 
	
protected:
	UPROPERTY()
	UGPItemData* Data;

	int Price;
};
