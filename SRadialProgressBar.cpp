// The code has been based upon the SImage.cpp file developed by Epic Games Inc.

#include "SRadialProgressBar.h"
#include "LayerImageAsset.h"
#include "Rendering/DrawElements.h"
#include "Widgets/IToolTip.h"

#if WITH_ACCESSIBILITY
#include "Widgets/Accessibility/SlateCoreAccessibleWidgets.h"
#endif


SLATE_IMPLEMENT_WIDGET(SRadialProgressBar)
void SRadialProgressBar::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "Image", ImageAttribute, EInvalidateWidgetReason::Layout);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "ColorAndOpacity", ColorAndOpacityAttribute, EInvalidateWidgetReason::Paint);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "DesiredSizeOverride", DesiredSizeOverrideAttribute, EInvalidateWidgetReason::Layout);
  SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "ProgressPercentage", ProgressPercentageAttribute, EInvalidateWidgetReason::None);
}

SRadialProgressBar::SRadialProgressBar()
	: ImageAttribute(*this)
	, ColorAndOpacityAttribute(*this)
	, DesiredSizeOverrideAttribute(*this)
  , ProgressPercentageAttribute(*this)
{
	SetCanTick(false);
	bCanSupportFocus = false;
}

void SRadialProgressBar::Construct( const FArguments& InArgs )
{
	ImageAttribute.Assign(*this, InArgs._Image);
	ColorAndOpacityAttribute.Assign(*this, InArgs._ColorAndOpacity);
  ProgressPercentageAttribute.Assign(*this, InArgs._ProgressPercentage);
	bFlipForRightToLeftFlowDirection = InArgs._FlipForRightToLeftFlowDirection;
	LayerImageAssetClass = InArgs._LayerImageAssetClass;
	BarTexture = InArgs._BarTexture;

	DesiredSizeOverrideAttribute.Assign(*this, InArgs._DesiredSizeOverride);

	if (InArgs._OnMouseButtonDown.IsBound())
	{
		SetOnMouseButtonDown(InArgs._OnMouseButtonDown);
	}
}

