// The code has been based upon the Image.cpp file developed by Epic Games Inc.

#include "RadialProgressBar.h"
#include "Slate/SlateBrushAsset.h"
#include "Materials/MaterialInterface.h"
#include "Engine/Texture2D.h"
#include "Engine/Texture2DDynamic.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SRadialProgressBar.h"
#include "LayerImageAsset.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RadialProgressBar)

#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
// URadialProgressBar

URadialProgressBar::URadialProgressBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ColorAndOpacity(FLinearColor::White)
	, ProgressPercentage(0.0f)
{
}

void URadialProgressBar::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyImage.Reset();
}

TSharedRef<SWidget> URadialProgressBar::RebuildWidget()
{
	MyImage = SNew(SRadialProgressBar)
			.FlipForRightToLeftFlowDirection(bFlipForRightToLeftFlowDirection)
			.LayerImageAssetClass(LayerImageAssetClass)
			.ProgressPercentage(ProgressPercentage)
			.BarTexture(BarTexture);

	return MyImage.ToSharedRef();
}

void URadialProgressBar::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	TAttribute<FSlateColor> ColorAndOpacityBinding = PROPERTY_BINDING(FSlateColor, ColorAndOpacity);
	TAttribute<float> ProgressPercentageBinding = PROPERTY_BINDING(float, ProgressPercentage);
	TAttribute<const FSlateBrush*> ImageBinding = OPTIONAL_BINDING_CONVERT(FSlateBrush, Brush, const FSlateBrush*, ConvertImage);

	if (MyImage.IsValid())
	{
		MyImage->SetImage(ImageBinding);
		MyImage->SetColorAndOpacity(ColorAndOpacityBinding);
		MyImage->SetProgressPercentage(ProgressPercentageBinding);
		MyImage->SetOnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonDown));
	}
}

void URadialProgressBar::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
	ColorAndOpacity = InColorAndOpacity;
	if ( MyImage.IsValid() )
	{
		MyImage->SetColorAndOpacity(ColorAndOpacity);
	}
}

void URadialProgressBar::SetProgressPercentage(float InProgressPercentage)
{
	ProgressPercentage = InProgressPercentage;
	if ( MyImage.IsValid() )
	{
		MyImage->SetProgressPercentage(ProgressPercentage);
	}
}

void URadialProgressBar::SetOpacity(float InOpacity)
{
	ColorAndOpacity.A = InOpacity;
	if ( MyImage.IsValid() )
	{
		MyImage->SetColorAndOpacity(ColorAndOpacity);
	}
}

const FSlateBrush* URadialProgressBar::ConvertImage(TAttribute<FSlateBrush> InImageAsset) const
{
	URadialProgressBar* MutableThis = const_cast<URadialProgressBar*>( this );
	MutableThis->Brush = InImageAsset.Get();

	return &Brush;
}

void URadialProgressBar::SetBrush(const FSlateBrush& InBrush)
{
	if(Brush != InBrush)
	{
		Brush = InBrush;

		if (MyImage.IsValid())
		{
			MyImage->SetImage(&Brush);
		}
	}
}

void URadialProgressBar::SetBrushSize(FVector2D DesiredSize)
{
	SetDesiredSizeOverride(DesiredSize);
}

void URadialProgressBar::SetDesiredSizeOverride(FVector2D DesiredSize)
{
	if (MyImage.IsValid())
	{
		MyImage->SetDesiredSizeOverride(DesiredSize);
	}
}

void URadialProgressBar::SetBrushTintColor(FSlateColor TintColor)
{
	if(Brush.TintColor != TintColor)
	{
		Brush.TintColor = TintColor;

		if (MyImage.IsValid())
		{
			MyImage->SetImage(&Brush);
		}
	}
}

void URadialProgressBar::SetBrushResourceObject(UObject* ResourceObject)
{
	if (Brush.GetResourceObject() != ResourceObject)
	{
		Brush.SetResourceObject(ResourceObject);

		if (MyImage.IsValid())
		{
			MyImage->SetImage(&Brush);
		}
	}
}

