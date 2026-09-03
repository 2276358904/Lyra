// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "LyraPawnExtensionComponent.generated.h"

class ULyraPawnData;
class ULyraAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class LYRA_API ULyraPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	/** The name of this overall feature, this one depends on the other named component features */
	static const FName NAME_ActorFeatureName;

public:
	ULyraPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	/** Gets the pawn data, which is used to specify pawn properties in data */
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	/** Sets the current pawn data */
	void SetPawnData(const ULyraPawnData* InPawnData);

	//~IGameFrameworkInitStateInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End IGameFrameworkInitStateInterface interface

protected:
	/** Pawn data used to create the pawn. Specified from a spawn function or on a placed instance. */
	UPROPERTY(EditInstanceOnly, Category = "Lyra|Pawn")
	TObjectPtr<const ULyraPawnData> PawnData;

	/** Pointer to the ability system component that is cached for convenience. */
	UPROPERTY()
	TObjectPtr<ULyraAbilitySystemComponent> AbilitySystemComponent;
};
