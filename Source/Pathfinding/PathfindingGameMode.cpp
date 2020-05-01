// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PathfindingGameMode.h"
#include "PathfindingPlayerController.h"
#include "PathfindingPawn.h"

APathfindingGameMode::APathfindingGameMode()
{
	// no pawn by default
	DefaultPawnClass = APathfindingPawn::StaticClass();
	// use our own player controller class
	PlayerControllerClass = APathfindingPlayerController::StaticClass();
}
