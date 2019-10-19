#include "FrustumClass.h"


FrustumClass::FrustumClass()
{
}

FrustumClass::FrustumClass(const FrustumClass& other)
{
}

FrustumClass::~FrustumClass()
{
}

void FrustumClass::ConstructFrustum(float screenDepth, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	float zMinimum, r;
	D3DXMATRIX matrix;

	
	// Calculate the minimum Z distance in the frustum.
	zMinimum = -projectionMatrix._43 / projectionMatrix._33;
	r = screenDepth / (screenDepth - zMinimum);
	projectionMatrix._33 = r;
	projectionMatrix._43 = -r * zMinimum;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	D3DXMatrixMultiply(&matrix, &viewMatrix, &projectionMatrix);

	// Calculate near plane of frustum.
	m_Planes[0].a = matrix._14 + matrix._13;
	m_Planes[0].b = matrix._24 + matrix._23;
	m_Planes[0].c = matrix._34 + matrix._33;
	m_Planes[0].d = matrix._44 + matrix._43;
	D3DXPlaneNormalize(&m_Planes[0], &m_Planes[0]);

	// Calculate far plane of frustum.
	m_Planes[1].a = matrix._14 - matrix._13; 
	m_Planes[1].b = matrix._24 - matrix._23;
	m_Planes[1].c = matrix._34 - matrix._33;
	m_Planes[1].d = matrix._44 - matrix._43;
	D3DXPlaneNormalize(&m_Planes[1], &m_Planes[1]);

	// Calculate left plane of frustum.
	m_Planes[2].a = matrix._14 + matrix._11; 
	m_Planes[2].b = matrix._24 + matrix._21;
	m_Planes[2].c = matrix._34 + matrix._31;
	m_Planes[2].d = matrix._44 + matrix._41;
	D3DXPlaneNormalize(&m_Planes[2], &m_Planes[2]);

	// Calculate right plane of frustum.
	m_Planes[3].a = matrix._14 - matrix._11; 
	m_Planes[3].b = matrix._24 - matrix._21;
	m_Planes[3].c = matrix._34 - matrix._31;
	m_Planes[3].d = matrix._44 - matrix._41;
	D3DXPlaneNormalize(&m_Planes[3], &m_Planes[3]);

	// Calculate top plane of frustum.
	m_Planes[4].a = matrix._14 - matrix._12; 
	m_Planes[4].b = matrix._24 - matrix._22;
	m_Planes[4].c = matrix._34 - matrix._32;
	m_Planes[4].d = matrix._44 - matrix._42;
	D3DXPlaneNormalize(&m_Planes[4], &m_Planes[4]);

	// Calculate bottom plane of frustum.
	m_Planes[5].a = matrix._14 + matrix._12;
	m_Planes[5].b = matrix._24 + matrix._22;
	m_Planes[5].c = matrix._34 + matrix._32;
	m_Planes[5].d = matrix._44 + matrix._42;
	D3DXPlaneNormalize(&m_Planes[5], &m_Planes[5]);

	return;
}

bool FrustumClass::ScanBox(D3DXVECTOR3 center, D3DXVECTOR3 radius)
{
	int i;


	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	for(i=0; i<6; i++)
	{
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((center.x - radius.x), (center.y - radius.y), (center.z - radius.z))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((center.x + radius.x), (center.y - radius.y), (center.z - radius.z))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((center.x - radius.x), (center.y + radius.y), (center.z - radius.z))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((center.x - radius.x), (center.y - radius.y), (center.z + radius.z))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((center.x + radius.x), (center.y + radius.y), (center.z - radius.z))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((center.x + radius.x), (center.y - radius.y), (center.z + radius.z))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((center.x - radius.x), (center.y + radius.y), (center.z + radius.z))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((center.x + radius.x), (center.y + radius.y), (center.z + radius.z))) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}