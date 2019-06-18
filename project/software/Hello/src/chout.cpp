#include "chout.hpp"
#include <stdlib.h>
#include <string.h>

skConsole::skConsole(size_t Width, size_t Height) :
	mWidth(Width),
	mHeight(Height),
	mBuff(new uint16_t(Width * Height)),  
	mCsrX(0),
	mCsrY(0),
	ForeColor(0xffff),
	BackColor(0x0000)
{
	memset(mBuff, 0, Width*Height*sizeof(*mBuff));
}

skConsole::~skConsole()
{
	delete mBuff;
}

void skConsole::Write(char* str)
{ 
	while(*str != 0)
	{
		WriteChar(*str++);
	}
}

void skConsole::SetCursor(int x, int y)
{
	
}

void skConsole::GetCursor(int& x, int& y) const
{
	
}

void skConsole::Print(volatile uint32_t* output_addr) const
{
	uint16_t const* lpHead = mBuff;
	uint16_t const* const lpEnd = mBuff + mWidth * mHeight;
	
	while(lpHead < lpEnd)
	{
		output_addr[0] = ((lpHead[1]&0xffff) << 16) | lpHead[0];  
		++output_addr;
		lpHead += 2;
	}
}

void skConsole::WriteChar(char p)
{
	switch(p)
	{ 
	case '\t':
		mCsrX = (((mCsrX / 8) / 4) + 1) * 4 * 8;
		if(mCsrX >= mWidth)
		{
			mCsrX = 0;
			mCsrY += 16;
			if(mCsrY >= mHeight)
			{
				mCsrY = 0;
			}
		}
		return;
	case '\n':
		mCsrX = 0;
		mCsrY += 16;
		if(mCsrY >= mHeight)
		{
			mCsrY = 0;
		}
		return;
	case 0x08: //Backspace
		if((mCsrX-=8) < 0)
		{
			mCsrX = mWidth - 8;
			mCsrY -= 16;
			if(mCsrY < 0)
			{
				mCsrY = mHeight - 16;
			}
		} 
		break;
	default:
		break;
	}
	uint8_t const* lpChar = &CGROM[p*16];
	uint16_t* lpPivot = &mBuff[mCsrY * mWidth + mCsrX];
	
	for(int row = 0; row < 16; ++row)
	{
		uint8_t ch = *lpChar++;
		uint16_t* lpHead = lpPivot;
		lpPivot += mWidth;
		for(int dot = 0; dot < 8; ++dot)
		{
			*lpHead++ = ch & 0x80 ? ForeColor : BackColor;
			ch <<= 1; 
		}
	} 
	
	if((mCsrX+=8) >= mWidth)
	{
		mCsrX = 0;
		mCsrY += 16;
		if(mCsrY >= mHeight)
		{
			mCsrY = 0;
		}
	}
}



