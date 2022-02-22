// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathfindingBlock.generated.h"

/** A block that can be clicked */
UCLASS(minimalapi)
class APathfindingBlock : public AActor
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** StaticMesh component for the clickable block */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BlockMesh;

public:
	APathfindingBlock();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Category = Highlight, VisibleAnywhere, BlueprintReadWrite)
	float HighlightTime = 0;

	UPROPERTY(Category = Highlight, VisibleAnywhere, BlueprintReadWrite)
	float PathTime = 0;

	UPROPERTY(Category = Highlight, VisibleAnywhere, BlueprintReadWrite)
	float PathStartTime = 0;

	UPROPERTY(Category = Highlight, VisibleAnywhere, BlueprintReadWrite)
	bool bIsHighlightTimeSet;

	UPROPERTY(Category = Highlight, VisibleAnywhere, BlueprintReadWrite)
	float RunningTime;

	/** Are we currently active? */
	bool bIsActive;

	UPROPERTY(Category = Algorithm, VisibleAnywhere, BlueprintReadWrite)
	bool bVisited;

	UPROPERTY(Category = Algorithm, VisibleAnywhere, BlueprintReadWrite)
	float Distance;

	UPROPERTY(Category = Algorithm, VisibleAnywhere, BlueprintReadWrite)
	float Heuristic;

	UPROPERTY(Category = Algorithm, VisibleAnywhere, BlueprintReadOnly)
	bool bIsWall;

	UPROPERTY(Category = Algorithm, VisibleAnywhere, BlueprintReadOnly)
	bool bIsStart;

	UPROPERTY(Category = Algorithm, VisibleAnywhere, BlueprintReadOnly)
	bool bIsEnd;

	UPROPERTY(Category = Algorithm, VisibleAnywhere, BlueprintReadOnly)
	bool bIsEdgeWall;

	UPROPERTY(Category = Algorithm, VisibleAnywhere, BlueprintReadOnly)
	bool bIsShortestPath;

	UPROPERTY(Category = Algorithm, VisibleAnywhere, BlueprintReadOnly)
	bool bMazeVisited;

	UPROPERTY(Category = Algorithm, VisibleAnywhere, BlueprintReadOnly)
	int MazeIndex;

	/** Pointer to white material used on the focused block */
	UPROPERTY()
	class UMaterial* BaseMaterial;

	/** Pointer to blue material used on inactive blocks */
	UPROPERTY()
	class UMaterialInstance* BlueMaterial;

	/** Pointer to orange material used on active blocks */
	UPROPERTY()
	class UMaterialInstance* WallMaterial;

	/** Pointer to orange material used on the start block */
	UPROPERTY()
	class UMaterialInstance* StartMaterial;

	/** Pointer to orange material used on the end block */
	UPROPERTY()
	class UMaterialInstance* EndMaterial;

	/** Pointer to orange material used on the end block */
	UPROPERTY()
	class UMaterialInstance* PathMaterial;

	/** Grid that owns us */
	UPROPERTY()
	class APathfindingBlockGrid* OwningGrid;

	/** Handle the block being clicked */
	UFUNCTION()
	void BlockClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked);

	/** Handle the block being touched  */
	UFUNCTION()
	void OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

	void HandleClicked(FString HighlightType);

	void Highlight(bool bOn);

public:
	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns BlockMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetBlockMesh() const { return BlockMesh; }
};



