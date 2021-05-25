// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyThreadCharacter.h"
#include "MyThreadProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "ThreadManage.h"
#include "Windows/WindowsPlatformThread.h"


DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

// struct FMyStruct;
// struct FMyStructSP;
FCriticalSection	Mutex;
TArray<FCoroutinesHandle> CoroutinesHandle;
TArray<FGraphEventRef> ArrayEventRef;
TArray<FThreadHandle>	ThreadHandle; //用于测试Bind线程

void ThreadP(const FString Mes)
{
	{
		FScopeLock ScopeLock(&Mutex);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, *Mes);
		}
	}
	FPlatformProcess::Sleep(1.f);//测试Windows线程时使用
}

void AMyThreadCharacter::T1(int32 i)
{
	ThreadP(FString::Printf(TEXT("T1 : %i"), i));
}

void AMyThreadCharacter::T2(int32 i, FString Mes)
{
	ThreadP(FString::Printf(TEXT("T1 : %i, Mes = %s"), i, *Mes));
}


void AMyThreadCharacter::Do()
{
	if (TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	}
	//	同步：FThreadProxyManage方法
	// 	for (auto& temp : ThreadHandle)
	// 	{
	// 		GThread::GetProxy().Join(temp);
	// 	}

	//	异步：FThreadProxyManage方法
	// 	for (auto& temp : ThreadHandle)
	// 	{
	// 		GThread::GetProxy().Detach(temp);
	// 	}

	//	唤醒Create协程
	for (auto& temp : CoroutinesHandle)
	{
		if (temp.IsValid())
		{
			temp.Pin()->Awaken();
		}
	}

}

struct FMyStruct
{
	void Hello(FString Mes)
	{
		ThreadP(FString::Printf(TEXT("FMyStruct::Hello : %s"), *Mes));
	}
};

struct FMyStructSP :public TSharedFromThis<FMyStructSP>
{
	void HelloSP(FString Mes)
	{
		ThreadP(FString::Printf(TEXT("FMyStructSP::Hello : %s"), *Mes));
	}
};



FMyStruct	MyStruct1;
TSharedPtr<FMyStructSP> MyStructSP1 = MakeShareable(new FMyStructSP);


void AMyThreadCharacter::Run()
{

	if (0)
	{
		GThread::GetGraph().CreateUObject(this, &AMyThreadCharacter::T1, 777);
		GThread::GetGraph().CreateRaw(&MyStruct1, &FMyStruct::Hello, FString("Hello~"));
		GThread::GetGraph().CreateSP(MyStructSP1.ToSharedRef(), &FMyStructSP::HelloSP, FString("HelloSP~"));
		GThread::GetGraph().CreateUFunction(this, TEXT("T2"), 123, FString("T22222"));
		GThread::GetGraph().CreateLambda([](FString Mes)
			{
				ThreadP(Mes);
			}, "Lambda");
	}


	if (0)
	{
		ArrayEventRef.SetNum(5);
		ArrayEventRef[0] = GThread::GetGraph().BindUObject(this, &AMyThreadCharacter::T1, 777);
// 		ArrayEventRef[1] = GThread::GetGraph().BindRaw(&MyStruct1, &FMyStruct::Hello, FString("Hello~"));
// 		ArrayEventRef[2] = GThread::GetGraph().BindSP(MyStructSP1.ToSharedRef(), &FMyStructSP::HelloSP, FString("HelloSP~"));
// 		ArrayEventRef[3] = GThread::GetGraph().BindUFunction(this, TEXT("T2"), 123, FString("T22222"));
// 		ArrayEventRef[4] = GThread::GetGraph().BindLambda([](FString Mes)
// 			{
// 				ThreadP(Mes);
// 			}, "Lambda");

		//GThread::GetGraph().Wait(ArrayEventRef[0]);

		FGraphEventArray ArrayEvent;
		for (auto &Tmp : ArrayEventRef)
		{
			ArrayEvent.Add(Tmp);
		}

		GThread::GetGraph().Wait(ArrayEvent); //阻塞主线程等待所有的完成后才执行。
		ThreadP("Wait-oK");
	}
	return;

	if(0)
	{
		ArrayEventRef.SetNum(5);
		CALL_THREAD_UOBJECT(ArrayEventRef[0], NULL, ENamedThreads::AnyThread, this, &AMyThreadCharacter::T1, 777);
		CALL_THREAD_Raw(ArrayEventRef[1], ArrayEventRef[0], ENamedThreads::AnyThread, &MyStruct1, &FMyStruct::Hello, FString("Hello~"));//等待[0]线程执行完毕后执行	
// 		CALL_THREAD_SP(ArrayEventRef[2], NULL, ENamedThreads::AnyThread, MyStructSP1.ToSharedRef(), &FMyStructSP::HelloSP, FString("HelloSP~"));
// 		CALL_THREAD_UFunction(ArrayEventRef[3], NULL, ENamedThreads::AnyThread, this, TEXT("T2"), 123, FString("T22222"));
// 		CALL_THREAD_Lambda(ArrayEventRef[4], NULL, ENamedThreads::AnyThread, [](FString Mes)
// 			{
// 				ThreadP(Mes);
// 			}, "Lambda");
	}
}