// Contents of the widget
int32 SRadialProgressBar::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const
{

	const ULayerImageAsset* LayerAsset = GetDefault<ULayerImageAsset>(LayerImageAssetClass);
	ESlateDrawEffect DrawEffects = ESlateDrawEffect::NoPixelSnapping;

    for (const FSlateBrush& ImageBrush : LayerAsset->Layers){

       TArray<FSlateVertex> Vertices;
       TArray<SlateIndex> Indices;

       FVector2D CenterPoint = AllottedGeometry.GetLocalSize() / 2.0f;
	     const FVector2f fCenterPoint = FVector2f(CenterPoint);

       float RadiusInner = 32.0f; // Radius Inner circle
	     float RadiusOuter = 16.0f; // Radius outer circle

	     const FVector2f TexCoord = {0, 0};
	     const FColor ColorInnerCircle = FColor::White;
	     const FColor ColorOuterCircle = FColor::Red;
       const FColor ColorInnerCircleCenter = FColor::Transparent;

       constexpr int BASE_INDEX = 1; // Needed for the Centre vertex
       constexpr int Resolution = 16; // Number of vertices for each circle

	     const float ProgressPercentageSafe = FMath::Clamp(ProgressPercentageAttribute.Get(), 0.0f, 100.0f);
	     const int NumberOfQuadsToOutput = FMath::Floor(ProgressPercentageSafe / 100.0f * (Resolution-1));
      

       // Add a vertice in the centre of the circle
       Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(AllottedGeometry.GetAccumulatedRenderTransform(), fCenterPoint, TexCoord, TexCoord, ColorInnerCircleCenter));

       // Generate vertices for the inner circle
       for (int i = 0; i < Resolution; ++i)
       {
         float Angle = i / (float)Resolution * 2 * UE_PI;
         float X = CenterPoint.X + FMath::Cos(Angle) * RadiusInner;
         float Y = CenterPoint.Y + FMath::Sin(Angle) * RadiusInner;
         const FVector2f Position = FVector2f(X, Y);
         Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(AllottedGeometry.GetAccumulatedRenderTransform(), Position, TexCoord, TexCoord, ColorInnerCircle));
       }
       // Generate vertices for the outer circle
       for (int i = 0; i < Resolution; ++i)
       {
         float Angle = i / (float)Resolution * 2 * UE_PI;
         float X = (CenterPoint.X + FMath::Cos(Angle) * RadiusInner) + FMath::Cos(Angle) * RadiusOuter;
         float Y = (CenterPoint.Y + FMath::Sin(Angle) * RadiusInner) + FMath::Sin(Angle) * RadiusOuter;
         const FVector2f Position = FVector2f(X, Y);
         Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(AllottedGeometry.GetAccumulatedRenderTransform(), Position, TexCoord, TexCoord, ColorOuterCircle));
       }


       // Generate indices
    for(int CurrentQuadIndex = 0; CurrentQuadIndex < NumberOfQuadsToOutput; ++CurrentQuadIndex)
    {
        if(CurrentQuadIndex == Resolution-1) break; // Never render the last quad
        
        int BottomLeftIndex = CurrentQuadIndex + BASE_INDEX;
        int BottomRightIndex = CurrentQuadIndex + 1 + BASE_INDEX;
        int TopLeftIndex = BottomLeftIndex + Resolution;
        int TopRightIndex = BottomRightIndex + Resolution;
        
        Indices.Add(BottomLeftIndex); Indices.Add(TopLeftIndex); Indices.Add(BottomRightIndex);
        Indices.Add(TopLeftIndex); Indices.Add(TopRightIndex); Indices.Add(BottomRightIndex);
    }

    // Handle the last quad
    {
        int BottomLeftIndex = FMath::Min(NumberOfQuadsToOutput, Resolution-BASE_INDEX) + BASE_INDEX;
        int TopLeftIndex = BottomLeftIndex + Resolution;
        int BottomRightIndex = Vertices.Num();
        int TopRightIndex = Vertices.Num()+1;
                
        Indices.Add(BottomLeftIndex); Indices.Add(TopLeftIndex); Indices.Add(BottomRightIndex);
        Indices.Add(TopLeftIndex); Indices.Add(TopRightIndex); Indices.Add(BottomRightIndex);

        float Angle = ProgressPercentageSafe / 100.0f * 2 * UE_PI;
        float X1 = CenterPoint.X + FMath::Cos(Angle) * RadiusInner;
        float Y1 = CenterPoint.Y + FMath::Sin(Angle) * RadiusInner;
        float X2 = (CenterPoint.X + FMath::Cos(Angle) * RadiusInner) + FMath::Cos(Angle) * RadiusOuter;
        float Y2 = (CenterPoint.Y + FMath::Sin(Angle) * RadiusInner) + FMath::Sin(Angle) * RadiusOuter;
        const FVector2f P1 = FVector2f(X1, Y1);
        const FVector2f P2 = FVector2f(X2, Y2);
        Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(AllottedGeometry.GetAccumulatedRenderTransform(), P1, TexCoord, TexCoord, ColorInnerCircle));
        Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(AllottedGeometry.GetAccumulatedRenderTransform(), P2, TexCoord, TexCoord, ColorOuterCircle));
    }

     // Actually draw the vertices on screen
     
	 if(BarTexture){
		    FSlateBrush RadialProgressBarBrush;
        RadialProgressBarBrush.SetResourceObject(BarTexture);
        RadialProgressBarBrush.DrawAs = ESlateBrushDrawType::Image;

	     const FSlateResourceHandle& CircularTextureResource = FSlateApplication::Get().GetRenderer()->GetResourceHandle(MyCircularProgressBarBrush);

		FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId, CircularTextureResource, Vertices, Indices, nullptr, 0, 0);
	 };


}
return LayerId;
}

FVector2D SRadialProgressBar::ComputeDesiredSize( float ) const
{
	const FSlateBrush* ImageBrush = ImageAttribute.Get();
	if (ImageBrush != nullptr)
	{
		const TOptional<FVector2D>& CurrentSizeOverride = DesiredSizeOverrideAttribute.Get();

		return CurrentSizeOverride.IsSet() ? CurrentSizeOverride.GetValue() : ImageBrush->ImageSize;
	}
	return FVector2D::ZeroVector;
}

void SRadialProgressBar::SetColorAndOpacity(TAttribute<FSlateColor> InColorAndOpacity )
{
	ColorAndOpacityAttribute.Assign(*this, MoveTemp(InColorAndOpacity));
}

void SRadialProgressBar::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
	ColorAndOpacityAttribute.Set(*this, InColorAndOpacity);
}

void SRadialProgressBar::SetImage(TAttribute<const FSlateBrush*> InImage)
{
	ImageAttribute.Assign(*this, MoveTemp(InImage));
}

void SRadialProgressBar::SetDesiredSizeOverride(TAttribute<TOptional<FVector2D>> InDesiredSizeOverride)
{
	DesiredSizeOverrideAttribute.Assign(*this, MoveTemp(InDesiredSizeOverride));
}

void SRadialProgressBar::SetProgressPercentage(TAttribute<float> InProgressPercentage )
{
	ProgressPercentageAttribute.Assign(*this, MoveTemp(InProgressPercentage));
}

void SRadialProgressBar::SetProgressPercentage(float InProgressPercentage )
{
	ProgressPercentageAttribute.Set(*this, InProgressPercentage);
}


#if WITH_ACCESSIBILITY
TSharedRef<FSlateAccessibleWidget> SRadialProgressBar::CreateAccessibleWidget()
{
	return MakeShareable<FSlateAccessibleWidget>(new FSlateAccessibleImage(SharedThis(this)));
}
#endif
