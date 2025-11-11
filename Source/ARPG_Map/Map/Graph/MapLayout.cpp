#include "MapLayout.h"

int32 FMapSegment::GetLength() const
{
	int32 Length = 0;
	
	for (FMapSegmentSection Section : Sections)
	{
		Length += Section.Length;
	}

	return Length;
}
