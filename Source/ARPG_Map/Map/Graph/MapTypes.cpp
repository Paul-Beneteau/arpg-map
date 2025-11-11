#include "MapTypes.h"

void FMapGraphCoord::Step(EMapDirection Direction, int32 Distance)
{
	switch (Direction)
	{
	case EMapDirection::North:
		Row += Distance;
		break;
		
	case EMapDirection::South:
		Row -= Distance;
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