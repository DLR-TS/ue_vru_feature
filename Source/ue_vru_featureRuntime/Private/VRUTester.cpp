/*
 * Copyright (c) 2026 DLR e.V.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "VRUTester.h"
#include "Kismet/GameplayStatics.h"
#include "VRULog.h"

// Sets default values
AVRUTester::AVRUTester()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AVRUTester::BeginPlay()
{
	Super::BeginPlay();
	testposition1.SetLocation(FVector(0, 0, 0));
	testposition2.SetLocation(FVector(0, 200, 0));
	testposition3.SetLocation(FVector(0, -200, 0));
}

// Called every frame
void AVRUTester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsValid(VRUManager)) {
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVRUManager::StaticClass(), FoundActors);
		if (FoundActors.Num() != 1) {
			UE_LOG(VRU, Error, TEXT("Multiple Managers have spawned! Expected 1 but currently %i"),
				FoundActors.Num());
			return;
		}
		else {
			UE_LOG(VRU, Log, TEXT("Found VRU Manager"));
			VRUManager = Cast<AVRUManager>(FoundActors[0]);
		}
	}
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVRUBase::StaticClass(), FoundActors);
	for (auto& actor : FoundActors) {
		if (actor->GetName() == ExpectedMaleActorName) {
			TransformMale = actor->GetActorTransform();
		} else if (actor->GetName() == ExpectedFemaleActorName) {
			TransformFemale = actor->GetActorTransform();
		} else if (actor->GetName() == ExpectedChildActorName) {
			TransformChild = actor->GetActorTransform();
		}
	}

	FVRUDataUpdateData NewDataMale = calculateNewPosition1();
	FVRUDataUpdateData NewDataFemale = calculateNewPosition2();
	FVRUDataUpdateData NewDataChild = calculateNewPosition3();

	VRUManager->VRUDataUpdate_Implementation(NewDataMale);
	VRUManager->VRUDataUpdate_Implementation(NewDataFemale);
	VRUManager->VRUDataUpdate_Implementation(NewDataChild);

	if (!bFirst) {
		CheckDiff(TransformMale, NewDataMale);
		CheckDiff(TransformFemale, NewDataFemale);
		CheckDiff(TransformChild, NewDataChild);
	}
	bFirst = false;
	TransformMale = NewDataMale.Position;
	TransformFemale = NewDataFemale.Position;
	TransformChild = NewDataChild.Position;

	UE_LOG(VRU, Log, TEXT("Tester ticks."));
	if (TestCounter == 180) {
		UE_LOG(VRU, Log, TEXT("VRU test done."));
		if (GIsEditor)
		{
			// Running inside Unreal Editor
			UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), TEXT("Exit"));
		}
		else
		{
			// Running as packaged game or standalone
			UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
		}
	}
	TestCounter++;
}

FVRUDataUpdateData AVRUTester::calculateNewPosition1() {
	FVRUDataUpdateData data;
	data.Name = testnameMan;
	data.Provider = testprovider;
	data.BoundingBox = FVector(60, 60, 180);
	data.speed = 0.5;
	data.Position = testposition1;
	testposition1.SetLocation(testposition1.GetLocation() + FVector(2, 0, 0));
	testposition1.SetRotation(testposition1.GetRotation() * FRotator(0, 1, 0).Quaternion());
	return data;
}

FVRUDataUpdateData AVRUTester::calculateNewPosition2() {
	FVRUDataUpdateData data;
	data.Name = testnameWoman;
	data.Provider = testprovider;
	data.BoundingBox = FVector(50, 50, 170);
	data.speed = 0.5;
	data.Position = testposition2;
	testposition2.SetLocation(testposition2.GetLocation() + FVector(2, 0, 0));
	testposition2.SetRotation(testposition2.GetRotation() * FRotator(0, 1, 0).Quaternion());
	return data;
}

FVRUDataUpdateData AVRUTester::calculateNewPosition3() {
	FVRUDataUpdateData data;
	data.Name = testnameChild;
	data.Provider = testprovider;
	data.BoundingBox = FVector(40, 40, 100);
	data.speed = 0.5;
	data.Position = testposition3;
	testposition3.SetLocation(testposition3.GetLocation() + FVector(2, 0, 0));
	testposition3.SetRotation(testposition3.GetRotation() * FRotator(0, 1, 0).Quaternion());
	return data;
}

void AVRUTester::CheckDiff(FTransform& oldTransform, FVRUDataUpdateData& newData) {
	FVector diff = oldTransform.GetLocation() + FVector(2, 0, 0) - newData.Position.GetLocation();
	FQuat expected =
		(oldTransform.GetRotation() * FRotator(0, 1, 0).Quaternion()).GetNormalized();

	FQuat actual =
		newData.Position.GetRotation().GetNormalized();

	FQuat diffQ = expected.Inverse() * actual;
	diffQ.Normalize();
	double length = diff.Length();
	if (length == 0.0) {
		if (diffQ.Equals(FQuat::Identity, 1e-6f)) {
			UE_LOG(VRU, Log, TEXT("Position Update is correct."));
		}
		else {
			UE_LOG(VRU, Warning, TEXT("Only position is correct, but not rotation."));
		}
	}
	else {
		UE_LOG(VRU, Error, TEXT("Position Update is not correct."));
	}
}
