// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PathfindingPawn.h"
#include "PathfindingBlock.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

APathfindingPawn::APathfindingPawn(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APathfindingPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UCameraComponent* OurCamera = PC->GetViewTarget()->FindComponentByClass<UCameraComponent>())
			{
				FVector Start = OurCamera->GetComponentLocation();
				FVector End = Start + (OurCamera->GetComponentRotation().Vector() * 8000.0f);
				TraceForBlock(Start, End, true);
			}
		}
		else
		{
			FVector Start, Dir, End;
			PC->DeprojectMousePositionToWorld(Start, Dir);
			End = Start + (Dir * 8000.0f);
			TraceForBlock(Start, End, false);
		}
	}
}

void APathfindingPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", EInputEvent::IE_Pressed, this, &APathfindingPawn::OnResetVR);
	PlayerInputComponent->BindAction("TriggerClick", EInputEvent::IE_Pressed, this, &APathfindingPawn::TriggerClick);
	PlayerInputComponent->BindAction("SetStart", EInputEvent::IE_Pressed, this, &APathfindingPawn::SetStart);
	PlayerInputComponent->BindAction("SetEnd", EInputEvent::IE_Pressed, this, &APathfindingPawn::SetEnd);
	PlayerInputComponent->BindAction("SetWall", EInputEvent::IE_Pressed, this, &APathfindingPawn::SetWall);
	PlayerInputComponent->BindAction("SetWall", EInputEvent::IE_Released, this, &APathfindingPawn::ReleaseWall);
	PlayerInputComponent->BindAction("ResetBlock", EInputEvent::IE_Pressed, this, &APathfindingPawn::ResetBlock);
	PlayerInputComponent->BindAction("ResetBlock", EInputEvent::IE_Released, this, &APathfindingPawn::ReleaseReset);
	PlayerInputComponent->BindAction("ResetBoard", EInputEvent::IE_Pressed, this, &APathfindingPawn::ResetBoard);

}

void APathfindingPawn::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);

	OutResult.Rotation = FRotator(-90.0f, -90.0f, 0.0f);
}

void APathfindingPawn::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void APathfindingPawn::TriggerClick()
{
	if (CurrentBlockFocus)
	{
		CurrentBlockFocus->HandleClicked("Trigger");
	}
}

void APathfindingPawn::SetStart()
{
	//TODO
	//Allow for only one start  to be selected, reset the other when a new one is selected
	//Highlight start with a specific color (gold)
	if (CurrentBlockFocus)
	{
		CurrentBlockFocus->HandleClicked("Start");
	}
}

void APathfindingPawn::SetEnd()
{
	//TODO
	//Allow for only one end to be selected, reset the other when a new one is selected
	//Highlight end with a specific color (purple)
	if (CurrentBlockFocus)
	{
		CurrentBlockFocus->HandleClicked("End");
	}
}

void APathfindingPawn::SetWall()
{
	bLeftMouseHeld = true;
	if (CurrentBlockFocus)
	{
		CurrentBlockFocus->HandleClicked("Wall");
	}
}

void APathfindingPawn::ReleaseWall()
{
	bLeftMouseHeld = false;
}

void APathfindingPawn::ResetBlock()
{
	bRightMouseHeld = true;
	if (CurrentBlockFocus)
	{
		CurrentBlockFocus->HandleClicked("Reset");
	}
}

void APathfindingPawn::ReleaseReset()
{
	bRightMouseHeld = false;
}

void APathfindingPawn::ResetBoard()
{
	UE_LOG(LogTemp, Warning, TEXT("Reset Board"));
}

void APathfindingPawn::TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers)
{
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
	if (bDrawDebugHelpers)
	{
		DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red);
		DrawDebugSolidBox(GetWorld(), HitResult.Location, FVector(20.0f), FColor::Red);
	}
	if (HitResult.Actor.IsValid())
	{
		APathfindingBlock* HitBlock = Cast<APathfindingBlock>(HitResult.Actor.Get());
		if (CurrentBlockFocus != HitBlock)
		{
			if (bLeftMouseHeld)
			{
				SetWall();
			}
			else if (bRightMouseHeld)
			{
				ResetBlock();
			}

			if (CurrentBlockFocus)
			{
				//CurrentBlockFocus->Highlight(false);
			}

			if (HitBlock)
			{
				//HitBlock->Highlight(true);
			}
			CurrentBlockFocus = HitBlock;
		}
	}
	else if (CurrentBlockFocus)
	{
		CurrentBlockFocus->Highlight(false);
		CurrentBlockFocus = nullptr;
	}
}