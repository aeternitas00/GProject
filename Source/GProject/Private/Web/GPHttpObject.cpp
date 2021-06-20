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

	Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		FString Status;
		//Create a pointer to hold the json serialized data
		TSharedPtr<FJsonObject> JsonObject = this->JSONRead(Response, Status);

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
		this->OnLoginReceivedDelegate.Broadcast(Succeed);
		this->LoginCheck();
	});

	Request->ProcessRequest();
}

void UGPHttpObject::LoginCheck()
{
	FHttpRequestRef Request = RequestSetup("http://localhost/account/check", "POST");

	Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		FString Status;
		//Create a pointer to hold the json serialized data
		TSharedPtr<FJsonObject> JsonObject = this->JSONRead(Response, Status);

		//Deserialize the json data given Reader and the actual object to deserialize
		if (JsonObject != NULL)
		{
			GP_LOG(Display, TEXT("LoginCheck Response Received"));

			if (Status == "ok")
			{
				GP_LOG(Display, TEXT("Status : OK"));
				this->CurrentDisplayName = JsonObject->GetStringField("displayname");
				this->CurrentEmail = JsonObject->GetStringField("email");
			}
			else
			{
				GP_LOG(Display, TEXT("Status : Not OK / Message : %s"), *JsonObject->GetStringField("message"));
			}
		}
	});

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

void UGPHttpObject::AchievementList(const FString& Email)
{
	FString URL = "http://localhost/achievement/" + Email;
	if (Email.IsEmpty()) URL += CurrentEmail;
	FHttpRequestRef Request = RequestSetup(URL, "GET");

	Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		FString Status;
		//Create a pointer to hold the json serialized data
		TSharedPtr<FJsonObject> JsonObject = this->JSONRead(Response, Status);

		//Deserialize the json data given Reader and the actual object to deserialize
		if (JsonObject != NULL)
		{
			GP_LOG(Display, TEXT("AchievementList Response Received"));

			if (Status == "ok")
			{
				GP_LOG(Display, TEXT("Status : OK"));
				TArray<TSharedPtr<FJsonValue>> Achievements = JsonObject->GetArrayField("achievements");
				
				TArray<FGPAchievementData> ReturnArray;

				for (auto Ach : Achievements)
				{
					ReturnArray.Add(FGPAchievementData(Ach->AsObject()->GetIntegerField("AchievementID"), Ach->AsObject()->GetNumberField("Progress")));
					GP_LOG(Display, TEXT("ID %d : Progress : %f"), Ach->AsObject()->GetIntegerField("AchievementID"),Ach->AsObject()->GetNumberField("Progress"));
				}

				this->OnAchievementReceivedDelegate.Broadcast(ReturnArray);
			}
			else
			{
				GP_LOG(Display, TEXT("Status : Not OK / Message : %s"), *JsonObject->GetStringField("message"));
			}
		}
	});

	Request->ProcessRequest();
}

void UGPHttpObject::AchievementUpdate(const int32& AchID, const float& Progress, bool bIsAdd, const FString& Email)
{
	FHttpRequestRef Request = RequestSetup("http://localhost/achievement", "POST");

	FString outStr;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&outStr);

	JsonWriter->WriteObjectStart();

	if (Email.IsEmpty())	JsonWriter->WriteValue(TEXT("email"), *CurrentEmail);
	else 					JsonWriter->WriteValue(TEXT("email"), Email);
	JsonWriter->WriteValue(TEXT("achievement"), AchID);
	JsonWriter->WriteValue(TEXT("progress"), Progress);
	JsonWriter->WriteValue(TEXT("add"), bIsAdd);

	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

	Request->SetContentAsString(outStr);

	Request->OnProcessRequestComplete().BindLambda([this, Email](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		FString Status;
		TSharedPtr<FJsonObject> JsonObject = JSONRead(Response, Status);

		//Deserialize the json data given Reader and the actual object to deserialize
		if (JsonObject != NULL)
		{
			GP_LOG(Display, TEXT("AchievementUpdate Response Received"));

			GP_LOG(Display, TEXT("Status : %s / Message : %s"), *Status, *JsonObject->GetStringField("message"));
			
			if (Status == "ok")
				AchievementList(Email);

			//OnAchUpdateReceivedDelegate->Broadcast(Email);
			//const TSharedPtr<FJsonObject>* ReqJsonObject;
			//if (JsonObject->TryGetObjectField("request", ReqJsonObject))
			//{
			//	GP_LOG(Display, TEXT("Got Request"));
			//	GP_LOG(Display, TEXT("Email : %s"), *ReqJsonObject->Get()->GetStringField("email"));
			//	GP_LOG(Display, TEXT("ACID : %d"), ReqJsonObject->Get()->GetIntegerField("achievement"));
			//	GP_LOG(Display, TEXT("Progress : %f"), ReqJsonObject->Get()->GetNumberField("progress"));
			//	GP_LOG(Display, TEXT("Add : %s"), ReqJsonObject->Get()->GetBoolField("add") ? TEXT("True") : TEXT("False"));
			//}
		}
	});
	Request->ProcessRequest();
}


//=============== Receive Response ===================


void UGPHttpObject::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject = JSONRead(Response);

	//Deserialize the json data given Reader and the actual object to deserialize
	if (JsonObject != NULL)
	{
		GP_LOG(Display, TEXT("Response : %s"), *Response->GetContentAsString());
	}

}

void UGPHttpObject::BeginDestroy()
{
	FHttpRequestRef Request = RequestSetup("http://localhost/account/logout", "POST");
	Request->ProcessRequest();

	Super::BeginDestroy();
}

