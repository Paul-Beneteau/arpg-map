#include "MapGraphUtils.h"

#include "MapGraph.h"
#include "MapLayout.h"
#include "MapTypes.h"

namespace MapUtils
{
	EMapDirection RotateLeft(EMapDirection Direction)
	{
		switch(Direction)
		{
		case EMapDirection::North:
			return EMapDirection::West;
		
		case EMapDirection::West:
			return EMapDirection::South;
		
		case EMapDirection::South:
			return EMapDirection::East;

		case EMapDirection::East:
			return EMapDirection::North;
		
		default:
			return EMapDirection::None;
		}
	}

	EMapDirection RotateRight(EMapDirection Direction)
	{
		switch(Direction)
		{
		case EMapDirection::North:
			return EMapDirection::East;
		
		case EMapDirection::West:
			return EMapDirection::North;
		
		case EMapDirection::South:
			return EMapDirection::West;

		case EMapDirection::East:
			return EMapDirection::South;
		
		default:
			return EMapDirection::None;
		}
	}

	EMapDirection Opposite(EMapDirection Direction)
	{
		switch(Direction)
		{
		case EMapDirection::North:
			return EMapDirection::South;
		
		case EMapDirection::West:
			return EMapDirection::East;
		
		case EMapDirection::South:
			return EMapDirection::North;

		case EMapDirection::East:
			return EMapDirection::West;
		
		default:
			return EMapDirection::None;
		}
	}

	TArray<EMapDirection> Perpendicular(EMapDirection Direction)
	{
		return { RotateLeft(Direction), RotateRight(Direction) };
	}
	
	EMapDirection GetTurnDirection(EMapDirection Direction, EMapTurn Turn)
	{
		switch (Turn)
		{
		case EMapTurn::Left:
			return RotateLeft(Direction);
			
		case EMapTurn::Right:
			return RotateRight(Direction);
			
		default:
			return EMapDirection::None;
		}
	}

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

	FString GetDirectionText(EMapDirection Direction)
	{
		switch(Direction)
		{
		case EMapDirection::North:
			return "North";
		
		case EMapDirection::West:
			return "West";
		
		case EMapDirection::South:
			return "South";

		case EMapDirection::East:
			return "East";
		
		default:
			return "None";
		}
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
	
	void PrintGraph(FMapGraph& MapGraph, const UWorld* InWorld)
	{
		for (int32 Row = 0; Row < MapGraph.GetRows(); ++Row)
		{
			for (int32 Column = 0; Column < MapGraph.GetColumns(); ++Column)
			{
				constexpr float Size = 100.f;
				const FVector Location((-Row) * Size, Column * Size, 120.f);
				FColor DebugColor = FColor::Black;

				FMapGraphCell Cell = MapGraph.At(FMapGraphCoord(Row, Column));
				
				if (!Cell.IsValid())
				{
					DrawDebugBox(InWorld, Location, FVector(Size * 0.8 / 2, Size * 0.8 / 2, 1.f), DebugColor, true, 0.f, 0, 3.f);
					continue;
				}
			
				if (Cell.Role == EMapRole::MainPathStart)
					DebugColor = FColor::Green;
				else if (Cell.Role == EMapRole::MainPathEnd)
					DebugColor = FColor::Red;
				else if (Cell.Role == EMapRole::MainPath)
					DebugColor = FColor::Blue;
				else
					DebugColor = FColor::Turquoise;

				DrawDebugBox(InWorld, Location, FVector(Size * 0.8 / 2, Size * 0.8 / 2, 1.f), DebugColor, true, 0.f, 0, 3.f);

				for (EMapDirection ConnectorDirection : Cell.Connectors)
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
}
