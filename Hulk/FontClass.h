#ifndef FontClass_H
#define FontClass_H

// INCLUDES //
#include <fstream>

// MY INCLUDES //
#include "TextureClass.h"

using namespace std;

class FontClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};
public:
	struct FontType
	{
		unsigned int 
			width,
			ascii;
		char character;
		float leftU, rightU;
	};
public:
	FontClass();
	FontClass(const FontClass&);
	~FontClass();

	bool Initialize(ID3D10Device* device, char* textureFilename);
	bool Render(ID3D10Device* device, char* text, float fontSize, float y, float x, unsigned int windowWidth, unsigned int windowHeight);
	void Shutdown();

	unsigned long GetIndexCount();
	float MeasureTextWidth(char* text, float fontSize);
	ID3D10ShaderResourceView* GetTexture();

private:
	// METHODS //  
	bool LoadPixels(char*);
	bool LoadFontData();
	bool LoadTexture(ID3D10Device*, char*);
	bool InitializeBuffers(ID3D10Device*);
	
	bool UpdateSentence(char*, bool&);
	bool UpdateBuffers(float, float, float, unsigned int, unsigned int, bool&);
	void RenderBuffers(ID3D10Device*);

	void ReleaseTexture();
	void ShutdownBuffers();

	// VARIABLES // 
	unsigned int 
		m_SentenceLength,
		m_Rows, m_Columns;
	unsigned int 
		m_vertexCount, m_indexCount;
	float 
		m_FontSize,
	    m_previousPosX, m_previousPosY;
	FontType 
		*m_Sentence,
		*m_FontData;
	D3DXCOLOR **m_Pixels;
	ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
	TextureClass *m_Texture;
};

#endif 
