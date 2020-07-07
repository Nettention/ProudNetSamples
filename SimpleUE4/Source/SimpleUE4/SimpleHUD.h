// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "GameFramework/HUD.h"
#include "SimpleHUD.generated.h"

UCLASS()
class SIMPLEUE4_API ASimpleHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly);
	UTexture2D* m_background;

public:
	virtual void DrawHUD() override;
};
