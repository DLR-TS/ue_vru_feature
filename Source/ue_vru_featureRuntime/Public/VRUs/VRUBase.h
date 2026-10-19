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
#include "GameFramework/Actor.h"
#include "VRUBase.generated.h"

UCLASS()
class UE_VRU_FEATURERUNTIME_API AVRUBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AVRUBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
