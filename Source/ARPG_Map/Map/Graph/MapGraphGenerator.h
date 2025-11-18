#pragma once

#include "CoreMinimal.h"
#include "MapGraph.h"
#include "ARPG_Map/Map/Layout/MapLayoutGenerator.h"
#include "GameFramework/Actor.h"
#include "MapGraphGenerator.generated.h"

struct FBranchRule;

/**
 * Generates a procedural map graph from a layout configuration.
 * 1. GenerateLayout() - Creates a layout structure based on LayoutConfig
 * 2. PlaceMainPath() - Places the main path into the graph
 * 3. PlaceBranches() - Places branch segments from the main path
 * The generated graph is cached in CachedMapGraph.
 */
UCLASS(ClassGroup=(Map), meta=(BlueprintSpawnableComponent))
class ARPG_MAP_API UMapGraphGenerator : public UActorComponent
{
	GENERATED_BODY()

public:
	// Generate a layout from layout config, then build the main path and branches from the main path using the layout generated.
	FMapGraph BuildMapGraph();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Graph Generation")
	int32 Rows { 0 };
	UPROPERTY(EditDefaultsOnly, Category = "Graph Generation")
	int32 Columns { 0 };

	// Map graph generated
	FMapGraph CachedMapGraph;
	
	// Configuration for layout generation (layout type, segment lengths, themes, etc.)
	UPROPERTY(EditDefaultsOnly, Category = "Graph Generation")
	TObjectPtr<UDataTable> LayoutConfigTable;
	
	// maximum number of retries if the layout generation fails
	UPROPERTY(EditDefaultsOnly, Category = "Graph Generation")
	int32 MaxLayoutGenerationRetries { 5 };
	
	// Layout generated with GenerateLayout()
	FMapLayout CachedLayout;

	FMapLayoutConfig PickLayoutConfig();

	// Use FMapLayoutGenerator to generate a layout from layout config
	void GenerateLayout(const FMapLayoutConfig& LayoutConfig);

	// Place every segment of the main path from CachedLayout. Tag first cell with EMapRole::MainPathStart and last cell with EMapRole::MainPathEnd
	void PlaceMainPath();

	// Place Branches on the main path from CachedLayout
	void PlaceBranches();
	// Place Branches on the main path using a BranchRule defining the behavior branches, like their directions, interval or count.
	void PlaceBranchesForRule(const FBranchRule& BranchRule);

	// Place a segment in CachedMapGraph
	void PlaceSegment(const FMapSegment& Segment);
	// Place a cell in CachedMapGraph
	void PlaceCell(const FMapGraphCoord Coord, const FMapGraphCell& Cell);

	// Adds connectors between the two cells
	void ConnectCells(const FMapGraphCoord FirstCell, const FMapGraphCoord SecondCell);
};