void AMyThreadCharacter::OK()
{

}




//////////////////////////////////////////////////////////////////////////
// AMyThreadCharacter

AMyThreadCharacter::AMyThreadCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}



TSharedPtr<FStreamableHandle> StreamableHandle;

void AMyThreadCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}

	{

		auto La = [](TSharedPtr<FStreamableHandle> *InHandle)
		{
			TArray<UObject *> ExampleObject;
			(*InHandle)->GetLoadedAssets(ExampleObject);

			for (UObject *Tmp : ExampleObject)
			{
				ThreadP(Tmp->GetName()); //由于其它线程调用打印会阻塞
			}
		};

		//异步 1.存储路径 2.异步加载完路径的资源后进行代理。
		//【注意】: 此处传入的是智能指针的地址，此时的智能指针指向是空的，等异步执行完后返回完句柄给StreamableHandle后，才进行执行这个Lambda
		StreamableHandle = GThread::GetResourceLoading() >> ObjectPath >> FSimpleDelegate::CreateLambda(La, &StreamableHandle);

		//StreamableHandle = GThread::GetResourceLoading().CreateLambda(La, &StreamableHandle);

		//同步
	   //////////////////////////////////////////////////////////////////////////
// 		StreamableHandle = GThread::GetResourceLoading() << ObjectPath;
// 		La(&StreamableHandle);
	}
	return;


	{//原生C++
		GThread::GetGraph();
		FWindowsPlatformThread::RunDelegate.BindUObject(this, &AMyThreadCharacter::Run);
		FWindowsPlatformThread::CompletedDelegate.BindUObject(this, &AMyThreadCharacter::OK);
		FWindowsPlatformThread::Show();
	}
	return;

	{
		CoroutinesHandle.SetNum(5);
		CoroutinesHandle[0] = GThread::GetCoroutines().CreateUObject(this, &AMyThreadCharacter::T1, 777);
		CoroutinesHandle[1] = GThread::GetCoroutines().CreateRaw(&MyStruct1, &FMyStruct::Hello, FString("Hello~"));
		CoroutinesHandle[2] = GThread::GetCoroutines().CreateSP(MyStructSP1.ToSharedRef(), &FMyStructSP::HelloSP, FString("HelloSP~"));
		CoroutinesHandle[3] = GThread::GetCoroutines().CreateUFunction(this, TEXT("T2"), 123, FString("T22222"));
		CoroutinesHandle[4] = GThread::GetCoroutines().CreateLambda([](FString Mes)
			{
				ThreadP(Mes);
			}, "Lambda");

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMyThreadCharacter::Do, 5.f);
	}
	return;

	{
		GThread::GetCoroutines().BindUObject(1.f, this, &AMyThreadCharacter::T1, 777);
		GThread::GetCoroutines().BindRaw(2.f, &MyStruct1, &FMyStruct::Hello, FString("Hello~"));
		GThread::GetCoroutines().BindSP(2.4f, MyStructSP1.ToSharedRef(), &FMyStructSP::HelloSP, FString("HelloSP~"));
		GThread::GetCoroutines().BindUFunction(4.f, this, TEXT("T2"), 123, FString("T22222"));
		GThread::GetCoroutines().BindLambda(7.f, [](FString Mes)
			{
				ThreadP(Mes);
			}, "Lambda");
	}
	return;

	{
		SYNCTASK_UOBJECT(this, &AMyThreadCharacter::T1, 777);
		SYNCTASK_Raw(&MyStruct1, &FMyStruct::Hello, FString("Hello~"));
		SYNCTASK_SP(MyStructSP1.ToSharedRef(), &FMyStructSP::HelloSP, FString("HelloSP~"));
		SYNCTASK_UFunction(this, TEXT("T2"), 123, FString("T22222"));
		SYNCTASK_Lambda([](FString Mes)
			{
				ThreadP(Mes);
			}, "Lambda");
	}
	return;

	{
		ASYNCTASK_UOBJECT(this, &AMyThreadCharacter::T1, 777);
		ASYNCTASK_Raw(&MyStruct1, &FMyStruct::Hello, FString("Hello~"));
		ASYNCTASK_SP(MyStructSP1.ToSharedRef(), &FMyStructSP::HelloSP, FString("HelloSP~"));
		ASYNCTASK_UFunction(this, TEXT("T2"), 123, FString("T22222"));
		ASYNCTASK_Lambda([](FString Mes)
			{
				ThreadP(Mes);
			}, "Lambda");
	}
	return;

	{
		GThread::GetAbandonable().BindUObject(this, &AMyThreadCharacter::T1, 777);
		GThread::GetAbandonable().BindRaw(&MyStruct1, &FMyStruct::Hello, FString("Hello~"));
		GThread::GetAbandonable().BindSP(MyStructSP1.ToSharedRef(), &FMyStructSP::HelloSP, FString("HelloSP~"));
		GThread::GetAbandonable().BindUFunction(this, TEXT("T2"), 123, FString("T22222"));
		GThread::GetAbandonable().BindLambda([](FString Mes)
			{
				ThreadP(Mes);
			}, "Lambda");
	}
	return;

