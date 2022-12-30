// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.h"
#include "Survivor_PC.h"
#include "MyGameInstance.h"
#include "MyGameMode.h"

// Sets default values
ASurvivorCharacter::ASurvivorCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));

	//GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	GetCharacterMovement()->JumpZVelocity = 400.0f;
	GetCharacterMovement()->MaxWalkSpeed = 200.0f;

	// 캐릭터가 자연스럽게 회전하게 
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	bCanRun = true;
	bCanCrouching = true;

	CurrentPlayerState = EPlayerState::ALIVE;
	CurrentWeaponState = EWeaponState::PUNCH;

}

// Called when the game starts or when spawned
void ASurvivorCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CharacterAnim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
}

// Called every frame
void ASurvivorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASurvivorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASurvivorCharacter::UpDown(float NewAxisValue)
{
	FVector Direction = FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X);
	Direction.Z = 0.0f;
	Direction.Normalize();

	if (NewAxisValue < 0)
	{
		CharacterAnim->IsBack = true;
	}

	else
	{
		CharacterAnim->IsBack = false;
	}

	AddMovementInput(Direction, NewAxisValue);
}

void ASurvivorCharacter::LeftRight(float NewAxisValue)
{
	FVector Direction = FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y);
	Direction.Z = 0.0f;
	Direction.Normalize();

	AddMovementInput(Direction, NewAxisValue);
}

void ASurvivorCharacter::LookUp(float NewAxisValue)
{
	AddControllerPitchInput(NewAxisValue);
}

void ASurvivorCharacter::Turn(float NewAxisValue)
{
	AddControllerYawInput(NewAxisValue);
}

void ASurvivorCharacter::Run()
{
	if (bCanRun)
	{
		GetCharacterMovement()->MaxWalkSpeed *= 2;
		bCanCrouching = false;
	}
}

void ASurvivorCharacter::StopRun()
{
	if (bCanRun)
	{
		GetCharacterMovement()->MaxWalkSpeed /= 2;
		bCanCrouching = true;
	}
}

void ASurvivorCharacter::Jump()
{
	Super::Jump();
}

void ASurvivorCharacter::StopJumping()
{
	Super::StopJumping();
}

void ASurvivorCharacter::GetItem()
{
	if (CurrentWeaponState == EWeaponState::PUNCH)
	{
		CurrentWeaponState = EWeaponState::SHOOT;
		CharacterAnim->IsFire = true;
		OnWeaponEquip();
	}

	else
	{
		CurrentWeaponState = EWeaponState::PUNCH;
		CharacterAnim->IsFire = false;
	}
}

void ASurvivorCharacter::Crouching()
{
	if (bCanCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed /= 2;
		CharacterAnim->IsCrouching = true;
		bCanRun = false;
	}
}

void ASurvivorCharacter::StopCrouching()
{
	if (bCanCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed *= 2;
		CharacterAnim->IsCrouching = false;
		bCanRun = true;
	}
}

void ASurvivorCharacter::Punching()
{
	CharacterAnim->PlayAttackMontage();
}

void ASurvivorCharacter::OnFire()
{
	CharacterAnim->PlayFireMontage();
	OnEventFire();
}

void ASurvivorCharacter::PlayerAttack()
{
	if (CurrentWeaponState == EWeaponState::SHOOT)
	{
		OnFire();
	}

	else if (CurrentWeaponState == EWeaponState::PUNCH)
	{
		Punching();
	}
}

void ASurvivorCharacter::ToAim()
{
	Camera->SetRelativeLocation(FVector(450.0f, 0.0f, 50.0f));
}

void ASurvivorCharacter::EndAim()
{
	Camera->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
}

void ASurvivorCharacter::Reload()
{
	OnEventReload();
}

