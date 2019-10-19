#include "BoxClass.h"


BoxClass::BoxClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

BoxClass::BoxClass(const BoxClass& other)
{
}

BoxClass::~BoxClass()
{
}

bool BoxClass::Initialize(ID3D10Device* device, D3DXVECTOR3* ModelVertices, int ModelVertexCount)
{
	VertexType *vertices;
	unsigned long* indices;
	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	float 
		maxX = FLT_MIN,
		minX = FLT_MAX,
		maxY = FLT_MIN,
		minY = FLT_MAX,
		maxZ = FLT_MIN,
		minZ = FLT_MAX;
	HRESULT result;

	
	m_vertexCount = 8;
	m_indexCount = 36;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
		return false;

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
		return false;


	for(int i=0; i<ModelVertexCount; i++)
	{
		// finds the largest X value of the model
		if(ModelVertices[i].x > maxX)
			maxX = ModelVertices[i].x;
		
		// finds the smallest X value of the model
		if(ModelVertices[i].x < minX)
			minX = ModelVertices[i].x;
		
		// finds the largest Y value of the model
		if(ModelVertices[i].y > maxY)
			maxY = ModelVertices[i].y;
		
		// finds the smallest Y value of the model
		if(ModelVertices[i].y < minY)
			minY = ModelVertices[i].y;
		
		// finds the largest Z value of the model
		if(ModelVertices[i].z > maxZ)
			maxZ = ModelVertices[i].z;
		
		// finds the smallest Z value of the model
		if(ModelVertices[i].z < minZ)
			minZ = ModelVertices[i].z;
	}

	// calculates the radius in each axis
	m_radius = D3DXVECTOR3((maxX-minX)/2.0f, (maxY-minY)/2.0f, (maxZ-minZ)/2.0f);

	// calculates the center of the model
	m_center = D3DXVECTOR3(maxX-m_radius.x, maxY-m_radius.y, maxZ-m_radius.z);

	// VERTICES:
	// top-left-front
	vertices[0].x = minX;
	vertices[0].y = maxY;
	vertices[0].z = minZ;

	// top-right-front
	vertices[1].x = maxX;
	vertices[1].y = maxY;
	vertices[1].z = minZ;

	// bottom-left-front
	vertices[2].x = minX;
	vertices[2].y = minY;
	vertices[2].z = minZ;

	// bottom-right-front
	vertices[3].x = maxX;
	vertices[3].y = minY;
	vertices[3].z = minZ;

	// top-left-back
	vertices[4].x = minX;
	vertices[4].y = maxY;
	vertices[4].z = maxZ;

	// top-right-back
	vertices[5].x = maxX;
	vertices[5].y = maxY;
	vertices[5].z = maxZ;

	// bottom-left-back
	vertices[6].x = minX;
	vertices[6].y = minY;
	vertices[6].z = maxZ;

	// bottom-right-back
	vertices[7].x = maxX;
	vertices[7].y = minY;
	vertices[7].z = maxZ;

	// INDICES:
	// front faces
	indices[0] = 0;
	indices[1] = 3;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 1;
	indices[5] = 3;

	// back faces
	indices[6] = 4;
	indices[7] = 7;
	indices[8] = 6;
	indices[9] = 4;
	indices[10] = 5;
	indices[11] = 7;

	// top faces
	indices[12] = 0;
	indices[13] = 4;
	indices[14] = 1;
	indices[15] = 4;
	indices[16] = 5;
	indices[17] = 1;

	// bottom faces
	indices[18] = 2;
	indices[19] = 6;
	indices[20] = 3;
	indices[21] = 6;
	indices[22] = 7;
	indices[23] = 3;

	// right faces
	indices[24] = 1;
	indices[25] = 7;
	indices[26] = 3;
	indices[27] = 1;
	indices[28] = 5;
	indices[29] = 7;

	// left faces
	indices[30] = 4;
	indices[31] = 2;
	indices[32] = 6;
	indices[33] = 4;
	indices[34] = 0;
	indices[35] = 2;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
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

void BoxClass::Render(ID3D10Device* device)
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

void BoxClass::Shutdown()
{
	if(!m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		delete  m_vertexBuffer;
		m_vertexBuffer = 0;
	}


	if(!m_indexBuffer)
	{
		m_indexBuffer->Release();
		delete  m_indexBuffer;
		m_indexBuffer = 0;
	}
	
	return;
}

int BoxClass::GetIndexCount()
{
	return m_indexCount;
}

D3DXVECTOR3 BoxClass::GetCenter()
{
	return m_center;
}

D3DXVECTOR3 BoxClass::GetRadius()
{
	return m_radius;
}
