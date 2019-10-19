#include "SpecMapModelClass.h"


SpecMapModelClass::SpecMapModelClass()
{
	m_VertexBuffer = 0;
	m_IndexBuffer = 0;
	m_Model = 0;
	m_TexturesArray = 0;
	m_Box = 0;
}

SpecMapModelClass::SpecMapModelClass(const SpecMapModelClass& other)
{
}

SpecMapModelClass::~SpecMapModelClass()
{
}

bool SpecMapModelClass::Initialize(ID3D10Device* device, char* modelFilename, char* textureFilename, char* bumpMapFilename, char* specMapFilename)
{
	bool result;

	// If its an OBJ model type
	if (!strcmp(&modelFilename[(unsigned int)strlen(modelFilename) - 3], "obj"))
	{
		if (!LoadModelFromOBJ(modelFilename))
			return false;
	}
	else if (!strcmp(&modelFilename[(unsigned int)strlen(modelFilename) - 3], "txt"))
	{
		if (!LoadModelFromTXT(modelFilename))
			return false;
	}
	else 
		return false;

	// Calculate the normal, tangent, and binormal vectors for the model.
	CalculateModelVectors();

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers(device);
	if(!result)
		return false;

	// Load the texture array for this model.
	result = LoadTextures(device, textureFilename, bumpMapFilename, specMapFilename);
	if(!result)
		return false;

	// Initialize the box model.
	if(!InitializeBox(device))
		return false;

	return true;
}

bool SpecMapModelClass::LoadModelFromOBJ(char* modelFilename)
{
	char input;
	unsigned int
		verticesCount,
		texCoordsCount,
		normalsCount,
		facesCount;
	ifstream file;
	D3DXVECTOR2
		*texCoords;
	D3DXVECTOR3
		*vertices,
		*normals,
		*faces;


	// Count the vertices, texCoords, normals and faces
	file.open(modelFilename, ios::binary);
	if (file.is_open())
	{
		// Initialize the counters
		verticesCount = texCoordsCount = normalsCount = facesCount = 0;

		file.get(input);
		while (!file.eof())
		{
			if (input == 'v')
			{
				file.get(input);
				if (input == ' ') verticesCount++;
				else if (input == 't') texCoordsCount++;
				else if (input == 'n') normalsCount++;
			}
			else if (input == 'f')
			{
				file.get(input);
				if (input == ' ') facesCount++;
			}

			while (input != '\n')
				file.get(input);

			file.get(input);
		}
	}
	else return false;

	file.close();

	// Fill the vertices, texCoords, normals and faces arrays
	file.open(modelFilename, ios::binary);
	if (file.is_open())
	{
		// Create the objects
		vertices = new D3DXVECTOR3[verticesCount];
		if (!vertices) return false;

		texCoords = new D3DXVECTOR2[texCoordsCount];
		if (!texCoords) return false;

		normals = new D3DXVECTOR3[normalsCount];
		if (!normals) return false;

		faces = new D3DXVECTOR3[3 * facesCount];
		if (!faces) return false;


		// Initialize the counters
		verticesCount = texCoordsCount = normalsCount = facesCount = 0;

		file.get(input);
		while (!file.eof())
		{
			if (input == 'v')
			{
				file.get(input);
				if (input == ' ')
				{
					file >> vertices[verticesCount].x >> vertices[verticesCount].y >> vertices[verticesCount].z;

					// Invert the Z vertex to change to left hand system.
					vertices[verticesCount].z = vertices[verticesCount].z * -1.0f;
					verticesCount++;
				}
				else if (input == 't')
				{
					file >> texCoords[texCoordsCount].x >> texCoords[texCoordsCount].y;

					texCoords[texCoordsCount].y = 1.0f - texCoords[texCoordsCount].y;
					texCoordsCount++;
				}
				else if (input == 'n')
				{
					file >> normals[normalsCount].x >> normals[normalsCount].y >> normals[normalsCount].z;

					// Invert the Z vertex to change to left hand system.
					normals[normalsCount].z = normals[normalsCount].z * -1.0f;
					normalsCount++;
				}
			}
			else if (input == 'f')
			{
				file.get(input);
				if (input == ' ')
				{
					file >>
						faces[facesCount + 2].x >> input >> faces[facesCount + 2].y >> input >> faces[facesCount + 2].z >>
						faces[facesCount + 1].x >> input >> faces[facesCount + 1].y >> input >> faces[facesCount + 1].z >>
						faces[facesCount].x >> input >> faces[facesCount].y >> input >> faces[facesCount].z;

					facesCount += 3;
				}
			}

			while (input != '\n')
				file.get(input);

			file.get(input);
		}
	}
	else return false;

	file.close();

	// Set the vertices and indecis count
	m_VertexCount = m_IndexCount = facesCount;

	// Create the model
	m_Model = new SpecMapModelType[facesCount];
	if (!m_Model)
		return false;

	//Fill the model
	for (int i = 0; i < facesCount; i++)
	{
		m_Model[i].position = vertices[(int)faces[i].x - 1];
		m_Model[i].texture = texCoords[(int)faces[i].y - 1];
		m_Model[i].normal = normals[(int)faces[i].z - 1];
	}

	// Release arrays
	delete[] vertices;
	vertices = 0;

	delete[] texCoords;
	texCoords = 0;

	delete[] normals;
	normals = 0;

	delete[] faces;
	faces = 0;


	return true;
}

