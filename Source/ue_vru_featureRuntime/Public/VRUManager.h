/*
 * Copyright (c) 2026 DLR e.V.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GemstarSimulationSubsystem.h"
#include "Interfaces/Movement/VRUDataUpdate.h"
#include "VRUs/VRUBase.h"
#include "GameFramework/Actor.h"
#include "VRUManager.generated.h"

USTRUCT(BlueprintType)
struct FVRUInstance
{
	GENERATED_BODY()

	// Name of the vehicle object
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	// vehicle object
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AVRUBase* VRUInstance;

	FVRUInstance() : Name(NAME_None), VRUInstance(nullptr) {}
	FVRUInstance(FName Name, AVRUBase* VehicleInstance) : Name(Name), VRUInstance(VehicleInstance) {}
};

USTRUCT(BlueprintType)
struct FVRUInstances
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray <FVRUInstance> Instances;

	FVRUInstances() : Instances() {}
};

UCLASS()
class UE_VRU_FEATURERUNTIME_API AVRUManager : public AActor,
	public IGemstarManager,
	public IVRUDataUpdate
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVRUManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, TSubclassOf<AVRUBase>> AvailableVRUs;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FName, FVRUInstances> SpawnedVRUs;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void UpdateVRUMap();

	void Configure(TMap<FString, FInnerConfig> Configurations, UGemstarSimulationSubsystem* GemstarSubsystem);
	virtual void VRUDataUpdate_Implementation(FVRUDataUpdateData& Data) override;

private:
	UPROPERTY()
	TObjectPtr<UGemstarSimulationSubsystem> GemstarSimulationSubsystem;
	
	AVRUBase* SpawnVRU(FVRUDataUpdateData& Data);
	TArray<FAssetData> GetAllAssetsInFolder(FString Path);
};
