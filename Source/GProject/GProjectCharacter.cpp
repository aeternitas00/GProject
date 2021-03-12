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
#include "GPInteractionComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"

AGProjectCharacter::AGProjectCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	ActionComponent = CreateDefaultSubobject<UActionComponent>("ActionComp");
	InteractionComponent = CreateDefaultSubobject<UGPInteractionComponent>("InteractionComp");

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

	// config?
	CameraSmoothness = 25.0f;

	bUsingOptic = false;
	bHasCameraDestUpdated = false;

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AGProjectCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (CursorToWorld != nullptr)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;

			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult); //def trace channel name?
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);

			// Camera Movement
			if (bUsingOptic)
			{
				const static float DEFMAXOFVIEW_X = 533.3f;
				const static float DEFMAXOFVIEW_Y = 300.0f;

				FVector2D Res;
				FVector2D MousePos;

				GEngine->GameViewport->GetViewportSize(Res);

				// Get resolution and get ratio of current pos

				PC->GetMousePosition(MousePos.X, MousePos.Y);

				MousePos.X = 2 * (MousePos.X - Res.X/2) / Res.X;
				MousePos.Y = 2 * (MousePos.Y - Res.Y/2) / Res.Y;
	
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, MousePos.ToString());

				CameraDest.Y = MousePos.X * DEFMAXOFVIEW_X;
				CameraDest.X = -MousePos.Y * DEFMAXOFVIEW_Y;
				CameraDest.Z = 0;

				//AimedPosition = CameraDest;

				CameraDest = CameraDest.RotateAngleAxis(-60,FVector::YAxisVector);
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, CursorToWorld->GetRelativeLocation().ToString());////
			}
		}

		FVector CameraDist = (CameraDest - TopDownCameraComponent->GetRelativeLocation()) / CameraSmoothness;
			
		TopDownCameraComponent->AddRelativeLocation(CameraDist);
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

	FInputActionBinding ActionBindQuickMelee("QuickMelee", IE_Pressed);
	ActionBindQuickMelee.ActionDelegate.GetDelegateForManualSet().BindLambda([this](){ActionComponent->StartActionByName(this, "QuickMelee"); });

	PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &AGProjectCharacter::OnInteractionPressed);
	PlayerInputComponent->BindAction("Interaction", IE_Released, this, &AGProjectCharacter::OnInteractionReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGProjectCharacter::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGProjectCharacter::OnMoveRight);

	//PlayerInputComponent->BindAxis("MouseX", this, &AGProjectCharacter::OnMouseX);
	//PlayerInputComponent->BindAxis("MouseY", this, &AGProjectCharacter::OnMouseY);

}

void AGProjectCharacter::CharacterDodge()
{
	GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Red,TEXT("Dodge"));
}

void AGProjectCharacter::OnSightChanged()
{

	
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

void AGProjectCharacter::OnInteractionPressed()
{
	//if (InteractionComp)
	//{
	//	InteractionComp->PrimaryInteract();
	//}
}

void AGProjectCharacter::OnInteractionReleased()
{
}


void AGProjectCharacter::OnLeftShiftPressed()
{
	FTimerDelegate DodgeDelegate;
	DodgeDelegate.BindLambda([this](){ ActionComponent->StartActionByName(this, "Sprint"); });
	GetWorldTimerManager().SetTimer(LShiftTimer, DodgeDelegate, 0.15f, false);
}

void AGProjectCharacter::OnLeftShiftReleased()
{
	if (GetWorldTimerManager().IsTimerActive(LShiftTimer))
		ActionComponent->StartActionByName(this, "Dodge");

	GetWorldTimerManager().ClearTimer(LShiftTimer);

	ActionComponent->StopActionByName(this, "Sprint");
}

void AGProjectCharacter::OnRClickPressed()
{
	bUsingOptic=true;
}

void AGProjectCharacter::OnRClickReleased()
{	
	bUsingOptic=false;
	CameraDest=FVector::ZeroVector;
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

//void AGProjectCharacter::OnMouseX(const float Axis)
//{
//	/*if (bHasCameraDestUpdated)
//	{
//		CameraDest.Y += Axis * CameraSpeed;
//		bHasCameraDestUpdated = false;
//	}*/
//}
//
//void AGProjectCharacter::OnMouseY(const float Axis)
//{
//	/*if (bHasCameraDestUpdated)
//	{
//		CameraDest.X += Axis * CameraSpeed;
//		bHasCameraDestUpdated = false;
//	}*/
//}