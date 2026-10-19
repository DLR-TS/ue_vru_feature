/*
 * Copyright (c) 2026 DLR e.V.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "GEMSTAR_VRU_Loader.h"
#include "Subsystems/GemstarSimulationSubsystem.h"

// Sets default values for this component's properties
UGEMSTAR_VRU_Loader::UGEMSTAR_VRU_Loader()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UGEMSTAR_VRU_Loader::BeginPlay()
{
	Super::BeginPlay();

	UGemstarSimulationSubsystem* GemstarSimulationSubsystem =  UGemstarSimulationSubsystem::Get(GetWorld());
	
	if (GemstarSimulationSubsystem) {
		FTransform SpawnTransform;

		FActorSpawnParameters SpawnParams;
		AVRUManager *SpawnedVRUManager = GetWorld()->SpawnActor<AVRUManager>(AVRUManager::StaticClass(), SpawnTransform, SpawnParams);

		TMap<FString, FInnerConfig> Config = GemstarSimulationSubsystem->RegisterManagerAndGetConfiguration(
			SpawnedVRUManager, "ue_vru_feature");
		SpawnedVRUManager->Configure(Config, GemstarSimulationSubsystem);
		UE_LOG(VRU, Log, TEXT("Registered and configured VRU"));
	}
	else
	{
		UE_LOG(VRU, Error, TEXT("Error: Cast of GemstarSimulationSubsystem was not successful"));
	}
}
