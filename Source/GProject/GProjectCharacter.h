// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GProjectCharacter.generated.h"

UCLASS(Blueprintable)
class AGProjectCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGProjectCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void CharacterDodge();

	void OnSightChanged();

	void OnSpacePressed();
	void OnSpaceReleased();

	FTimerHandle LShiftTimer;
	void OnLeftShiftPressed();
	void OnLeftShiftReleased();

	void OnRClickPressed();
	void OnRClickReleased();

	void OnMoveForward(const float Value);
	void OnMoveRight(const float Value);

	void OnMouseX(const float Axis);
	void OnMouseY(const float Axis);
	

	//bool bPressedSpace;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bUsingOptic;

	bool bHasCameraDestUpdated;

	FVector StaticCameraDistance;
	FVector CameraDest;
	float CameraSpeed;
	float CameraSmoothness;
	FVector AimedPosition;
};

