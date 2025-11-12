#pragma once

#include "CoreMinimal.h"
#include "MapGraph.h"
#include "MapLayout.h"
#include "GameFramework/Actor.h"
#include "MapGraphGenerator.generated.h"

UCLASS()
class ARPG_MAP_API AMapGraphGenerator : public AActor
{
	GENERATED_BODY()
	
protected:
	FMapGraph CachedMapGraph;
	
	UPROPERTY(EditDefaultsOnly)
	int32 Rows { 10 };
	UPROPERTY(EditDefaultsOnly)
	int32 Columns { 10 };

	UPROPERTY(EditDefaultsOnly)
	FMapLayoutConfig LayoutConfig;
	
	virtual void BeginPlay() override;
	
	// Build the complete map graph
	void BuildMapGraph();

	void GenerateMainPath();

	void GenerateBranchesFromMainPath(const FBranchRule& BranchRule);
	
	// Build a segment in the graph from an anchor. The segment starts one cell after the anchor.
	void GenerateSegment(FMapGraphCoord Anchor, const FMapSegment& Segment);

	// Place a cell in the graph
	void PlaceCell(const FMapGraphCoord Coord, const FMapGraphCell& Cell);

	// Adds connectors between two cells
	void LinkCells(const FMapGraphCoord FirstCell, const FMapGraphCoord SecondCell);
};
