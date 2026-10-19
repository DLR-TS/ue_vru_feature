/*
 * Copyright (c) 2026 DLR e.V.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "LoadVRUAssetsAction.h"

#include "GameFeaturesSubsystemSettings.h"
#include "VRUFeatureDefinition.h"

#if WITH_EDITORONLY_DATA
void ULoadVRUAssetsAction::AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData)
{
	Super::AddAdditionalAssetBundleData(AssetBundleData);

	if (Definition.IsNull()) {
		return;
	}

	// Definition path itself is declared by meta=(AssetBundles) on the property
	// Actions is an Instanced array
	// InitializeAssetBundlesFromMetadata walks into this object and finds it
	//
	// Metadata doesn't follow soft references inside other assets
	// VRUs are stored as TSoftClassPtr
	// Bundle loading doesn't recurse across assets
	//The loop ensures VRUs are loaded before falling back to LoadSynchronous
	const UVRUFeatureDefinition* Loaded = Definition.LoadSynchronous();
	if (Loaded == nullptr)
	{
		return;
	}

	for (const TSoftClassPtr<AVRUBase>& LoadedVRU : Loaded->VRUs)
	{
		if (!LoadedVRU.IsNull())
		{
			AssetBundleData.AddBundleAssetTruncated(
				UGameFeaturesSubsystemSettings::LoadStateClient, LoadedVRU.ToSoftObjectPath());
		}
	}
}
#endif
