// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathfindingBlock.h"
#include "PathfindingBlockGrid.generated.h"

/** Class used to spawn blocks and manage score */
UCLASS(minimalapi)
class APathfindingBlockGrid : public AActor
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** Text component for the score */
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* ScoreText;

public:
	APathfindingBlockGrid();

	/** How many blocks have been clicked */
	int32 Score;

	/** Number of blocks along each side of grid */
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	int32 Size;

	/** Spacing of blocks */
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	float BlockSpacing;

	UPROPERTY(BlueprintReadWrite)
	bool bDone;

	int TestRunCount = 0;

protected:
	// Begin AActor interface
	virtual void BeginPlay() override;
	// End AActor interface

public:
	/** Handle the block being clicked */
	void AddScore();

	UFUNCTION(BlueprintCallable)
	TArray<APathfindingBlock*> DijkstraAlgorithm(TArray<APathfindingBlock*> Array);

	UFUNCTION(BlueprintCallable)
	TArray<APathfindingBlock*> SortBlocksByDistance(TArray<APathfindingBlock*> UnvisitedArray, int LeftIndex, int RightIndex);

	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns ScoreText subobject **/
	FORCEINLINE class UTextRenderComponent* GetScoreText() const { return ScoreText; }

	UPROPERTY(BlueprintReadWrite)
	TArray<APathfindingBlock*> BlockArray;

	UPROPERTY(BlueprintReadWrite)
	TArray<APathfindingBlock*> MazeGridArray;

	UFUNCTION(BlueprintCallable)
	void GetShortestPath(TArray<APathfindingBlock*> VisitedNodes);

	UFUNCTION(BlueprintCallable)
	void HighlightBlock(TArray<APathfindingBlock*> VisitedNodes);

	UFUNCTION(BlueprintCallable)
	TArray<APathfindingBlock*> CreateMazeGrid();

	UFUNCTION(BlueprintCallable)
	void MazeGenerator(TArray<APathfindingBlock*> GridArray, int Index, TArray<APathfindingBlock*> VisitedArra);

	int EndDistance;

};



