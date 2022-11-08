// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineTool.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "Components/BillboardComponent.h"

#include "Kismet/KismetMathLibrary.h"

#include "Components/BoxComponent.h" //for debug
#include "DrawDebugHelpers.h"// for debug

ASplineTool::ASplineTool()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	check(SplineComp)
	SetRootComponent(SplineComp);

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComp"));
	Billboard->SetupAttachment(GetRootComponent());
}

void ASplineTool::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASplineTool::OnConstruction(const FTransform& Transform)
{
	switch (SplineElementType)
	{
	case ESplineElementType::ESET_Mesh:
		MeshSpline();
		break;
	case ESplineElementType::ESET_Decal:
		DecalSpline();
		break;
	}	
}

void ASplineTool::MeshSpline()
{
	for (auto storedComp : ISMCompStoredArray)
	{
		//두번의 valid check
		if (!storedComp) break;
		if (storedComp->GetInstanceCount() < 1) break;

		//storedComp->ClearInstances(); 
		storedComp->DestroyComponent();
	}

	for (auto ele : SplineMeshArr)
	{
		if (ele.WantToPlaceMesh)
		{			
			//UInstancedStaticMeshComponent* ISMComp = NewObject<UInstancedStaticMeshComponent>(this);						
			UHierarchicalInstancedStaticMeshComponent* ISMComp = NewObject<UHierarchicalInstancedStaticMeshComponent>(this);
			ISMComp->SetupAttachment(GetRootComponent());
			ISMComp->SetStaticMesh(ele.WantToPlaceMesh);
			ISMComp->RegisterComponent();
			
			

			ISMCompStoredArray.AddUnique(ISMComp);

			FBox PlacedMeshBox = ele.WantToPlaceMesh->GetBoundingBox();
			
			//Mesh의 가장 왼쪽 아래부터 오른쪽 상단까지 덮는 Box의 Max - Min값을 해 크기를 구함. (Min이 왼쪽아래, Max가 오른쪽 위 지점임)
			FVector PlacedMeshSize = PlacedMeshBox.Max - PlacedMeshBox.Min;
		
			//PlacedMesh의 간격을 따로 저장함.
			Gap = PlacedMeshSize.X + ele.GapOffset;

			//SplineLength에 Gap을 나눈값을 소수점 이하 절삭하여 Placed될 Mesh의 개수를 구한다.
			int32 PlacedMeshCount = GetPlacedeleCount(Gap);


			for (int32 i = 0; i <= PlacedMeshCount; i++)
			{
				//Spline을 따라 Mesh가 Placing되는 거리, Spline을 따라 Placing된 Mesh의 총 길이가 된다.

				//Spline의 시작 위치에서 주어진 길이만큼 떨어진 위치의 Local Space 위치를 구한걸 InstancedMesh의 위치로 넣어주면 된다.

				FVector PlacedLocalVec;
				FVector PlaceRotateVec;
				GetSplineElementPlacedLoRo(PlacedLocalVec,PlaceRotateVec,i*Gap); //Newfunction				

				//FVector AdjustLocalVec = PlacedLocalVec + ele.ElementTransform.GetLocation();

				/* Old Version */
				//이전 InstantMesh를 / 지금 배치하는 Instant의 Vector값을 참조해 구한 NormalVector로 회전값을 수정한다.
				/*
				if (i > 0)// && ele.bTestbool)
				{
					FTransform PreviousInstTF;
					if (ISMComp->GetInstanceTransform(i - 1, PreviousInstTF))
					{
						FTransform AdjustPreviousInstTF = AdjustPreviouseleRotation(PlacedLocalVec, PreviousInstTF);
						ISMComp->UpdateInstanceTransform(i - 1, AdjustPreviousInstTF, false); //for HISM
					}
				}
				*/

				FTransform newTransform = CalculateFinalPlacedElement(PlacedLocalVec,PlaceRotateVec,ele.ElementTransform);

				/* Old Version */
				/*
				FRotator initRot = ele.ElementTransform.GetRotation().Rotator();
				FRotator newRot = UKismetMathLibrary::ComposeRotators(initRot, UKismetMathLibrary::MakeRotFromZX(PlaceRotateVec, FVector::UpVector));
				FTransform newTransform = FTransform(newRot, PlacedLocalVec, ele.ElementTransform.GetScale3D());*/

				//FTransform PlacedLocalTF = FTransform(UKismetMathLibrary::Conv_VectorToRotator(PlaceRotateVec), AdjustLocalVec, ele.ElementTransform.GetScale3D());
				//ISMComp->AddInstance(PlacedLocalTF);
				ISMComp->AddInstance(newTransform);
			}



		}
	}
}

