#include<windows.h>
#include<Windowsx.h>
#include<stdio.h>		// for File IO
#include<d3d11.h>		// for DirectX11
#include<d3dcompiler.h>	// for D3D11 compiler

#pragma warning (disable: 4838)
#include"XNAMath/xnamath.h"

#include"Camera.h"

#pragma comment (lib,"d3d11.lib")		// import library for d3d11.dll which is dll for Direct3D
#pragma comment (lib,"D3dcompiler.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variable declaration

// For File IO
FILE* gpFile			= NULL;
char gszLogFileName[]	= "Log.txt";

HWND			ghwnd	= NULL;

DWORD			dwStyle;
WINDOWPLACEMENT wpPrev	= { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow			= false;
bool gbEscapeKeyIsPressed	= false;
bool gbFullScreen			= false;

// DirectX11 Specific
float gClearColor[4];		// to hold color value of ClearColor

// interfaces
IDXGISwapChain*			gpIDXGISwapChain			= NULL;		// Implements one or more surfaces for storing rendered data before presenting it to an output.
// We Do Swapping of buffers in double buffer case, so to hold those buffers we main chain so that we can swap them.
ID3D11Device*			gpID3D11Device				= NULL;		// The device interface represents a virtual adapter; it is used to create resources.
// to provide Direct 3D supported Device
// Device : logical Graphic Card
ID3D11DeviceContext*	gpID3D11DeviceContext		= NULL;		// The ID3D11DeviceContext interface represents a device context which generates rendering commands.
ID3D11RenderTargetView* gpID3D11RenderTargetView	= NULL;		// A render-target-view interface identifies the render-target subresources that can be accessed during rendering.
//A rendertarget is a resource that can be written by the output - merger stage at the end of a render pass.Each render - target should also have a corresponding depth - stencil view.
// RenderTargetView : the place where rendering happens


ID3D11VertexShader*		gpID3D11VertexShader							= NULL;
ID3D11PixelShader*		gpID3D11PixelShader								= NULL;

ID3D11InputLayout*		gpID3D11InputLayout								= NULL;

ID3D11Buffer*			gpID3D11Buffer_ConstantBuffer					= NULL;

ID3D11Buffer*			gpID3D11Buffer_VertexBuffer_Triangle_Position	= NULL;
ID3D11Buffer*			gpID3D11Buffer_VertexBuffer_Triangle_Color		= NULL;

ID3D11Buffer*			gpID3D11Buffer_VertexBuffer_Rectangle_Position	= NULL;
ID3D11Buffer*			gpID3D11Buffer_VertexBuffer_Rectangle_Color		= NULL;

ID3D11RasterizerState*	gpID3D11RasterizerState							= NULL;


struct CBUFFER
{
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;
};

XMMATRIX gPerspectiveProjectionMatrix;


// Other Global Variables
float gfAngleTriangle = 0.0f;

Camera camera(vec3(0.0f, 0.0f, 10.0f));

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// declaration of functions
	HRESULT initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);
	void ToggleFullScreen(void);

	// declaration of variables
	MSG			msg;
	WNDCLASSEX	wndclass;
	TCHAR		szAppName[] = TEXT("Direct3D11- Two 2D shapes - Colored Animated");
	HWND		hwnd;

	bool		bDone = false;


	// code
	// log file
	if (fopen_s(&gpFile, gszLogFileName, "w") != 0)
	{
		MessageBox(NULL, TEXT("Unable to create Log File !!"), TEXT("ERROR "), MB_OK | MB_ICONSTOP | MB_TOPMOST);
		exit(0);
	}
	else
	{
		fprintf_s(gpFile, "Log File Is Successfully Opened.\n");
		fclose(gpFile);
	}

	// code
	wndclass.cbSize			= sizeof(WNDCLASSEX);
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hInstance		= hInstance;
	wndclass.lpfnWndProc	= WndProc;
	wndclass.lpszClassName	= szAppName;
	wndclass.lpszMenuName	= NULL;
	wndclass.style			= CS_VREDRAW | CS_HREDRAW | CS_OWNDC;

	if (!RegisterClassEx(&wndclass))
	{
		MessageBox(NULL, TEXT("ERROR : Unable to Register Class !!"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		exit(0);
	}

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("Direct3D11 Two 2D shapes Colored Animated "),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	// ShowWindow(hwnd, iCmdShow);		// This should be ideal call
	ShowWindow(hwnd, SW_MAXIMIZE);		// to show window in maximum size (Not FullScreen)
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	

	// Initialize D3D
	HRESULT hr;
	hr = initialize();
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "Initialize Failed. Exiting Now....\n");
		fclose(gpFile);
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "initialize() Succeeded.\n");
		fclose(gpFile);
	}

	ToggleFullScreen();					// To start window in default fullscreen mode

	// game loop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true)
				{
					bDone = true;
				}
				update();
			}
			display();
		}
	}

	// clean-up
	uninitialize();

	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// declaration of function
	HRESULT resize(int, int);
	void ToggleFullScreen(void);
	void uninitialize(void);
	
	HRESULT hr;

	POINT pt;

	static int old_x_pos;
	static int old_y_pos;

	static int new_x_pos;
	static int new_y_pos;

	static int x_offset;
	static int y_offset;


	// code
	switch (iMsg)
	{
	case WM_CREATE:
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "WM_CREATE() Succeeded.\n");
		fclose(gpFile);

		GetCursorPos(&pt);
		old_x_pos = pt.x;
		old_y_pos = pt.y;

		break;

	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)		// if 0, Window is Activate
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;
	
	case WM_ERASEBKGND:
		return(0);				// return(0) : so that it will go to else part in game loop( to call display() ), instead of DefWindowProc()

	case WM_SIZE:
		if (gpID3D11DeviceContext)
		{
			hr = resize(LOWORD(lParam), HIWORD(lParam));
			if (FAILED(hr))
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "resize() Failed.\n");
				fclose(gpFile);
				return(hr);								// hr is HRESULT and return parameter of WndProc is LRESULT, So upcasting is allowed.
			}
			else
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "resize() Succeeded.\n");
				fclose(gpFile);
			}
		}
		break;
	
	/*
	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;
	*/

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			if (gbEscapeKeyIsPressed == false)
				gbEscapeKeyIsPressed = true;
			break;

		case 0x46:
		case 'f':
			ToggleFullScreen();
			break;

		default:
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		break;

	case WM_CHAR:
		switch (wParam)
		{

		case 'A':
		case 'a':
			camera.ProcessKeyboard(LEFT);
			break;

		case 'W':
		case 'w':
			camera.ProcessKeyboard(FORWARD);
			break;

		case 'D':
		case 'd':
			camera.ProcessKeyboard(RIGHT);
			break;

		case 'S':
		case 's':
			camera.ProcessKeyboard(BACKWARD);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		new_x_pos = GET_X_LPARAM(lParam);
		new_y_pos = GET_Y_LPARAM(lParam);


		x_offset = new_x_pos - old_x_pos;
		y_offset = new_y_pos - old_y_pos;


		if (new_x_pos == 0 && old_x_pos < 10)
		{
			SetCursorPos(1920, new_y_pos);
			new_x_pos = 1920;
		}
		if (new_x_pos == 1919 && old_x_pos > 1900)
		{
			SetCursorPos(0, new_y_pos);
			new_x_pos = 0;
		}

		old_x_pos = new_x_pos;
		old_y_pos = new_y_pos;

	
		camera.ProcessMouseMovement(x_offset, y_offset);

		break;

	case WM_CLOSE:
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "WM_CLOSE.\n");
		fclose(gpFile);
		uninitialize();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen(void)
{

	// declataion of variable
	MONITORINFO mi;

	// code
	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,
					HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					(mi.rcMonitor.right - mi.rcMonitor.left),
					(mi.rcMonitor.bottom - mi.rcMonitor.top),
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullScreen = true;

	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		gbFullScreen = false;

	}
}

