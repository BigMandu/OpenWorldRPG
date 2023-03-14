// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SplineTool.generated.h"

//UENUM()
//enum class EMeshForwardAxis
//{
//	X	UMETA(Displayname = "X"),
//	Y	UMETA(Displayname = "Y"),
//	Z	UMETA(Displayname = "Z")
//};

UENUM()
enum class ESplineElementType
{
	ESET_Mesh	UMETA(Displayname = "HIStaticMesh"),
	ESET_Decal	UMETA(Displayname = "DecalActor")
};

// 에디터 On/off하는 방법임
//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "IsLocationConstant"), Category = "Location Settings", meta = (AllowPreserveRatio))
//FVector Offset = FVector::ZeroVector;

USTRUCT()
struct FSplineElement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "SplineTool")
	class UStaticMesh* WantToPlaceMesh;

	UPROPERTY(EditAnywhere, Category = "SplineTool")
	class UMaterialInstance* WantToPlaceDecal;

	UPROPERTY(EditAnywhere, Category = "SplineTool")
	float GapOffset = 50.f;

	//UPROPERTY(EditAnywhere, Category = "SplineTool")
	//EMeshForwardAxis ForwardAxis;

	UPROPERTY(EditAnywhere, Category = "SplineTool")
	FTransform ElementTransform;
	
	/*UPROPERTY(EditAnywhere, Category = "SplineTool")
	bool bTestbool = true;*/
	
	/*UPROPERTY(EditAnywhere, Category = "SplineTool", meta = (ClampMin = "0.0", ClampMax = "360.0", UIMin = "0.0", UIMax = "360.0"))
	float MeshRotate;*/
};

UCLASS()
class OPENWORLDRPG_API ASplineTool : public AActor
{
	GENERATED_BODY()
	
public:	
	ASplineTool();

protected:
	virtual void BeginPlay() override;

	class UHierarchicalInstancedStaticMeshComponent* HISMesh;
	float Gap;
	//TArray<UInstancedStaticMeshComponent*>ISMCompStoredArray;
	TArray<UHierarchicalInstancedStaticMeshComponent*>ISMCompStoredArray;
	
	UHierarchicalInstancedStaticMeshComponent* HISMComp;

	TArray<UMaterialInstance*> DecalArray;

public:	
	UPROPERTY(EditAnywhere, Category = "SplineTool")
	bool bSetNewVersion;

	UPROPERTY(EditAnywhere, Category = "SplineTool")
	FSplineElement NewSplineSetting;

	UPROPERTY(EditAnywhere, Category = "SplineTool")
	class UBillboardComponent* Billboard;

	UPROPERTY(EditAnywhere, Category = "SplineTool")
	TArray<FSplineElement> SplineMeshArr;


	UPROPERTY(EditAnywhere, Category = "SplineTool")
	ESplineElementType SplineElementType;

	UPROPERTY(VisibleAnywhere, Category = "SplineTool")
	class USplineComponent* SplineComp;

	virtual void OnConstruction(const FTransform& Transform) override;



	//****function ***//

	/*virtual FTransform GetSplineElementPlacedTransform(float Space);

	virtual FTransform GetElementTransform();
	virtual bool AdjustPreviousElement(int32 index);*/
	
	void GetSplineElementPlacedLoRo(FVector& LoVec, FVector& RoVec, float Space, bool bIsWorld = false);
	int32 GetPlacedeleCount(int32 space);

	FTransform AdjustPreviouseleRotation(FVector CurrenteleVec, FTransform PreviousElement);

	FTransform CalculateFinalPlacedElement(FVector Vec, FVector RotVec, FTransform ElementTransform);

	void NewMeshSpline();

	void MeshSpline();
	void DecalSpline();
};
