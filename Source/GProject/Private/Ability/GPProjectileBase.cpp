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
	bReplicates=true;
	SetReplicateMovement(true);
	ProjMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	ProjMovementComp->InitialSpeed=1000.0f;
	ProjMovementComp->MaxSpeed= 1000.0f;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));

	RootComponent = SphereCollision;
	SphereCollision->InitSphereRadius(40.0f);

	SphereCollision->SetCollisionProfileName(TEXT("WorldDynamic"));
	SphereCollision->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	SphereCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);

	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	ArrowComp->SetupAttachment(RootComponent);

	//InitialLifeSpan = 7.777f;
}

void AGPProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	SphereCollision->IgnoreActorWhenMoving(GetInstigator(), true);
}

void AGPProjectileBase::ProjectileHit(AActor* OverlappedActor, AActor* OtherActor)
{
	//GP_LOG(Warning, TEXT("Exec ProjectileOverlapped"));

	//GP_LOG(Warning, TEXT("Name is %s"),*OtherActor->GetFName().ToString());

	if (HitActors.Contains(OtherActor)) return;

	HitActors.Add(OtherActor);

	if (OtherActor == GetInstigator()) return;

	TArray<AActor*> TempActor;
	TArray<FHitResult> EmptyResult;

	TempActor.Add(OtherActor);

	UGPBPFuncLibrary::ApplyExternalEffectContainerSpec(
	UGPBPFuncLibrary::AddTargetsToEffectContainerSpec(EffectContainerSpec,EmptyResult,TempActor));


	//SetActorEnableCollision(false);
	//GP_LOG(Warning, TEXT("Success"));
}

// Called every frame
void AGPProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FORCEINLINE void AGPProjectileBase::AddIgnoreActorWhenMoving(AActor* inActor)
{
	SphereCollision->IgnoreActorWhenMoving(inActor,true);
}
