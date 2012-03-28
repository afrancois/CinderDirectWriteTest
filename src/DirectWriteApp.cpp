#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h";
#include "cinder/ImageIo.h";
#include <dwrite.h>
#include "CustomDWriteRender.h"
#include "boost/filesystem/path.hpp";

using namespace ci;
using namespace ci::app;
using namespace std;

class DirectWriteAppApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();

	virtual void prepareSettings( Settings *settings );


	IDWriteFactory* pDWriteFactory_;
	IDWriteTextFormat* pTextFormat_;
	IDWriteTextLayout* pTextLayout_;
	CustomDWriteRender* renderer;
	ci::gl::Texture colorbars;

	std::wstring mText;
	int32_t mTextLength;
};

void DirectWriteAppApp::setup()
{
	colorbars = ci::gl::Texture(ci::loadImage(app::getAppPath().string() + "colorbars.jpg"));
	HRESULT hr;
	
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(IDWriteFactory),reinterpret_cast<IUnknown**>(&pDWriteFactory_));
	mText = L"This is how we roll. We can only regard this as a finer point of lesser understanding.";
	if(SUCCEEDED(hr)){
		hr = pDWriteFactory_->CreateTextFormat(L"Bookman Old Style",NULL,DWRITE_FONT_WEIGHT_SEMI_BOLD,DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,72.0f,L"en-us",&pTextFormat_);
	}
	renderer = new CustomDWriteRender(pDWriteFactory_);

	if(SUCCEEDED(hr)){
		hr = pTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		hr = pTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	if(SUCCEEDED(hr)){
		hr = pDWriteFactory_->CreateTextLayout(mText.c_str(),mText.length(),pTextFormat_,1024,768,&pTextLayout_);
	}

	IDWriteTypography *typo;
	pDWriteFactory_->CreateTypography(&typo);
	
	
	pTextLayout_->Draw(NULL,renderer,0,0);

	
}

void DirectWriteAppApp::mouseDown( MouseEvent event )
{
}

void DirectWriteAppApp::update()
{

}

void DirectWriteAppApp::draw()
{
	// clear out the window with black
	gl::enableAlphaBlending();
	//gl::enableAdditiveBlending();
	gl::enableAlphaBlending(true);
	//glblend(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::color(Color::white());
	gl::draw(colorbars,Vec2i(0,0));
	gl::color(ColorAf(1.0,1.0,1.0,1.0f));
	gl::draw(renderer->mTexture,Vec2i(0,0));
}

void DirectWriteAppApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize(1024,768);
}


CINDER_APP_BASIC( DirectWriteAppApp, RendererGl )
