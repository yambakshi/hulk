#include "FontClass.h"


FontClass::FontClass()
{
	m_Texture = 0;
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	m_FontData = 0;
	m_Sentence = 0;
	m_Pixels = 0;
}

FontClass::FontClass(const FontClass& other)
{
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D10Device* device, char* textureFilename)
{
	// Initialize the previous rendering position to negative one.
	m_previousPosX = -1;
	m_previousPosY = -1;

	// Initialzie the font size
	m_FontSize = -1;
			
	if(!LoadPixels(textureFilename))
		return false;
	
	if(!LoadFontData())
		return false;
	
	if(!LoadTexture(device, textureFilename))
		return false;

	if(!InitializeBuffers(device))
		return false;


	return true;
}

bool FontClass::LoadPixels(char* textureFilename)
{
	ifstream file;
	int dummy;


	file.open(textureFilename, ios::binary);
	if(file.is_open())
	{
		// Get texture height
		for(int i=0; i<12; i++)
			dummy = file.get();

		m_Rows = file.get();
		m_Rows += 256*file.get();
		m_Rows += 256*file.get();
		m_Rows += 256*file.get();

		m_Columns = file.get();
		m_Columns += 256*file.get();
		m_Columns += 256*file.get();
		m_Columns += 256*file.get();
		
		// Create the pixels array
		m_Pixels = new D3DXCOLOR*[m_Rows];
		if(!m_Pixels)
			return false;

		for(int i=0; i<m_Rows; i++)
			m_Pixels[i] = new D3DXCOLOR[m_Columns];

		// Move to pixel data at 128
		for(int i=0; i<108; i++)
			dummy = file.get();

		// Fill the array row after row
		for(int row=0; row<m_Rows; row++)
		{
			// Fill the row
			for(int column=0; column<m_Columns; column++)
			{
				m_Pixels[row][column].b = file.get();
				m_Pixels[row][column].g = file.get();
				m_Pixels[row][column].r = file.get();
				m_Pixels[row][column].a = file.get();
			}		
		}
			
	}
	else
		return false;
	

	return true;
}

bool FontClass::LoadFontData()
{
	bool 
		foundChar,
		blackColumn;
	unsigned int 
		firstCharsGap, currCharsGap, charWidth,
		charCount;


	// Create the font data array
	m_FontData = new FontType[95];
	if(!m_FontData)
		return false;

	m_FontData[0].ascii = 32;
	m_FontData[0].character = ' ';
	m_FontData[0].leftU = 0.0f;
	m_FontData[0].rightU = 0.0f;
	m_FontData[0].width = 0;

	for(int i=33; i<127; i++)
	{
		m_FontData[i-32].character = (char)i;
		m_FontData[i-32].ascii = i;
	}

	charCount = 1;
	charWidth = 0;
	foundChar = false;
	
	// Go through the array column after column
	for(int column=0; column<m_Columns; column++)
	{
		blackColumn = true;

		// Go through the column
		for(int row=0; row<m_Rows; row++)
		{
			// If a pixel in the column is not black
			if(m_Pixels[row][column].a != 0)
			{
				// Mark it as the beginning of the char
				blackColumn = false;

				// If its the beginning of the char
				if(!foundChar)
				{
					foundChar = true;
					m_FontData[charCount].leftU = column / (float)m_Columns;
				}

				break;
			}
		}

		// If the column is not completely black
		// add to the character total width
		if(!blackColumn)
			charWidth++;
		else // If column is completely black
		{
			// Check if its the end of the character width
			if(foundChar)
			{
				// Set the width of the character
				if(charCount < 95)
					m_FontData[charCount].width = charWidth;

				// If its the second character
				if(charCount == 2)
				{
					// Calculatae the gap between the two first characters
					firstCharsGap = m_FontData[charCount-1].rightU * m_Columns;
					firstCharsGap  = (m_FontData[charCount].leftU * m_Columns) - firstCharsGap;		
				}
								
				if(charCount > 2)
				{
					// Calculate the gap between the current  
					// character, and the one before
					currCharsGap = m_FontData[charCount-1].rightU * m_Columns;
					currCharsGap = (m_FontData[charCount].leftU * m_Columns) - currCharsGap;

					// If the current gap is significantly small 
					// compare to the first two characters gap
					// it means its a gap within the previous character
					if(firstCharsGap - (firstCharsGap *0.75f) > currCharsGap)
					{
						// Set the correct width of the previous character
						m_FontData[charCount-1].width = column - (m_FontData[charCount-1].leftU * m_Columns);
						m_FontData[charCount-1].rightU = column / (float)m_Columns;
					}
					// If its not a significantly small gap
					// it means these are two different characters
					else
					{
						// Calculate the right u coordinate of the current character
						m_FontData[charCount].rightU = column / (float)m_Columns;
						charCount++;
					}
				}
				else
				{
					// Calculate the right u coordinate of the current character
					m_FontData[charCount].rightU = column / (float)m_Columns;
					charCount++;
				}
				
				charWidth = 0;
				foundChar = false;	
			}
		}
	}

	// Release pixels array
	delete [] m_Pixels;
	m_Pixels = 0;


	return true;
}

