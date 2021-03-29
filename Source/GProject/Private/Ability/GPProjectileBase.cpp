// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability/GPProjectileBase.h"
#include "Components/ArrowComponent.h"
#include "Types/GPBPFuncLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AGPProjectileBase::AGPProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));

	RootComponent = SphereCollision;
	SphereCollision->InitSphereRadius(40.0f);
	SphereCollision->SetCollisionProfileName(TEXT("Pawn"));

	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	ArrowComp->SetupAttachment(RootComponent);

	FScriptDelegate OverlapDel;

	OverlapDel.BindUFunction(this,FName("ProjectileOverlapped"));

	OnActorBeginOverlap.Add(OverlapDel);
	//AActor* OverlappedActor, AActor* OtherActor
}

// Called when the game starts or when spawned
void AGPProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGPProjectileBase::ProjectileOverlapped(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!HitActors.Contains(OtherActor)) return;

	HitActors.Add(OtherActor);

	if (OtherActor == GetInstigator()) return;

	TArray<AActor*> TempActor;
	TArray<FHitResult> EmptyResult;

	UGPBPFuncLibrary::ApplyExternalEffectContainerSpec(
	UGPBPFuncLibrary::AddTargetsToEffectContainerSpec(EffectContainerSpec,EmptyResult,TempActor));
	// TODO : DO SOME EFFECT WITH EffectContainerSpec

}

// Called every frame
void AGPProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

