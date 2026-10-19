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
#include "GameFeatureAction.h"
#include "VRUFeatureDefinition.h"
#include "LoadVRUAssetsAction.generated.h"

UCLASS(EditInlineNew, meta = (DisplayName = "Load VRU Assets"))
class UE_VRU_FEATURERUNTIME_API ULoadVRUAssetsAction : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	// Harvested into the feature's bundle by UpdateAssetBundleData
	// see the .cpp for what it cannot reach
	UPROPERTY(EditAnywhere, Category = "VRU", meta = (AssetBundles = "Client"))
	TSoftObjectPtr<UVRUFeatureDefinition> Definition;

#if WITH_EDITORONLY_DATA
	virtual void AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData) override;
#endif
};
