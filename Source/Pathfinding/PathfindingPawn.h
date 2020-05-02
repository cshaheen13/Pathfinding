// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PathfindingPawn.generated.h"

UCLASS(config=Game)
class APathfindingPawn : public APawn
{
	GENERATED_UCLASS_BODY()

public:

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

protected:
	void OnResetVR();
	void TriggerClick();
	void SetStart();
	void SetEnd();
	void SetWall();
	void ReleaseWall();
	void ResetBlock();
	void ReleaseReset();
	void TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers);
	bool bLeftMouseHeld = false;
	bool bRightMouseHeld = false;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class APathfindingBlock* CurrentBlockFocus;
};
