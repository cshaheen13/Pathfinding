// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PathfindingBlockGrid.h"
#include "PathfindingBlock.h"
#include "Components/TextRenderComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
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
	bDone = false;
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
		NewBlock->SetActorTickEnabled(false);
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

void APathfindingBlockGrid::ResetBoard()
{
	for (auto& Block : BlockArray)
	{
		Block->HandleClicked("Reset");
	}

	VisitedNodesInOrder.Empty();
	UnvisitedNodes.Empty();
	bDone = false;
	bPathAvailable = false;
}

void APathfindingBlockGrid::ResetPathfinding()
{
	for (auto& Block : BlockArray)
	{
		if (!Block->bIsEnd && !Block->bIsStart && !Block->bIsWall)
		{
			Block->HandleClicked("Reset");
		}
	}

	VisitedNodesInOrder.Empty();
	UnvisitedNodes.Empty();
	bDone = false;
	bPathAvailable = false;
}

TArray<APathfindingBlock*> APathfindingBlockGrid::DijkstraAlgorithm(TArray<APathfindingBlock*> Array)
{
	TotalBlocksVisited = 0;
	for (auto& Block : Array)
	{
		UnvisitedNodes.Add(Block);
	}

	while (!bDone)
	{
		//Sort block array by distance
		UnvisitedNodes = SortBlocksByDistance(UnvisitedNodes, 0, UnvisitedNodes.Num() - 1);
		//Set 1st element to visited
		UnvisitedNodes[0]->bVisited = true;
		TotalBlocksVisited = TotalBlocksVisited + 1;
		if (UnvisitedNodes[0]->Distance == 9999)
		{
			UE_LOG(LogTemp, Warning, TEXT("Blocked Path"));
			bDone = true;
			return VisitedNodesInOrder;
		}

		//Push neighbor nodes (line trace) to NeighborArray
		TArray <APathfindingBlock*> NeighborsHitArray;
		FHitResult NeighborHit;

		FVector Start = UnvisitedNodes[0]->GetActorLocation();
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
				//NeighborFound->Highlight(true);
				if ((UnvisitedNodes[0]->Distance + 1) < NeighborFound->Distance)
				{
					NeighborFound->Distance = 1 + UnvisitedNodes[0]->Distance;
					NeighborFound->SetActorTickEnabled(true);	
				}
			}
		}

		//Push visited node (element 0) to VisitedNodesInOrder Array and Remove it from the BlockArray
		VisitedNodesInOrder.Add(UnvisitedNodes[0]);
		UnvisitedNodes.RemoveAt(0);

		//If 1st element->bIsEnd then return VisitedNodesInOrder
		if (VisitedNodesInOrder.Last()->bIsEnd == true)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found End at %s"), *VisitedNodesInOrder.Last()->GetName());
			EndDistance = VisitedNodesInOrder.Last()->Distance;
			bDone = true;
			bPathAvailable = true;
			UE_LOG(LogTemp, Warning, TEXT("Number of Visited Blocks = %i"), TotalBlocksVisited);
			return VisitedNodesInOrder;
		}
	}

	return VisitedNodesInOrder;
}

TArray<APathfindingBlock*> APathfindingBlockGrid::AStarAlgorithm(TArray<APathfindingBlock*> Array)
{
	float TimeCount = 1;
	TotalBlocksVisited = 0;
	for (auto& Block : Array)
	{
		Block->Heuristic = (Block->GetDistanceTo(EndBlock)) / BlockSpacing;
		UnvisitedNodes.Add(Block);
	}
	
	while (!bDone)
	{
		//Sort block array by distance
		UnvisitedNodes = SortBlocksByWeightedDistance(UnvisitedNodes, 0, UnvisitedNodes.Num() - 1);
		//Set 1st element to visited
		UnvisitedNodes[0]->bVisited = true;
		TotalBlocksVisited = TotalBlocksVisited + 1;
		if (UnvisitedNodes[0]->Distance == 9999)
		{
			UE_LOG(LogTemp, Warning, TEXT("Blocked Path"));
			bDone = true;
			return VisitedNodesInOrder;
		}

		//Push neighbor nodes (line trace) to NeighborArray
		TArray <APathfindingBlock*> NeighborsHitArray;
		FHitResult NeighborHit;

		FVector Start = UnvisitedNodes[0]->GetActorLocation();
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
				if ((UnvisitedNodes[0]->Distance + 1) < NeighborFound->Distance)
				{
					NeighborFound->Distance = 1 + UnvisitedNodes[0]->Distance;
					NeighborFound->SetActorTickEnabled(true);
				}
			}
		}

		//Push visited node (element 0) to VisitedNodesInOrder Array and Remove it from the BlockArray
		UE_LOG(LogTemp, Warning, TEXT("Heuristic (%f) + Distance (%f) = %f"), UnvisitedNodes[0]->Heuristic, UnvisitedNodes[0]->Distance, UnvisitedNodes[0]->Heuristic + UnvisitedNodes[0]->Distance);
		VisitedNodesInOrder.Add(UnvisitedNodes[0]);
		UnvisitedNodes.RemoveAt(0);

		//If 1st element->bIsEnd then return VisitedNodesInOrder
		if (VisitedNodesInOrder.Last()->bIsEnd == true)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found End at %s"), *VisitedNodesInOrder.Last()->GetName());
			EndDistance = VisitedNodesInOrder.Last()->Distance;
			bDone = true;
			bPathAvailable = true;
			UE_LOG(LogTemp, Warning, TEXT("Number of Visited Blocks = %i"), TotalBlocksVisited);
			return VisitedNodesInOrder;
		}
	}

	return VisitedNodesInOrder;
}