HRESULT initialize(void)
{
	// function declaration
	void uninitialize(void);
	HRESULT resize(int, int);

	// variable declarations
	HRESULT hr;


#pragma region BASIC_REQUIRED

	D3D_DRIVER_TYPE		d3dDriverType;
	D3D_DRIVER_TYPE		d3dDriverTypes[]			= { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE };
	// this sequence is important, we want rendering via hardware, if not possible go for warp. if not then at low priorit software

	D3D_FEATURE_LEVEL	d3dFeatureLevel_required	= D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL	d3dFeatureLevel_acquired	= D3D_FEATURE_LEVEL_10_0;		// default, lowest

	UINT createDeviceFlags	= 0;
	UINT numDriverTypes		= 0;
	UINT numFeatureLevels	= 1;			// this is for required feature level 11.1

	numDriverTypes = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]);	// calculating size of array

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;									// Describes a swap chain.
	ZeroMemory((void*)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	dxgiSwapChainDesc.BufferCount						= 1;	// This is for number of buffer we want to create. DirectX by default provide us 1 buffer (Front Buffer), hence we need to create only 1 new (Back) Buffer 
	dxgiSwapChainDesc.BufferDesc.Width					= WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height					= WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;		// UNORM : Unsigned Normalize
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator	= 60;								// 60 FPS count
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator= 1;
	dxgiSwapChainDesc.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT;	// Give me such buffer, which is Render Target (On Which I Want To Show Output)
	dxgiSwapChainDesc.OutputWindow						= ghwnd;
	dxgiSwapChainDesc.SampleDesc.Count					= 1;		// single level sampling
	dxgiSwapChainDesc.SampleDesc.Quality				= 0;		// Quality default (or let it decide by DirectX)
	dxgiSwapChainDesc.Windowed							= TRUE;		// For full screen mode
	
	// We have set our priority, to get the driver type.
	// best is hardware then WARP and at last software
	// In below loop we are iterating over those 3 type and we get break out from loop, which ever found 1st (and hence hardware mentioned first)
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		d3dDriverType = d3dDriverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(							// Creates a device that represents the display adapter and a swap chain used for rendering.
			NULL,						// Graphic Card Adapter, for multiple graphic card setup. NULL = primary
			d3dDriverType,				// Type Of Driver
			NULL,						// If we are going to write software rasterizer
			createDeviceFlags,			// we set 0, for default
			&d3dFeatureLevel_required,
			numFeatureLevels,
			D3D11_SDK_VERSION,
			&dxgiSwapChainDesc,
			&gpIDXGISwapChain,
			&gpID3D11Device,
			&d3dFeatureLevel_acquired,
			&gpID3D11DeviceContext);

		// D3D11CreateDeviceAndSwapChain() is DirectX helper function, as it is not from any class

		if (SUCCEEDED(hr))
		{
			break;
		}
	}
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3D11CreateDeviceAndSwapChain() Failed.\n");
		fclose(gpFile);
		return(hr);								
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3D11CreateDeviceAndSwapChain() Succeeded.\n");
		
		fprintf_s(gpFile, "The Chosen Driver is of : ");
		if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
		{
			fprintf_s(gpFile, "HARDWARE TYPE. \n");
			fflush(gpFile);
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_WARP)
		{
			fprintf_s(gpFile, "WARP TYPE. \n");
			fflush(gpFile);
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			fprintf_s(gpFile, "REFERENCE TYPE. \n");
			fflush(gpFile);
		}
		else
		{
			fprintf_s(gpFile, "UNKNOWN TYPE. \n");
			fflush(gpFile);
		}

		fprintf_s(gpFile, "The Supported Heighest Feature Level is : ");
		if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
		{
			fprintf_s(gpFile, "11.0 \n");
			fflush(gpFile);
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
		{
			fprintf_s(gpFile, "10.1 \n");
			fflush(gpFile);
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
		{
			fprintf_s(gpFile, "10.0 \n");
			fflush(gpFile);
		}
		else
		{
			fprintf_s(gpFile, "UNKNOWN ...\n");
			fflush(gpFile);
		}

		fclose(gpFile);
	}


#pragma endregion

	ID3DBlob* pID3DBlob_VertexShaderCode	= NULL;
	ID3DBlob* pID3DBlob_PixelShaderCode		= NULL;
	ID3DBlob* pID3DBlob_Error				= NULL;

	// VERTEX SHADER
#pragma region VERTEX_SHADER

	const char* vertexShaderSourceCode = 
		"cbuffer ConstantBuffer" \
		"{" \
			"float4x4 worldMatrix;" \
			"float4x4 viewMatrix;" \
			"float4x4 projectionMatrix;" \
		"}" \

		"struct vertex_output" \
		"{" \
			"float4 position	: SV_POSITION;" \
			"float4 color		: COLOR;" \
		"};" \
		
		"vertex_output main(float4 pos : POSITION, float4 color : COLOR)" \
		"{" \
			"vertex_output output;" \

			"float4 temp_position	= mul(worldMatrix,pos);" \
			"temp_position			= mul(viewMatrix,temp_position);" \
			"temp_position			= mul(projectionMatrix,temp_position);" \

			"output.position	= temp_position;" \
			"output.color		= color;" \
			"return(output);" \
		"}";

	hr = D3DCompile(
		vertexShaderSourceCode,
		lstrlenA(vertexShaderSourceCode) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_VertexShaderCode,
		&pID3DBlob_Error);

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() Failed For Vertex Shader : %s .\n", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3DCompile() Succeeded For Vertex Shader.\n");
		fclose(gpFile);
	}

	hr = gpID3D11Device->CreateVertexShader(
		pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11VertexShader);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreateVertexShader() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreateVertexShader() Succeeded.\n");
		fclose(gpFile);
	}

	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, 0, 0);