bool SpecMapModelClass::LoadModelFromTXT(char* filename)
{
	ifstream fin;
	char input;
	int i;


	// Open the model file.  If it could not open the file then exit.
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> m_VertexCount;

	// Set the number of indices to be the same as the vertex count.
	m_IndexCount = m_VertexCount;

	// Create the model using the vertex count that was read in.
	m_Model = new SpecMapModelType[m_VertexCount];
	if (!m_Model)
	{
		return false;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i<m_VertexCount; i++)
	{
		fin >> m_Model[i].position.x >> m_Model[i].position.y >> m_Model[i].position.z;
		fin >> m_Model[i].texture.x >> m_Model[i].texture.y;
		fin >> m_Model[i].normal.x >> m_Model[i].normal.y >> m_Model[i].normal.z;
	}

	// Close the model file.
	fin.close();

	return true;
}

void SpecMapModelClass::CalculateModelVectors()
{
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	D3DXVECTOR3 tangent, binormal, normal;


	// Calculate the number of faces in the model.
	faceCount = m_VertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (i = 0; i<faceCount; i++)
	{
		// Get the three vertices for this face from the model.
		vertex1.position = m_Model[index].position;
		vertex1.texture = m_Model[index].texture;
		vertex1.normal = m_Model[index].normal;
		index++;

		vertex2.position = m_Model[index].position;
		vertex2.texture = m_Model[index].texture;
		vertex2.normal = m_Model[index].normal;
		index++;

		vertex3.position = m_Model[index].position;
		vertex3.texture = m_Model[index].texture;
		vertex3.normal = m_Model[index].normal;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		CalculateNormal(tangent, binormal, normal);

		// Store the normal, tangent, and binormal for this face back in the model structure.
		//m_Model[index - 1].normal = normal;
		m_Model[index - 1].tangent = tangent;
		m_Model[index - 1].binormal = binormal;

		//m_Model[index - 2].normal = normal;
		m_Model[index - 2].tangent = tangent;
		m_Model[index - 2].binormal = binormal;

		//m_Model[index - 3].normal = normal;
		m_Model[index - 3].tangent = tangent;
		m_Model[index - 3].binormal = binormal;
	}

	return;
}

void SpecMapModelClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3,
	D3DXVECTOR3& tangent, D3DXVECTOR3& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;


	// Calculate the two vectors for this face.
	vector1[0] = vertex2.position.x - vertex1.position.x;
	vector1[1] = vertex2.position.y - vertex1.position.y;
	vector1[2] = vertex2.position.z - vertex1.position.z;

	vector2[0] = vertex3.position.x - vertex1.position.x;
	vector2[1] = vertex3.position.y - vertex1.position.y;
	vector2[2] = vertex3.position.z - vertex1.position.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.texture.x - vertex1.texture.x;
	tvVector[0] = vertex2.texture.y - vertex1.texture.y;

	tuVector[1] = vertex3.texture.x - vertex1.texture.x;
	tvVector[1] = vertex3.texture.y - vertex1.texture.y;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of this normal.
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}

void SpecMapModelClass::CalculateNormal(D3DXVECTOR3 tangent, D3DXVECTOR3 binormal, D3DXVECTOR3& normal)
{
	float length;


	// Calculate the cross product of the tangent and binormal which will give the normal vector.
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// Calculate the length of the normal.
	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	// Normalize the normal.
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;
}

