#pragma once

#include "CoreMinimal.h"
#include "MapGraph.generated.h"

struct FMapGraphCoord;
enum class EMapRole : uint8;
enum class EMapDirection : uint8;

// Cell of the map graph. It has a role (What the cell represent), a theme (visual style) and connectors to links adjacent cells
USTRUCT(BlueprintType)
struct FMapGraphCell
{
	GENERATED_BODY()

	// The function of the cell (start, main path, end, wall, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapRole Role;

	// Visual theme
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Theme;

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
	// Default constructor needed by Unreal Engine
	FMapGraph() = default;	
	FMapGraph(int32 InRows, int32 InColumns);
	
	void Resize(const int32 InRows, const int32 InColumns);
	
	FMapGraphCell& At(const FMapGraphCoord& Coord);

	int32 GetRows() const { return Rows; }
	int32 GetColumns() const { return Columns; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMapGraphCell> Cells;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Rows { 0 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Columns { 0 };
};