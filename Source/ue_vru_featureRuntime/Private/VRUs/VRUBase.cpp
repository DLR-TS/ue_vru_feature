/*
 * Copyright (c) 2026 DLR e.V.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "VRUs/VRUBase.h"

// Sets default values
AVRUBase::AVRUBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVRUBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVRUBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
