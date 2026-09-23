// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/TestAbilitySystem.h"

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class UTestAttributeSet : public UAttributeSet
{
public:
	UTestAttributeSet()
	{ }

	ATTRIBUTE_ACCESSORS(UTestAttributeSet, Attribute01);
	ATTRIBUTE_ACCESSORS(UTestAttributeSet, Attribute02);

private:
	FGameplayAttributeData Attribute01;
	FGameplayAttributeData Attribute02;
};


UTestAbilitySystem::UTestAbilitySystem()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
}

void UTestAbilitySystem::TestAbilitySystemComponent_01()
{
	UTestAttributeSet* AttributeSet = NewObject<UTestAttributeSet>(AbilitySystemComponent);
	TArray<UAttributeSet*> AttributeSets;
	AttributeSets.Add(AttributeSet);
	TArray<FGameplayAttribute> Attributes;
	//添加属性集
	AbilitySystemComponent->AddAttributeSetSubobject<UTestAttributeSet>(AttributeSet);
	AbilitySystemComponent->AddSet<UTestAttributeSet>();
	AbilitySystemComponent->AddSpawnedAttribute(AttributeSet);
	//删除属性集
	AbilitySystemComponent->RemoveSpawnedAttribute(AttributeSet);
	AbilitySystemComponent->RemoveAllSpawnedAttributes();
	//修改属性集
	AbilitySystemComponent->SetSpawnedAttributes(AttributeSets);
	//修改属性集中某个属性的值
	AbilitySystemComponent->ApplyModToAttribute(UTestAttributeSet::GetAttribute01Attribute(), EGameplayModOp::Additive, 1);
	AbilitySystemComponent->ApplyModToAttributeUnsafe(UTestAttributeSet::GetAttribute01Attribute(), EGameplayModOp::Additive, 1);
	AbilitySystemComponent->SetNumericAttributeBase(UTestAttributeSet::GetAttribute01Attribute(), 1);
	//获取属性集
	const UTestAttributeSet* AttributeSet01 = AbilitySystemComponent->GetSet<UTestAttributeSet>();
	const UTestAttributeSet* AttributeSet02 = AbilitySystemComponent->GetSetChecked<UTestAttributeSet>();
	const UTestAttributeSet* AttributeSet03 = Cast<UTestAttributeSet>(AbilitySystemComponent->GetAttributeSet(UTestAttributeSet::StaticClass()));
	AttributeSets = AbilitySystemComponent->GetSpawnedAttributes();
	//获取属性
	AbilitySystemComponent->GetAllAttributes(Attributes);
	//获取属性值
	AbilitySystemComponent->GetNumericAttribute(UTestAttributeSet::GetAttribute01Attribute());
	AbilitySystemComponent->GetNumericAttributeBase(UTestAttributeSet::GetAttribute01Attribute());
	AbilitySystemComponent->GetNumericAttributeChecked(UTestAttributeSet::GetAttribute01Attribute());
	//是否有某个属性对应的属性集
	AbilitySystemComponent->HasAttributeSetForAttribute(UTestAttributeSet::GetAttribute01Attribute());
	//获取属性对应的属性集
	FGameplayAttribute Attribute = UTestAttributeSet::GetAttribute01Attribute();
	AbilitySystemComponent->GetAttributeSet(Attribute.GetAttributeSetClass());
}

void UTestAbilitySystem::TestAbilitySystemComponent_02()
{
	//构造一个新实例
	UGameplayAbility* GameplayAbility = NewObject<UGameplayAbility>(AbilitySystemComponent);
	//使用类默认实例
	GameplayAbility = Cast<UGameplayAbility>(UGameplayAbility::StaticClass()->GetDefaultObject());
	//构建FGameplayAbilitySpec
	FGameplayAbilitySpec GameplayAbilitySpec(UGameplayAbility::StaticClass(), 1, 1);
	GameplayAbilitySpec = AbilitySystemComponent->BuildAbilitySpecFromClass(UGameplayAbility::StaticClass(), 1);
	//赋予技能
	FGameplayAbilitySpecHandle GameplayAbilitySpecHandle = AbilitySystemComponent->GiveAbility(GameplayAbilitySpec);
	GameplayAbilitySpecHandle = AbilitySystemComponent->GiveAbilityAndActivateOnce(GameplayAbilitySpec);
	//移除技能
	AbilitySystemComponent->ClearAbility(GameplayAbilitySpecHandle);
	AbilitySystemComponent->ClearAllAbilities();
	AbilitySystemComponent->ClearAllAbilitiesWithInputID();
	AbilitySystemComponent->SetRemoveAbilityOnEnd(GameplayAbilitySpecHandle);
	//激活技能
	AbilitySystemComponent->TryActivateAbility(GameplayAbilitySpecHandle);
	AbilitySystemComponent->TryActivateAbilitiesByTag(GameplayAbility->AbilityTags);
	AbilitySystemComponent->TryActivateAbilityByClass(UGameplayAbility::StaticClass());
	//取消技能
	AbilitySystemComponent->CancelAbilities(&GameplayAbility->AbilityTags);
	AbilitySystemComponent->CancelAbility(GameplayAbility);
	AbilitySystemComponent->CancelAbilityHandle(GameplayAbilitySpecHandle);
	AbilitySystemComponent->CancelAllAbilities();
	//获取赋予的技能
	TArray<FGameplayAbilitySpec> ActivatableAbilitySpecs = AbilitySystemComponent->GetActivatableAbilities();
	TArray<FGameplayAbilitySpecHandle> ActivatableAbilitySpecHandles;
	AbilitySystemComponent->GetAllAbilities(ActivatableAbilitySpecHandles);
	FGameplayAbilitySpec* GameplayAbilitySpec1 = AbilitySystemComponent->FindAbilitySpecFromClass(UGameplayAbility::StaticClass());
	GameplayAbilitySpec1 = AbilitySystemComponent->FindAbilitySpecFromHandle(GameplayAbilitySpecHandle);
	GameplayAbilitySpec1 = AbilitySystemComponent->FindAbilitySpecFromInputID(0);
	AbilitySystemComponent->FindAllAbilitiesWithTags(ActivatableAbilitySpecHandles, GameplayAbility->AbilityTags);
	AbilitySystemComponent->FindAllAbilitiesWithInputID(ActivatableAbilitySpecHandles, 0);
	
}

