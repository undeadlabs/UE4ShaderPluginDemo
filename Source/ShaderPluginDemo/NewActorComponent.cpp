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


		//ReadPixels();
		//ReadPixelFence.BeginFence();
		//bReadPixelsStarted = true;
		//...
		//	// To check if we are done reading: 
		//	// I do this in my tick function
		//	if (bReadPixelsStarted && ReadPixelFence.IsFenceComplete())
		//	{
		//		// do something with the pixels
		//	}


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


void UNewActorComponent::ReadPixels()
{
	//borrowed from RenderTarget::ReadPixels()
	FTextureRenderTarget2DResource* RenderResource = (FTextureRenderTarget2DResource*)RenderTarget->Resource;

	// Read the render target surface data back.	
	struct FReadSurfaceContext
	{
		FRenderTarget* SrcRenderTarget;
		TArray<FColor>* OutData;
		FIntRect Rect;
		FReadSurfaceDataFlags Flags;
	};

	ColorBuffer.Reset();
	FReadSurfaceContext ReadSurfaceContext =
	{
		RenderResource,
		&ColorBuffer,
		FIntRect(0, 0, RenderResource->GetSizeXY().X, RenderResource->GetSizeXY().Y),
		FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX)
	};

	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
		ReadSurfaceCommand,
		FReadSurfaceContext, Context, ReadSurfaceContext,
		{
			RHICmdList.ReadSurfaceData(
			Context.SrcRenderTarget->GetRenderTargetTexture(),
				Context.Rect,
				*Context.OutData,
				Context.Flags
				);
		});
}