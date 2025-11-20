#pragma once

#include "CoreMinimal.h"
#include "ARPG_Map/Map/Types/MapTypes.h"
#include "MapGraph.generated.h"

struct FMapGraphCoord;
enum class EMapDirection : uint8;

// Cell of the map graph. It has a Type (What the cell represent, corridor, etc), a theme (visual style) and connectors to links adjacent cells
USTRUCT(BlueprintType)
struct FMapGraphCell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapTileType Type = EMapTileType::None;
	
	// Visual theme
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Theme = "None";

	// TODO: make it an array if a cell is a crossroad. Direction of the path the cell is in. For example, if it is part of river, it has the direction
	// of the river
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapDirection FlowDirection = EMapDirection::None;
	
	// Links with adjacent cells
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EMapDirection> Connectors;
	
	bool IsUsed() const { return !Theme.IsNone(); };
};

// Represents the map graph as a 2D grid of cells.
USTRUCT(BlueprintType)
struct FMapGraph
{
	GENERATED_BODY()

public:
	TArray<FMapGraphCell> Cells;
	
	int32 Rows = 0;
	int32 Columns = 0;

	FMapGraphCoord MainPathStart = FMapGraphCoord(0, 0);
	FMapGraphCoord MainPathEnd = FMapGraphCoord(0, 0);
	
	FMapGraph() = default;	
	FMapGraph(const int32 InRows, const int32 InColumns) : Rows(InRows), Columns(InColumns) { Cells.SetNum(Rows * Columns); };
	
	FORCEINLINE FMapGraphCell& At(const FMapGraphCoord& Coord)	{ return Cells[Coord.Row * Columns + Coord.Column]; }
	FORCEINLINE const FMapGraphCell& At(const FMapGraphCoord& Coord) const { return Cells[Coord.Row * Columns + Coord.Column]; }
	
	FORCEINLINE void Reset() { Cells.Reset(); }
};