#include<windows.h>
#include<stdio.h>
#include<GL/glew.h>
#include<gl/GL.h>
#define _USE_MATH_DEFINES 1
#include<math.h>
#include"vmath.h"
#include<time.h>
#include<vector>

// for libraries
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

using namespace vmath;

// declaration of global function
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// declaration of global variables
HWND			ghwnd;
bool			bIsFullScreen = false;
DWORD			dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

// for openGL
HDC				ghdc = NULL;
HGLRC			ghrc = NULL;
bool			gbActiveWindow = false;

// for file io
FILE *gpFile = NULL;


// enum
enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

// global variables
GLuint gShaderProgramObject;

GLuint vao;
GLuint vbo_position;
GLuint vbo_color;
GLuint mvpUniform;

mat4 PerspectiveProjectionMatrix;


std::vector<float> vertices_position;
std::vector<float> vertices_color;

// for different attaractor
bool startAnimation = false;
GLfloat gfAmplitude = 0.0f;

// main
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// declaration of functions
	int initialize(void);
	void display(void);
	void update(void);
	void ToggleFullScreen(void);

	// declaration of variables
	MSG			msg;
	WNDCLASSEX	wndclass;
	TCHAR		szAppName[] = TEXT("Heart Using Sine Wave");
	HWND		hwnd;

	bool		bDone = false;
	int			iRet = 0;

	// code
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Unable to create Log File !!"), TEXT("ERROR "), MB_OK | MB_ICONERROR);
		exit(0);
	}
	else
	{
		//MessageBox(NULL, TEXT("Successfully created Log File !!"), TEXT("SUCCESS "), MB_OK);
	}

	// code
	wndclass.cbClsExtra = 0;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;

	if (!RegisterClassEx(&wndclass))
	{
		MessageBox(NULL, TEXT("ERROR : Unable to Register Class !!"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		exit(0);
	}

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("Heart Using Sine Wave"),
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

	iRet = initialize();
	if (iRet == -1)
	{
		fprintf(gpFile, "ChoosePixelFormat() Fail \n");
		fflush(gpFile);
		DestroyWindow(hwnd);
	}
	else if (iRet == -2)
	{
		fprintf(gpFile, "SetPixelFormat() Fail \n");
		fflush(gpFile);
		DestroyWindow(hwnd);
	}
	else if (iRet == -3)
	{
		fprintf(gpFile, "wglCreateContext() Fail \n");
		fflush(gpFile);
		DestroyWindow(hwnd);
	}
	else if (iRet == -4)
	{
		fprintf(gpFile, "wglMakeCurrent() Fail \n");
		fflush(gpFile);
		DestroyWindow(hwnd);
	}
	else if (iRet == -5)
	{
		fprintf(gpFile, "glewInit() Fail \n");
		fflush(gpFile);
		DestroyWindow(hwnd);
	}
	else if (iRet == -6)
	{
		fprintf(gpFile, "Some Compile time error occurs at Vertex Shader \n");
		fflush(gpFile);
		DestroyWindow(hwnd);
	}
	else if (iRet == -7)
	{
		fprintf(gpFile, "Some Compile time error occurs at Fragment Shader \n");
		fflush(gpFile);
		DestroyWindow(hwnd);
	}
	else if (iRet == -8)
	{
		fprintf(gpFile, "Some Link time error occurs at Shader Program linking time \n");
		fflush(gpFile);
		DestroyWindow(hwnd);
	}
	else
	{
		fprintf(gpFile, "Initialization Successed \n");
		fflush(gpFile);
	}


	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	ToggleFullScreen();

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
				update();
			}
			display();
		}
	}

	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// declaration of function
	void ToggleFullScreen(void);
	void resize(int, int);
	//void display(void);
	void uninitialize(void);

	// code
	switch (iMsg)
	{
	case WM_CREATE:
		fprintf(gpFile, "In WM_CREATE \n");
		fflush(gpFile);
		break;

	case WM_SETFOCUS:
		fprintf(gpFile, "In WM_SETFOCUS \n");
		fflush(gpFile);
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		fprintf(gpFile, "In WM_KILLFOCUS \n");
		fflush(gpFile);
		gbActiveWindow = false;
		break;

	case WM_SIZE:
		fprintf(gpFile, "In WM_SIZE \n");
		fflush(gpFile);
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_ERASEBKGND:
		return(0);				// return(0) : so that it will go to else part in game loop( to call display() ), instead of DefWindowProc()

	case WM_CLOSE:
		fprintf(gpFile, "IN WM_CLOSE \n");
		fflush(gpFile);
		DestroyWindow(hwnd);
		break;

	case WM_KEYDOWN:
		fprintf(gpFile, "In WM_KEYDOWN \n");
		fflush(gpFile);
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		case 0x46:
		case 'f':
			ToggleFullScreen();
			//UpdateWindow(hwnd);
			break;

		case VK_SPACE:
			startAnimation = !startAnimation;
			break;
		}
		break;

	case WM_DESTROY:
		fprintf(gpFile, "In WM_DESTROY \n");
		fflush(gpFile);
		uninitialize();
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen(void)
{

	fprintf(gpFile, "In : ToggleFullScreen() \n");
	fflush(gpFile);

	// declataion of variable
	MONITORINFO mi;

	// code
	if (bIsFullScreen == false)
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
		bIsFullScreen = true;

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
		bIsFullScreen = false;

	}
	fprintf(gpFile, "OUT : ToggleFullScreen() \n");
	fflush(gpFile);
}

