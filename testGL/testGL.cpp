// testGL.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "testGL.h"
#include <glad/glad.h>
#include <gl/GL.h>
#include <string>
#include <functional>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
// 初始化GL
bool initGL(HWND hwd);
//  绘制函数
void draw(HWND hwd);
/// <summary>
/// 圆环内径坐标vec2
/// </summary>
float positionIn[720];
/// <summary>
/// 圆环外径坐标vec2
/// </summary>
float positionOut[720];
/// <summary>
/// 圆环颜色描述
/// </summary>
float color[1080];
float* highlightB;
unsigned int buff;
unsigned int program[3];
/// 可更改参数
/// <summary>
/// 圆心
/// </summary>
const float center[2] = { 0.0f,0.0f };
/// <summary>
/// 半径
/// </summary>
const float r = 0.2;
/// <summary>
/// 圆环宽度
/// </summary>
const float width = 0.05f;
/// <summary>
/// 高亮颜色
/// </summary>
const float highlight[3] = { 1.0f,1.0f,1.0f };
/// <summary>
/// 高亮条长度
/// </summary>
const float lenth = 0.2f;
/// <summary>
/// 渐变开始颜色
/// </summary>
const float beginColor[3] = { 0.1f,0.1f,0.1f };
/// <summary>
/// 渐变结束颜色
/// </summary>
const float endColor[3] = { 0.9f,0.9f,0.9f };
/// <summary>
/// 渐变开始角度 0度为正向x轴方向
/// </summary>
const float rate = 270.0f;

void* GetAnyGLFuncAddress(const char* name)
{
    void* p = (void*)wglGetProcAddress(name);
    if (p == 0 ||
        (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
        (p == (void*)-1))
    {
        HMODULE module = LoadLibraryA("opengl32.dll");
        p = (void*)GetProcAddress(module, name);
    }

    return p;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TESTGL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTGL));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTGL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TESTGL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   if (!initGL(hWnd)) {
       return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
        draw(hWnd);
        }
        break;
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        glDeleteBuffers(1, &buff);
        HDC h = wglGetCurrentDC();
        HGLRC hrc = wglGetCurrentContext();
        wglMakeCurrent(h, hrc);
        if (hrc) {
            wglDeleteContext(hrc);
        }
        if (h) {
            ReleaseDC(hWnd, h);
        }
    }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