#pragma endregion

	 // PIXEL SHADER
#pragma region PIXEL_SHADER

	const char* pixelShaderSourceCode =
		"float4 main(float4 pos : SV_POSITION, float4 color : COLOR) : SV_TARGET" \
		"{" \
			"return(color);" \
		"}";

	pID3DBlob_Error = NULL;

	hr = D3DCompile(
		pixelShaderSourceCode,
		lstrlenA(pixelShaderSourceCode) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_PixelShaderCode,
		&pID3DBlob_Error);

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() Failed For Pixel Shader : %s .\n", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3DCompile() Succeeded For Pixel Shader.\n");
		fclose(gpFile);
	}

	hr = gpID3D11Device->CreatePixelShader(
		pID3DBlob_PixelShaderCode->GetBufferPointer(),
		pID3DBlob_PixelShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11PixelShader);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreatePixelShader() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreatePixelShader() Succeeded.\n");
		fclose(gpFile);
	}

	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, 0, 0);
	 
#pragma endregion


	// Create and set input layout
#pragma region INPUT_LAYOUT

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];

	ZeroMemory(inputElementDesc, sizeof(D3D11_INPUT_ELEMENT_DESC));

	inputElementDesc[0].SemanticName			= "POSITION";
	inputElementDesc[0].SemanticIndex			= 0;
	inputElementDesc[0].Format					= DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot				= 0;							// 0th slot for vertex data
	inputElementDesc[0].AlignedByteOffset		= 0;
	inputElementDesc[0].InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate	= 0;

	inputElementDesc[1].SemanticName			= "COLOR";
	inputElementDesc[1].SemanticIndex			= 0;
	inputElementDesc[1].Format					= DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[1].InputSlot				= 1;							// 1st slot for color data
	inputElementDesc[1].AlignedByteOffset		= D3D11_APPEND_ALIGNED_ELEMENT;	// as we have 0th slot occupied, we need to alined and append data
	inputElementDesc[1].InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].InstanceDataStepRate	= 0;

	hr = gpID3D11Device->CreateInputLayout(
		inputElementDesc,
		_ARRAYSIZE(inputElementDesc),						// size of array
		pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		&gpID3D11InputLayout);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreateInputLayout() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreateInputLayout() Succeeded.\n");
		fclose(gpFile);
	}

	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);

