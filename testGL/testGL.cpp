// testGL.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "testGL.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <string>

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
bool initGL(void);
//  绘制函数
void draw(HWND hwd);
/// <summary>
/// 圆环内径坐标vec2
/// </summary>
float positionIn[720];
/// <summary>
/// 圆环外径坐标vec3
/// </summary>
float positionOut[720];
/// <summary>
/// 圆环颜色描述
/// </summary>
float color[1080];
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
    if (!initGL()) {
        return -1;
    }

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
            case WM_PAINT:
                draw(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
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

bool initGL(void) {
    const GLubyte* version = glGetString(GL_VERSION);
    // 如果version为NULL 则是Windows自带的OpenGL
    if(version==NULL){
        /// 计算圆环轮廓 可以加OMP
        for (int i = 0; i < 360; ++i) {
            positionIn[i * 2] = fmaf(cosf(rate + i), r, center[0]);
            positionIn[i * 2 + 1] = fmaf(sinf(rate + i), r, center[1]);
            positionOut[i * 2] = fmaf(cosf(rate + i), r + width, center[0]);
            positionOut[i * 2 + 1] = fmaf(sinf(rate + i), r + width, center[1]);
        }
        /// 计算圆环轮廓渐变
        float distent = sqrtf(fmaf(positionOut[0] - positionOut[180], positionOut[0] - positionOut[180], 0) + fmaf(positionOut[1] - positionOut[181], positionOut[1] - positionOut[181], 0));

    }
    else
    {
        // OpenGL 4.3+ glDispatchCompute
    }
    return true;
}

void draw(HWND hwd) {

}