bool FontClass::InitializeBuffers(ID3D10Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Set the number of vertices in the vertex array.
	m_vertexCount = 256*6;

	// Set the number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Load the index array with data.
	for(i=0; i<m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
    vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;

	// Now finally create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
    indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;


	return true;
}

bool FontClass::LoadTexture(ID3D10Device* device, char* filename)
{
	m_Texture = new TextureClass;
	if(!m_Texture)
		return false;

	if(!m_Texture->Initialize(device, filename))
		return false;

	return true;
}

bool FontClass::Render(ID3D10Device* device, char *text, float font_size, float x, float y, unsigned int windowWidth, unsigned int windowHeight)
{
	bool changed = true;


	if(!UpdateSentence(text, changed))
		return false;

	if(!UpdateBuffers(font_size, x, y, windowWidth, windowHeight, changed))
		return false;

	RenderBuffers(device);

	return true;
}

bool FontClass::UpdateSentence(char *text, bool& changed)
{
	unsigned int i, charIndex;

	
	// Check if there new sentence length is the same as the old one
	if(m_SentenceLength == (unsigned int)strlen(text))
	{
		// Compare each character
		for(i=0; i<m_SentenceLength; i++)
		{
			// If there are changes in the sentence
			if(m_Sentence[i].character != text[i])
				break;

		}

		// If there are no changes in the sentence
		if(i == m_SentenceLength)
		{
			changed = false;
			return true;
		}
	}
	// If it's not the same length it recreates the sentence
	else
	{
		// Count the characters in the sentence
		m_SentenceLength = (unsigned int)strlen(text);

		// If theres already an
		// existing sentence release it
		if(m_Sentence)
		{
			delete [] m_Sentence;
			m_Sentence = 0;
		}

		// Create the sentence array object
		m_Sentence = new FontType[m_SentenceLength];
		if(!m_Sentence)
			return false;
	}

	// Fill the sentence array
	for(int i=0; i<m_SentenceLength; i++)
	{
		charIndex = text[i] - 32;
		m_Sentence[i].ascii = m_FontData[charIndex].ascii;
		m_Sentence[i].character = m_FontData[charIndex].character;
		m_Sentence[i].leftU = m_FontData[charIndex].leftU;
		m_Sentence[i].rightU = m_FontData[charIndex].rightU;
		m_Sentence[i].width = m_FontData[charIndex].width;
	}
	

	return true;
}

bool FontClass::UpdateBuffers(float fontSize, float positionX, float positionY, unsigned int windowWidth, unsigned int windowHeight, bool& changed)
{
	float 
		charsWidth, gap,
		left, right, top, bottom;
	VertexType* vertices;
	void* verticesPtr;
	HRESULT result;


	// If the position we are rendering this bitmap to has not changed then don't update the vertex buffer since it
	// currently has the correct parameters.
	if(
		m_FontSize == fontSize && !changed &&
		(positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}
	
	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// If it has changed then update the position it is being rendered to.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// If the font size has changed it updates the font size member
	m_FontSize = fontSize;
		
	// Calculate the screen coordinates of the left side of the bitmap.
	left = (windowWidth / -2.0f) + (float)positionX;

	// Initialize the characters gap and width values
	charsWidth = 0;
	gap = fontSize * 0.05f;

	// Create the sentence
	for(int i=0; i<m_SentenceLength; i++)
	{
		// Adds the gap between words
		if(m_Sentence[i].character == ' ')
		{
			left += (gap*7);
			continue;
		}
		
		// Adds the gap between characters
		if(i != 0)
		{
			left += (gap + charsWidth);
		}

		// Calculate the char's width according to the font size
		charsWidth = (fontSize * m_Sentence[i].width) / (float)m_Rows;
		
		// Calculate the screen coordinates of the right side of the bitmap.
		right = left + (float)charsWidth;

		// Calculate the screen coordinates of the top of the bitmap.
		top = (windowHeight / 2.0f) - (float)positionY;

		// Calculate the screen coordinates of the bottom of the bitmap.
		bottom = top - (float)fontSize;

		// Load the vertex array with data.
		// First triangle.
		vertices[i*6].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
		vertices[i*6].texture = D3DXVECTOR2(m_Sentence[i].leftU, 0.0f);

		vertices[(i*6)+1].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
		vertices[(i*6)+1].texture = D3DXVECTOR2(m_Sentence[i].rightU, 1.0f);

		vertices[(i*6)+2].position = D3DXVECTOR3(left, bottom, 0.0f);  // Bottom left.
		vertices[(i*6)+2].texture = D3DXVECTOR2(m_Sentence[i].leftU, 1.0f);

		// Second triangle.
		vertices[(i*6)+3].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
		vertices[(i*6)+3].texture = D3DXVECTOR2(m_Sentence[i].leftU, 0.0f);

		vertices[(i*6)+4].position = D3DXVECTOR3(right, top, 0.0f);  // Top right.
		vertices[(i*6)+4].texture = D3DXVECTOR2(m_Sentence[i].rightU, 0.0f);

		vertices[(i*6)+5].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
		vertices[(i*6)+5].texture = D3DXVECTOR2(m_Sentence[i].rightU, 1.0f);
	}

	// Initialize the vertex buffer pointer to null first.
	verticesPtr = 0;

	// Lock the vertex buffer.
	result = m_vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&verticesPtr);
	if(FAILED(result))
	{
		return false;
	}

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	m_vertexBuffer->Unmap();

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;


	return true;
}

void FontClass::RenderBuffers(ID3D10Device* device)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	device->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	device->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void FontClass::Shutdown()
{
	if(m_FontData)
	{
		delete m_FontData;
		m_FontData = 0;
	}

	ReleaseTexture();

	ShutdownBuffers();


	return;
}

void FontClass::ReleaseTexture()
{
	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
	

	return;
}

void FontClass::ShutdownBuffers()
{
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	return;
}

unsigned long FontClass::GetIndexCount()
{
	return m_indexCount;
}

float FontClass::MeasureTextWidth(char* text, float fontSize)
{
	unsigned int charIndex;
	float textWidth, gap;


	// Set the gap according to the font size
	gap = fontSize * 0.05f;

	// Initialize the text width
	textWidth = 0;

	for(int i=0; i<(unsigned int)strlen(text); i++)
	{
		// Adds the gap between words
		if(text[i] == ' ')
		{
			textWidth += (gap*7);
			continue;
		}

		// Adds the gap between characters
		if(i != 0)
			textWidth += gap;		
		
		// Calculate the char's width according to the font size
		charIndex = text[i] - 32;
		textWidth += (fontSize * m_FontData[charIndex].width) / (float)m_Rows;
	}


	return textWidth;
}

ID3D10ShaderResourceView* FontClass::GetTexture()
{
	return m_Texture->GetTexture();
}