#include "MapGraph.h"

#include "ARPG_Map/Map/Types/MapTypes.h"

FMapGraph::FMapGraph(const int32 InRows, const int32 InColumns)
	: Rows(InRows),	Columns(InColumns)
{
	Cells.SetNum(Rows * Columns);
}

FMapGraphCoord FMapGraph::GetStart() const
{
	for (int32 Row = 0; Row < Rows; ++Row)
	{
		for (int32 Column = 0; Column < Columns; ++Column)
		{ 
			if (Cells[Row * Columns + Column].Role == EMapRole::MainPathStart)
				return FMapGraphCoord(Row, Column);
		}
	}

	return FMapGraphCoord::None;
}