void UTestAbilitySystem::TestAbilitySystemComponent_03()
{
	//构建UGameplayEffect
	UGameplayEffect* GameplayEffect = NewObject<UGameplayEffect>(AbilitySystemComponent);
	GameplayEffect = Cast<UGameplayEffect>(UGameplayEffect::StaticClass()->GetDefaultObject());
	//构建GameplayEffectContext
	FGameplayEffectContextHandle GameplayEffectContextHandle = AbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpec GameplayEffectSpec(GameplayEffect, GameplayEffectContextHandle, 1);
	//赋予效果
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(GameplayEffectSpec);
	UAbilitySystemComponent* TargetAbilitySystemComponent = NewObject<UAbilitySystemComponent>(AbilitySystemComponent);
	ActiveGameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(GameplayEffectSpec, TargetAbilitySystemComponent);
	ActiveGameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectToSelf(GameplayEffect, 1, GameplayEffectContextHandle);
	//移除效果
	FGameplayTagContainer GameplayTagContainer;
	//移除效果（匹配GameplayEffect的OwningEffectTags)
	AbilitySystemComponent->RemoveActiveEffectsWithTags(GameplayTagContainer);
	//移除效果（匹配GameplayEffectSpec的SourceTags的GameplayTag)
	AbilitySystemComponent->RemoveActiveEffectsWithSourceTags(GameplayTagContainer);
	//移除效果（匹配GameplayEffectSpec的OwningTags=OwningEffectTags+GrantedTags+DynamicTags)
	AbilitySystemComponent->RemoveActiveEffectsWithAppliedTags(GameplayTagContainer);
	AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(GameplayTagContainer);
	//设置效果属性
	AbilitySystemComponent->UpdateActiveGameplayEffectSetByCallerMagnitude(ActiveGameplayEffectHandle, FGameplayTag(), 10);
	AbilitySystemComponent->SetActiveGameplayEffectLevel(ActiveGameplayEffectHandle, 1);
	//获取效果
	FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAllEffectTags(GameplayTagContainer);
	TArray<FActiveGameplayEffectHandle> ActiveGameplayEffectHandles = AbilitySystemComponent->GetActiveEffects(GameplayEffectQuery);
	ActiveGameplayEffectHandles = AbilitySystemComponent->GetActiveEffectsWithAllTags(GameplayTagContainer);
	const FActiveGameplayEffect* ActiveGameplayEffect = AbilitySystemComponent->GetActiveGameplayEffect(ActiveGameplayEffectHandle);
	FActiveGameplayEffectsContainer ActiveGameplayEffectsContainer = AbilitySystemComponent->GetActiveGameplayEffects();
	FGameplayAbilitySpecHandle GameplayAbilitySpecHandle;
	ActiveGameplayEffectHandle = AbilitySystemComponent->FindActiveGameplayEffectHandle(GameplayAbilitySpecHandle);
	//获取效果数量
	int32 Count = AbilitySystemComponent->GetGameplayEffectCount(UGameplayEffect::StaticClass(), AbilitySystemComponent);
	float Duration = AbilitySystemComponent->GetGameplayEffectDuration(ActiveGameplayEffectHandle);
	float Magnitude = AbilitySystemComponent->GetGameplayEffectMagnitude(ActiveGameplayEffectHandle, UTestAttributeSet::GetAttribute01Attribute());
	const UGameplayEffect* GameplayEffect1 = AbilitySystemComponent->GetGameplayEffectDefForHandle(ActiveGameplayEffectHandle);
}

void UTestAbilitySystem::TestAbilitySystemComponent_04()
{
	//添加Cue
	FGameplayTag GameplayTag;
	AbilitySystemComponent->AddGameplayCue(GameplayTag, AbilitySystemComponent->MakeEffectContext());
	FGameplayCueParameters GameplayCueParameters;
	AbilitySystemComponent->AddGameplayCue(GameplayTag, GameplayCueParameters);
	//移除Cue
	AbilitySystemComponent->RemoveGameplayCue(GameplayTag);
	AbilitySystemComponent->RemoveAllGameplayCues();
	//执行Cue
	AbilitySystemComponent->ExecuteGameplayCue(GameplayTag, AbilitySystemComponent->MakeEffectContext());
	AbilitySystemComponent->ExecuteGameplayCue(GameplayTag, GameplayCueParameters);
	//处理Cue事件
	AbilitySystemComponent->InvokeGameplayCueEvent(GameplayTag, EGameplayCueEvent::Executed, AbilitySystemComponent->MakeEffectContext());
	AbilitySystemComponent->InvokeGameplayCueEvent(GameplayTag, EGameplayCueEvent::Executed, GameplayCueParameters);
}

void UTestAbilitySystem::TestAbilitySystemComponent_05()
{
	AActor* Actor = NewObject<AActor>();
	AbilitySystemComponent->InitAbilityActorInfo(Actor, Actor);
	AbilitySystemComponent->RefreshAbilityActorInfo();
}
