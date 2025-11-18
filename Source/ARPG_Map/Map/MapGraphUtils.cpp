#include "MapGraphUtils.h"

#include "Graph/MapGraph.h"
#include "ARPG_Map/Map/Types/MapTypes.h"

namespace MapUtils
{
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

	EMapDirection GetInwardDirection(const FMapGraphCoord& Coord, const int32 Rows, const int32 Columns)
	{
		if (Coord.Row == 0)
			return EMapDirection::South;
		if (Coord.Row == Rows - 1)
			return EMapDirection::North;
		if (Coord.Column == 0)
			return EMapDirection::East;
		if (Coord.Column == Columns - 1)
			return EMapDirection::West;

		return EMapDirection::None;
	}

	FColor GetCellColor(const FMapGraphCell& Cell)
	{
		if (!Cell.IsUsed())
			return FColor::Black;
            
		switch (Cell.Role)
		{
		case EMapRole::MainPathStart:
			return FColor::Green;
		case EMapRole::MainPathEnd:
			return FColor::Red;
		case EMapRole::MainPath:
			return FColor::Blue;
		default:
			return FColor::Turquoise;
		}
	}
    
	FVector GetConnectorOffset(EMapDirection Direction, float CellSize)
	{
		const float Offset = CellSize * 0.9f / 2.0f;
        
		switch (Direction)
		{
		case EMapDirection::North:
			return FVector(Offset, 0, 0);
		case EMapDirection::South:
			return FVector(-Offset, 0, 0);
		case EMapDirection::West:
			return FVector(0, -Offset, 0);
		case EMapDirection::East:
			return FVector(0, Offset, 0);
		default:
			return FVector::ZeroVector;
		}
	}
    
	void DrawCell(const UWorld* World, const FVector& Location, const FMapGraphCell& Cell, float Size)
	{
		const FColor Color = GetCellColor(Cell);
		const FVector HalfSize(Size * 0.99f / 2.0f, Size * 0.99f / 2.0f, 1.0f);
		DrawDebugBox(World, Location, HalfSize, Color, true, 0.f, 0, 10.f);
	}
    
	void DrawConnectors(const UWorld* World, const FVector& Location, const FMapGraphCell& Cell, float Size)
	{
		for (EMapDirection Direction : Cell.Connectors)
		{
			const FVector Offset = GetConnectorOffset(Direction, Size);
			DrawDebugLine(World, Location + Offset, Location + (Offset * 1.2f), FColor::Purple, true, 0.f,
				0, 10.f);
		}
	}
	
	void PrintGraph(FMapGraph& MapGraph, const UWorld* InWorld, const int32 CellSize)
	{
		for (int32 Row = 0; Row < MapGraph.GetRows(); ++Row)
		{
			for (int32 Column = 0; Column < MapGraph.GetColumns(); ++Column)
			{
				const FMapGraphCell Cell = MapGraph.At(FMapGraphCoord(Row, Column));
				
				FVector Location((-Row) * CellSize, Column * CellSize, 10.0f);	
				
				DrawCell(InWorld, Location, Cell, CellSize);
            
				if (Cell.IsUsed())
					DrawConnectors(InWorld, Location, Cell, CellSize);
			}
		}	
	}
}
