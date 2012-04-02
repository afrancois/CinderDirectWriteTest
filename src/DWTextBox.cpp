#include "DWTextBox.h"
#include "Common.h"
#include "cinder/app/App.h"
#include "cinder/ip/Fill.h"


using namespace std;
using namespace ci;

DWTextBox::DWTextBox(void):mIsInitalized(false),mNeedsUpdate(true),mFormat(NULL),mLayout(NULL),mAlignment(LEADING),mLineSpacing(0)
{
	mFontContext.Initialize();
	mTexture = ci::gl::Texture(2,2);
}


DWTextBox::~DWTextBox(void)
{
	//cleanup;
	if(mIsInitalized)
	{
		SafeRelease(&mLayout);
		SafeRelease(&mFormat);
	}
}

std::wstring DWTextBox::convertToWide( std::string input)
{
	const char* mb = input.c_str();
	int bufferSize = MultiByteToWideChar(CP_UTF8,0,mb,-1,0,0);
	wchar_t* wide = new wchar_t[bufferSize];
	bufferSize = MultiByteToWideChar(CP_UTF8,0,mb,-1,wide,bufferSize);
	std::wstring result = std::wstring(wide);
	delete wide;
	return result;
}

std::string DWTextBox::convertToUTF8( std::wstring input )
{
	const wchar_t* widestr = input.c_str();
	int bufferSize = WideCharToMultiByte(CP_UTF8,0,widestr,-1,0,0,0,0);
	char* utf8 = new char[bufferSize];
	bufferSize = WideCharToMultiByte(CP_UTF8,0,widestr,-1,utf8,bufferSize,0,0);
	std::string result = std::string(utf8);
	delete utf8;
	return result;
}


void DWTextBox::setText( std::wstring str )
{
	mWStr = str;
	mNeedsUpdate = true;
}

void DWTextBox::setText( std::string str )
{
	setText(convertToWide(str));
}

void DWTextBox::setLineSpacing( int32_t spacing )
{
	mLineSpacing = spacing;
	mNeedsUpdate = true;
}

void DWTextBox::setAlignment( Alignment align )
{
	mAlignment = align;
	mNeedsUpdate = true;
}



void DWTextBox::init( std::string fontName,int fontSize,Vec2i maxSize,UINT* fontList/*=0*/,int fontListSize/*=0*/)
{
	
	HRESULT hr;
	
	//create a fontCollection;
	IDWriteFontCollection* fontCollection = NULL;
	if(fontListSize>0){
		UINT size = sizeof(fontList);
		hr = mFontContext.CreateFontCollection(fontList,size,&fontCollection);
	} else {
		hr = S_OK;
	}
	if(SUCCEEDED(hr)){

	wstring wDefaultFont = convertToWide(fontName);
	SafeRelease(&mFormat);
	hr = DWTextBox::factory()->CreateTextFormat(
		wDefaultFont.c_str(),
		fontCollection,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		L"en-us",
		&mFormat);
	}
	
	if(!SUCCEEDED(hr))
	{
		ci::app::console()<<"Failed to initialize textFormat"<<endl;
	}
	
	mMaxSize = maxSize;
	mSurface = Surface(maxSize.x,maxSize.y,true,ci::SurfaceChannelOrder::RGBA);
	ip::fill(&mSurface,ColorAf(0,0,0,0));
	mSurface.setPremultiplied(true);
	if(mTexture.getBounds().getWidth() != mSurface.getBounds().getWidth() || mTexture.getBounds().getHeight() != mSurface.getBounds().getHeight())
	{
		mTexture.reset();
		mTexture = ci::gl::Texture(mSurface);
	}
	
	mNeedsUpdate = true;
}

IDWriteFactory* DWTextBox::factory()
{
	if(mFactory == NULL){
		 
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(IDWriteFactory),reinterpret_cast<IUnknown**>(&mFactory));
		
	} 
	return mFactory;
}

CustomDWriteRender* DWTextBox::renderer()
{
	if(mRender == NULL){
		mRender = SafeAcquire(new CustomDWriteRender(DWTextBox::factory()));
	}
	return mRender;
}


ci::Surface DWTextBox::render(Vec2i maxSize/*=Vec2i(0,0)*/)
{
		if(!mNeedsUpdate) {
			return mSurface;
		}

		mNeedsUpdate = false;

		HRESULT hr;	
		if(maxSize == Vec2i::zero()){
			maxSize = mMaxSize;
		}

		

		SafeRelease(&mLayout);
		hr = DWTextBox::factory()->CreateTextLayout(mWStr.c_str(),mWStr.size(),mFormat,maxSize.x,maxSize.y,&mLayout);
		if(SUCCEEDED(hr))
		{
			switch(mAlignment)
			{
			case LEADING:
				mLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				mLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
				break;
			case CENTER:
				mLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				mLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				break;
			case TRAILING:
				mLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
				mLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
				break;
			}
			DWTBContext context;
			context.surface = &mSurface;
			context.yOffset = 0;
			context.calcOffset = 0;
			//linespacing
			if(mLineSpacing!=0){
				float linespacing,baseline,lineheight,fontSize;
				//grab the text metrics for computing the baseline offset.
				DWRITE_TEXT_METRICS metrics;
				mLayout->GetFontSize(0,&fontSize);
				mLayout->GetMetrics(&metrics);
				lineheight = metrics.height/metrics.lineCount;
				linespacing = lineheight+mLineSpacing;
				float ratio = 0.8;
				baseline = linespacing*ratio;
				context.yOffset = (fontSize-baseline);
				mLayout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM,linespacing,baseline);
			} else {
				mLayout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_DEFAULT,0,0);
			}
			ip::fill(&mSurface,ColorAf(0,0,0,0));
			hr = mLayout->Draw(&context,DWTextBox::renderer(),0,context.yOffset);
		}
		
		mTexture.update(mSurface);
		return mSurface;
}

ci::gl::Texture DWTextBox::getTexture()
{
	return mTexture;
}

IDWriteFactory* DWTextBox::mFactory = NULL;
ResourceFontContext DWTextBox::mFontContext;
CustomDWriteRender* DWTextBox::mRender = NULL;
