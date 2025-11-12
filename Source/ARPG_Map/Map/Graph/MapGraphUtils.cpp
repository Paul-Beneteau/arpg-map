#include "MapGraphUtils.h"

#include "MapLayout.h"
#include "MapTypes.h"

EMapDirection RotateLeft(EMapDirection Direction)
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

EMapDirection RotateRight(EMapDirection Direction)
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

EMapDirection GetBranchAbsoluteDirection(EMapDirection Direction, EMapBranchDirection BranchDirection)
{
	switch (BranchDirection)
	{
	case EMapBranchDirection::Left:
		return RotateLeft(Direction);
			
	case EMapBranchDirection::Right:
		return RotateRight(Direction);
			
	default:
		return EMapDirection::None;
	}
}