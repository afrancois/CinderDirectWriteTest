#pragma once
#include <vector>
#include <memory>
#include <dwrite.h>

#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/Vector.h"

#include "ResourceFontContext.h"
#include "CustomDWriteRender.h"


class DWTextBox
{
public:
	struct DWTBContext
	{
		float yOffset;
		float calcOffset;
		ci::Surface* surface;
	};
	
	enum Alignment{LEADING,TRAILING,CENTER};
	DWTextBox();
	~DWTextBox(void);

	//initialization functions
	void init(std::string fontName,int fontSize,ci::Vec2i maxSize,UINT* fontlist=0,int fontListSize=0);
	

	std::string text();
	std::wstring text_w();
	void setText(std::string str);
	void setText(std::wstring str);
	


	void setAlignment(DWTextBox::Alignment align);
	void setLineSpacing(int32_t spacing);

	ci::Surface render(ci::Vec2i maxSize=ci::Vec2i(0,0));
	static IDWriteFactory* factory();
	static CustomDWriteRender* renderer();
private:
	bool mIsInitalized;
	bool mNeedsUpdate;
	std::wstring mWStr;
	ci::Vec2i mMaxSize;
	ci::Surface mSurface;
	
	std::wstring convertToWide(std::string input);
	std::string convertToUTF8(std::wstring input);
	

	
	IDWriteTextFormat* mFormat;
	IDWriteTextLayout* mLayout;


	//static
	static IDWriteFactory* mFactory;
	static ResourceFontContext mFontContext;
	static CustomDWriteRender* mRender;
	int32_t mLineSpacing;
	Alignment mAlignment;
};

