// Copyright Epic Games, Inc. All Rights Reserved.


#include "SimpleUE4GameModeBase.h"

void ASimpleUE4GameModeBase::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (m_currentWidget != nullptr)
	{
		m_currentWidget->RemoveFromViewport();
		m_currentWidget = nullptr;
	}
	if (NewWidgetClass != nullptr)
	{
		m_currentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (m_currentWidget != nullptr)
		{
			m_currentWidget->AddToViewport();
		}
	}
}

void ASimpleUE4GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// 프로젝트 셋팅에서 지정한 위젯이 최초의 메뉴 위젯이 되도록 합니다.
	ChangeMenuWidget(m_startingWidgetClass);
}

void ASimpleUE4GameModeBase::SendRMI()
{
	g_simpleProxy.Chat(Proud::HostID_Server, Proud::RmiContext::ReliableSend, _PNT("Hi ~ I'm UE4 ~"), 44, 44.4f);
}

void ASimpleUE4GameModeBase::SendP2PRMI()
{
	g_simpleProxy.P2PChat(g_recentP2PGroupHostID, Proud::RmiContext::ReliableSend, _PNT("Are we all UE4 ?"), 22, 22.2f);
}

FString ASimpleUE4GameModeBase::GetMessage()
{
	return FString(g_message.GetString());
}
