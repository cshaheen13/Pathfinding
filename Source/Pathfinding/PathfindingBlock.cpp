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
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> OrangeMaterial;
		//ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> StartMaterial;
		//ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> EndMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.BlueMaterial"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/OrangeMaterial.OrangeMaterial"))
			//, StartMaterial(TEXT("/Game/Puzzle/Meshes/M_Metal_Gold.M_Metal_Gold'"))
			//, EndMaterial(TEXT("/Game/Puzzle/Meshes/M_Tech_Hex_Tile_Pulse.M_Tech_Hex_Tile_Pulse'"))
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
	BlockMesh->SetRelativeScale3D(FVector(0.5f,0.5f,0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f,0.f,25.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BlueMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);
	//BlockMesh->OnClicked.AddDynamic(this, &APathfindingBlock::BlockClicked);
	//BlockMesh->OnInputTouchBegin.AddDynamic(this, &APathfindingBlock::OnFingerPressedBlock);

	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();
	BlueMaterial = ConstructorStatics.BlueMaterial.Get();
	OrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
	//StartMaterial = ConstructorStatics.StartMaterial.Get();
	//EndMaterial = ConstructorStatics.EndMaterial.Get();
}

void APathfindingBlock::BlockClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked)
{
	FString Button = ButtonClicked.ToString();
	UE_LOG(LogTemp, Warning, TEXT("Key Pressed = %s"), *Button);
	HandleClicked();
}


void APathfindingBlock::OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	HandleClicked();
}

void APathfindingBlock::HandleClicked()
{
	// Check we are not already active
	if (!bIsActive)
	{
		bIsActive = true;

		// Change material
		BlockMesh->SetMaterial(0, OrangeMaterial);

		// Tell the Grid
		if (OwningGrid != nullptr)
		{
			OwningGrid->AddScore();
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