#pragma endregion

	// release 
	pID3DBlob_VertexShaderCode->Release();
	pID3DBlob_VertexShaderCode = NULL;

	pID3DBlob_PixelShaderCode->Release();
	pID3DBlob_PixelShaderCode = NULL;


	float triangle_vertices[] =
	{
		 0.0f, 1.0f,0.0f,	// apex
		 1.0f,-1.0f,0.0f,	// right
		-1.0f,-1.0f,0.0f	// left
	};

	float triangle_colors[] =
	{
		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f
	};

	float rectangle_vertices[] =
	{
		-1.0f, 1.0f,0.0f,	// top left
		 1.0f, 1.0f,0.0f,	// top right
		-1.0f,-1.0f,0.0f,	// bottom left
		 1.0f,-1.0f,0.0f	// bottom right
	};

	float rectangle_colors[] =
	{
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f
	};

	// create vertex buffer
#pragma region VERTEX_BUFFER
	D3D11_BUFFER_DESC			bufferDesc;
	D3D11_MAPPED_SUBRESOURCE	mappedSubresource;

	// Triangle 
	// VERTEX
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth		= sizeof(float) * _ARRAYSIZE(triangle_vertices);
	bufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(
		&bufferDesc,
		NULL,
		&gpID3D11Buffer_VertexBuffer_Triangle_Position);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreateBuffer() Failed for vertex buffer - vertices.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreateBuffer() Succeeded for vertex buffer - vertices.\n");
		fclose(gpFile);
	}

	// copy vertices into above buffer
	
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_VertexBuffer_Triangle_Position,
		NULL,
		D3D11_MAP_WRITE_DISCARD,
		NULL,
		&mappedSubresource);

	memcpy(mappedSubresource.pData, triangle_vertices, sizeof(triangle_vertices));

	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Triangle_Position, NULL);


	// COLOR
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth		= sizeof(float) * _ARRAYSIZE(triangle_colors);
	bufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(
		&bufferDesc,
		NULL,
		&gpID3D11Buffer_VertexBuffer_Triangle_Color);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreateBuffer() Failed for vertex buffer - color .\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreateBuffer() Succeeded for vertex buffer - color .\n");
		fclose(gpFile);
	}

	// copy vertices into above buffer
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_VertexBuffer_Triangle_Color,
		NULL,
		D3D11_MAP_WRITE_DISCARD,
		NULL,
		&mappedSubresource);

	memcpy(mappedSubresource.pData, triangle_colors, sizeof(triangle_colors));

	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Triangle_Color, NULL);




	// Rectangle 
	// VERTEX
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth		= sizeof(float) * _ARRAYSIZE(rectangle_vertices);
	bufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(
		&bufferDesc,
		NULL,
		&gpID3D11Buffer_VertexBuffer_Rectangle_Position);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreateBuffer() Failed for vertex buffer - vertices.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreateBuffer() Succeeded for vertex buffer - vertices.\n");
		fclose(gpFile);
	}

	// copy vertices into above buffer

	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_VertexBuffer_Rectangle_Position,
		NULL,
		D3D11_MAP_WRITE_DISCARD,
		NULL,
		&mappedSubresource);

	memcpy(mappedSubresource.pData, rectangle_vertices, sizeof(rectangle_vertices));

	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Rectangle_Position, NULL);


	// COLOR
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth		= sizeof(float) * _ARRAYSIZE(rectangle_colors);
	bufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(
		&bufferDesc,
		NULL,
		&gpID3D11Buffer_VertexBuffer_Rectangle_Color);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreateBuffer() Failed for vertex buffer - color .\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreateBuffer() Succeeded for vertex buffer - color .\n");
		fclose(gpFile);
	}

	// copy vertices into above buffer
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gpID3D11DeviceContext->Map(
		gpID3D11Buffer_VertexBuffer_Rectangle_Color,
		NULL,
		D3D11_MAP_WRITE_DISCARD,
		NULL,
		&mappedSubresource);

	memcpy(mappedSubresource.pData, rectangle_colors, sizeof(rectangle_colors));

	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Rectangle_Color, NULL);

