// Copyright Epic Games, Inc. All Rights Reserved.


#include "RPGProject.h"

#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"

// FDefaultGameModuleImpl 대신 FDefaultModuleImpl을 사용할 수도 있습니다.
class FRPGProjectModule : public FDefaultGameModuleImpl
{

};

// IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, RPGProject, "RPGProject" );
IMPLEMENT_PRIMARY_GAME_MODULE( FRPGProjectModule, RPGProject, "RPGProject" );
