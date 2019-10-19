#include "ApplicationClass.h"


ApplicationClass::ApplicationClass()
{
	m_Window = 0;
	m_Input = 0;
	m_Device = 0;
	m_Camera = 0;

	m_Frustum = 0;
	m_Hulk = 0;
	m_Light = 0;
	m_Text = 0;

	m_SpecMapShader = 0;
	m_ColorShader = 0;
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
	
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize()
{
	if (!InitializeFramework())
		return false;

	if (!InitializeObjects())
		return false;

	if (!InitializeShaders())
		return false;


	return true;
}

bool ApplicationClass::InitializeFramework()
{
	RECT windowRect;


	// Create window object
	m_Window = new WindowClass;
	if (!m_Window)
		return false;

	// Initialize window object
	if (!m_Window->Initialize(FULL_SCREEN))
		return false;

	// Create input object
	m_Input = new InputClass;
	if (!m_Input)
		return false;

	// Initialize window object
	m_Input->Initialize();

	GetWindowRect(m_Window->GetWindowHandle(), &windowRect);

	m_Device = new DirectXClass;
	if (!m_Device)
		return false;

	if (!m_Device->Initialize(m_Window->GetWindowHandle(),
		(windowRect.right - windowRect.left), (windowRect.bottom - windowRect.top),
		VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
		return false;


	return true;
}

bool ApplicationClass::InitializeObjects()
{
	D3DXMATRIX viewMatrix2D;


	// Create the camera object
	m_Camera = new CameraClass;
	if (!m_Camera)
		return false;

	// Set the camera position
	m_Camera->SetPosition(0.0f, 2.0f, -8.0f);

	// Create the frustum object
	m_Frustum = new FrustumClass;
	if(!m_Frustum)
		return false;

	// Create the specmap model object
	m_Hulk = new SpecMapModelClass;
	if(!m_Hulk)
		return false;
	
	// Initialize the specmap model object
	if(!m_Hulk->Initialize(m_Device->GetDevice(), 
		"Data/Models/Hulk.obj", "Data/Textures/Hulk.dds", "Data/Bumpmaps/Hulk.dds", "Data/Specmaps/Hulk.dds"))
		return false;
	
	// Create th elight object
	m_Light = new LightClass;
	if(!m_Light)
		return false;

	// Initialize the light object.
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetAmbientColor(0.2f, 0.2f, 0.2f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(16.0f);

	// Get the 2D view matrix
	m_Camera->Render2D();
	m_Camera->Get2DViewMatrix(viewMatrix2D);
		
	// Create the text object
	m_Text = new TextClass;
	if(!m_Text)
		return false;

	// Initialize the text object
	if(!m_Text->Initialize(m_Device->GetDevice(), m_Window->GetWindowHandle(), viewMatrix2D, "Data/Textures/Font.dds"))
		return false;


	return true;
}

bool ApplicationClass::InitializeShaders()
{
	// Create the specmap shader class
	m_SpecMapShader = new SpecMapShaderClass;
	if(!m_SpecMapShader)
		return false;

	// Initialize the specmap shader class
	if(!m_SpecMapShader->Initialize(m_Device->GetDevice(), m_Window->GetWindowHandle()))
		return false;
		
	// Create the specmap shader class
	m_ColorShader = new ColorShaderClass;
	if(!m_ColorShader)
		return false;

	// Initialize the specmap shader class
	if(!m_ColorShader->Initialize(m_Device->GetDevice(), m_Window->GetWindowHandle()))
		return false;


	return true;
}

void ApplicationClass::Run()
{
	MSG msg;


	// Initialziethe message object
	ZeroMemory(&msg, sizeof(MSG));
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;
		else
		{
			Update(msg);
			Render();
		}
	}


	return;
}

void ApplicationClass::Update(MSG &msg)
{
	RECT windowRect;


	m_Input->Update(msg);

	if (m_Input->IsKeyDown((unsigned int)'F'))
	{
		m_Input->KeyUp((unsigned int)'F');
		
		// Toggle full screen and windowed mode
		if (FULL_SCREEN)
		{
			FULL_SCREEN = false;
			m_Window->FullScreenOFF();
		}
		else
		{
			FULL_SCREEN = true;
			m_Window->FullScreenON();
		}

		// Get window rectangle
		GetWindowRect(m_Window->GetWindowHandle(), &windowRect);

		m_Device->ResizeBackBuffer((windowRect.right - windowRect.left), (windowRect.bottom - windowRect.top),
			FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	}

	if (m_Input->IsKeyDown((unsigned int)'C'))
	{
		// Get window rectangle
		GetWindowRect(m_Window->GetWindowHandle(), &windowRect);

		m_Input->KeyUp((unsigned int)'C');

		if (SHOW_CURSOR)
			SHOW_CURSOR = false;
		else
			SHOW_CURSOR = true;

		SetCursorPos(windowRect.left + (windowRect.right - windowRect.left) / 2.0f, windowRect.top + (windowRect.bottom - windowRect.top) / 2.0f);
		ShowCursor(SHOW_CURSOR);
	}


	return;
}

void ApplicationClass::Render()
{	
	static float lightAngle = D3DX_PI / 180.0f;
	float fontSize;
	D3DXVECTOR3 lightDirection;
	D3DXMATRIX 
		worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, 
		rotationMatrix;
	RECT windowRect;

	
	// Get window rectangle
	GetWindowRect(m_Window->GetWindowHandle(), &windowRect);

	if (!SHOW_CURSOR)
	{
		// Create the view matrix
		m_Camera->Render(m_Input,
			windowRect.left + (windowRect.right - windowRect.left) / 2.0f, windowRect.top + (windowRect.bottom - windowRect.top) / 2.0f);
	}

	// Set the font size
	fontSize = (windowRect.bottom - windowRect.top) * 0.05f;

	// Initialize matrices
	m_Device->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Device->GetProjectionMatrix(projectionMatrix);
	m_Device->GetOrthoMatrix(orthoMatrix);

	// Rotate the light
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, 0.0f, lightAngle, 0.0f);
	D3DXVec3TransformCoord(&lightDirection, &m_Light->GetDirection(), &rotationMatrix);
	m_Light->SetDirection(lightDirection.x, lightDirection.y, lightDirection.z);

	// Creates the frustrum
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, viewMatrix, projectionMatrix);

	// Begin rendering
	m_Device->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Check if the model is in the camera's field of view
	if(m_Frustum->ScanBox(m_Hulk->GetCenter(), m_Hulk->GetRadius()))
	{
		// Render model
		m_Hulk->Render(m_Device->GetDevice());

		m_SpecMapShader->Render(m_Device->GetDevice(), m_Hulk->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix,
			m_Hulk->GetTexturesArray(), m_Camera->GetPosition(),
			m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(),
			m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
		/*
		// Render the box that surrounds the model
		m_Hulk->RenderBox(m_Device->GetDevice());

		m_ColorShader->Render(m_Device->GetDevice(), m_Hulk->GetBoxIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix, D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.3f));
			*/
		// Prepare for 2D rendering
		m_Device->TurnZBufferOff();

		// Render text
		m_Text->Render(m_Device->GetDevice(), "Rendering", fontSize, 0, 0, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f),
			worldMatrix, orthoMatrix, windowRect.right-windowRect.left, windowRect.bottom-windowRect.top);

		// Back to 3D rendering mode
		m_Device->TurnZBufferOn();
	}
	else		
	{
		// Prepare for 2D rendering
		m_Device->TurnZBufferOff();
		
		// Render text
		m_Text->Render(m_Device->GetDevice(), "Not Rendering", fontSize, 0, 0, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f),
			worldMatrix, orthoMatrix, windowRect.right-windowRect.left, windowRect.bottom-windowRect.top);
		
		// Back to 3D rendering mode
		m_Device->TurnZBufferOn();
	}	

	m_Device->EndScene();


	return;
}

void ApplicationClass::Shutdown()
{
	// Framework
	if (m_Window)
	{
		m_Window->Shutdown();
		delete m_Window;
		m_Window = 0;
	}

	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	if (m_Device)
	{
		m_Device->Shutdown();
		delete m_Device;
		m_Device = 0;
	}

	// Objects
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
		
	if(m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	if(m_Hulk)
	{
		m_Hulk ->Shutdown();
		delete m_Hulk;
		m_Hulk = 0;
	}

	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	if(m_Text)
	{
		m_Text ->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// Shaders
	if(m_SpecMapShader)
	{
		m_SpecMapShader->Shutdown();
		delete m_SpecMapShader;
		m_SpecMapShader = 0;
	}
	
	if(m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}


	return;
}