#pragma endregion

	// define and set constant buffer
#pragma region CONSTANT_BUFFER
	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;

	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));

	bufferDesc_ConstantBuffer.Usage		= D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = gpID3D11Device->CreateBuffer(
		&bufferDesc_ConstantBuffer,
		nullptr,
		&gpID3D11Buffer_ConstantBuffer);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreateBuffer() Failed for Constant Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreateBuffer() Succeeded for Constant Buffer.\n");
		fclose(gpFile);
	}

	gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);
#pragma endregion

#pragma region RASTER_STAGE
	// set rasterization state
	// In D3D, backface culling is by default 'ON'
	// Means backface of geometry will not be visible.
	// this causes our 2D tringles backface to vanish on rotation
	// so set culling 'OFF'

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory((void*)&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.AntialiasedLineEnable	= FALSE;
	rasterizerDesc.CullMode					= D3D11_CULL_NONE;		// this will allow both side of 2D geometry visible on rotation
	rasterizerDesc.DepthBias				= 0;
	rasterizerDesc.DepthBiasClamp			= 0.0f;
	rasterizerDesc.DepthClipEnable			= TRUE;
	rasterizerDesc.FillMode					= D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise	= FALSE;
	rasterizerDesc.MultisampleEnable		= FALSE;
	rasterizerDesc.ScissorEnable			= FALSE;
	rasterizerDesc.SlopeScaledDepthBias		= 0.0f;

	hr = gpID3D11Device->CreateRasterizerState(&rasterizerDesc, &gpID3D11RasterizerState);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreateRasterizerState() Failed for Constant Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device->CreateRasterizerState() Succeeded for Constant Buffer.\n");
		fclose(gpFile);
	}

	gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);

