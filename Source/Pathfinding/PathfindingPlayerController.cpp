// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PathfindingPlayerController.h"

APathfindingPlayerController::APathfindingPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	bEnableMouseOverEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	EnableInput(this);
}
