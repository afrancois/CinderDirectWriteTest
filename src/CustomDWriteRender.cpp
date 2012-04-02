#include "CustomDWriteRender.h"
#include "cinder/Surface.h"
#include "cinder/ip/Blend.h"
#include <algorithm>
#include <ppl.h>
#include "DWTextBox.h"


using namespace ci;

CustomDWriteRender::CustomDWriteRender(IDWriteFactory* dwriteFactory):mDWriteFactory(dwriteFactory)
{
}

CustomDWriteRender::~CustomDWriteRender(void)
{
}

STDMETHODIMP CustomDWriteRender::DrawGlyphRun( __maybenull void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY, DWRITE_MEASURING_MODE measuringMode, __in DWRITE_GLYPH_RUN const* glyphRun, __in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription, __maybenull IUnknown* clientDrawingEffect )
{
	
	HRESULT hr;
	IDWriteGlyphRunAnalysis* glyphRunAnalysis;
	DWTextBox::DWTBContext* ctx = static_cast<DWTextBox::DWTBContext*>(clientDrawingContext);
	ci::Surface* finalSurface = ctx->surface;
	RECT textureBounds;
	hr = mDWriteFactory->CreateGlyphRunAnalysis(glyphRun,0.75f,NULL,DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC,DWRITE_MEASURING_MODE_GDI_CLASSIC,0.0f,0.0f,&glyphRunAnalysis);
	if(hr!=S_OK)
	{
		OutputDebugString(L"Error!\n");
	}

	glyphRunAnalysis->GetAlphaTextureBounds(DWRITE_TEXTURE_CLEARTYPE_3x1,&textureBounds);
	int width = textureBounds.right-textureBounds.left;
	int height = textureBounds.bottom -textureBounds.top;

	ci::Surface surface(width,height,true,ci::SurfaceChannelOrder::ARGB);
	//surface.setPremultiplied(true);
	uint8_t *dataAlpha = static_cast<uint8_t*>(malloc(width*height*4));
	uint8_t *data = static_cast<uint8_t*>(malloc(width*height*3));
	DWRITE_FONT_METRICS fontMetrics;
	glyphRun->fontFace->GetMetrics(&fontMetrics);
	glyphRunAnalysis->CreateAlphaTexture(DWRITE_TEXTURE_CLEARTYPE_3x1,&textureBounds,data,width*height*3);
	Concurrency::parallel_for(0,width*height,[&](int i){
		float combAlpha = (data[i*3] + data[i*3+1] +data[i*3+2])/3;
		float scale = combAlpha/255;

		dataAlpha[i*4+0] = combAlpha;
		
		dataAlpha[i*4+1] = 255*scale;//data[i*3+0];
		dataAlpha[i*4+2] = 255*scale;//data[i*3+0];
		dataAlpha[i*4+3] = 255*scale;//data[i*3+0];
		//dataAlpha[i*4+0] = (data[i*3] + data[i*3+1] +data[i*3+2])/3;
	});
	memcpy(surface.getData(),dataAlpha,width*height*4);
	float ratio = (float)(glyphRun->fontEmSize)/(float)fontMetrics.designUnitsPerEm;
	float ascend = surface.getBounds().y2 + fontMetrics.descent*ratio;
	
	//finalSurface->copyFrom(surface,surface.getBounds(),Vec2i(baselineOriginX,baselineOriginY-ascend));
	ci::ip::blend(finalSurface,surface,surface.getBounds(),Vec2i(baselineOriginX+textureBounds.left,baselineOriginY+textureBounds.top));
	//mTexture = gl::Texture(surface);
	free (data);
	free (dataAlpha);
	return S_OK;
}

STDMETHODIMP CustomDWriteRender::DrawUnderline( __maybenull void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY, __in DWRITE_UNDERLINE const* underline, __maybenull IUnknown* clientDrawingEffect )
{
	return S_OK;
}

STDMETHODIMP CustomDWriteRender::DrawStrikethrough( __maybenull void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY, __in DWRITE_STRIKETHROUGH const* strikethrough, __maybenull IUnknown* clientDrawingEffect )
{
	return S_OK;
}

STDMETHODIMP CustomDWriteRender::DrawInlineObject( __maybenull void* clientDrawingContext, FLOAT originX, FLOAT originY, IDWriteInlineObject* inlineObject, BOOL isSideways, BOOL isRightToLeft, __maybenull IUnknown* clientDrawingEffect )
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CustomDWriteRender::QueryInterface( REFIID riid, __RPC__deref_out void **ppvObject )
{
	if (__uuidof(IDWriteTextRenderer) == riid)
	{
		*ppvObject = this;
	}
	else if (__uuidof(IDWritePixelSnapping) == riid)
	{
		*ppvObject = this;
	}
	else if (__uuidof(IUnknown) == riid)
	{
		*ppvObject = this;
	}
	else
	{
		*ppvObject = NULL;
		return E_FAIL;
	}

	this->AddRef();

	return S_OK;
}

ULONG STDMETHODCALLTYPE CustomDWriteRender::AddRef( void )
{
	return InterlockedIncrement(&cRefCount_);
}

ULONG STDMETHODCALLTYPE CustomDWriteRender::Release( void )
{
	mTexture.reset();
	unsigned long newCount = InterlockedDecrement(&cRefCount_);
	if (newCount == 0)
	{
		delete this;
		return 0;
	}

	return newCount;
}

IFACEMETHODIMP CustomDWriteRender::IsPixelSnappingDisabled(
	__maybenull void* clientDrawingContext,
	__out BOOL* isDisabled
	)
{
	*isDisabled = FALSE;
	return S_OK;
}

IFACEMETHODIMP CustomDWriteRender::GetCurrentTransform(
	__maybenull void* clientDrawingContext,
	__out DWRITE_MATRIX* transform
	)
{
	//forward the render target's transform
	transform->m11 = 1;
	transform->m12 = 0;
	transform->m21 = 0;
	transform->m22 = 1;
	transform->dx = 0;
	transform->dy = 0;
	return S_OK;
}

IFACEMETHODIMP CustomDWriteRender::GetPixelsPerDip(
	__maybenull void* clientDrawingContext,
	__out FLOAT* pixelsPerDip
	)
{
	float x, yUnused;

	
	*pixelsPerDip = 72 / 96;

	return S_OK;
}