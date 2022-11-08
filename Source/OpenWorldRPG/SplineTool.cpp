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
		//�ι��� valid check
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
			
			//Mesh�� ���� ���� �Ʒ����� ������ ��ܱ��� ���� Box�� Max - Min���� �� ũ�⸦ ����. (Min�� ���ʾƷ�, Max�� ������ �� ������)
			FVector PlacedMeshSize = PlacedMeshBox.Max - PlacedMeshBox.Min;
		
			//PlacedMesh�� ������ ���� ������.
			Gap = PlacedMeshSize.X + ele.GapOffset;

			//SplineLength�� Gap�� �������� �Ҽ��� ���� �����Ͽ� Placed�� Mesh�� ������ ���Ѵ�.
			int32 PlacedMeshCount = GetPlacedeleCount(Gap);


			for (int32 i = 0; i <= PlacedMeshCount; i++)
			{
				//Spline�� ���� Mesh�� Placing�Ǵ� �Ÿ�, Spline�� ���� Placing�� Mesh�� �� ���̰� �ȴ�.

				//Spline�� ���� ��ġ���� �־��� ���̸�ŭ ������ ��ġ�� Local Space ��ġ�� ���Ѱ� InstancedMesh�� ��ġ�� �־��ָ� �ȴ�.

				FVector PlacedLocalVec;
				FVector PlaceRotateVec;
				GetSplineElementPlacedLoRo(PlacedLocalVec,PlaceRotateVec,i*Gap); //Newfunction				

				//FVector AdjustLocalVec = PlacedLocalVec + ele.ElementTransform.GetLocation();

				/* Old Version */
				//���� InstantMesh�� / ���� ��ġ�ϴ� Instant�� Vector���� ������ ���� NormalVector�� ȸ������ �����Ѵ�.
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


		//Decal�� X axis�� �����ϱ����� axis�� ���ȴ�. ���� y axis�� �ؾ���.
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

//SplineLength�� Gap�� �������� �Ҽ��� ���� �����Ͽ� Placed�� Mesh�� ������ ���Ѵ�.
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