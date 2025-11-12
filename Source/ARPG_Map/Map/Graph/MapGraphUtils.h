# pragma once

#include "CoreMinimal.h"

struct FMapGraphCoord;
enum class EMapTurnDirection : uint8;
enum class EMapDirection : uint8;

namespace MapUtils
{
	// Rotate direction 90 degree left
	EMapDirection RotateLeft(EMapDirection Direction);
	
	// Rotate direction 90 degree Right
	EMapDirection RotateRight(EMapDirection Direction);

	// Get opposite direction
	EMapDirection Opposite(EMapDirection Direction);

	// Get the branch direction from the turn direction applied to the segment direction (e.g., North segment with a
	// right turn gives the east direction)
	EMapDirection GetBranchDirection(EMapDirection SegmentDirection, EMapTurnDirection TurnDirection);

	EMapDirection GetDirectionToward(const FMapGraphCoord& FromCell, const FMapGraphCoord& TowardCell);

}