void URadialProgressBar::SetBrushFromAsset(USlateBrushAsset* Asset)
{
	if(!Asset || Brush != Asset->Brush)
	{
		CancelImageStreaming();
		Brush = Asset ? Asset->Brush : FSlateBrush();

		if (MyImage.IsValid())
		{
			MyImage->SetImage(&Brush);
		}
	}
}

void URadialProgressBar::SetBrushFromTexture(UTexture2D* Texture, bool bMatchSize)
{
	CancelImageStreaming();

	if(Brush.GetResourceObject() != Texture)
	{
		Brush.SetResourceObject(Texture);

		if (Texture) // Since this texture is used as UI, don't allow it affected by budget.
		{
			Texture->bForceMiplevelsToBeResident = true;
			Texture->bIgnoreStreamingMipBias = true;
		}

		if (bMatchSize)
		{
			if (Texture)
			{
				Brush.ImageSize.X = Texture->GetSizeX();
				Brush.ImageSize.Y = Texture->GetSizeY();
			}
			else
			{
				Brush.ImageSize = FVector2D(0, 0);
			}
		}

		if (MyImage.IsValid())
		{
			MyImage->SetImage(&Brush);
		}
	}
}

void URadialProgressBar::SetBrushFromAtlasInterface(TScriptInterface<ISlateTextureAtlasInterface> AtlasRegion, bool bMatchSize)
{
	if(Brush.GetResourceObject() != AtlasRegion.GetObject())
	{
		CancelImageStreaming();
		Brush.SetResourceObject(AtlasRegion.GetObject());

		if (bMatchSize)
		{
			if (AtlasRegion)
			{
				FSlateAtlasData AtlasData = AtlasRegion->GetSlateAtlasData();
				Brush.ImageSize = AtlasData.GetSourceDimensions();
			}
			else
			{
				Brush.ImageSize = FVector2D(0, 0);
			}
		}

		if (MyImage.IsValid())
		{
			MyImage->SetImage(&Brush);
		}
	}
}

void URadialProgressBar::SetBrushFromTextureDynamic(UTexture2DDynamic* Texture, bool bMatchSize)
{
	if(Brush.GetResourceObject() != Texture)
	{
		CancelImageStreaming();
		Brush.SetResourceObject(Texture);

		if (bMatchSize && Texture)
		{
			Brush.ImageSize.X = Texture->SizeX;
			Brush.ImageSize.Y = Texture->SizeY;
		}

		if (MyImage.IsValid())
		{
			MyImage->SetImage(&Brush);
		}
	}
}

void URadialProgressBar::SetBrushFromMaterial(UMaterialInterface* Material)
{
	if(Brush.GetResourceObject() != Material)
	{
		CancelImageStreaming();
		Brush.SetResourceObject(Material);

		//TODO UMG Check if the material can be used with the UI

		if (MyImage.IsValid())
		{
			MyImage->SetImage(&Brush);
		}
	}
}

void URadialProgressBar::CancelImageStreaming()
{
	if (StreamingHandle.IsValid())
	{
		StreamingHandle->CancelHandle();
		StreamingHandle.Reset();
	}

	StreamingObjectPath.Reset();
}

void URadialProgressBar::RequestAsyncLoad(TSoftObjectPtr<UObject> SoftObject, TFunction<void()>&& Callback)
{
	RequestAsyncLoad(SoftObject, FStreamableDelegate::CreateLambda(MoveTemp(Callback)));
}