// 	{
// 		GThread::GetAbandonable().CreateUObject(this, &AMyThreadCharacter::T1, 777);
// 		GThread::GetAbandonable().CreateRaw(&MyStruct1, &FMyStruct::Hello, FString("Hello~"));
// 		GThread::GetAbandonable().CreateSP(MyStructSP1.ToSharedRef(), &FMyStructSP::HelloSP, FString("HelloSP~"));
// 		GThread::GetAbandonable().CreateUFunction(this, TEXT("T2"), 123, FString("T22222"));
// 		GThread::GetAbandonable().CreateLambda([](FString Mes)
// 			{
// 				ThreadP(Mes);
// 			}, "Lambda");
// 	}
// 	retur

	{
		GThread::GetTask().CreateUObject(this, &AMyThreadCharacter::T1, 777);
		GThread::GetTask().CreateRaw(&MyStruct1, &FMyStruct::Hello, FString("Hello~"));
		GThread::GetTask().CreateSP(MyStructSP1.ToSharedRef(), &FMyStructSP::HelloSP, FString("HelloSP~"));
		GThread::GetTask().CreateUFunction(this, TEXT("T2"), 123, FString("T22222"));
		GThread::GetTask().CreateLambda([](FString Mes)
			{
				ThreadP(Mes);
			}, "Lambda");
	}
	return;

	{
		GThread::GetTask().BindUObject(this, &AMyThreadCharacter::T1, 777);
		GThread::GetTask().BindRaw(&MyStruct1, &FMyStruct::Hello, FString("Hello~"));
		GThread::GetTask().BindSP(MyStructSP1.ToSharedRef(), &FMyStructSP::HelloSP, FString("HelloSP~"));
		GThread::GetTask().BindUFunction(this, TEXT("T2"), 123, FString("T22222"));
		GThread::GetTask().BindLambda([](FString Mes)
			{
				ThreadP(Mes);
			}, "Lambda");
	}
	return;



	//测试我们的Bind线程
	{
		ThreadHandle.SetNum(5);
		FMyStruct MyStruct;
		TSharedPtr<FMyStructSP> MyStructSP = MakeShareable(new FMyStructSP);

		ThreadHandle[0] = GThread::GetProxy().BindUObject(this, &AMyThreadCharacter::T1, 777);
		ThreadHandle[1] = GThread::GetProxy().BindRaw(&MyStruct, &FMyStruct::Hello, FString("Hello~"));
		ThreadHandle[2] = GThread::GetProxy().BindSP(MyStructSP.ToSharedRef(), &FMyStructSP::HelloSP, FString("HelloSP~"));
		ThreadHandle[3] = GThread::GetProxy().BindUFunction(this, TEXT("T2"), 123, FString("T22222"));
		ThreadHandle[4] = GThread::GetProxy().BindLambda([](FString Mes)
			{
				ThreadP(Mes);
			}, "Lambda");

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMyThreadCharacter::Do, 3.f);
	}
	return;

	//测试我们的Create线程
	{
		FMyStruct MyStruct;
		TSharedPtr<FMyStructSP> MyStructSP = MakeShareable(new FMyStructSP);
		  
		GThread::GetProxy().CreateUObject(this, &AMyThreadCharacter::T1, 777);
		GThread::GetProxy().CreateRaw(&MyStruct, &FMyStruct::Hello, FString("Hello~"));
		GThread::GetProxy().CreateSP(MyStructSP.ToSharedRef(), &FMyStructSP::HelloSP, FString("HelloSP~"));
		GThread::GetProxy().CreateUFunction(this, TEXT("T2"), 123, FString("T22222"));
		GThread::GetProxy().CreateLambda([](FString Mes)
			{
				ThreadP(Mes);
			}, "Lambda");

	}


}

void AMyThreadCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GThread::Destroy();
}


//////////////////////////////////////////////////////////////////////////
// Input

void AMyThreadCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyThreadCharacter::OnFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMyThreadCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyThreadCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyThreadCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyThreadCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyThreadCharacter::LookUpAtRate);
}

void AMyThreadCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<AMyThreadProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<AMyThreadProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AMyThreadCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMyThreadCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AMyThreadCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AMyThreadCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AMyThreadCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMyThreadCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMyThreadCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyThreadCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AMyThreadCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AMyThreadCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AMyThreadCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AMyThreadCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}
