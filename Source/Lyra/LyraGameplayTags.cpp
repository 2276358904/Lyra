// Fill out your copyright notice in the Description page of Project Settings.


#include "LyraGameplayTags.h"

namespace LyraGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_InitState_Spawned, "Gameplay.InitState.Spawned");
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_InitState_DataAvailable, "Gameplay.InitState.DataAvailable");
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_InitState_DataInitialized, "Gameplay.InitState.DataInitialized");
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_InitState_GameplayReady, "Gameplay.InitState.GameplayReady");

	UE_DEFINE_GAMEPLAY_TAG(Gameplay_Input_Move, "Gameplay.Input.Move");
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_Input_Look, "Gameplay.Input.Look");
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_Input_Crouch, "Gameplay.Input.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_Input_AutoRun, "Gameplay.Input.AutoRun");

	UE_DEFINE_GAMEPLAY_TAG(Gameplay_State_Crouching, "Gameplay.State.Crouching");
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_State_Running, "Gameplay.State.Running");
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_State_Death, "Gameplay.State.Death");
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_State_Death_Dying, "Gameplay.State.Death.Dying");
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_State_Death_Dead, "Gameplay.State.Death.Dead");

	UE_DEFINE_GAMEPLAY_TAG(Gameplay_Event_Death, "Gameplay.Event.Death");
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_Event_Reset, "Gameplay.Event.Reset");
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_Event_RequestReset, "Gameplay.Event.RequestReset");

	UE_DEFINE_GAMEPLAY_TAG(Gameplay_Damage, "Gameplay.Damage");
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_DamageImmunity, "Gameplay.Damage.Immunity");
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_DamageSelfDestruct, "Gameplay.Damage.SelfDestruct");
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_DamageFellOutOfWorld, "Gameplay.Damage.FellOutOfWorld");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_SetByCaller_Damage, "Gameplay.SetByCaller.Damage", "SetByCaller tag used by damage gameplay effects.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_SetByCaller_Heal, "Gameplay.SetByCaller.Heal", "SetByCaller tag used by healing gameplay effects.");
}