void URadialProgressBar::RequestAsyncLoad(TSoftObjectPtr<UObject> SoftObject, FStreamableDelegate DelegateToCall)
{
	CancelImageStreaming();

	if (UObject* StrongObject = SoftObject.Get())
	{
		DelegateToCall.ExecuteIfBound();
		return;  // No streaming was needed, complete immediately.
	}

	OnImageStreamingStarted(SoftObject);

	TWeakObjectPtr<URadialProgressBar> WeakThis(this);
	StreamingObjectPath = SoftObject.ToSoftObjectPath();
	StreamingHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
		StreamingObjectPath,
		[WeakThis, DelegateToCall, SoftObject]() {
			if (URadialProgressBar* StrongThis = WeakThis.Get())
			{
				// If the object paths don't match, then this delegate was interrupted, but had already been queued for a callback
				// so ignore everything and abort.
				if (StrongThis->StreamingObjectPath != SoftObject.ToSoftObjectPath())
				{
					return; // Abort!
				}

				// Call the delegate to do whatever is needed, probably set the new image.
				DelegateToCall.ExecuteIfBound();

				// Note that the streaming has completed.
				StrongThis->OnImageStreamingComplete(SoftObject);
			}
		},
		FStreamableManager::AsyncLoadHighPriority);
}

void URadialProgressBar::OnImageStreamingStarted(TSoftObjectPtr<UObject> SoftObject)
{
	// No-Op
}

void URadialProgressBar::OnImageStreamingComplete(TSoftObjectPtr<UObject> LoadedSoftObject)
{
	// No-Op
}

void URadialProgressBar::SetBrushFromSoftTexture(TSoftObjectPtr<UTexture2D> SoftTexture, bool bMatchSize)
{
	TWeakObjectPtr<URadialProgressBar> WeakThis(this); // using weak ptr in case 'this' has gone out of scope by the time this lambda is called

	RequestAsyncLoad(SoftTexture,
		[WeakThis, SoftTexture, bMatchSize]() {
			if (URadialProgressBar* StrongThis = WeakThis.Get())
			{
				ensureMsgf(SoftTexture.Get(), TEXT("Failed to load %s"), *SoftTexture.ToSoftObjectPath().ToString());
				StrongThis->SetBrushFromTexture(SoftTexture.Get(), bMatchSize);
			}
		}
	);
}

void URadialProgressBar::SetBrushFromSoftMaterial(TSoftObjectPtr<UMaterialInterface> SoftMaterial)
{
	TWeakObjectPtr<URadialProgressBar> WeakThis(this); // using weak ptr in case 'this' has gone out of scope by the time this lambda is called

	RequestAsyncLoad(SoftMaterial,
		[WeakThis, SoftMaterial]() {
			if (URadialProgressBar* StrongThis = WeakThis.Get())
			{
				ensureMsgf(SoftMaterial.Get(), TEXT("Failed to load %s"), *SoftMaterial.ToSoftObjectPath().ToString());
				StrongThis->SetBrushFromMaterial(SoftMaterial.Get());
			}
		}
	);
}

UMaterialInstanceDynamic* URadialProgressBar::GetDynamicMaterial()
{
	UMaterialInterface* Material = NULL;

	UObject* Resource = Brush.GetResourceObject();
	Material = Cast<UMaterialInterface>(Resource);

	if ( Material )
	{
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);

		if ( !DynamicMaterial )
		{
			DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
			Brush.SetResourceObject(DynamicMaterial);

			if ( MyImage.IsValid() )
			{
				MyImage->SetImage(&Brush);
			}
		}
		
		return DynamicMaterial;
	}

	//TODO UMG can we do something for textures?  General purpose dynamic material for them?

	return NULL;
}

FReply URadialProgressBar::HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if ( OnMouseButtonDownEvent.IsBound() )
	{
		return OnMouseButtonDownEvent.Execute(Geometry, MouseEvent).NativeReply;
	}

	return FReply::Unhandled();
}

#if WITH_ACCESSIBILITY
TSharedPtr<SWidget> URadialProgressBar::GetAccessibleWidget() const
{
	return MyImage;
}
#endif

#if WITH_EDITOR

const FText URadialProgressBar::GetPaletteCategory()
{
	return LOCTEXT("Common", "Common");
}

#endif


/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

