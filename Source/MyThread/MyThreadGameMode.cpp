// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyThreadGameMode.h"
#include "MyThreadHUD.h"
#include "MyThreadCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMyThreadGameMode::AMyThreadGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMyThreadHUD::StaticClass();
}