#pragma endregion

	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 0.1f;
	gClearColor[3] = 1.0f;

	gPerspectiveProjectionMatrix = XMMatrixIdentity();

	hr = resize(WIN_WIDTH, WIN_HEIGHT);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "resize() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "resize() Succeeded.\n");
		fclose(gpFile);
	}

	return(S_OK);
}

HRESULT resize(int width, int height)
{
	// code
	HRESULT hr = S_OK;

	// free any size-dependent resource
	// doing safe release beacause, if we are going to enter in resize for first time we will not find this value, as we are createing render target view in this function only
	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	gpIDXGISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);		// Changes the swap chain's back buffer size, format, and number of buffers.
	// This is important. As we have set SwapChain Information in Initialize(), which is called only once when program started. at that time windows size might be 800 by 600, 
	// and full screen might be for 1920 by 1080. hence whenever size of window is change, we need to update that SwapChain structure.

	ID3D11Texture2D* pID3D11Texture2D_BackBuffer;		// we are using texture buffer because texture memory is fastest memory, texture memory is fast accessable 
	gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pID3D11Texture2D_BackBuffer);				// Accesses one of the swap-chain's back buffers.
	// front buffer is default, which is provided by DirectX. 
	// we have created(in initialize() for dxgiSwapChainDesc.BufferCount) one buffer which is back buffer 

	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer, NULL, &gpID3D11RenderTargetView);		// Creates a render-target view for accessing resource data.
	// As window is resized our buffer(back buffer and front buffer) is resized and as buffer is resized we need to resize the render target view, which we have released

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "CreateRenderTargetView() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "CreateRenderTargetView() Succeeded.\n");
		fclose(gpFile);
	}

	pID3D11Texture2D_BackBuffer->Release();
	pID3D11Texture2D_BackBuffer = NULL;

	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, NULL);		// Bind one or more render targets atomically and the depth-stencil buffer to the output-merger stage.
	// to change render target view from pipeline as well (as their might be old RenderTargetView attached in pipeline, and that also needs to be update)
	// DeviceContext control Pipeline
	/*
	OMSetRenderTargets : Target's' because their are multiple Render Targets. 1. Render Target View and 2. Depth Stencil
	*/


	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX	= 0;
	d3dViewPort.TopLeftY	= 0;
	d3dViewPort.Width		= (float)width;
	d3dViewPort.Height		= (float)height;
	d3dViewPort.MinDepth	= 0.0f;
	d3dViewPort.MaxDepth	= 1.0f;

	// pipeline is having old view port, so go to pipeline and update the view port
	gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort);		// Bind an array of viewports to the rasterizer stage of the pipeline.

	// set perspective matrix
	gPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),
		(float)width / (float)height,
		0.1f, 
		100.0f);

	return(hr);
}

