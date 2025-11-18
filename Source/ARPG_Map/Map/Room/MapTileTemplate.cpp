#include "MapTileTemplate.h"

#include "MapTile.h"

bool UMapTileTemplate::HasConnectors(const TArray<EMapDirection>& Connectors)
{
	TArray<TObjectPtr<UMapTileConnector>> DefaultTileConnectors = TileClass.GetDefaultObject()->Connectors;	
	TArray<EMapDirection> TileConnectors;
	
	for (UMapTileConnector* TileConnector : DefaultTileConnectors)
		TileConnectors.Add(TileConnector->Direction);
	
	for (EMapDirection Connector : Connectors)
	{
		if (TileConnectors.Contains(Connector))
			return false;
	}
	
	return true;
}
