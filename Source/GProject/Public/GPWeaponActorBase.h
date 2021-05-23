// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "Component/GPAbilitySystemComponent.h"
#include "GPWeaponAttributeSet.h"
#include "GPWeaponActorBase.generated.h"

//class USoundCue;
//class UAnimSequence;

UCLASS()
class GPROJECT_API AGPWeaponActorBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGPWeaponActorBase();

	UGPAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// Implement IAbilitySystemInterface
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadonly)
	FPrimaryAssetId WeaponItem;

	UPROPERTY(EditAnywhere, BlueprintReadonly)
	FPrimaryAssetId AmmoItem;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	TMap<EWAttachmentType,class UGPWAttachmentComponent*> AttachmentSlot;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PreInitializeComponents() override;

	UFUNCTION(BlueprintCallable)
	UGPGameplayAbility* TryActivateWeapon();

	UFUNCTION(BlueprintCallable)
	void DeactivateWeapon();

	UFUNCTION(BlueprintCallable)
	void ReloadWeapon(float inValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnWeaponActivate();

	//UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon")
	//void OnWeaponFire();

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnWeaponDeactivate();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon")
	void OnWeaponReloadStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnWeaponReloadEnd();

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnWeaponRaised();

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnWeaponLowered();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Weapon")
	bool IsWeaponHasToReload() const;

	UFUNCTION(BlueprintCallable)
	float GetCurrentMag() const;

	UFUNCTION(BlueprintCallable)
	float GetMagSize() const;

	UFUNCTION(BlueprintCallable)
	EWFiringMode GetCurrentFiringMode() const;
	
	UFUNCTION(BlueprintCallable)
	bool CanActivateWeapon() const;
protected:
	bool bInitialized;

	EWFireType CurrentFireMode;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	FGameplayTagContainer Tagcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	TMap<EWFireType, EWFiringMode> FiringModeMap;

	UPROPERTY()
	UGPAbilitySystemComponent* W_AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly)
	UGPWeaponAttributeSet* W_AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	TSubclassOf<UGameplayEffect> DefaultAttributes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Abilities)
	TMap<EWFireType,TSubclassOf<class UGPGameplayAbility>> WeaponAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	UAnimMontage* WeaponAnimMontage;
	//TMap<EWFireType, FGameplayAbilitySpecHandle> AbilityHandles;

	void GASComponentInitialize();

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnCurrentMagChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	virtual void HandleCurrentMagChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnMagSizeChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	virtual void HandleMagSizeChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	friend UGPWeaponAttributeSet;
};