void display(void)
{
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);		// Set all the elements in a render target to one value.

	UINT strid	= sizeof(float) * 3;
	UINT offset = 0;
	
	// TRIANGLE
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_Triangle_Position, &strid, &offset);
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_Triangle_Color, &strid, &offset);

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX worldMatrix		= XMMatrixIdentity();
	XMMATRIX viewMatrix			= XMMatrixIdentity();
	XMMATRIX translationMatrix	= XMMatrixIdentity();
	XMMATRIX rotationMatrix		= XMMatrixIdentity();

	translationMatrix			= XMMatrixTranslation(-2.0f, 0.0f, 6.0f);
	rotationMatrix				= XMMatrixRotationY(-gfAngleTriangle);

	worldMatrix					= translationMatrix;
	worldMatrix					= rotationMatrix * worldMatrix;
	
	CBUFFER constantBuffer;

	constantBuffer.WorldMatrix		= worldMatrix;
	//constantBuffer.ViewMatrix		= XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -10.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	constantBuffer.ViewMatrix		= camera.GetViewMatrix();
	constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

	gpID3D11DeviceContext->Draw(3, 0);


	// Rectangle
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_Rectangle_Position, &strid, &offset);
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_Rectangle_Color, &strid, &offset);

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	worldMatrix			= XMMatrixIdentity();
	viewMatrix			= XMMatrixIdentity();
	translationMatrix	= XMMatrixIdentity();
	rotationMatrix		= XMMatrixIdentity();

	translationMatrix	= XMMatrixTranslation(2.0f, 0.0f, 6.0f);
	rotationMatrix		= XMMatrixRotationX(-gfAngleTriangle);

	worldMatrix = translationMatrix;
	worldMatrix = rotationMatrix * worldMatrix;

	constantBuffer.WorldMatrix		= worldMatrix;
	//constantBuffer.ViewMatrix		= XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	constantBuffer.ViewMatrix		= camera.GetViewMatrix();
	constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

	gpID3D11DeviceContext->Draw(4, 0);



	gpIDXGISwapChain->Present(0, 0);		// Presents a rendered image to the user.
	// similar to wglSwapBuffer()
}

void update(void)
{
	//  code
	if (gfAngleTriangle < 360.0f)
		gfAngleTriangle = gfAngleTriangle + 0.0002f;
	else
		gfAngleTriangle = 0.0f;
}

void uninitialize(void)
{
	// code

	if (gpID3D11RasterizerState)
	{
		gpID3D11RasterizerState->Release();
		gpID3D11RasterizerState = NULL;
	}

	if (gpID3D11Buffer_ConstantBuffer)
	{
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer = NULL;
	}

	if (gpID3D11InputLayout)
	{
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_Triangle_Position)
	{
		gpID3D11Buffer_VertexBuffer_Triangle_Position->Release();
		gpID3D11Buffer_VertexBuffer_Triangle_Position = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_Triangle_Color)
	{
		gpID3D11Buffer_VertexBuffer_Triangle_Color->Release();
		gpID3D11Buffer_VertexBuffer_Triangle_Color = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_Rectangle_Position)
	{
		gpID3D11Buffer_VertexBuffer_Rectangle_Position->Release();
		gpID3D11Buffer_VertexBuffer_Rectangle_Position = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_Rectangle_Color)
	{
		gpID3D11Buffer_VertexBuffer_Rectangle_Color->Release();
		gpID3D11Buffer_VertexBuffer_Rectangle_Color = NULL;
	}

	if (gpID3D11PixelShader)
	{
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}

	if (gpID3D11VertexShader)
	{
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
	}

	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	if (gpIDXGISwapChain)
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}

	if (gpID3D11DeviceContext)
	{
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}

	if (gpID3D11Device)
	{
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}
	
	if (gpFile)
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "Uninitialize() Succeeded.\n");
		fprintf_s(gpFile, "Log File is sucessfully closed. \n");
		fclose(gpFile);
	}
}
