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
	//GP_LOG(Warning, TEXT("Exec ProjectileOverlapped"));

	//GP_LOG(Warning, TEXT("Name is %s"),*OtherActor->GetFName().ToString());

	if (HitActors.Contains(OtherActor)){ GP_LOG(Warning, TEXT("Double check")); return;}

	HitActors.Add(OtherActor);

	if (OtherActor == GetInstigator()) { GP_LOG(Warning, TEXT("Instigator")); return;}

	TArray<AActor*> TempActor;
	TArray<FHitResult> EmptyResult;

	TempActor.Add(OtherActor);

	UGPBPFuncLibrary::ApplyExternalEffectContainerSpec(
	UGPBPFuncLibrary::AddTargetsToEffectContainerSpec(EffectContainerSpec,EmptyResult,TempActor));
	//GP_LOG(Warning, TEXT("Success"));
}

// Called every frame
void AGPProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

