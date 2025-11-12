#include "MapGraphUtils.h"

#include "MapLayout.h"
#include "MapTypes.h"

namespace MapUtils
{
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

	EMapDirection Opposite(EMapDirection Direction)
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

	EMapDirection GetBranchDirection(EMapDirection SegmentDirection, EMapTurnDirection TurnDirection)
	{
		switch (TurnDirection)
		{
		case EMapTurnDirection::Left:
			return RotateLeft(SegmentDirection);
			
		case EMapTurnDirection::Right:
			return RotateRight(SegmentDirection);
			
		default:
			return EMapDirection::None;
		}
	}

	EMapDirection GetDirectionToward(const FMapGraphCoord& FromCell, const FMapGraphCoord& TowardCell)
	{
		const int32 RowDelta = TowardCell.Row - FromCell.Row;
		const int32 ColumnDelta = TowardCell.Column - FromCell.Column;

		if (RowDelta == 0 && ColumnDelta != 0)
			return ColumnDelta > 0 ? EMapDirection::East : EMapDirection::West;
		
		if (ColumnDelta == 0 && RowDelta != 0)
			return RowDelta > 0 ? EMapDirection::South : EMapDirection::North;
		
		return EMapDirection::None;
	}
}
