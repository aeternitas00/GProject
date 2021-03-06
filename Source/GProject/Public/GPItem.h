// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GPItem.generated.h"
#include "Items/GPItemData.h"

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

	virtual void Loot(); //������ �κ��丮�� ȹ��.
	virtual void Drop(); //�κ��丮���� �ٽ� ������. 
	
protected:
	UGPItemData Data;

	int Price;


};
