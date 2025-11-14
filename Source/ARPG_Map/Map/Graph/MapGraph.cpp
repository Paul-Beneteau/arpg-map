#include "MapGraph.h"

#include "MapTypes.h"

FMapGraph::FMapGraph(int32 InRows, int32 InColumns)
	: Rows(InRows), Columns(InColumns)
{
	Nodes.SetNum(Rows * Columns);
}

void FMapGraph::Resize(const int32 InRows, const int32 InColumns)
{
	Rows = InRows;
	Columns = InColumns;

	Nodes.Reset();
	Nodes.SetNum(Rows * Columns);
}

FMapGraphCell& FMapGraph::At(const FMapGraphCoord& Coord)
{
	check (Nodes.IsValidIndex(Coord.Row * Columns + Coord.Column));
	return Nodes[Coord.Row * Columns + Coord.Column];
}

bool FMapGraph::IsValidCoord(const FMapGraphCoord& Coord) const
{
	return Coord.Row >= 0 && Coord.Row < Rows && Coord.Column >= 0 && Coord.Column < Columns;
}
