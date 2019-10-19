#ifndef APPLICATIONCLASS_H
#define APPLICATIONCLASS_H

// MY INCLUDES //
#include "WindowClass.h"
#include "InputClass.h"
#include "DirectXClass.h"
#include "CameraClass.h"

#include "FrustumClass.h"
#include "SpecMapModelClass.h"
#include "LightClass.h"
#include "TextClass.h"

#include "SpecMapShaderClass.h"
#include "ColorShaderClass.h"

// GLOBALS //
static bool FULL_SCREEN = true;
static bool SHOW_CURSOR = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize();
	void Run();
	void Shutdown();

private:
	// Methods
	bool InitializeFramework();
	bool InitializeObjects();
	bool InitializeShaders();
	void Update(MSG&);
	void Render();

	// Members
	WindowClass *m_Window;
	InputClass *m_Input;
	DirectXClass *m_Device;
	CameraClass *m_Camera;

	FrustumClass *m_Frustum;
	SpecMapModelClass *m_Hulk;
	LightClass *m_Light;
	TextClass *m_Text;

	SpecMapShaderClass *m_SpecMapShader;
	ColorShaderClass *m_ColorShader;
};

#endif