TArray<APathfindingBlock*> APathfindingBlockGrid::SortBlocksByDistance(TArray<APathfindingBlock*> UnvisitedArray, int LeftIndex, int RightIndex)
{
	for (int i = LeftIndex + 1; i <= RightIndex; i++)
	{
		float temp = UnvisitedArray[i]->Distance;
		int j = i - 1;
		while (j >= LeftIndex && UnvisitedArray[j]->Distance > temp)
		{
			UnvisitedArray.Swap(j + 1, j);
			j--;
		}
	}

	return UnvisitedArray;
}

TArray<APathfindingBlock*> APathfindingBlockGrid::SortBlocksByWeightedDistance(TArray<APathfindingBlock*> UnvisitedArray, int LeftIndex, int RightIndex)
{
	for (int i = LeftIndex + 1; i <= RightIndex; i++)
	{
		float temp = UnvisitedArray[i]->Distance + UnvisitedArray[i]->Heuristic;
		int j = i - 1;
		while (j >= LeftIndex && (UnvisitedArray[j]->Distance + UnvisitedArray[j]->Heuristic) > temp)
		{
			UnvisitedArray.Swap(j + 1, j);
			j--;
		}
	}
	return UnvisitedArray;
}

void APathfindingBlockGrid::GetShortestPath(TArray<APathfindingBlock*> VisitedNodes)
{
	//Traverse backwards through the visited nodes as long as there is a path
	APathfindingBlock* EndNode = VisitedNodes.Last();
	if (bPathAvailable == true)
	{
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
						NeighborFound->bIsShortestPath = true;
						EndNode = NeighborFound;
					}
				}
			}
	}
	
}

void APathfindingBlockGrid::HighlightBlock(TArray<APathfindingBlock*> VisitedNodes)
{
	APathfindingBlock* BlockFound = Cast<APathfindingBlock>(VisitedNodes[0]);
	if (BlockFound)
	{
		BlockFound->Highlight(true);
		UE_LOG(LogTemp, Warning, TEXT("Block Highlight"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Block To Highlight"));
	}
}

TArray<APathfindingBlock*> APathfindingBlockGrid::CreateMazeGrid()
{
	int row = 1;
	int MazeIndexCount = 0;
	ResetBoard();

	//Generate Grid
	for (int i = 0; i < BlockArray.Num(); i++)
	{
		//Reached end column
		if ((i % 25 == 0) && (i != 0))
		{
			row++;
		}
		//Wall off all odd rows
		if (row % 2 != 0) {
			BlockArray[i]->HandleClicked("Wall");
		}
		//Wall  off every other block of even rows
		else
		{
			if (i % 2 != 0)
			{
				BlockArray[i]->HandleClicked("Wall");
			}
		}

		if ((i < 25) || (i > 599) || (i % 25 == 0) || (i % 25 == 24))
		{
			BlockArray[i]->bIsEdgeWall = true;
		}

		if ((i >= 25) && (i <= 599) && (i % 25 != 0) && (i % 25 != 24) && (!BlockArray[i]->bIsWall))
		{
			MazeGridArray.Add(BlockArray[i]);
			BlockArray[i]->MazeIndex = MazeIndexCount;
			MazeIndexCount++;
			UE_LOG(LogTemp, Warning, TEXT("MazeBlock: %s"), *BlockArray[i]->GetName());
		}
	}

	return MazeGridArray;
}

void APathfindingBlockGrid::MazeGenerator(TArray<APathfindingBlock*> GridArray, int Index, TArray<APathfindingBlock*> VisitedArray)
{
	TArray<APathfindingBlock*> VisitedArr = VisitedArray;
	TArray<APathfindingBlock*> GridArr = GridArray;

	//UE_LOG(LogTemp, Warning, TEXT("T INDEX: %i"), Index);
	VisitedArr.Add(GridArr[Index]);

	GridArr[Index]->bMazeVisited = true;

	//Check Neighbors to set maze walls
	FHitResult NeighborHit;
	FHitResult WallBetween;
	FVector End;
	FVector EndWall;

	int Direction;
	int DirectionCount = 0;

	FVector Start = GridArr[Index]->GetActorLocation();

	TArray<FVector> DirectionArray;
	DirectionArray.Add(FVector(150, 0, 0));
	DirectionArray.Add(FVector(-150, 0, 0));
	DirectionArray.Add(FVector(0, -150, 0));
	DirectionArray.Add(FVector(0, 150, 0));

	//Shuffle directions
	for (int i = 0; i <= 25; i++)
	{
		Direction = FMath::RandRange(0, 3);
		DirectionArray.Swap(0, Direction);
	}

	for (auto& Dir : DirectionArray)
	{
		End = Start + Dir;
		DirectionCount++;

		//Line Trace to block (non wall)
		FVector NS = End - FVector(0, 0, 10);
		GetWorld()->LineTraceSingleByChannel(NeighborHit, Start, End, ECC_GameTraceChannel4);

		APathfindingBlock* NeighborFound = Cast<APathfindingBlock>(NeighborHit.GetActor());
		if ((NeighborFound) && (NeighborFound->bIsEdgeWall == false) && (NeighborFound->bMazeVisited == false) && (NeighborFound->MazeIndex != 0))
		{

			//Get the wall in between two open neighbors and reset it
			EndWall = Start + (Dir / 2);
			GetWorld()->LineTraceSingleByChannel(WallBetween, Start, EndWall, ECC_Visibility);

			APathfindingBlock* WallFound = Cast<APathfindingBlock>(WallBetween.GetActor());
			if (WallFound)
			{
				WallFound->HandleClicked("Reset");
				MazeGenerator(GridArray, NeighborFound->MazeIndex, VisitedArr);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
