#include "MapTileTemplate.h"

#include "MapTile.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"

bool UMapTileTemplate::HasConnectors(const TArray<EMapDirection>& Connectors) const
{
	TArray<EMapDirection> TemplateConnectors;
	
	UBlueprintGeneratedClass* BlueprintTileClass = Cast<UBlueprintGeneratedClass>(TileClass);
	if (BlueprintTileClass && BlueprintTileClass->SimpleConstructionScript)
	{
		// Get all nodes from the SCS
		const TArray<USCS_Node*>& Nodes = BlueprintTileClass->SimpleConstructionScript->GetAllNodes();
        
		for (USCS_Node* Node : Nodes)
		{
			if (Node && Node->ComponentTemplate)
			{
				UMapTileConnector* Connector = Cast<UMapTileConnector>(Node->ComponentTemplate);
				if (Connector)
				{
					TemplateConnectors.Add(Connector->Direction);
				}
			}
		}
	}
	
	for (EMapDirection Connector : Connectors)
	{
		if (!TemplateConnectors.Contains(Connector))
			return false;
	}
	
	return true;
}