int initialize()
{
	fprintf(gpFile, "IN : initialize() \n");
	fflush(gpFile);

	// function declaration
	void resize(int, int);

	// variables declarations
	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;


	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	GLenum result;

	// variables for error checking 
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar * szInfoLog = NULL;
	GLint iProgramLinkStatus = 0;

	// code
#pragma region BASIC_OGL_PART

	// initialize pfd structure
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	//else use
	// memset((void *)&pfd,NULL,sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;								// PFG_RGBA ;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;			// for depth (3D)

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		return(-1);
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		return(-2);
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		return(-3);
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		return(-4);
	}

	result = glewInit();
	if (result != GLEW_OK)
	{
		return(-5);
	}

#pragma endregion


	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	const GLchar * vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vColor;" \

		"uniform mat4 u_mvp_matrix;" \

		"out vec3 out_color;"
		
		"void main(void)" \
		"{" \
			"gl_Position	= u_mvp_matrix * vPosition;" \
			"out_color		= vColor;" \
		"}";

// VS
if(1){
	glShaderSource(gVertexShaderObject, 1, (GLchar **)&vertexShaderSourceCode, NULL);
	glCompileShader(gVertexShaderObject);

	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "\n--------------------------------------------------- \nCompile time log for Vertex Shader : \n %s", szInfoLog);
				fflush(gpFile);
				free(szInfoLog);
				return(-6);
			}
		}
	}
}

	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar * fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec3 out_color;" \

		"out vec4 FragColor;" \
		
		"void main(void)" \
		"{" \
			"FragColor = vec4(out_color,0.8f);" \
		"}";
//FS
if (1) {
		glShaderSource(gFragmentShaderObject, 1, (GLchar**)&fragmentShaderSourceCode, NULL);
		glCompileShader(gFragmentShaderObject);

		iShaderCompileStatus = 0;
		iInfoLogLength = 0;
		szInfoLog = NULL;

		glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
		if (iShaderCompileStatus == GL_FALSE)
		{
			glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
			if (iInfoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(iInfoLogLength);
				if (szInfoLog != NULL)
				{
					GLsizei written;
					glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
					fprintf(gpFile, "\n--------------------------------------------------- \nCompile time log for Fragment Shader : \n %s", szInfoLog);
					fflush(gpFile);
					free(szInfoLog);
					return(-7);
				}
			}
		}
}

	gShaderProgramObject = glCreateProgram();
	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");
	glLinkProgram(gShaderProgramObject);

// SPO
if(1){
	iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "\n--------------------------------------------------- \nLink time log for Shader Program : \n %s", szInfoLog);
				fflush(gpFile);
				free(szInfoLog);
				return(-8);
			}
		}
	}
}

	mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");

