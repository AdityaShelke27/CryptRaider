// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
	{
		UE_LOG(LogTemp, Display, TEXT("%s"), *PhysicsHandle->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Nothing Here"));
	}
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle)
		return;
	
	FVector End = GetComponentLocation() + GetForwardVector() * GrabDistance;
	PhysicsHandle->SetTargetLocationAndRotation(End, GetComponentRotation());
}

void UGrabber::Grab()
{
	FHitResult hit;
	bool HasHit = GetGrabbableInReach(hit);

	if (!PhysicsHandle)
		return;
	if (!HasHit)
		return;

	UPrimitiveComponent* primitiveComponent = hit.GetComponent();
	hit.GetActor()->Tags.Add("Grabbed");
	primitiveComponent->WakeAllRigidBodies();
	PhysicsHandle->GrabComponentAtLocationWithRotation(primitiveComponent, NAME_None, hit.ImpactPoint, GetComponentRotation());
}
bool UGrabber::GetGrabbableInReach(FHitResult& OutHit)
{
	FVector Start = GetComponentLocation();
	FVector End = Start + GetForwardVector() * MaxDistance;

	return GetWorld()->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel2, FCollisionShape::MakeSphere(GrabRadius));
}
void UGrabber::Release()
{
	if (!PhysicsHandle)
		return;
	if (!PhysicsHandle->GetGrabbedComponent())
		return;
	PhysicsHandle->GetGrabbedComponent()->GetOwner()->Tags.Remove("Grabbed");
	PhysicsHandle->ReleaseComponent();
}
