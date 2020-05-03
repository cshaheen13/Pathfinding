// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PathfindingBlockGrid.h"
#include "PathfindingBlock.h"
#include "Components/TextRenderComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

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
}

TArray<APathfindingBlock*> APathfindingBlockGrid::DijkstraAlgorithm()
{
	TArray <APathfindingBlock*> VisitedNodesInOrder;
	while (BlockArray.Num() != 0)
	{
		//Sort block array by distance
		BlockArray = SortBlocksByDistance(BlockArray, 0, BlockArray.Num() - 1);
		//Set 1st element to visited
		BlockArray[0]->bVisited = true;

		if (BlockArray[0]->Distance == 999) 
		{ 
			UE_LOG(LogTemp, Warning, TEXT("Blocked Path"));
			return VisitedNodesInOrder;
		}

		//Push neighbor nodes (line trace) to NeighborArray
		TArray <APathfindingBlock*> NeighborsHitArray;
		FHitResult NeighborHit;

		FVector Start = BlockArray[0]->GetActorLocation();
		FVector EndN = Start + FVector(75, 0, 0);
		FVector EndS = Start + FVector(-75, 0, 0);
		FVector EndW = Start + FVector(0, -75, 0);
		FVector EndE = Start + FVector(0, 75, 0);
		FVector End;

		for (int dir = 1; dir <= 4; dir++)
		{
			if (dir == 1) 
			{
				End = EndN;
			}
			else if (dir == 2)
			{
				End = EndS;
			}
			else if (dir == 3)
			{
				End = EndW;
			}
			else if (dir == 4)
			{
				End = EndE;
			}

			GetWorld()->LineTraceSingleByChannel(NeighborHit, Start, End, ECC_Visibility);

			APathfindingBlock* NeighborFound = Cast<APathfindingBlock>(NeighborHit.GetActor());
			if (NeighborFound && !NeighborFound->bIsWall && !NeighborFound->bVisited)
			{
				NeighborsHitArray.Add(NeighborFound);
				NeighborFound->Highlight(true);
				if ((BlockArray[0]->Distance + 1) < NeighborFound->Distance)
				{
					NeighborFound->Distance = 1 + BlockArray[0]->Distance;
				}
			}
		}

		//Push visited node (element 0) to VisitedNodesInOrder Array and Remove if from the BlockArray
		VisitedNodesInOrder.Add(BlockArray[0]);
		BlockArray.RemoveAt(0);

		//If 1st element->bIsEnd then return VisitedNodesInOrder
		if (VisitedNodesInOrder.Last()->bIsEnd == true)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found End at %s"), *VisitedNodesInOrder.Last()->GetName());
			return VisitedNodesInOrder;
		}
	}
	return VisitedNodesInOrder;
}

TArray<APathfindingBlock*> APathfindingBlockGrid::SortBlocksByDistance(TArray<APathfindingBlock*> UnvisitedArray, int LeftIndex, int RightIndex)
{
	for (int i = LeftIndex + 1; i <= RightIndex; i++)
	{
		int temp = UnvisitedArray[i]->Distance;
		int j = i - 1;
		while (j >= LeftIndex && UnvisitedArray[j]->Distance > temp)
		{
			UnvisitedArray.Swap(j + 1, j);
			j--;
		}
	}

	return UnvisitedArray;
}

void APathfindingBlockGrid::GetShortestPath(TArray<APathfindingBlock*> VisitedNodes)
{
	//Traverse backwards through the visited nodes
	APathfindingBlock* EndNode = VisitedNodes.Last();
	while (!EndNode->bIsStart)
	{
		//Check if the node is a neighbor and the distance = currentDistance - 1
		int dist = EndNode->Distance;

		FHitResult NeighborHit;

		FVector Start = EndNode->GetActorLocation();
		FVector EndN = Start + FVector(75, 0, 0);
		FVector EndS = Start + FVector(-75, 0, 0);
		FVector EndW = Start + FVector(0, -75, 0);
		FVector EndE = Start + FVector(0, 75, 0);
		FVector End;

		for (int dir = 1; dir <= 4; dir++)
		{
			if (dir == 1)
			{
				End = EndN;
			}
			else if (dir == 2)
			{
				End = EndS;
			}
			else if (dir == 3)
			{
				End = EndW;
			}
			else if (dir == 4)
			{
				End = EndE;
			}

			GetWorld()->LineTraceSingleByChannel(NeighborHit, Start, End, ECC_Visibility);

			APathfindingBlock* NeighborFound = Cast<APathfindingBlock>(NeighborHit.GetActor());
			if (NeighborFound && (NeighborFound->Distance == EndNode->Distance - 1))
			{
				NeighborFound->HandleClicked("Path");
				EndNode = NeighborFound;
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