// obj
if(1){
	
	for (GLfloat i = -10.0f; i <= 10.0f; i = i + 0.0025f)
	{
		
		vertices_position.push_back(i);
		vertices_color.push_back((i + 10.0f) / 20.0f);
		//vertices_color.push_back(1.0f);

		vertices_position.push_back(0.0f);
		//vertices_color.push_back((i + 5.0f)/ 10.0f);
		vertices_color.push_back(0.0f);

		vertices_position.push_back(0.0f);
		//vertices_color.push_back((i + 0.0f) / 10.0f);
		vertices_color.push_back(0.0f);
			
	}

	// create vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// create vbo_position
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, vertices_position.size() * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, vertices_color.size() * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

	// for blue screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// for depth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL, 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(4.0f);

	
	PerspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);			// this leads to upper part cut

	fprintf(gpFile, "OUT : initialize() \n");
	fflush(gpFile);

	return(0);
}

void resize(int iWidth, int iHeight)
{
	fprintf(gpFile, "IN : resize() \n");
	fflush(gpFile);

	if (iHeight == 0)
	{
		iHeight = 1;
	}

	glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight);
	
//gluPerspective (45.0f, ((GLfloat)iWidth / (GLfloat)iHeight), 0.1f, 100.0f);
	PerspectiveProjectionMatrix = perspective(45.0f, ((GLfloat)iWidth / (GLfloat)iHeight), 0.1f, 1000.0f);

	fprintf(gpFile, "OUT : resize() \n");
	fflush(gpFile);
}

void display(void)
{

	for (int i = 0; i < vertices_position.size(); i = i + 3)
	{
		GLfloat temp_x				= vertices_position[i + 0];
		GLfloat Y1					= pow(fabs(temp_x), (2.0f / 3.0f)) + (0.9f * pow((3.3f - pow(fabs(temp_x), 2.0f)), (1.0f / 2.0f))) * sin(gfAmplitude * M_PI * temp_x);
		vertices_position[i + 1]	= Y1;	
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);

	mat4 translationMatrix;
	mat4 rotationMatrix;
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	
	translationMatrix			= mat4::identity();
	rotationMatrix				= mat4::identity();
	modelViewMatrix				= mat4::identity();
	modelViewProjectionMatrix	= mat4::identity();
	
	translationMatrix	= translate(0.0f, 0.0f, -10.0f);
	//rotationMatrix		= rotate(147.0f,0.0f,1.0f,0.0f);

	modelViewMatrix				= translationMatrix;
	modelViewMatrix				= modelViewMatrix * rotationMatrix;
	modelViewProjectionMatrix	= PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	// bind with vao
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, vertices_position.size() * sizeof(float), &vertices_position[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, vertices_color.size() * sizeof(float), &vertices_color[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_POINTS,0, vertices_position.size());
	//glDrawArrays(GL_LINE_LOOP,0, vertices_position.size());
	glBindVertexArray(0);

	glUseProgram(0);

	SwapBuffers(ghdc);	// for double buffer
}

void update(void)
{
	static int siGoBack = 0;

	if (startAnimation)
	{
		if (gfAmplitude <= 7.5f && siGoBack == 0)
		{
			gfAmplitude = gfAmplitude + 0.01f;
		}
		else
		{
			siGoBack = 1;
		}
		if (gfAmplitude >= -7.5f && siGoBack == 1)
		{
			gfAmplitude = gfAmplitude - 0.01f;
		}
		else
		{
			siGoBack = 0;
		}
	}
}

void uninitialize(void)
{

	fprintf(gpFile, "IN : uninitialize() \n");
	fflush(gpFile);

	GLsizei ShaderCount;
	GLsizei ShaderNumber;

	if (bIsFullScreen == true)
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
	}

	if (vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	if (gShaderProgramObject)
	{
		glUseProgram(gShaderProgramObject);
		glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &ShaderCount);
		GLuint *pShaders = (GLuint *)malloc(sizeof(GLuint)*ShaderCount);
		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObject, ShaderCount, &ShaderCount, pShaders);
			for (ShaderNumber = 0; ShaderNumber < ShaderCount; ShaderNumber++)
			{
				glDetachShader(gShaderProgramObject, pShaders[ShaderNumber]);
				glDeleteShader(pShaders[ShaderNumber]);
				pShaders[ShaderNumber] = 0;
			}
			free(pShaders);
		}
		glDeleteProgram(gShaderProgramObject);
		gShaderProgramObject = 0;
		glUseProgram(0);
	}


	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "Log File is Closed !!\n\n");
		fflush(gpFile);
		fclose(gpFile);
		gpFile = NULL;
	}

}
