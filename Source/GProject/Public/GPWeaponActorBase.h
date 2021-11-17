// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "Component/GPAbilitySystemComponent.h"
#include "Attributes/GPWeaponAttributeSet.h"
#include "GPWeaponActorBase.generated.h"

//class USoundCue;
//class UAnimSequence;
//class UGPItemAttachment;

UCLASS()
class GPROJECT_API AGPWeaponActorBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAttributeDelegate, float, DeltaValue, const struct FGameplayTagContainer&, EventTags);

public:	
	// Sets default values for this actor's properties
	AGPWeaponActorBase();

	UGPAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// Implement IAbilitySystemInterface
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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

	UFUNCTION(BlueprintCallable)
	class UGPItemAttachment* AddAttachment(UGPItemAttachment* inAttachmentItem);

	UFUNCTION(BlueprintCallable)
	UGPItemAttachment* RemoveAttachment(const EWAttachmentType& inType);

	UFUNCTION(BlueprintCallable,BlueprintPure) 
	bool IsAttachableItem(UGPItemAttachment* inAttachmentItem) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnWeaponDeactivate();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon")
	void OnWeaponReloadStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnWeaponReloadEnd();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon")
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
	float GetFireRate() const;

	UFUNCTION(BlueprintCallable)
	float GetAccuracy() const;

	UFUNCTION(BlueprintCallable)
	float GetReloadSpeed() const;

	UFUNCTION(BlueprintCallable)
	FVector2D GetRecoil() const;

	UFUNCTION(BlueprintCallable)
	float GetOpticMagnification() const;

	UFUNCTION(BlueprintCallable)
	EWFiringMode GetCurrentFiringMode() const;
	
	UFUNCTION(BlueprintCallable)
	bool CanActivateWeapon() const;

	UPROPERTY(BlueprintAssignable)
	FAttributeDelegate OnCurrentMagChanged_D;

	UPROPERTY(BlueprintAssignable)
	FAttributeDelegate OnMagSizeChanged_D;

protected:
	bool bInitialized;

	EWFireType CurrentFireMode;

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

	UPROPERTY(EditAnywhere, BlueprintReadonly)
	FPrimaryAssetId WeaponItem;

	UPROPERTY(EditAnywhere, BlueprintReadonly)
	FPrimaryAssetId AmmoItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Attachment)
	TArray<EWAttachmentType> AttachableTypes;

	//UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = Attachment)
	TMap<EWAttachmentType, UGPItemAttachment*> AttachmentSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attachment)
	TArray<EWeaponType> WeaponType;

	//TMap<EWAttachmentType, UGPWAttachmentComponent*> AttachmentComponents;

	void GASComponentInitialize();

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnCurrentMagChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	virtual void HandleCurrentMagChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnMagSizeChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	virtual void HandleMagSizeChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	friend UGPWeaponAttributeSet;
	//friend UGPWAttachmentComponent;
};


USTRUCT(BlueprintType)
struct GPROJECT_API FGPWeaponSpec
{
	GENERATED_BODY()

	FGPWeaponSpec()
	{}

	/** The number of instances of this item in the inventory, can never be below 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	TMap<EWAttachmentType, UGPItemAttachment*> AttachedItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	float FilledMag;

};
