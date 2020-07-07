// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>
#include <sstream>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#define WIN32_LEAN_AND_MEAN
#include "ProudNetClient.h"

#include "../../../Simple/Common/Vars.h"

#include "../../../Simple/Common/Simple_common.h"
#include "../../../Simple/Common/Simple_proxy.h"
#include "../../../Simple/Common/Simple_stub.h"

#include "ProudNetActor.generated.h"

extern Simple::Proxy g_simpleProxy;
extern Proud::HostID g_recentP2PGroupHostID;
extern Proud::String g_message;

class SimpleStub : public Simple::Stub
{
public:
	DECRMI_Simple_ShowChat;
	DECRMI_Simple_SystemChat;
	DECRMI_Simple_P2PChat;
};

extern SimpleStub g_simpleStub;

UCLASS()
class SIMPLEUE4_API AProudNetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AProudNetActor();
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTimeSecond) override;

	std::shared_ptr<Proud::CNetClient> m_pNetClient;
};
