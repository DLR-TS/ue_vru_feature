/*
 * Copyright (c) 2026 DLR e.V.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "VRUManager.h"
#include "Engine/AssetManager.h"
#include "VRULog.h"
#include "VRUFeatureDefinition.h"
#include "AssetRegistry/AssetRegistryModule.h"

AVRUManager::AVRUManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AVRUManager::BeginPlay()
{
	Super::BeginPlay();
	UpdateVRUMap();
}

void AVRUManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(GemstarSimulationSubsystem)){
		GemstarSimulationSubsystem->UnregisterManager(this);
		GemstarSimulationSubsystem = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

void AVRUManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVRUManager::Configure(TMap<FString, FInnerConfig> Configurations, UGemstarSimulationSubsystem* GemstarSubsystem) {
	GemstarSimulationSubsystem = GemstarSubsystem;

	if (Configurations.Num() > 1) {
		UE_LOG(VRU, Warning, TEXT("VRU Manager. Multiple configs."));
	}
	for (const TPair<FString, FInnerConfig>& Pair : Configurations) {
		const FString* Inputs = Pair.Value.Entries.Find("Input");
		FDataTarget dt;
		dt.Actor = this;
		ConnectInputs(Inputs, GemstarSimulationSubsystem, dt, GemstarInterfaceType::VRUUPDATE);
	}
}

void AVRUManager::VRUDataUpdate_Implementation(FVRUDataUpdateData& Data) {
	if (Data.Name.IsNone()) {
		UE_LOG(VRU, Log, TEXT("Update VRU, but no identifier is given. Skip."));
		return;
	}

	FVRUInstances* VRUsFromController = SpawnedVRUs.Find(Data.Provider);
	if (VRUsFromController == nullptr) {
		UE_LOG(VRU, Log, TEXT("No controller registered with this name. % s Creating a controller now."), *Data.Provider.ToString());

		FVRUInstances Instances;
		SpawnedVRUs.Add(Data.Provider, Instances);
		VRUsFromController = SpawnedVRUs.Find(Data.Provider);
	}

	AVRUBase* Base;
	FVRUInstance* vruInstance = VRUsFromController->Instances.FindByPredicate([&Data](const FVRUInstance& instance)
		{
			return instance.Name.IsEqual(Data.Name);
		});
	if (vruInstance == nullptr) {
		//vru does not exist
		Base = SpawnVRU(Data);
		VRUsFromController->Instances.Add({ Data.Name, Base });
	}
	else {
		Base = vruInstance->VRUInstance;
	}

	//update position
	Base->SetActorTransform(Data.Position, false, nullptr, ETeleportType::TeleportPhysics);
	//TODO Z position
}

AVRUBase* AVRUManager::SpawnVRU(FVRUDataUpdateData& data) {
	AVRUBase* Actor = nullptr;
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FName search = "BP_LowPolyMale";
	if (data.Name.ToString().Contains("female")) {
		search = "BP_LowPolyFemale";
	}
	else if (data.Name.ToString().Contains("child")) {
		search = "BP_LowPolyKid";
	}

	TSubclassOf<AVRUBase> VRUAsset = *AvailableVRUs.Find(search);
	if (VRUAsset && *VRUAsset)
	{
		UClass* VRUClass = *VRUAsset;

		Actor = GetWorld()->SpawnActor<AVRUBase>(
			VRUClass,
			data.Position,
			SpawnInfo
		);
	}

	return Actor;
}

void AVRUManager::UpdateVRUMap()
{
	AvailableVRUs.Empty();

	// Preferred: the feature definition, which is bundled and therefore cooked
	const FPrimaryAssetId DefinitionId(
		UVRUFeatureDefinition::PrimaryAssetType, TEXT("DA_VRUFeatureDefinition"));
	UAssetManager& AssetManager = UAssetManager::Get();

	const UVRUFeatureDefinition* Definition =
		Cast<UVRUFeatureDefinition>(AssetManager.GetPrimaryAssetObject(DefinitionId));

	if (!IsValid(Definition)) {
		Definition = Cast<UVRUFeatureDefinition>(
			AssetManager.GetPrimaryAssetPath(DefinitionId).TryLoad());
	}

	if (IsValid(Definition) && Definition->VRUs.Num() > 0) {

		for (const TSoftClassPtr<AVRUBase>& Soft : Definition->VRUs) {

			if (UClass* Loaded = Soft.LoadSynchronous()) {
				// Key by the asset name the table rows use: a class reference's name carries a
				// _C suffix the rows do not
				FString KeyName = Soft.GetAssetName();
				KeyName.RemoveFromEnd(TEXT("_C"));
				AvailableVRUs.Add(FName(*KeyName), Loaded);
			}
		}
		UE_LOG(VRU, Log, TEXT("Found total: %d (from the VRU feature definition)"),
			AvailableVRUs.Num());
		return;
	}

	// Legacy path: scan the folder. Convenient, but invisible to the cooker
	UE_LOG(VRU, Warning,
		TEXT("GEMSTAR: no VRU feature definition resolved; falling back to the folder scan. "
			"These assets are not referenced by anything the cooker can see."));

	TArray<FAssetData> AssetList;
	AssetList = GetAllAssetsInFolder(TEXT("/ue_vru_feature/Blueprints/VRU"));

	for (const FAssetData& Asset : AssetList)
	{
		FString ClassPath = Asset.GetObjectPathString() + TEXT("_C");
		UClass* AssetClass = LoadObject<UClass>(nullptr, *ClassPath);
		UE_LOG(VRU, Log, TEXT("Check asset: %s (%s)"), *Asset.AssetName.ToString(), *ClassPath);

		if (AssetClass && AssetClass->IsChildOf(AVRUBase::StaticClass()))
		{
			//FName AssetName = Asset.AssetName;
			UE_LOG(VRU, Log, TEXT("Add available VRUs: %s"), *Asset.AssetName.ToString());
			AvailableVRUs.Add(Asset.AssetName, AssetClass);
		}
	}
	UE_LOG(VRU, Log, TEXT("Found total: %d"), AvailableVRUs.Num());
}

TArray<FAssetData> AVRUManager::GetAllAssetsInFolder(FString path)
{
	TArray<FAssetData> AssetList;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistryModule.Get().GetAssetsByPath(*path, AssetList, false);
	return AssetList;
}
