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
#include "VRUManager.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VRUTester.generated.h"

UCLASS()
class UE_VRU_FEATURERUNTIME_API AVRUTester : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVRUTester();
	UPROPERTY()
	TObjectPtr<AVRUManager> VRUManager;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FVRUDataUpdateData calculateNewPosition1();
	FVRUDataUpdateData calculateNewPosition2();
	FVRUDataUpdateData calculateNewPosition3();
	void CheckDiff(FTransform& oldTransform, FVRUDataUpdateData& newData);

	FTransform TransformMale;
	FTransform TransformFemale;
	FTransform TransformChild;

	FName testnameMan = "male";
	FName testnameWoman = "female";
	FName testnameChild = "child";
	FName testprovider = "testprovider";
	FTransform testposition1;
	FTransform testposition2;
	FTransform testposition3;

	FString ExpectedMaleActorName = "BP_LowPolyMale_C_0";
	FString ExpectedFemaleActorName = "BP_LowPolyFemale_C_0";
	FString ExpectedChildActorName = "BP_LowPolyChild_C_0";

	bool bFirst = true;
	int TestCounter = 0;
};
