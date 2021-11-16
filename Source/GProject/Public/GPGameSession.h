// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "GameFramework/GameSession.h"
#include "GPGameSession.generated.h"

/**
 * 
 */
UCLASS()
class GPROJECT_API AGPGameSession : public AGameSession
{
	GENERATED_BODY()

	virtual void RegisterServer() override;
};
