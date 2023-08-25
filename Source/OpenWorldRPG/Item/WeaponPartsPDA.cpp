// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/WeaponPartsPDA.h"

bool UWeaponPartsPDA::CanEditChange(const FProperty* InProperty) const
{
	const bool ReturnVal = Super::CanEditChange(InProperty);

	if ( ReturnVal )
	{
		if ( InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UWeaponPartsPDA, TacticalEmitEffect) )
		{
			return TacticalPartsType == ETacticalPartsType::ETPT_Laser;
		}
		if ( InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UWeaponPartsPDA, TacticalImpactEmitEffect) )
		{
			return TacticalPartsType == ETacticalPartsType::ETPT_Laser;
		}
		if ( InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UWeaponPartsPDA, TacticalEmitLimitRange) )
		{
			return TacticalPartsType == ETacticalPartsType::ETPT_Laser;
		}
		if ( InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UWeaponPartsPDA, LaserEmitColor) )
		{
			return TacticalPartsType == ETacticalPartsType::ETPT_Laser;
		}
		if ( InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UWeaponPartsPDA, LaserWidth) )
		{
			return TacticalPartsType == ETacticalPartsType::ETPT_Laser;
		}

		if ( InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UWeaponPartsPDA, Flashlight_InnerConeRadius) )
		{
			return TacticalPartsType == ETacticalPartsType::ETPT_Flashlight;
		}
		if ( InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UWeaponPartsPDA, Flashlight_OuterConeRadius) )
		{
			return TacticalPartsType == ETacticalPartsType::ETPT_Flashlight;
		}
		if ( InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UWeaponPartsPDA, Flashlight_Distance) )
		{
			return TacticalPartsType == ETacticalPartsType::ETPT_Flashlight;
		}
		if ( InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UWeaponPartsPDA, Flashlight_Brightness) )
		{
			return TacticalPartsType == ETacticalPartsType::ETPT_Flashlight;
		}

	}


	return ReturnVal;
}