bool SpecMapModelClass::InitializeBuffers(ID3D10Device* device)
{
	unsigned long* indices;
	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create the index array.
	indices = new unsigned long[m_IndexCount];
	if (!indices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for (i = 0; i<m_IndexCount; i++)
		indices[i] = i;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SpecMapModelType)* m_VertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_Model;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_IndexCount;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] indices;
	indices = 0;

	return true;
}

bool SpecMapModelClass::LoadTextures(ID3D10Device* device, char* textureFilename, char* bumpMapFilename, char* specMapFilename)
{
	HRESULT result;

	// Create the texture object
	m_TexturesArray = new ID3D10ShaderResourceView*[3];
	if (!m_TexturesArray)
		return false;

	// Load the texture in
	result = D3DX10CreateShaderResourceViewFromFile(device, textureFilename, NULL, NULL, &m_TexturesArray[0], NULL);
	if (FAILED(result))
		return false;

	// Load the texture in
	result = D3DX10CreateShaderResourceViewFromFile(device, bumpMapFilename, NULL, NULL, &m_TexturesArray[1], NULL);
	if (FAILED(result))
		return false;

	// Load the texture in
	result = D3DX10CreateShaderResourceViewFromFile(device, specMapFilename, NULL, NULL, &m_TexturesArray[2], NULL);
	if (FAILED(result))
		return false;


	return true;
}

bool SpecMapModelClass::InitializeBox(ID3D10Device* device)
{		
	D3DXVECTOR3 *vertices;


	// creates the box object
	m_Box = new BoxClass;
	if(!m_Box)
		return false;

	vertices = new D3DXVECTOR3[m_VertexCount];
	if(!vertices)
		return false;

	for(int i=0; i<m_VertexCount; i++)
		vertices[i] = D3DXVECTOR3(m_Model[i].position.x, m_Model[i].position.y, m_Model[i].position.z);

	// initialize the box object
	if(!m_Box->Initialize(device, vertices, m_VertexCount))
		return false;

	delete [] vertices;

	return true;
}

void SpecMapModelClass::Render(ID3D10Device* device)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(device);

	return;
}

void SpecMapModelClass::RenderBuffers(ID3D10Device* device)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(SpecMapModelType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	device->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	device->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void SpecMapModelClass::RenderBox(ID3D10Device* device)
{
	m_Box->Render(device);

	return;
}

void SpecMapModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_IndexBuffer)
	{
		m_IndexBuffer->Release();
		m_IndexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_VertexBuffer)
	{
		m_VertexBuffer->Release();
		m_VertexBuffer = 0;
	}

	return;
}

void SpecMapModelClass::Shutdown()
{
	// Release the model texture array.
	ReleaseTextures();

	// Release the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();
	
	// Release the model object.
	ShutdownBox();

	return;
}

void SpecMapModelClass::ReleaseTextures()
{
	// Release the texture array object.
	if (m_TexturesArray)
	{
		m_TexturesArray[0]->Release();
		m_TexturesArray[0] = 0;

		m_TexturesArray[1]->Release();
		m_TexturesArray[1] = 0;

		m_TexturesArray[2]->Release();
		m_TexturesArray[2] = 0;

		delete[] m_TexturesArray;
		m_TexturesArray = 0;
	}

	return;
}

void SpecMapModelClass::ReleaseModel()
{
	if (m_Model)
	{
		delete[] m_Model;
		m_Model = 0;
	}

	return;
}

void SpecMapModelClass::ShutdownBox()
{
	if(m_Box)
	{
		m_Box->Shutdown();
		delete m_Box;
		m_Box = 0;
	}

	return;
}

int SpecMapModelClass::GetIndexCount()
{
	return m_IndexCount;
}

int SpecMapModelClass::GetBoxIndexCount()
{
	return m_Box->GetIndexCount();
}

ID3D10ShaderResourceView** SpecMapModelClass::GetTexturesArray()
{
	return m_TexturesArray;
}

D3DXVECTOR3 SpecMapModelClass::GetCenter()
{
	return m_Box->GetCenter();
}

D3DXVECTOR3 SpecMapModelClass::GetRadius()
{
	return m_Box->GetRadius();
}