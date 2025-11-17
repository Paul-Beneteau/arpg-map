#include "MapLayout.h"

int32 FMapSegment::GetLength() const
{
	int32 Length = 0;
	
	for (FMapSegmentSection Section : Sections)
		Length += Section.Length;

	return Length;
}

FMapSegmentSection FMapSegment::GetSection(const FMapGraphCoord Coord) const
{
	int32 SegmentOffset = 0;

	for (const FMapSegmentSection& Section : Sections)
	{
		for (int32 SectionCellIndex = 0; SectionCellIndex < Section.Length; ++SectionCellIndex)
		{
			if (GetCoordAt(SegmentOffset + SectionCellIndex) == Coord)
				return Section;
		}
		
		SegmentOffset += Section.Length;
	}

	return FMapSegmentSection();
}

TArray<FMapGraphCoord> FMapSegment::GetCells() const
{
	TArray<FMapGraphCoord> Cells;
	
	for (int32 SegmentIndex = 0;  SegmentIndex < GetLength(); ++SegmentIndex)
		Cells.Add(GetCoordAt(SegmentIndex));

	return Cells;
}

TArray<FMapGraphCoord> FMapMainPath::GetCells() const
{
	TArray<FMapGraphCoord> Cells;
	
	for (FMapSegment Segment : Segments)
		Cells.Append(Segment.GetCells());

	return Cells;
}

FMapSegment FMapMainPath::GetSegmentAt(int32 PathIndex) const
{
	for (FMapSegment Segment : Segments)
	{
		if (PathIndex < Segment.GetLength())
			return Segment;

		PathIndex -= Segment.GetLength();
	}

	return FMapSegment();
}

FMapSegment FMapMainPath::GetSegmentAt(FMapGraphCoord Cell) const
{	
	for (FMapSegment Segment : Segments)
	{
		if (Segment.GetCells().Contains(Cell))
			return Segment;
	}

	return FMapSegment();
}