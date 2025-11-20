#include "MapTileSelector.h"

#include "MapTileTemplate.h"
#include "ARPG_Map/Map/MapGraphUtils.h"
#include "ARPG_Map/Map/Graph/MapGraph.h"

TArray<FMapTileSpawnData> UMapTileSelector::SelectTiles(const FMapGraph& Graph)
{
	TArray<FMapTileSpawnData> TileSpawnData;
	TileSpawnData.Reserve(Graph.Rows * Graph.Columns);
	
	for (int32 Row = 0; Row < Graph.Rows; ++Row)
	{
		for (int32 Column = 0; Column < Graph.Columns; ++Column)
		{			
			const FMapGraphCoord CurrentCoord(Row, Column);
			const FMapGraphCell CurrentCell = Graph.At(CurrentCoord);
			
			if (!CurrentCell.IsUsed())
				continue;
			
			const UMapTileTemplate* Template = PickTemplateForCell(CurrentCell);
			if (!Template)
			{
				UE_LOG(LogTemp, Error, TEXT("UMapTileSelector: couldn't find a valid template for the cell: %d-%d"), Row, Column);
				continue;
			}

			const FRotator TileRotation = PickTemplateRotationForCell(*Template, CurrentCell);
			
			TileSpawnData.Add(FMapTileSpawnData(Template->TileClass, CurrentCoord, TileRotation));
		}
	}

	return TileSpawnData;
}

UMapTileTemplate* UMapTileSelector::PickTemplateForCell(const FMapGraphCell& Cell)
{
	TArray<UMapTileTemplate*> Templates = GetMatchingTemplates(Cell);
	if (Templates.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("UMapTileSelector: No valid tile templates found for cell"));
		return nullptr;
	}	

	auto GetWeight = [](const UMapTileTemplate* Template) -> float { return Template ? Template->Weight : 0.f; };    
	return MapUtils::PickWeightedRandom<UMapTileTemplate>(Templates, GetWeight);
}

FRotator UMapTileSelector::PickTemplateRotationForCell(const UMapTileTemplate& Template, const FMapGraphCell& Cell) const
{
	const TArray<FRotator> MatchingRotations = GetMatchingRotations(&Template, Cell);
	return MatchingRotations[FMath::RandRange(0, MatchingRotations.Num() - 1)];
}

TArray<UMapTileTemplate*> UMapTileSelector::GetMatchingTemplates(const FMapGraphCell& Cell)
{
	TArray<UMapTileTemplate*> Matches;

	for (UMapTileTemplate* Template : TileTemplates)
	{
		if (!Template || Template->Type != Cell.Type || Template->Theme != Cell.Theme)
			continue;
		
		if (DoesTemplateMatchCell(Template, Cell))
			Matches.Add(Template);
	}

	return Matches;
}

bool UMapTileSelector::DoesTemplateMatchCell(const UMapTileTemplate* Template, const FMapGraphCell& Cell) const
{
	// If template has a forward direction, tile can only be rotated so that his forward direction matches the cell flow direction
	if (Template->ForwardDirection != EMapDirection::None)
	{
		EMapRotation RequiredRotation = MapUtils::GetRotationBetween(Template->ForwardDirection, Cell.FlowDirection);
		return DoesTemplateMatchWithRotation(Template, Cell, RequiredRotation);
	}

	// Check is the template match cell for all possible rotations
	for (EMapRotation Rotation : { EMapRotation::Degree0, EMapRotation::Degree90, EMapRotation::Degree180, EMapRotation::Degree270 })
	{
		if (DoesTemplateMatchWithRotation(Template, Cell, Rotation))
			return true;
	}	

	return false;
}

bool UMapTileSelector::DoesTemplateMatchWithRotation(const UMapTileTemplate* Template, const FMapGraphCell& Cell, EMapRotation Rotation) const
{
	TArray<EMapDirection> RotatedConnectors = Cell.Connectors;
	
	for (EMapDirection& Connector : RotatedConnectors)
		Connector = MapUtils::RotateClockwise(Connector, Rotation);

	return Template->HasConnectors(RotatedConnectors);
}

TArray<FRotator> UMapTileSelector::GetMatchingRotations(const UMapTileTemplate* Template, const FMapGraphCell& Cell) const
{
	TArray<FRotator> MatchingRotations;
	TArray<EMapRotation> PossibleRotations { EMapRotation::Degree0, EMapRotation::Degree90, EMapRotation::Degree180, EMapRotation::Degree270 };

	// If template has a forward direction, tile can only be rotated so that his forward direction matches the cell flow direction
	if (Template->ForwardDirection != EMapDirection::None)
		PossibleRotations = { MapUtils::GetRotationBetween(Template->ForwardDirection, Cell.FlowDirection) };
	
	// Check if the candidate can fit with the 4 possible clockwise rotation (0°, 90°, 180°; 270°) of the tile.
	for (const EMapRotation Rotation : PossibleRotations)
	{
		TArray<EMapDirection> RotatedConnectors = Cell.Connectors;
			
		for (EMapDirection& Connector : RotatedConnectors)
			Connector = MapUtils::RotateClockwise(Connector, Rotation);

		// If the template has connectors with the same direction as the ones from the cell
		if (Template->HasConnectors(RotatedConnectors))
		{
			// We checked by rotating the cell clockwise, so we have to rotate the template inverse clockwise to fit the cell, so we subtract
			// the yaw to the rotation.
			FRotator TileRotation(0.f, -MapUtils::RotationToYaw(Rotation), 0.f);
			MatchingRotations.Add(TileRotation);
		}
	}

	return MatchingRotations;
}