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

FMapGraphNode& FMapGraph::At(const FMapGraphCoord& Coord)
{
	check (Nodes.IsValidIndex(Coord.Row * Columns + Coord.Column));
	
	return Nodes[Coord.Row * Columns + Coord.Column];
}

void FMapGraph::Print(const UWorld* InWorld)
{
	for (int32 Row = 0; Row < Rows; ++Row)
	{
		for (int32 Column = 0; Column < Columns; ++Column)
		{
			constexpr float Size = 100.f;
			const FVector Location(Row * Size, Column * Size, 120.f);
			FColor DebugColor = FColor::Black;
			
			if (!Nodes[Row * Columns + Column].IsValid())
			{
				DrawDebugBox(InWorld, Location, FVector(Size * 0.8 / 2, Size * 0.8 / 2, 1.f), DebugColor, true, 0.f, 0, 3.f);
				continue;
			}

			FMapGraphNode GraphNode = Nodes[Row * Columns + Column];
			
			if (GraphNode.Role == EMapRole::MainPathStart)
				DebugColor = FColor::Yellow;
			else if (GraphNode.Role == EMapRole::MainPathEnd)
				DebugColor = FColor::Magenta;
			else if (GraphNode.Role == EMapRole::MainPath)
				DebugColor = FColor::Green;
			else
				DebugColor = FColor::Blue;

			DrawDebugBox(InWorld, Location, FVector(Size * 0.8 / 2, Size * 0.8 / 2, 1.f), DebugColor, true, 0.f, 0, 3.f);

			for (EMapDirection ConnectorDirection : GraphNode.Connectors)
			{
				FVector OffSet { FVector::ZeroVector };

				switch (ConnectorDirection)
				{
				case EMapDirection::North:
					OffSet += FVector(Size * 0.9 / 2, 0, 0);
					break;
					
				case EMapDirection::South:
					OffSet += FVector(-Size * 0.9 / 2,0,  0);
					break;
					
				case EMapDirection::West:
					OffSet += FVector(0, -Size * 0.9 / 2,0);
					break;
					
				case EMapDirection::East:
					OffSet += FVector(0, Size * 0.9 / 2,0);
					break;
					
				default:
					break;
				}

				DrawDebugLine(InWorld, Location + OffSet, Location + (OffSet * 1.2), FColor::Blue, true, 0.f, 0, 4.f);
			}
		}
	}	
}

bool FMapGraph::IsValidCoord(const FMapGraphCoord& Coord) const
{
	return Coord.Row >= 0 && Coord.Row < Rows && Coord.Column >= 0 && Coord.Column < Columns;
}
