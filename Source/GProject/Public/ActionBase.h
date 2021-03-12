// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionComponent.h"
#include "GameplayTagContainer.h"
#include "ActionBase.generated.h"

class UWorld;


USTRUCT()
struct FActionRepData
{
	GENERATED_BODY()

public:

	UPROPERTY()
	bool bIsRunning;

	UPROPERTY()
	AActor* Instigator;
};

/// �׼� : ĳ���Ͱ� ���� �� �ִ� ��� �ൿ��
/// ex) �޸���, ����� ���, ��ȣ �ۿ� ���
UCLASS(Blueprintable)
class GPROJECT_API UActionBase : public UObject
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> Icon;

	// ����
	UPROPERTY(Replicated)
	UActionComponent* ActionComp;

	// ���� ����
	UFUNCTION(BlueprintCallable, Category = "Action")
	UActionComponent* GetOwningComponent() const;

	// �±׷� �׼� ���ŷ ���� - �� �׼��� ���� �� �� �� �����̳ʿ� �ִ� �±׸� �߰�, ���� �� ����
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer GrantsTags;

	// �� �����̳ʿ� �ִ� �±׸� ���ʰ� ������ ���� ���� �� ���� ����
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer BlockedTags;

	// ���� ������ ���ø����̼� ������
	UPROPERTY(ReplicatedUsing = "OnRep_RepData")
	FActionRepData RepData;

	UPROPERTY(Replicated)
	float TimeStarted;

	UFUNCTION()
	void OnRep_RepData();

public:

	void Initialize(UActionComponent* NewActionComp);

	// �� �׼��� �߰� �� �� �ڵ� ������ ����
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	bool bAutoStart;

	// ���ø����̼� ������ �����Ͽ� ���������� �ľ�
	UFUNCTION(BlueprintCallable, Category = "Action")
	bool IsRunning() const;

	// ���ʿ� ���� �±� ����
	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	bool CanStart(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* Instigator);

	// �׼� ������Ʈ�� StartActionByName �޼ҵ� �̿� ���� 
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FName ActionName;

	UWorld* GetWorld() const override;

	bool IsSupportedForNetworking() const override
	{
		return true;
	}
};
