// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "Types/GPTypes.h"

GPROJECT_API DECLARE_LOG_CATEGORY_EXTERN(LogGProject, Log, All);

DECLARE_STATS_GROUP(TEXT("GProject_Game"), STATGROUP_GPGAME, STATCAT_Advanced);

#define GP_LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define GP_LOG_C(Verbosity) UE_LOG(LogGProject, Verbosity, TEXT("%s"), *GP_LOG_CALLINFO)
#define GP_LOG(Verbosity, Format, ...) UE_LOG(LogGProject, Verbosity, TEXT("%s %s"), *GP_LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__)) 