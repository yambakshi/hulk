#ifndef SPECMAPMODELCLASS_H
#define SPECMAPMODELCLASS_H

// Includes
#include <D3DX10.h>
#include <fstream>
using namespace std;

// My Includes
#include "BoxClass.h"


class SpecMapModelClass
{
private:
	struct SpecMapModelType
	{
		D3DXVECTOR3 position;
	    D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
	};

	struct TempVertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

public:
	SpecMapModelClass();
	SpecMapModelClass(const SpecMapModelClass&);
	~SpecMapModelClass();

	bool Initialize(ID3D10Device* device, char* modelFilename, char* textureFilename, char* bumpMapFilename, char* specMapFilename);
	void Render(ID3D10Device*);
	void RenderBox(ID3D10Device* device);
	void Shutdown();

	int GetIndexCount();
	int GetBoxIndexCount();
	ID3D10ShaderResourceView** GetTexturesArray();
	D3DXVECTOR3 GetCenter();
	D3DXVECTOR3 GetRadius();

private:
	// Methods
	bool InitializeBuffers(ID3D10Device*);
	bool InitializeBox(ID3D10Device*);
	bool LoadTextures(ID3D10Device*, char*, char*, char*);
	bool LoadModelFromOBJ(char*);
	bool LoadModelFromTXT(char*);
	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, D3DXVECTOR3&, D3DXVECTOR3&);
	void CalculateNormal(D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR3&);
	void RenderBuffers(ID3D10Device*);
	void ReleaseModel();
	void ReleaseTextures();
	void ShutdownBuffers();
	void ShutdownBox();

	// Members
	BoxClass *m_Box;
	ID3D10Buffer *m_VertexBuffer, *m_IndexBuffer;
	int m_VertexCount, m_IndexCount;
	SpecMapModelType* m_Model;
	ID3D10ShaderResourceView** m_TexturesArray;
};

#endif