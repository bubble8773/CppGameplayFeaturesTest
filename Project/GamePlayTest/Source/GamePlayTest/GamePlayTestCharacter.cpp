// Copyright Epic Games, Inc. All Rights Reserved.

#include "GamePlayTestCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet\KismetSystemLibrary.h"
#include "Engine/World.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/ActorComponent.h"

//#include"IInteractWithObjects.h"


//////////////////////////////////////////////////////////////////////////
// AGamePlayTestCharacter

AGamePlayTestCharacter::AGamePlayTestCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	//PhysicsHandle->SetupAttachment(GetRootComponent());
}

void AGamePlayTestCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			//adding double jump
			JumpMaxCount = MaxJumps;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Grab and throw
void AGamePlayTestCharacter::GrabObject( bool ToggleDebug, float Range)
{
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = GetActorLocation() + GetActorForwardVector() * Range;
	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("Trace")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(HitResult, TraceStart, TraceEnd, FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TraceParams);
	if(ToggleDebug)
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, HitResult.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 10.0f);
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	if (ComponentToGrab)
	{
		PhysicsHandle->GrabComponent(ComponentToGrab, NAME_None, ComponentToGrab->GetOwner()->GetActorLocation(), true);
		bIsGrabbing = true;
	}
}

void AGamePlayTestCharacter::ThrowObject()
{
	if (bIsGrabbing) {
		PhysicsHandle->ReleaseComponent();
		bIsGrabbing = false;
	}
}

void AGamePlayTestCharacter::GrabbingLoop( float Range)
{
	FVector TraceEnd = GetActorLocation() + GetActorForwardVector() * Range;
	if (bIsGrabbing) {
		PhysicsHandle->SetTargetLocation(TraceEnd);

	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGamePlayTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGamePlayTestCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGamePlayTestCharacter::Look);

		//Interacting 
		EnhancedInputComponent->BindAction(Interact, ETriggerEvent::Triggered, this, &AGamePlayTestCharacter::BeginInteract);
	}

}

void AGamePlayTestCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AGamePlayTestCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
	
}

//void AGamePlayTestCharacter::BeginInteract()
//{
//	// Set what actors to seek out from it's collision channel
//	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
//	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
//
//	// Ignore any specific actors
//	TArray<AActor*> ignoreActors;
//	// Ignore self or remove this line to not ignore any
//	ignoreActors.Init(this, 1);
//
//	// Array of actors that are inside the radius of the sphere
//	TArray<AActor*> outActors;
//
//	// Total radius of the sphere
//	float radius = 750.0f;
//	// Sphere's spawn loccation within the world
//	FVector sphereSpawnLocation = GetActorLocation();
//	// Class that the sphere should hit against and include in the outActors array (Can be null)
//	UClass* seekClass =  NULL;
//	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), sphereSpawnLocation, radius, traceObjectTypes, seekClass, ignoreActors, outActors);
//
//	for (AActor* overlappedActor : outActors) {
//		//UKismetSystemLibrary::DoesImplementInterface(overlappedActor,IIInteractWithObjects );
//		overlappedActor->Interact()
//	}
//}




