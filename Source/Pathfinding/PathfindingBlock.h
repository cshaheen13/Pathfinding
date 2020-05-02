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

	/** Are we currently active? */
	bool bIsActive;

	UPROPERTY(Category=Algorithm, VisibleAnywhere)
	bool bVisited;

	UPROPERTY(Category=Algorithm, VisibleAnywhere)
	int Distance;

	UPROPERTY(Category = Algorithm, VisibleAnywhere)
	bool bIsWall;

	UPROPERTY(Category = Algorithm, VisibleAnywhere)
	bool bIsStart;

	UPROPERTY(Category = Algorithm, VisibleAnywhere)
	bool bIsEnd;

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



