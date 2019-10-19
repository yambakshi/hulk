#ifndef TEXTCLASS
#define TEXTCLASS

// INCLUDES //
#include <D3DX10.h>

// MY INCLUDES //
#include "FontClass.h"
#include "FontShaderClass.h"

class TextClass
{
public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D10Device* device, HWND hwnd, D3DXMATRIX viewMatrix, char* fontFilename);
	void Render(ID3D10Device* device, char* text, float fontSize, float x, float y, D3DXCOLOR fontColor,
		D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix,
		unsigned int windowWidth, unsigned int windowHeight);
	void Shutdown();

	void SetFontSize(float fontSize);
	float GetFontSize();
	float GetTextWidth(char* text, float font_size);

private:	
	// METHODS //
	bool InitializeFont(ID3D10Device* device, char* fontTextureFilename);
	bool InitializeShader(ID3D10Device *device, HWND hwnd);

	// VARIABLES //
	unsigned int m_charCount;
	float m_FontSize;
	D3DXMATRIX m_viewMatrix;
	FontClass *m_Font;
	FontShaderClass *m_FontShader;
};

#endif

