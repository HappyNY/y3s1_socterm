#include <stdint.h>
#include <stdio.h>

#define RGB(r, g, b) ((((r)&0x1f)<<11)|(((g)&0x3f)<<5)|(((b)&0x1f)<<0))

extern const unsigned char CGROM[2048];
class skConsole
{
public:
	skConsole(size_t Width, size_t Height);
	~skConsole();
	void Write(char* str);
	void SetCursor(int x, int y);
	void GetCursor(int& x, int& y) const;
	void Print(volatile uint32_t* output_addr) const;
private:
	void WriteChar(char p);

public:
	uint16_t ForeColor;
	uint16_t BackColor;
	
private:
	size_t mCsrX;
	size_t mCsrY;
	size_t const mWidth;
	size_t const mHeight;
	uint16_t* const mBuff;
};
