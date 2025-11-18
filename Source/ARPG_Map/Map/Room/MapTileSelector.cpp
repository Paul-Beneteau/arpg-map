#include "MapTileSelector.h"

#include "MapTileTemplate.h"
#include "ARPG_Map/Map/Graph/MapGraph.h"
#include "ARPG_Map/Map/Layout/MapLayoutGenerator.h"

TArray<FMapTileSpawnData> UMapTileSelector::SelectMapGraphTiles(const FMapGraph& Graph)
{
	TArray<FMapTileSpawnData> TileSpawnData;
	TileSpawnData.Reserve(Graph.GetRows() * Graph.GetColumns());
	
	for (int32 Row = 0; Row < Graph.GetRows(); ++Row)
	{
		for (int32 Column = 0; Column < Graph.GetColumns(); ++Column)
		{			
			const FMapGraphCoord CurrentCoord(Column, Row);
			const FMapGraphCell CurrentCell = Graph.At(CurrentCoord);
			
			if (!CurrentCell.IsUsed())
				continue;
			
			const UMapTileTemplate* TileTemplate = PickTileTemplate(CurrentCell);
			if (!TileTemplate)
			{
				UE_LOG(LogTemp, Error, TEXT("UMapTileSelector: couldn't find a valid template for this cell"));
				continue;
			}
			
			TileSpawnData.Add(FMapTileSpawnData(TileTemplate->TileClass, CurrentCoord));
		}
	}

	return TileSpawnData;
}

const UMapTileTemplate* UMapTileSelector::PickTileTemplate(const FMapGraphCell& Cell)
{
	TArray<const UMapTileTemplate*> CandidateTemplates = GetCandidateTileTemplates(Cell);

	if (CandidateTemplates.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("UMapTileSelector: No valid tile templates found for cell"));
		return nullptr;
	}
	
	float TotalWeight = 0.f;

	for (const UMapTileTemplate* TileTemplate : CandidateTemplates)
	{
		TotalWeight += TileTemplate->Weight;
	}

	if (TotalWeight <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("UMapTileSelector: Total weight is 0"));
		return nullptr;
	}

	float RandomWeight = FMath::FRandRange(0.0f, TotalWeight);
	float CurrentWeight = 0.0f;
	
	for (const UMapTileTemplate* TileTemplate : CandidateTemplates)
	{
		if (!TileTemplate)
			continue;
		
		CurrentWeight += TileTemplate->Weight;
		
		if (RandomWeight < CurrentWeight)
			return TileTemplate;
	}

	return nullptr;
}

TArray<const UMapTileTemplate*> UMapTileSelector::GetCandidateTileTemplates(const FMapGraphCell& Cell)
{
	TArray<const UMapTileTemplate*> CandidateTemplates;

	for (UMapTileTemplate* TileTemplate : TileTemplates)
	{
		if (TileTemplate->HasConnectors(Cell.Connectors))
			CandidateTemplates.Add(TileTemplate);
	}

	return CandidateTemplates;
}