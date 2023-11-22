// The code has been based upon the SImage.h file developed by Epic Games Inc.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateColor.h"
#include "Styling/CoreStyle.h"
#include "Widgets/SLeafWidget.h"
#include "Styling/SlateTypes.h"

class FPaintArgs;
class FSlateWindowElementList;

/**
 * Implements a widget that displays a progress bar with the desired height, width and progress percentage.
 */
class TANKMAYHEM_API SRadialProgressBar
	: public SLeafWidget
{
	SLATE_DECLARE_WIDGET(SRadialProgressBar, SLeafWidget)

public:
	SLATE_BEGIN_ARGS( SRadialProgressBar )
		: _Image( FCoreStyle::Get().GetDefaultBrush() )
		, _ColorAndOpacity( FLinearColor::White )
		, _ProgressPercentage(0.0f)
		, _FlipForRightToLeftFlowDirection( false )
		, _LayerImageAssetClass( nullptr )
		, _BarTexture( nullptr )
		{ }

		/** Image resource */
		SLATE_ATTRIBUTE(const FSlateBrush*, Image)

		/** Color and opacity */
		SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)

		/** When specified, ignore the brushes size and report the DesiredSizeOverride as the desired image size. */
		SLATE_ATTRIBUTE(TOptional<FVector2D>, DesiredSizeOverride)

        /** Progress Percentage */
		SLATE_ATTRIBUTE( float, ProgressPercentage )

		/** Flips the image if the localization's flow direction is RightToLeft */
		SLATE_ARGUMENT( bool, FlipForRightToLeftFlowDirection )
        
		/** Layer Image Asset Class */
		SLATE_ARGUMENT( TSubclassOf<class ULayerImageAsset>, LayerImageAssetClass )
        
		/** Progress Bar Texture */
		SLATE_ARGUMENT( UTexture2D*, BarTexture)

		/** Invoked when the mouse is pressed in the widget. */
		SLATE_EVENT(FPointerEventHandler, OnMouseButtonDown)
	SLATE_END_ARGS()

	/** Constructor */
	SRadialProgressBar();

	/**
	 * Construct this widget
	 *
	 * @param	InArgs	The declaration data for this widget
	 */
	void Construct( const FArguments& InArgs );

public:

	/** Set the ColorAndOpacity attribute */
	void SetColorAndOpacity(TAttribute<FSlateColor> InColorAndOpacity);

	/** See the ColorAndOpacity attribute */
	void SetColorAndOpacity( FLinearColor InColorAndOpacity );

	/** Set the Image attribute */
	void SetImage(TAttribute<const FSlateBrush*> InImage);

	/** Set SizeOverride attribute */
	void SetDesiredSizeOverride(TAttribute<TOptional<FVector2D>> InDesiredSizeOverride);

	/** Set the ProgressPercentage attribute */
	void SetProgressPercentage(TAttribute<float> InProgressPercentage);

	/** See the ProgressPercentage attribute */
	void SetProgressPercentage( float InProgressPercentage );

public:

	// SWidget overrides
	virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;
#if WITH_ACCESSIBILITY
	virtual TSharedRef<FSlateAccessibleWidget> CreateAccessibleWidget() override;
#endif

protected:
	// Begin SWidget overrides.
	virtual FVector2D ComputeDesiredSize(float) const override;
	// End SWidget overrides.

	/** @return an attribute reference of Image */
	TSlateAttributeRef<const FSlateBrush*> GetImageAttribute() const { return TSlateAttributeRef<FSlateBrush const*>(SharedThis(this), ImageAttribute); }

	/** @return an attribute reference of ColorAndOpacity */
	TSlateAttributeRef<FSlateColor> GetColorAndOpacityAttribute() const { return TSlateAttributeRef<FSlateColor>(SharedThis(this), ColorAndOpacityAttribute); }

	/** @return an attribute reference of DesiredSizeOverride */
	TSlateAttributeRef<TOptional<FVector2D>> GetDesiredSizeOverrideAttribute() const { return TSlateAttributeRef<TOptional<FVector2D>>(SharedThis(this), DesiredSizeOverrideAttribute); }

	/** @return an attribute reference of ProgressPercentage */
	TSlateAttributeRef<float> GetProgressPercentageAttribute() const { return TSlateAttributeRef<float>(SharedThis(this), ProgressPercentageAttribute); }

#if WITH_EDITORONLY_DATA
	UE_DEPRECATED(5.0, "Direct access to Image is now deprecated. Use the setter or getter.")
	FInvalidatableBrushAttribute Image;
	UE_DEPRECATED(5.0, "Direct access to ColorAndOpacity is now deprecated. Use the setter or getter.")
	TSlateDeprecatedTAttribute<FSlateColor> ColorAndOpacity;
	UE_DEPRECATED(5.0, "Direct access to DesiredSizeOverride is now deprecated. Use the setter or getter.")
	TSlateDeprecatedTAttribute<TOptional<FVector2D>> DesiredSizeOverride;
#endif

private:
	/** The slate brush to draw for the ImageAttribute that we can invalidate. */
	TSlateAttribute<const FSlateBrush*> ImageAttribute;

	/** Color and opacity scale for this ImageAttribute */
	TSlateAttribute<FSlateColor> ColorAndOpacityAttribute;

	/** When specified, ignore the content's desired size and report the.HeightOverride as the Box's desired height. */
	TSlateAttribute<TOptional<FVector2D>> DesiredSizeOverrideAttribute;

	/** Progress Percentage Value for this ProgressPercentageAttribute */
	TSlateAttribute<float> ProgressPercentageAttribute;

protected:
	/** Flips the image if the localization's flow direction is RightToLeft */
	bool bFlipForRightToLeftFlowDirection;

	TSubclassOf<class ULayerImageAsset> LayerImageAssetClass; 

	UTexture2D* BarTexture;
};
