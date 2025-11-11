#include "MapGraph.h"

#include "MapRoomConnector.h"

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

FMapGraphNode& FMapGraph::At(const int32 Row, const int32 Column)
{
	check (Nodes.IsValidIndex(Row * Columns + Column));
	
	return Nodes[Row * Columns + Column];
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
			
			if (GraphNode.Role == "MainPathStart")
				DebugColor = FColor::Yellow;
			else if (GraphNode.Role == "MainPathEnd")
				DebugColor = FColor::Magenta;
			else
				DebugColor = FColor::Green;			

			DrawDebugBox(InWorld, Location, FVector(Size * 0.8 / 2, Size * 0.8 / 2, 1.f), DebugColor, true, 0.f, 0, 3.f);

			for (EMapConnectorDir ConnectorDirection : GraphNode.Connectors)
			{
				FVector OffSet { FVector::ZeroVector };

				switch (ConnectorDirection)
				{
				case EMapConnectorDir::North:
					OffSet += FVector(Size * 0.9 / 2, 0, 0);
					break;
					
				case EMapConnectorDir::South:
					OffSet += FVector(-Size * 0.9 / 2,0,  0);
					break;
					
				case EMapConnectorDir::West:
					OffSet += FVector(0, -Size * 0.9 / 2,0);
					break;
					
				case EMapConnectorDir::East:
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
