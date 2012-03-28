#pragma once
#include <dwrite.h>
#include <cinder/gl/Texture.h>
class CustomDWriteRender :
	public IDWriteTextRenderer 
{
public:
	CustomDWriteRender(IDWriteFactory* dwriteFactory);
	~CustomDWriteRender(void);

	IDWriteFactory* mDWriteFactory;
	ci::gl::Texture mTexture;
	unsigned long cRefCount_;

	IFACEMETHOD(IsPixelSnappingDisabled)(
		__maybenull void* clientDrawingContext,
		__out BOOL* isDisabled
		);

	IFACEMETHOD(GetCurrentTransform)(
		__maybenull void* clientDrawingContext,
		__out DWRITE_MATRIX* transform
		);

	IFACEMETHOD(GetPixelsPerDip)(
		__maybenull void* clientDrawingContext,
		__out FLOAT* pixelsPerDip
		);

	STDMETHODIMP DrawGlyphRun( 
		__maybenull void* clientDrawingContext, 
		FLOAT baselineOriginX, 
		FLOAT baselineOriginY, 
		DWRITE_MEASURING_MODE measuringMode, 
		__in DWRITE_GLYPH_RUN const* glyphRun, 
		__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription, 
		__maybenull IUnknown* clientDrawingEffect );

	STDMETHODIMP DrawUnderline( 
		__maybenull void* clientDrawingContext, 
		FLOAT baselineOriginX, 
		FLOAT baselineOriginY, 
		__in DWRITE_UNDERLINE const* underline, 
		__maybenull IUnknown* clientDrawingEffect );

	STDMETHODIMP DrawStrikethrough( 
		__maybenull void* clientDrawingContext, 
		FLOAT baselineOriginX, 
		FLOAT baselineOriginY, 
		__in DWRITE_STRIKETHROUGH const* strikethrough, 
		__maybenull IUnknown* clientDrawingEffect );

	STDMETHODIMP DrawInlineObject( 
		__maybenull void* clientDrawingContext, 
		FLOAT originX, 
		FLOAT originY, 
		IDWriteInlineObject* inlineObject, 
		BOOL isSideways, 
		BOOL isRightToLeft, 
		__maybenull IUnknown* clientDrawingEffect );

	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, __RPC__deref_out void **ppvObject );

	ULONG STDMETHODCALLTYPE AddRef( void );

	ULONG STDMETHODCALLTYPE Release( void );

};

