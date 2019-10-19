#ifndef COLORSHADERCLASS_H
#define COLORSHADERCLASS_H


#include <d3d10.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;

class ColorShaderClass
{
public:
	ColorShaderClass();
	ColorShaderClass(const ColorShaderClass&);
	~ColorShaderClass();

	bool Initialize(ID3D10Device*, HWND);
	void Shutdown();
	void Render(ID3D10Device* device, long indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXCOLOR color);

private:
	// METHODS // 
	bool InitializeShader(ID3D10Device*, HWND, LPCSTR);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, LPCSTR);

	void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXCOLOR);
	void RenderShader(ID3D10Device*, long);

	// VARIABLES // 
	ID3D10Effect* m_effect;
	ID3D10EffectTechnique* m_technique;
	ID3D10InputLayout* m_layout;

	ID3D10EffectMatrixVariable* m_worldMatrixPtr;
	ID3D10EffectMatrixVariable* m_viewMatrixPtr;
	ID3D10EffectMatrixVariable* m_projectionMatrixPtr;

	ID3D10EffectVectorVariable* m_colorPtr; 
};

#endif


