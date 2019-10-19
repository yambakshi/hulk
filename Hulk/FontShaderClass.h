#ifndef FONTSHADERCLASS_H
#define FONTSHADERCLASS_H

// INCLUDES //
#include <d3d10.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;

class FontShaderClass
{
public:
	FontShaderClass();
	FontShaderClass(const FontShaderClass&);
	~FontShaderClass();

	bool Initialize(ID3D10Device*, HWND);
	void Shutdown();
	void Render(ID3D10Device* device, long indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
				ID3D10ShaderResourceView* texture, D3DXCOLOR fontColor);

private:
	// METHODS // 
	bool InitializeShader(ID3D10Device*, HWND, char*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, char*);

	void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, D3DXCOLOR);
	void RenderShader(ID3D10Device*, long);

	// VARIABLES // 
	ID3D10Effect* m_effect;
	ID3D10EffectTechnique* m_technique;
	ID3D10InputLayout* m_layout;

	ID3D10EffectMatrixVariable* m_worldMatrixPtr;
	ID3D10EffectMatrixVariable* m_viewMatrixPtr;
	ID3D10EffectMatrixVariable* m_projectionMatrixPtr;
	ID3D10EffectShaderResourceVariable* m_texturePtr;
	ID3D10EffectVectorVariable* m_fontColorPtr;
};

#endif


