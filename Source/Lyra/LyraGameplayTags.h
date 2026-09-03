// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace LyraGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_InitState_Spawned);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_InitState_DataAvailable);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_InitState_DataInitialized);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_InitState_GameplayReady);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Input_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Input_Look);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Input_Crouch);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Input_AutoRun);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_State_Crouching);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_State_Running);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_State_Death);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_State_Death_Dying);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_State_Death_Dead);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Event_Death);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Event_Reset);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Event_RequestReset);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_DamageImmunity);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_DamageSelfDestruct);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_DamageFellOutOfWorld);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_SetByCaller_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_SetByCaller_Heal);
};
