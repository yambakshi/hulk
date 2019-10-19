#ifndef BOXCLASS_H
#define BOXCLASS_H
 
// Includes
#include <D3DX10.h>

class BoxClass
{
private:
	struct VertexType
	{
		float x,y,z;
	};
public:
	BoxClass();
	BoxClass(const BoxClass&);
	~BoxClass();

	bool Initialize(ID3D10Device* device, D3DXVECTOR3* ModelVertices, int ModelVertexCount);
	void Render(ID3D10Device* device);
	void Shutdown();

	int GetIndexCount();
	D3DXVECTOR3 GetCenter();
	D3DXVECTOR3 GetRadius();

private:	
	ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	D3DXVECTOR3 m_center, m_radius;
};

#endif

