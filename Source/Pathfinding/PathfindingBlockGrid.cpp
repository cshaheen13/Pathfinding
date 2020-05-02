// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PathfindingBlockGrid.h"
#include "PathfindingBlock.h"
#include "Components/TextRenderComponent.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "PuzzleBlockGrid"

APathfindingBlockGrid::APathfindingBlockGrid()
{
	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	ScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScoreText0"));
	ScoreText->SetRelativeLocation(FVector(200.f,0.f,0.f));
	ScoreText->SetRelativeRotation(FRotator(90.f,0.f,0.f));
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(0)));
	ScoreText->SetupAttachment(DummyRoot);

	// Set defaults
	Size = 25;
	BlockSpacing = 75.f;
}


void APathfindingBlockGrid::BeginPlay()
{
	Super::BeginPlay();

	// Number of blocks
	const int32 NumBlocks = Size * Size;

	// Loop to spawn each block
	for(int32 BlockIndex=0; BlockIndex<NumBlocks; BlockIndex++)
	{
		const float XOffset = (BlockIndex/Size) * BlockSpacing; // Divide by dimension
		const float YOffset = (BlockIndex%Size) * BlockSpacing; // Modulo gives remainder

		// Make position vector, offset from Grid location
		const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		// Spawn a block
		APathfindingBlock* NewBlock = GetWorld()->SpawnActor<APathfindingBlock>(BlockLocation, FRotator(0,0,0));
		BlockArray.Add(NewBlock);

		// Tell the block about its owner
		if (NewBlock != nullptr)
		{
			NewBlock->OwningGrid = this;
		}
	}
}


void APathfindingBlockGrid::AddScore()
{
	// Increment score
	Score++;

	// Update text
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(Score)));
}

void APathfindingBlockGrid::DijkstraAlgorithm()
{
	TArray <APathfindingBlock*> VisitedNodesInOrder;
	while (BlockArray.Num() != 0)
	{
		//Sort block array by distance
		BlockArray = SortBlocksByDistance(BlockArray, 0, BlockArray.Num() - 1);
		//Set 1st element to visited
		BlockArray[0]->bVisited = true;
		//Remove 1st element (should be closest node)
		BlockArray.RemoveAt(0);
		//Push visited node to VisitedNodesInOrder Array

		//If 1st element->bIsEnd then return VisitedNodesInOrder

		//Push neighbor nodes (line trace?) to NeighborArray
	}

	for (int32 Index = 0; Index != BlockArray.Num(); ++Index)
	{
		//FString test = BlockArray[Index]->GetName();
		//UE_LOG(LogTemp, Warning, TEXT("Test = %s"), *test);
		int testDistance = BlockArray[Index]->Distance;
		UE_LOG(LogTemp, Warning, TEXT("Test = %i"), testDistance);
	}

	for (int32 Index = 0; Index != BlockArray.Num(); ++Index)
	{
		FString test = BlockArray[Index]->GetName();
		UE_LOG(LogTemp, Warning, TEXT("Name = %s"), *test);
	}

}

TArray<APathfindingBlock*> APathfindingBlockGrid::SortBlocksByDistance(TArray<APathfindingBlock*> UnvisitedArray, int LeftIndex, int RightIndex)
{
	for (int i = LeftIndex + 1; i <= RightIndex; i++)
	{
		int temp = UnvisitedArray[i]->Distance;
		APathfindingBlock* tempObj = UnvisitedArray[i];
		int j = i - 1;
		while (j >= LeftIndex && UnvisitedArray[j]->Distance > temp)
		{
			//UnvisitedArray[j + 1] = UnvisitedArray[j];
			UE_LOG(LogTemp, Warning, TEXT("Swapping %s"), *UnvisitedArray[j+1]->GetName());
			UE_LOG(LogTemp, Warning, TEXT("with %s"), *UnvisitedArray[j]->GetName());
			UnvisitedArray.Swap(j + 1, j);
			j--;
		}
	}

	return UnvisitedArray;
}

#undef LOCTEXT_NAMESPACE
