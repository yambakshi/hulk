#include "TextClass.h"


TextClass::TextClass()
{
	m_Font = 0;
	m_FontShader = 0;
}

TextClass::TextClass(const TextClass& other)
{
}

TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D10Device* device, HWND hwnd, D3DXMATRIX viewMatrix, char* fontFilename)
{
	m_viewMatrix = viewMatrix;

	if(!InitializeFont(device, fontFilename))
		return false;

	if(!InitializeShader(device, hwnd))
		return false;


	return true;
}

bool TextClass::InitializeFont(ID3D10Device* device, char* fontTextureFilename)
{	
	//Create the font object
	m_Font = new FontClass;
	if(!m_Font)
		return false;
	
	// Initialize font object.
	if(!m_Font->Initialize(device, fontTextureFilename))
		return false;


	return true;
}

bool TextClass::InitializeShader(ID3D10Device *device, HWND hwnd)
{
	// Create the font shader object.	
	m_FontShader = new FontShaderClass;
	if(!m_FontShader)
		return false;

	// Initialize the font shader object
	if(FAILED(m_FontShader->Initialize(device, hwnd)))
		return false;


	return true;
}

void TextClass::Render(ID3D10Device* device, char* text, float fontSize, float x, float y, D3DXCOLOR fontColor,
	D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix, unsigned int windowWidth, unsigned int windowHeight)
{				
	m_Font->Render(device, text, fontSize, x, y, windowWidth, windowHeight);

	m_FontShader->Render(device, m_Font->GetIndexCount(),
		worldMatrix, m_viewMatrix, orthoMatrix, m_Font->GetTexture(), fontColor);


	return;
}

void TextClass::Shutdown()
{
	// Release font
	if(m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	// Release shader
	if(m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	
	return;
}

float TextClass::GetTextWidth(char* text, float font_size)
{
	return m_Font->MeasureTextWidth(text, font_size);
}

void TextClass::SetFontSize(float fontSize)
{
	m_FontSize = fontSize;


	return;
}

float TextClass::GetFontSize()
{
	return m_FontSize;
}