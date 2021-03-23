// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GProject.h"
#include "GPCharacterBase.h"
#include "GProjectCharacter.generated.h"

class UActionComponent;
class UGPInteractionComponent;

UCLASS(Blueprintable)
class AGProjectCharacter : public AGPCharacterBase
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
	
	void OnInteractionPressed();
	void OnInteractionReleased();	

	FTimerHandle LShiftTimer;
	void OnLeftShiftPressed();
	void OnLeftShiftReleased();

	void OnRClickPressed();
	void OnRClickReleased();

	void OnMoveForward(const float Value);
	void OnMoveRight(const float Value);

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

	FVector CameraDest;
	float CameraSmoothness;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UActionComponent* ActionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UGPInteractionComponent* InteractionComponent;

	//FVector AimedPosition;
};

