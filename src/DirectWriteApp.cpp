#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h";
#include "cinder/ImageIo.h";
#include "cinder/Text.h";
#include "cinder/Font.h";

#include <dwrite.h>
#include "CustomDWriteRender.h"
#include "boost/filesystem/path.hpp";
#include "DWTextBox.h"
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

UINT ID(UINT x,std::string y){return x;}

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
	ci::gl::Texture text;
	std::wstring mText;
	int32_t mTextLength;
	DWTextBox* textBox;
	ci::TextBox textBoxOrg;
	ci::Font mfnt;
	bool mTouchToggle;
};

void DirectWriteAppApp::setup()
{
	colorbars = ci::gl::Texture(ci::loadImage(app::getAppPath().string() + "colorbars.jpg"));
	HRESULT hr;
	mTouchToggle = false;
	mfnt = ci::Font(loadResource(RES_BROADWAY_REGULAR),30);
	textBoxOrg.setFont(mfnt);
	textBoxOrg.setSize(Vec2i(512,768));
	

	/*hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(IDWriteFactory),reinterpret_cast<IUnknown**>(&pDWriteFactory_));
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
	*/
	
	UINT fontList[] = {
		ID(RES_HELVETICA_RND_LT_STD_BOLD),
		ID(RES_HELVETICA_RND_LT_STD_BOLD_O),
		ID(RES_HELVETICA_RND_LT_STD_BOLD_CN),
		ID(RES_HELVETICA_RND_LT_STD_BOLD_CN_O),
		ID(RES_HELVETICA_RND_LT_STD_BLACK),
		ID(RES_HELVETICA_RND_LT_STD_BLACK_O),
		ID(RES_BROADWAY_REGULAR)
	};
	textBox = new DWTextBox();
	textBox->init("Broadway Regular",30,Vec2i(512,768),fontList,1);
	textBox->setText("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras cursus diam suscipit orci adipiscing vehicula. Nulla sed sem nulla. Sed eget congue erat. Maecenas vel tincidunt lacus. Aenean commodo varius auctor. Nunc fermentum vehicula mi eget auctor. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed facilisis dui non felis vulputate eu consequat tortor auctor. Vivamus malesuada tincidunt sem ut faucibus. In posuere arcu nec justo mollis auctor. Curabitur elementum nisl id diam sodales tincidunt. Praesent gravida pellentesque sapien ac rutrum. Integer porta gravida erat aliquam interdum. Fusce pulvinar, sapien et congue dictum, erat felis varius diam, sed ornare libero ligula at enim. In quis nisl justo. Pellentesque ac ante et tellus sollicitudin lobortis.");
	textBox->setLineSpacing(0);

	
}

void DirectWriteAppApp::mouseDown( MouseEvent event )
{
	//textBox->setText("Four score and seven years ago, our forefathers brought forth on this continent a new nation...");
	if(!mTouchToggle){
		textBox->setLineSpacing(-20);
	} else {
		textBox->setLineSpacing(0);
	}
	mTouchToggle = !mTouchToggle;
}

void DirectWriteAppApp::update()
{
	textBox->render();
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
	gl::color(ColorAf(0.0,0.0,0.0,1.0f));
	gl::draw(textBox->getTexture(),Vec2i(0,0));
}

void DirectWriteAppApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize(1024,768);
}


CINDER_APP_BASIC( DirectWriteAppApp, RendererGl )
