#include "MapGraph.h"

#include "ARPG_Map/Map/Types/MapTypes.h"


FMapGraph::FMapGraph(int32 InRows, int32 InColumns)
	: Rows(InRows), Columns(InColumns)
{
	Cells.SetNum(Rows * Columns);
}

void FMapGraph::Resize(const int32 InRows, const int32 InColumns)
{
	Rows = InRows;
	Columns = InColumns;

	Cells.Reset();
	Cells.SetNum(Rows * Columns);
}

FMapGraphCell& FMapGraph::At(const FMapGraphCoord& Coord)
{
	check (Cells.IsValidIndex(Coord.Row * Columns + Coord.Column));
	return Cells[Coord.Row * Columns + Coord.Column];
}
