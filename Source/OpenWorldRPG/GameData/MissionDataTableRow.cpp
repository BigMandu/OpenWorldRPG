// Fill out your copyright notice in the Description page of Project Settings.
#include "OpenWorldRPG/GameData/MissionDataTableRow.h"


//bool FMissionDataTableRow::CanEditChange(const FProperty* InProperty) const
//{
//	const bool ReturnVal = Super::CanEditChange(InProperty);
//	if (ReturnVal)
//	{
//		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FMissionDataTableRow, Choice1Name))
//		{
//			return Missiontype == EMissionType::EMT_Choice;
//		}
//		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FMissionDataTableRow, Choice1Description))
//		{
//			return Missiontype == EMissionType::EMT_Choice;
//		}
//		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FMissionDataTableRow, Choice1Objective))
//		{
//			return Missiontype == EMissionType::EMT_Choice;
//		}
//
//		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FMissionDataTableRow, Choice2Name))
//		{
//			return Missiontype == EMissionType::EMT_Choice;
//		}
//		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FMissionDataTableRow, Choice2Description))
//		{
//			return Missiontype == EMissionType::EMT_Choice;
//		}
//		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FMissionDataTableRow, Choice2Objective))
//		{
//			return Missiontype == EMissionType::EMT_Choice;
//		}
//	}
//
//	return ReturnVal;
//}