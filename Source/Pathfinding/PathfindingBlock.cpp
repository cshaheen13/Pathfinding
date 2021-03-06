// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PathfindingBlock.h"
#include "PathfindingBlockGrid.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"
#include "UObject/NameTypes.h"
#include "InputCoreTypes.h"

APathfindingBlock::APathfindingBlock()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlueMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> WallMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> StartMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> EndMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> PathMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.BlueMaterial"))
			, WallMaterial(TEXT("/Game/Puzzle/Meshes/WallMaterial.WallMaterial"))
			, StartMaterial(TEXT("/Game/Puzzle/Meshes/GoldMaterial.GoldMaterial"))
			, EndMaterial(TEXT("/Game/Puzzle/Meshes/M_Tech_Hex_Tile_Pulse_Inst.M_Tech_Hex_Tile_Pulse_Inst"))
			, PathMaterial(TEXT("/Game/Puzzle/Meshes/PathMaterial.PathMaterial"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(0.25f,0.25f,1.0f));
	BlockMesh->SetRelativeLocation(FVector(0.f,0.f,0.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BlueMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);
	BlockMesh->OnClicked.AddDynamic(this, &APathfindingBlock::BlockClicked);
	BlockMesh->OnInputTouchBegin.AddDynamic(this, &APathfindingBlock::OnFingerPressedBlock);
	BlockMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block);

	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();
	BlueMaterial = ConstructorStatics.BlueMaterial.Get();
	WallMaterial = ConstructorStatics.WallMaterial.Get();
	StartMaterial = ConstructorStatics.StartMaterial.Get();
	EndMaterial = ConstructorStatics.EndMaterial.Get();
	PathMaterial = ConstructorStatics.PathMaterial.Get();

	PrimaryActorTick.bCanEverTick = true;
	bIsHighlightTimeSet = false;
	SetActorTickEnabled(false);

	Distance = 999;
	bVisited = false;
	bIsWall = false;
	bIsStart = false;
	bIsEnd = false;
	bIsEdgeWall = false;
	bMazeVisited = false;
}

//Called every frame
void APathfindingBlock::Tick(float DeltaTime)
{
	if (!bIsHighlightTimeSet)
	{
		bIsHighlightTimeSet = true;
		HighlightTime = Distance - (Distance / 1.25);
		PathStartTime = OwningGrid->EndDistance - (OwningGrid->EndDistance / 1.25) + 1;
		PathTime = PathStartTime + (HighlightTime - (HighlightTime/2));
	}

	RunningTime += DeltaTime;

	if ((bIsHighlightTimeSet) && (RunningTime >= HighlightTime))
	{
		Highlight(true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Highlight Time Set = %s"), bIsHighlightTimeSet ? TEXT("true") : TEXT("false"));
		UE_LOG(LogTemp, Warning, TEXT("Running Time = %f"), RunningTime);
	}

	if (bisShortestPath && (RunningTime > PathTime))
	{
		BlockMesh->SetMaterial(0, PathMaterial);
	}
}

void APathfindingBlock::BlockClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked)
{
	FString Button = ButtonClicked.ToString();
	HandleClicked("Wall");
}


void APathfindingBlock::OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	HandleClicked("Trigger");
}

void APathfindingBlock::HandleClicked(FString HighlightType)
{
	// Check we are not already active
	if (!bIsActive)
	{
		bIsActive = true;

		// Change material
		if (HighlightType == "Wall")
		{
			BlockMesh->SetMaterial(0, WallMaterial);
			BlockMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Ignore);
			bIsWall = true;
		}
		else if (HighlightType == "Start")
		{
			BlockMesh->SetMaterial(0, StartMaterial);
			Distance = 0;
			bIsStart = true;
		}
		else if (HighlightType == "End")
		{
			BlockMesh->SetMaterial(0, EndMaterial);
			bIsEnd = true;
		}
		else if (HighlightType == "Reset")
		{
			BlockMesh->SetMaterial(0, BlueMaterial);
			bIsActive = false;
			bIsWall = false;
			bIsStart = false;
			bIsEnd = false;
			bIsEdgeWall = false;
			bMazeVisited = false;
			Distance = 999;
			MazeIndex = 0;
		}

		// Tell the Grid
		if (OwningGrid != nullptr)
		{
			OwningGrid->AddScore();
		}
	}
	else
	{
		if (HighlightType == "Reset")
		{
		BlockMesh->SetMaterial(0, BlueMaterial);
		bIsActive = false;
		bIsWall = false;
		bIsStart = false;
		bIsEnd = false;
		bIsEdgeWall = false;
		bMazeVisited = false;
		Distance = 999;
		MazeIndex = 0;
		}
	}
}

void APathfindingBlock::Highlight(bool bOn)
{
	// Do not highlight if the block has already been activated.
	if (bIsActive)
	{
		return;
	}

	if (bOn)
	{
		BlockMesh->SetMaterial(0, BaseMaterial);
	}
	else
	{
		BlockMesh->SetMaterial(0, BlueMaterial);
	}
}