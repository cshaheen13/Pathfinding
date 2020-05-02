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
	SortBlocksByDistance(BlockArray, 0, BlockArray.Num() - 1);

	for (int32 Index = 0; Index != BlockArray.Num(); ++Index)
	{
		//FString test = BlockArray[Index]->GetName();
		//UE_LOG(LogTemp, Warning, TEXT("Test = %s"), *test);
		int testDistance = BlockArray[Index]->Distance;
		UE_LOG(LogTemp, Warning, TEXT("Test = %i"), testDistance);
	}

	//TArray <APathfindingBlock*> VisitedNodesInOrder;
	//while (BlockArray.Num != 0)
	//{
	//	SortBlocksByDistance(BlockArray, 0, BlockArray.Num - 1);
	//}

}

void APathfindingBlockGrid::SortBlocksByDistance(TArray<APathfindingBlock*> UnvisitedArray, int LeftIndex, int RightIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Sort"));
	for (int i = LeftIndex + 1; i <= RightIndex; i++)
	{
		int temp = UnvisitedArray[i]->Distance;
		int j = i - 1;
		while (j >= LeftIndex && UnvisitedArray[j]->Distance > temp)
		{
			UnvisitedArray[j + 1] = UnvisitedArray[j];
			j--;
		}
		UnvisitedArray[j + 1]->Distance = temp;
	}

	//// Base case: No need to sort arrays of length <= 1
	//if (LeftIndex >= RightIndex)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("No Sort"));
	//	return;
	//}

	//UE_LOG(LogTemp, Warning, TEXT("Start Sort"));
	//// Choose pivot to be the last element in the subarray
	//int pivot = UnvisitedArray[RightIndex]->Distance;

	//// Index indicating the "split" between elements smaller than pivot and 
	//// elements greater than pivot
	//int count = LeftIndex;

	//// Traverse through array from l to r
	//for (int i = LeftIndex; i <= RightIndex; i++)
	//{
	//	// If an element less than or equal to the pivot is found...
	//	if (UnvisitedArray[i]->Distance < pivot)
	//	{
	//		// Then swap arr[cnt] and arr[i] so that the smaller element arr[i] 
	//		// is to the left of all elements greater than pivot
	//		Swap(UnvisitedArray[count], UnvisitedArray[i]);
	//		UE_LOG(LogTemp, Warning, TEXT("Swap"));

	//		// Make sure to increment cnt so we can keep track of what to swap
	//		// arr[i] with
	//		count++;
	//	}
	//}

	//// NOTE: cnt is currently at one plus the pivot's index 
	//// (Hence, the cnt-2 when recursively sorting the left side of pivot)
	//SortBlocksByDistance(UnvisitedArray, LeftIndex, count - 2); // Recursively sort the left side of pivot
	//SortBlocksByDistance(UnvisitedArray, count, RightIndex);   // Recursively sort the right side of pivot
}

void APathfindingBlockGrid::Swap(APathfindingBlock* a, APathfindingBlock* b)
{
	UE_LOG(LogTemp, Warning, TEXT("a = %s"), *a->GetName());
	UE_LOG(LogTemp, Warning, TEXT("b = %s"), *b->GetName());
	APathfindingBlock* temp = a;
	a = b;
	b = temp;
}

#undef LOCTEXT_NAMESPACE
