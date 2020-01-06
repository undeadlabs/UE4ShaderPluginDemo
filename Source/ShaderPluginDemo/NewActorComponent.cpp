// Fill out your copyright notice in the Description page of Project Settings.


#include "NewActorComponent.h"

// Sets default values for this component's properties
UNewActorComponent::UNewActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	BufferSize = -1;
	// ...
}


// Called when the game starts
void UNewActorComponent::BeginPlay()
{
	Super::BeginPlay();
	for (int i = 0; i < 128; i++)
	{
		this->AddInstance(FTransform(FVector(0, 0, 0)));
	}
	// ...
	
}


// Called every frame
void UNewActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	if (RenderTarget != NULL)
	{
		if (BufferSize < 0)
		{
			BufferSize = RenderTarget->GetSurfaceWidth();
		}

		// this is a blocking read, so it's slow.  It pushes the render thread over 30ms.
		// I tried a more selective read but that is actually slower -- i assume the
		// real killer is the actual thread boundary...

		FTextureRenderTarget2DResource* textureResource = (FTextureRenderTarget2DResource*)RenderTarget->Resource;
		if (textureResource->ReadPixels(ColorBuffer))
		{
			for (int i = 0; i < this->GetInstanceCount(); i++)
			{
				int pixel = (i * BufferSize) + i;
				FColor thisColor = ColorBuffer[pixel];

				this->UpdateInstanceTransform(
					i,
					FTransform(
						FRotator::ZeroRotator,
						FVector(thisColor.R, thisColor.G, thisColor.B),
						FVector(this->InstanceScale, this->InstanceScale, this->InstanceScale)
						),
					false,
					true,
					true
				);
			}
		}
	}
}
