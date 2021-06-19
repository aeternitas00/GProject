// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "GPHttpObject.generated.h"

typedef TPair<FString, FString> FStrKeyValuePair;
typedef TArray<FStrKeyValuePair> FStrKeyValueArray;

UCLASS(BlueprintType)
class GPROJECT_API UGPHttpObject : public UObject
{
	GENERATED_BODY()
	
public:
	UGPHttpObject(const class FObjectInitializer& ObjectInitializer);
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "HTTP")
	void Login(const FString& inEmail, const FString& inPassword);

	UFUNCTION(BlueprintCallable, Category = "HTTP")
	void LoginCheck();

	UFUNCTION(BlueprintCallable, Category = "HTTP")
	void Logout();

	UFUNCTION(BlueprintCallable, Category = "HTTP")
	void UserList();

	UFUNCTION(BlueprintCallable, Category = "HTTP")
	void AchievementList();

	UFUNCTION(BlueprintCallable, Category = "HTTP")
	void AchievementCurrentInfo(const int32& AchID);

	UFUNCTION(BlueprintCallable, Category = "HTTP")
	void AchievementUpdate(const int32& AchID, const int32& Progress, bool Achieved);

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnLoginReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

protected:
	FHttpModule* Http;

	FHttpRequestRef RequestSetup(const FString& url, const FString& Method);
	FHttpRequestRef RequestSetup(const FString& url, const FString& Method, const FString& Body);

	FString JSONWrite(const FString& Key, const FString& Value);
	FString JSONWrite(const FStrKeyValueArray& Array);

	TSharedPtr<FJsonObject> JSONRead(FHttpResponsePtr Response);
	TSharedPtr<FJsonObject> JSONRead(FHttpResponsePtr Response, FString& outStatus);

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginReceived, bool, Succeed);

	UPROPERTY(BlueprintAssignable)
	FOnLoginReceived OnLoginReceivedDelegate;
};


