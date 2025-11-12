#include "MapTypes.h"

void FMapGraphCoord::Step(EMapDirection Direction, int32 Distance)
{
	switch (Direction)
	{
	case EMapDirection::North:
		Row -= Distance;
		break;
		
	case EMapDirection::South:
		Row += Distance;
		break;
		
	case EMapDirection::East:
		Column += Distance;
		break;
		
	case EMapDirection::West:
		Column -= Distance;
		break;

	default:
		break;
	}
}

FMapGraphCoord FMapGraphCoord::Stepped(EMapDirection Direction, int32 Distance)
{
	int32 RowOffset = 0;
	int32 ColumnOffset = 0;
	
	switch (Direction)
	{
	case EMapDirection::North:
		RowOffset -= Distance;
		break;
		
	case EMapDirection::South:
		RowOffset += Distance;
		break;
		
	case EMapDirection::East:
		ColumnOffset += Distance;
		break;
		
	case EMapDirection::West:
		ColumnOffset -= Distance;
		break;

	default:
		break;
	}

	return FMapGraphCoord(Row + RowOffset, Column + ColumnOffset);
}