bool initGL(HWND hwd) {
    HDC hdc = GetDC(hwd);
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd  
        1,                                // version number  
        PFD_DRAW_TO_WINDOW |              // support window  
        PFD_SUPPORT_OPENGL |              // support OpenGL  
        PFD_DOUBLEBUFFER,                 // double buffered  
        PFD_TYPE_RGBA,                    // RGBA type  
        24,                               // 24-bit color depth  
        0, 0, 0, 0, 0, 0,                 // color bits ignored  
        0,                                // no alpha buffer  
        0,                                // shift bit ignored  
        0,                                // no accumulation buffer  
        0, 0, 0, 0,                       // accum bits ignored  
        32,                               // 32-bit z-buffer      
        0,                                // no stencil buffer  
        0,                                // no auxiliary buffer  
        PFD_MAIN_PLANE,                   // main layer  
        0,                                // reserved  
        0, 0, 0                           // layer masks ignored  
    };
    int iPixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, iPixelFormat, &pfd);
    HGLRC hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);
    /*if (version == NULL) {
        /// 计算圆环轮廓 可以加OMP
        for (int i = 0; i < 360; ++i) {
            positionIn[i * 2] = fmaf(cosf(rate + i), r, center[0]);
            positionIn[i * 2 + 1] = fmaf(sinf(rate + i), r, center[1]);
            positionOut[i * 2] = fmaf(cosf(rate + i), r + width, center[0]);
            positionOut[i * 2 + 1] = fmaf(sinf(rate + i), r + width, center[1]);
        }
        /// 计算圆环轮廓渐变
        float r = (endColor[0] - beginColor[0])/180;
        float g = (endColor[1] - beginColor[1])/180;
        float b = (endColor[2] - beginColor[2])/180;
        color[0] = beginColor[0];
        color[1] = beginColor[1];
        color[2] = beginColor[2];
        for (int i = 1; i < 180; ++i) {
            color[i * 3] = beginColor[0] + r * i;
            color[i * 3 + 1] = beginColor[1] + g * i;
            color[i * 3 + 2] = beginColor[2] + b * i;
            color[(360 - i) * 3] = beginColor[0] + r * i;
            color[(360 - i) * 3 + 1] = beginColor[1] + g * i;
            color[(360 - i) * 3 + 2] = beginColor[2] + b * i;
        }
        color[3 * 180] = endColor[0];
        color[3 * 180 + 1] = endColor[1];
        color[3 * 180 + 2] = endColor[2];
        /// 计算白色圆弧
        float ratef = lenth * 180 / r / 3.1415926;
        int rateI = (int)ratef;
        float rateB = rate - rateI / 2;
        highlightB = new float[rateI * 2 + 2];
        highlightB[0] = center[0];
        highlightB[1] = center[1];
        for (int i = 0; i < rateI; ++i) {
            highlightB[(i + 1) * 2] = fmaf(cosf(rateB + i), r + 0.05, center[0]);
            highlightB[(i + 1) * 2 + 1] = fmaf(sinf(rateB + i), r + 0.05, center[1]);
        }
        /*void* f = GetAnyGLFuncAddress("glGenBuffers");
        ((void(*)(GLsizei, GLuint*))(f))(5, buff);
    }
    else
    {
        // OpenGL 4.3+ glDispatchCompute
    }*/
    /// 计算圆环轮廓 可以加OMP
    for (int i = 0; i < 360; ++i) {
        positionIn[i * 2] = fmaf(cosf(rate + i), r, center[0]);
        positionIn[i * 2 + 1] = fmaf(sinf(rate + i), r, center[1]);
        positionOut[i * 2] = fmaf(cosf(rate + i), r + width, center[0]);
        positionOut[i * 2 + 1] = fmaf(sinf(rate + i), r + width, center[1]);
    }
    /// 计算圆环轮廓渐变
    float r = (endColor[0] - beginColor[0]) / 180;
    float g = (endColor[1] - beginColor[1]) / 180;
    float b = (endColor[2] - beginColor[2]) / 180;
    color[0] = beginColor[0];
    color[1] = beginColor[1];
    color[2] = beginColor[2];
    for (int i = 1; i < 180; ++i) {
        color[i * 3] = beginColor[0] + r * i;
        color[i * 3 + 1] = beginColor[1] + g * i;
        color[i * 3 + 2] = beginColor[2] + b * i;
        color[(360 - i) * 3] = beginColor[0] + r * i;
        color[(360 - i) * 3 + 1] = beginColor[1] + g * i;
        color[(360 - i) * 3 + 2] = beginColor[2] + b * i;
    }
    color[3 * 180] = endColor[0];
    color[3 * 180 + 1] = endColor[1];
    color[3 * 180 + 2] = endColor[2];
    /// 计算白色圆弧
    float ratef = lenth * 180 / r / 3.1415926;
    int rateI = (int)ratef;
    float rateB = rate - rateI / 2;
    highlightB = new float[rateI * 2 + 2];
    unsigned int hbLenth = rateI * 2 + 2;
    highlightB[0] = center[0];
    highlightB[1] = center[1];
    for (int i = 0; i < rateI; ++i) {
        highlightB[(i + 1) * 2] = fmaf(cosf(rateB + i), r + 0.05, center[0]);
        highlightB[(i + 1) * 2 + 1] = fmaf(sinf(rateB + i), r + 0.05, center[1]);
    }
    glGenBuffers(1, &buff);
    glBindBuffer(GL_ARRAY_BUFFER, buff);
    glBufferData(GL_ARRAY_BUFFER, hbLenth * sizeof(float), highlightB, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
    program[0] = glCreateProgram();
    const char* hlVSs = "attribute vec4 position;\nvoid main()\n{\ngl_Position = position;\n}\n";
    unsigned int hlVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(hlVS, 1, &hlVSs, nullptr);
    glCompileShader(hlVS);
    int res;
    glGetShaderiv(hlVS, GL_COMPILE_STATUS, &res);
    if (res == GL_FALSE) {
        return false;
    }
    return true;
}

void draw(HWND hwd) {

}
