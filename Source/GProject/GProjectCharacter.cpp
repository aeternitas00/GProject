// Copyright Epic Games, Inc. All Rights Reserved.

#include "GProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"

AGProjectCharacter::AGProjectCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));

	StaticCameraDistance = FVector::XAxisVector * -800.0f;
	StaticCameraDistance = StaticCameraDistance.RotateAngleAxis(60, FVector::YAxisVector);

	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AGProjectCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (CursorToWorld != nullptr)
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UWorld* World = GetWorld())
			{
				FHitResult HitResult;
				FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
				FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
				FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
				Params.AddIgnoredActor(this);
				World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
				FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
				CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
			}
		}
		else if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);
		}

		// Camera Movement

		if (bUsingOptic)
		{
			CameraDest = CursorToWorld->GetComponentLocation() - TopDownCameraComponent->GetComponentLocation() + StaticCameraDistance;
			CameraDest.Z = 0;

			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, CameraDest.ToString());

			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TopDownCameraComponent->GetRelativeLocation().ToString());

			CameraDest = CameraDest.RotateAngleAxis(-60, FVector::YAxisVector);

			// 이곳에 거리제한 추가

			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, CameraDest.ToString());
		}

		FVector CameraDist = CameraDest - TopDownCameraComponent->GetRelativeLocation();
		TopDownCameraComponent->AddRelativeLocation(CameraDist / 10);

	}
}

void AGProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Space", IE_Pressed, this, &AGProjectCharacter::OnSpacePressed);
	PlayerInputComponent->BindAction("Space", IE_Released, this, &AGProjectCharacter::OnSpaceReleased);

	PlayerInputComponent->BindAction("RClick", IE_Pressed, this, &AGProjectCharacter::OnRClickPressed);
	PlayerInputComponent->BindAction("RClick", IE_Released, this, &AGProjectCharacter::OnRClickReleased);

	PlayerInputComponent->BindAction("LShift", IE_Pressed, this, &AGProjectCharacter::OnLeftShiftPressed);
	PlayerInputComponent->BindAction("LShift", IE_Released, this, &AGProjectCharacter::OnLeftShiftReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGProjectCharacter::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGProjectCharacter::OnMoveRight);

}

void AGProjectCharacter::CharacterDodge()
{
	GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Red,TEXT("Dodge"));
}

void AGProjectCharacter::OnSpacePressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Jump"));
	Jump();
}

void AGProjectCharacter::OnSpaceReleased()
{
	StopJumping();
}

void AGProjectCharacter::OnLeftShiftPressed()
{
	GetWorldTimerManager().SetTimer(LShiftTimer, 0.15f, false);
	Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed *= 3.0f;
}

void AGProjectCharacter::OnLeftShiftReleased()
{
	if (GetWorldTimerManager().IsTimerActive(LShiftTimer))
		CharacterDodge();

	GetWorldTimerManager().ClearTimer(LShiftTimer);
	Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed /= 3.0f;
}

void AGProjectCharacter::OnRClickPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("RClick"));
	bUsingOptic=true;



	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, StaticCameraDistance.ToString());

	//StaticCameraDistance = TopDownCameraComponent->GetComponentLocation()-GetActorLocation();

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, StaticCameraDistance.ToString());

}

void AGProjectCharacter::OnRClickReleased()
{	
	bUsingOptic=false;
	CameraDest=FVector::ZeroVector;
	//TopDownCameraComponent->SetRelativeLocation(FVector::ZeroVector);
}


void AGProjectCharacter::OnMoveForward(const float Value)
{
	if (Value != 0)
		AddMovementInput(FVector::ForwardVector, Value);
}

void AGProjectCharacter::OnMoveRight(const float Value)
{
	if (Value != 0)
		AddMovementInput(FVector::RightVector, Value);
}