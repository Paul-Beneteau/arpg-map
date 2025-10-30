// Fill out your copyright notice in the Description page of Project Settings.


#include "MapRoom.h"

AMapRoom::AMapRoom()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void AMapRoom::BeginPlay()
{
	Super::BeginPlay();

	Connectors.Empty();
	GetComponents<UMapRoomConnector>(Connectors);
}

