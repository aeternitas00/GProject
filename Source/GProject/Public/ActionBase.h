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

/// 액션 : 캐릭터가 취할 수 있는 모든 행동들
/// ex) 달리기, 장비의 사용, 상호 작용 등등
UCLASS(Blueprintable)
class GPROJECT_API UActionBase : public UObject
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> Icon;

	// 오너
	UPROPERTY(Replicated)
	UActionComponent* ActionComp;

	// 오너 게터
	UFUNCTION(BlueprintCallable, Category = "Action")
	UActionComponent* GetOwningComponent() const;

	// 태그로 액션 블로킹 제어 - 이 액션이 실행 될 때 이 컨테이너에 있는 태그를 추가, 끝날 때 제거
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer GrantsTags;

	// 이 컨테이너에 있는 태그를 오너가 가지고 있지 않을 때 실행 가능
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer BlockedTags;

	// 서버 관리용 레플리케이션 데이터
	UPROPERTY(ReplicatedUsing = "OnRep_RepData")
	FActionRepData RepData;

	UPROPERTY(Replicated)
	float TimeStarted;

	UFUNCTION()
	void OnRep_RepData();

public:

	void Initialize(UActionComponent* NewActionComp);

	// 이 액션이 추가 될 때 자동 실행의 여부
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	bool bAutoStart;

	// 레플리케이션 데이터 참조하여 실행중인지 파악
	UFUNCTION(BlueprintCallable, Category = "Action")
	bool IsRunning() const;

	// 오너에 대한 태그 조사
	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	bool CanStart(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* Instigator);

	// 액션 컴포넌트의 StartActionByName 메소드 이용 위함 
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FName ActionName;

	UWorld* GetWorld() const override;

	bool IsSupportedForNetworking() const override
	{
		return true;
	}
};
