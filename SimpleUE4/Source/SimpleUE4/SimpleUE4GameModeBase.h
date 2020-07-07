// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"

#define WIN32_LEAN_AND_MEAN
#include "ProudNetClient.h"
#include "ProudNetActor.h"

#include "SimpleUE4GameModeBase.generated.h"

UCLASS()
class SIMPLEUE4_API ASimpleUE4GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// 메뉴 위젯을 바꾸는 함수입니다. 블루프린트에 의해서 사용되어집니다.
	UFUNCTION(BlueprintCallable, Category = "UMG Game")
		void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	// Send 버튼이 눌렸을 때 블루프린트에 의해서 호출되어지는 서버로의 메세지 송신 함수입니다.
	UFUNCTION(BlueprintCallable, Category = "ProudNet")
		void SendRMI();

	// SendP2P 버튼이 눌렸을 때 블루프린트에 의해서 호출되어지는 P2P 그룹 내 피어들로의 메세지 송신 함수입니다.
	UFUNCTION(BlueprintCallable, Category = "ProudNet")
		void SendP2PRMI();

	// 전역 메세지 객체를 블루프린트가 얻어오는 함수입니다.
	UFUNCTION(BlueprintCallable, Category = "ProudNet")
		FString GetMessage();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
		TSubclassOf<UUserWidget> m_startingWidgetClass;

	UPROPERTY()
		UUserWidget* m_currentWidget;
};
