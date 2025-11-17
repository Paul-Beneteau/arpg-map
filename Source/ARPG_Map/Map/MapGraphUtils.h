# pragma once

#include "CoreMinimal.h"
#include "ARPG_Map/Map/Types/MapTypes.h"

struct FMapGraph;
enum class EMapTurn : uint8;
enum class EMapDirection : uint8;

namespace MapUtils
{
	// Rotate direction 90 degree left
	FORCEINLINE EMapDirection RotateLeft(EMapDirection Direction)
	{
		switch(Direction)
		{
		case EMapDirection::North:
			return EMapDirection::West;
		
		case EMapDirection::West:
			return EMapDirection::South;
		
		case EMapDirection::South:
			return EMapDirection::East;

		case EMapDirection::East:
			return EMapDirection::North;
		
		default:
			return EMapDirection::None;
		}
	}
	// Rotate direction 90 degree Right
	FORCEINLINE EMapDirection RotateRight(EMapDirection Direction)
	{
		switch(Direction)
		{
		case EMapDirection::North:
			return EMapDirection::East;
		
		case EMapDirection::West:
			return EMapDirection::North;
		
		case EMapDirection::South:
			return EMapDirection::West;

		case EMapDirection::East:
			return EMapDirection::South;
		
		default:
			return EMapDirection::None;
		}
	}

	// Get opposite direction
	FORCEINLINE EMapDirection Opposite(EMapDirection Direction)
	{
		switch(Direction)
		{
		case EMapDirection::North:
			return EMapDirection::South;
		
		case EMapDirection::West:
			return EMapDirection::East;
		
		case EMapDirection::South:
			return EMapDirection::North;

		case EMapDirection::East:
			return EMapDirection::West;
		
		default:
			return EMapDirection::None;
		}
	}
	// Get both perpendicular direction
	FORCEINLINE TArray<EMapDirection> Perpendicular(EMapDirection Direction)
	{
		return { RotateLeft(Direction), RotateRight(Direction) };
	}

	// Get the turn direction (e.g., North segment with a right turn gives the east direction)
	FORCEINLINE EMapDirection GetTurnDirection(EMapDirection Direction, EMapTurn Turn)
	{
		switch (Turn)
		{
		case EMapTurn::Left:
			return RotateLeft(Direction);
			
		case EMapTurn::Right:
			return RotateRight(Direction);
			
		default:
			return EMapDirection::None;
		}
	}

	FORCEINLINE bool IsInsideBounds(const int32 Rows, const int32 Columns, const FMapGraphCoord& Coord)
	{
		return Coord.Row >= 0 && Coord.Row < Rows	&& Coord.Column >= 0 && Coord.Column < Columns;
	}
	
	// Get the direction of a cell toward another cell
	EMapDirection GetDirectionToward(const FMapGraphCoord& FromCell, const FMapGraphCoord& TowardCell);
	// Get the direction toward the center of the graph
	EMapDirection GetInwardDirection(const FMapGraphCoord& Coord, const int32 Rows, const int32 Columns);

	FORCEINLINE FString GetDirectionText(EMapDirection Direction)
	{
		switch(Direction)
		{
		case EMapDirection::North:
			return "North";
		
		case EMapDirection::West:
			return "West";
		
		case EMapDirection::South:
			return "South";

		case EMapDirection::East:
			return "East";
		
		default:
			return "None";
		}
	}
	
	void PrintGraph(FMapGraph& MapGraph, const UWorld* InWorld);
}