void ASplineTool::DecalSpline()
{
	for(auto ele : SplineMeshArr)
	{
		if(ele.WantToPlaceDecal == false) break;


		//Decal의 X axis는 투영하기위한 axis로 사용된다. 따라서 y axis로 해야함.
		//ele.GapOffset =   ->DecalSize.Y * 2;
		Gap = ele.GapOffset; //+ DecalComp->DecalSize.Y * 2

		int32 PlacedDecalCount = GetPlacedeleCount(Gap);

		for (int32 i = 0; i <= PlacedDecalCount; i++)
		{
			UDecalComponent* DecalComp = NewObject<UDecalComponent>(this);
			DecalComp->SetupAttachment(GetRootComponent());
			DecalComp->RegisterComponent();
			DecalComp->CreationMethod = EComponentCreationMethod::SimpleConstructionScript;


			FVector PlacedLocalVec;
			FVector PlaceRotateVec;
			GetSplineElementPlacedLoRo(PlacedLocalVec, PlaceRotateVec, i * Gap, true);		


			FTransform newTransform = CalculateFinalPlacedElement(PlacedLocalVec, PlaceRotateVec, ele.ElementTransform);

			/* Move in to function*/
			/*
			FRotator initRot = ele.ElementTransform.GetRotation().Rotator();
			FRotator newRot = UKismetMathLibrary::ComposeRotators(initRot,UKismetMathLibrary::MakeRotFromZX(PlaceRotateVec,FVector::UpVector));
			FTransform newTransform = FTransform(newRot, PlacedLocalVec, ele.ElementTransform.GetScale3D());
			*/

			DecalComp->SetWorldTransform(newTransform);

			DecalComp->SetDecalMaterial(ele.WantToPlaceDecal);
			
		}
	}
}

FTransform ASplineTool::AdjustPreviouseleRotation(FVector CurrenteleVec, FTransform PreviousElement)
{
	FRotator AdjustRotate = (CurrenteleVec - PreviousElement.GetLocation()).Rotation();
	FRotator exceptXRot = FRotator(0.f, AdjustRotate.Yaw, AdjustRotate.Roll);
	FTransform AdjustPreviousInstTF = FTransform(exceptXRot, PreviousElement.GetLocation(), PreviousElement.GetScale3D());

	return AdjustPreviousInstTF;
}

void ASplineTool::GetSplineElementPlacedLoRo(FVector& LoVec, FVector& RoVec, float Space, bool bIsWorld)
{
	if (bIsWorld)
	{
		LoVec = SplineComp->GetLocationAtDistanceAlongSpline(Space, ESplineCoordinateSpace::World);
		RoVec = SplineComp->GetDirectionAtDistanceAlongSpline(Space, ESplineCoordinateSpace::World);
	}
	else
	{
		LoVec = SplineComp->GetLocationAtDistanceAlongSpline(Space, ESplineCoordinateSpace::Local);
		RoVec = SplineComp->GetDirectionAtDistanceAlongSpline(Space, ESplineCoordinateSpace::Local);
	}
}

//SplineLength에 Gap을 나눈값을 소수점 이하 절삭하여 Placed될 Mesh의 개수를 구한다.
int32 ASplineTool::GetPlacedeleCount(int32 space)
{
	return FMath::FloorToInt(SplineComp->GetSplineLength() / space);
}

FTransform ASplineTool::CalculateFinalPlacedElement(FVector Vec, FVector RotVec, FTransform ElementTransform)
{
	FRotator initRot = ElementTransform.GetRotation().Rotator();

	FRotator newRot = UKismetMathLibrary::ComposeRotators(initRot, UKismetMathLibrary::MakeRotFromZX(RotVec, FVector::UpVector));
	FTransform newTransform = FTransform(newRot, Vec, ElementTransform.GetScale3D());

	return newTransform;
}