// Copyright Epic Games, Inc. All Rights Reserved.

#include "GProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "ActionComponent.h"
#include "Component/GPInteractionComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"

ADEPRECATED_GProjectCharacter::ADEPRECATED_GProjectCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	//ActionComponent = CreateDefaultSubobject<UActionComponent>("ActionComp");
	//InteractionComponent = CreateDefaultSubobject<UGPInteractionComponent>("InteractionComp");

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
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	//CameraBoom->TargetArmLength = 800.f;
	//CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	//CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	//TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	//TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	//CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	//CursorToWorld->SetupAttachment(RootComponent);
	//static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	//if (DecalMaterialAsset.Succeeded())
	//{
	//	CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	//}
	//CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	//CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());


	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ADEPRECATED_GProjectCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);


	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FHitResult TraceHitResult;

		PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult); //def trace channel name?
		if (CursorToWorld != nullptr)
		{
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
		}
		// Camera Movement
		//if (bUsingOptic)
		//{
		//	const static float DEFMAXOFVIEW_X = 533.3f;
		//	const static float DEFMAXOFVIEW_Y = 300.0f;

		//	FVector2D Res;
		//	FVector2D MousePos;

		//	GEngine->GameViewport->GetViewportSize(Res);

		//	// Get resolution and get ratio of current pos

		//	PC->GetMousePosition(MousePos.X, MousePos.Y);

		//	MousePos.X = 2 * (MousePos.X - Res.X/2) / Res.X;
		//	MousePos.Y = 2 * (MousePos.Y - Res.Y/2) / Res.Y;
	
		//	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, MousePos.ToString());

		//	CameraDest.Y = MousePos.X * DEFMAXOFVIEW_X;
		//	CameraDest.X = -MousePos.Y * DEFMAXOFVIEW_Y;
		//	CameraDest.Z = 0;
		//	FVector CameraDest;
		//	FRotator CharacterRotator = CameraDest.Rotation();

		//	SetActorRotation(CharacterRotator);

		//	CameraDest = CameraDest.RotateAngleAxis(-60,FVector::YAxisVector);
		
	}
	//FVector CameraDist = (CameraDest - TopDownCameraComponent->GetRelativeLocation()) / CameraSmoothness;
	//TopDownCameraComponent->AddRelativeLocation(CameraDist);
}

void ADEPRECATED_GProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//FInputActionBinding ActionBindQuickMelee("QuickMelee", IE_Pressed);
	//ActionBindQuickMelee.ActionDelegate.GetDelegateForManualSet().BindLambda([this](){ActionComponent->StartActionByName(this, "QuickMelee"); });
}
