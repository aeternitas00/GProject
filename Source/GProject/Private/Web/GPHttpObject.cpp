// Fill out your copyright notice in the Description page of Project Settings.


#include "Web/GPHttpObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UGPHttpObject::UGPHttpObject(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Http = &FHttpModule::Get();
}

// ============ Setups ==============

FHttpRequestRef UGPHttpObject::RequestSetup(const FString& url, const FString& Method)
{
	FHttpRequestRef Request = Http->CreateRequest();

	//This is the url on which to process the request
	Request->SetURL(url);
	Request->SetVerb(Method);
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "application/json");

	return Request;
}

FHttpRequestRef UGPHttpObject::RequestSetup(const FString& url, const FString& Method, const FString& Body)
{
	FHttpRequestRef Request = RequestSetup(url, Method);

	Request->SetContentAsString(Body);

	return Request;
}

FString UGPHttpObject::JSONWrite(const FString& Key, const FString& Value)
{
	FString outStr;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&outStr);

	JsonWriter->WriteObjectStart();

	JsonWriter->WriteValue(Key, Value);

	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

	return outStr;
}

FString UGPHttpObject::JSONWrite(const FStrKeyValueArray& Array)
{
	FString outStr;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&outStr);

	JsonWriter->WriteObjectStart();

	for ( auto Pair : Array )
		JsonWriter->WriteValue(Pair.Key, Pair.Value);

	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

	return outStr;
}

TSharedPtr<FJsonObject> UGPHttpObject::JSONRead(FHttpResponsePtr Response)
{
	TSharedPtr<FJsonObject> JsonObject;

	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		return JsonObject;
	}

	return NULL;
}

TSharedPtr<FJsonObject> UGPHttpObject::JSONRead(FHttpResponsePtr Response, FString& outStatus)
{
	TSharedPtr<FJsonObject> JsonObject = JSONRead(Response);

	if (JsonObject != NULL)	
		outStatus = JsonObject->GetStringField("status");

	return JsonObject;
}

//=========== Custom Requests ============//


void UGPHttpObject::Login(const FString& inEmail, const FString& inPassword)
{
	FHttpRequestRef Request = RequestSetup("http://localhost/account/login", "POST");

	FStrKeyValueArray Array = {
		FStrKeyValuePair(TEXT("email"), inEmail),
		FStrKeyValuePair(TEXT("pass"), inPassword)
	};

	Request->SetContentAsString(JSONWrite(Array));
	Request->OnProcessRequestComplete().BindUObject(this, &UGPHttpObject::OnLoginReceived);

	Request->ProcessRequest();
}

void UGPHttpObject::LoginCheck()
{
	FHttpRequestRef Request = RequestSetup("http://localhost/account/check", "POST");

	Request->OnProcessRequestComplete().BindUObject(this, &UGPHttpObject::OnResponseReceived);

	Request->ProcessRequest();
}

void UGPHttpObject::Logout()
{
	FHttpRequestRef Request = RequestSetup("http://localhost/account/logout", "POST");

	Request->OnProcessRequestComplete().BindUObject(this, &UGPHttpObject::OnResponseReceived);

	Request->ProcessRequest();
}


void UGPHttpObject::UserList()
{
	FHttpRequestRef Request = RequestSetup("http://localhost/users/", "GET");

	Request->OnProcessRequestComplete().BindUObject(this, &UGPHttpObject::OnResponseReceived);

	Request->ProcessRequest();
}

void UGPHttpObject::AchievementList()
{
}

void UGPHttpObject::AchievementCurrentInfo(const int32& AchID)
{
}

void UGPHttpObject::AchievementUpdate(const int32& AchID, const int32& Progress, bool Achieved)
{
}


//=============== Receive Response ===================


void UGPHttpObject::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString Status;
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject = JSONRead(Response, Status);

	//Deserialize the json data given Reader and the actual object to deserialize
	if (JsonObject != NULL)
	{
		GP_LOG(Display,TEXT("Status : %s"),*Status);
		//GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, *Status );
	}

}

void UGPHttpObject::OnLoginReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString Status;
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject = JSONRead(Response, Status);

	bool Succeed = false;

	//Deserialize the json data given Reader and the actual object to deserialize
	if (JsonObject != NULL)
	{
		GP_LOG(Display, TEXT("Login Response Received"));

		if (Status == "ok")
		{	
			Succeed = true;
			GP_LOG(Display, TEXT("Status : OK"));
		}
		else
		{
			Succeed = false;
			GP_LOG(Display, TEXT("Status : Not OK / Message : %s"), *JsonObject->GetStringField("message"));
		}
	}

	OnLoginReceivedDelegate.Broadcast(Succeed);
}

void UGPHttpObject::BeginDestroy()
{
	FHttpRequestRef Request = RequestSetup("http://localhost/account/logout", "POST");
	Request->ProcessRequest();

	Super::BeginDestroy();
}

