# pragma once

#include "CoreMinimal.h"
#include "MapTypes.h"

struct FMapGraph;
enum class EMapTurn : uint8;
enum class EMapDirection : uint8;

namespace MapUtils
{
	// Rotate direction 90 degree left
	EMapDirection RotateLeft(EMapDirection Direction);
	
	// Rotate direction 90 degree Right
	EMapDirection RotateRight(EMapDirection Direction);

	// Get opposite direction
	EMapDirection Opposite(EMapDirection Direction);

	TArray<EMapDirection> Perpendicular(EMapDirection Direction);

	// Get the branch direction from the turn direction applied to the segment direction (e.g., North segment with a
	// right turn gives the east direction)
	EMapDirection GetTurnDirection(EMapDirection Direction, EMapTurn Turn);

	EMapDirection GetDirectionToward(const FMapGraphCoord& FromCell, const FMapGraphCoord& TowardCell);

	FString GetDirectionText(EMapDirection Direction);

	EMapDirection GetInwardDirection(const FMapGraphCoord& Coord, const int32 Rows, const int32 Columns);

	void PrintGraph(FMapGraph& MapGraph, const UWorld* InWorld);

	FORCEINLINE bool IsInsideBounds(const int32 Rows, const int32 Columns, const FMapGraphCoord& Coord)
	{
		return Coord.Row >= 0 && Coord.Row < Rows	&& Coord.Column >= 0 && Coord.Column < Columns;
	}

}

