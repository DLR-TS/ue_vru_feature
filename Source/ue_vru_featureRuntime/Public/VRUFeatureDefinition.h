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
#include "Engine/DataAsset.h"
#include "VRUFeatureDefinition.generated.h"

class AVRUBase;

UCLASS(BlueprintType)
class UE_VRU_FEATURERUNTIME_API UVRUFeatureDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const FPrimaryAssetType PrimaryAssetType;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	// Spawnable vehicle bodies, keyed at runtime by asset name to match VehicleAsset in the table
	UPROPERTY(EditDefaultsOnly, Category = "VRUs")
	TArray<TSoftClassPtr<AVRUBase>> VRUs;
};
