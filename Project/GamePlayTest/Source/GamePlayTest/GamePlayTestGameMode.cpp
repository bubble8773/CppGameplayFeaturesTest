// Copyright Epic Games, Inc. All Rights Reserved.

#include "GamePlayTestGameMode.h"
#include "GamePlayTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGamePlayTestGameMode::AGamePlayTestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
