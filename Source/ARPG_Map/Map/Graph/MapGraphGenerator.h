#pragma once

#include "CoreMinimal.h"
#include "MapGraph.h"
#include "MapLayout.h"
#include "ARPG_Map/Map/Layout/MapLayoutGenerator.h"
#include "GameFramework/Actor.h"
#include "MapGraphGenerator.generated.h"

UCLASS()
class ARPG_MAP_API AMapGraphGenerator : public AActor
{
	GENERATED_BODY()

public:
	void BuildMapGraph();
	
protected:
	UPROPERTY(EditDefaultsOnly)
	int32 Rows { 5 };
	UPROPERTY(EditDefaultsOnly)
	int32 Columns { 5 };
	
	UPROPERTY(EditDefaultsOnly)
	FMapLayoutConfig LayoutConfig;

	FMapGraph CachedMapGraph;
	
	FMapLayout CachedLayout;
	
	int32 MaxRetry { 5 };
	
	virtual void BeginPlay() override;
	
	bool GenerateLayoutL();

	void GenerateMainPathStart();

	FMapSegment GenerateFirstMainPathSegment();
	
	FMapSegment GenerateMainPathSegment(FMapGraphCoord Anchor, const TArray<EMapDirection>& PossibleDirections, int32 Length = -1);
	
	void GenerateMainPath();
	
	void GenerateBranchesFromMainPath(const FBranchRule& BranchRule);
	
	// Build a segment in the graph from an anchor. The segment starts one cell after the anchor. Return length of segment
	// generated
	bool GenerateSegment(const FMapSegment& Segment);

	// Place a cell in the graph
	void PlaceCell(const FMapGraphCoord Coord, const FMapGraphCell& Cell);

	// Adds connectors between two cells
	void LinkCells(const FMapGraphCoord FirstCell, const FMapGraphCoord SecondCell);

	TArray<EMapDirection> GetValidSegmentDirections(const FMapSegment& Segment, const FMapGraphCoord Anchor,
		const TArray<EMapDirection>& PossibleDirections = TArray<EMapDirection>({ EMapDirection::North, EMapDirection::East, EMapDirection::South, EMapDirection::West }));
};
