#ifndef FRUSTUMCLASS
#define FRUSTUMCLASS

#include <D3DX10math.h>

class FrustumClass
{
public:
	FrustumClass();
	FrustumClass(const FrustumClass&);
	~FrustumClass();

	void ConstructFrustum(float screenDepth, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);
	bool ScanBox(D3DXVECTOR3 center, D3DXVECTOR3 radius);

private:
	D3DXPLANE m_Planes[6];
};

#endif