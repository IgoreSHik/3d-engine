#include <windows.h>
#include <gl/gl.h>
#include "linmath.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

float plaszcz[] = {1,1,0,   1,-1,0,   -1,-1,0,   -1,1,0};
float xKat = 20;
float zKat = 0;
POINTFLOAT place = {0,0};

float vert[] = {-2,13,-1,   1,13,-1,   3,9,-1,   0,7,-1,   -3,9,-1,
                -2,13,10,   1,13,10,   3,9,10,   0,7,10,   -3,9,10};

GLuint index1[] = {0,1,6,5};
GLuint index2[] = {1,2,7,6};
GLuint index3[] = {2,3,8,7};
GLuint index4[] = {3,4,9,8};
GLuint index5[] = {0,4,9,5};

void showEverything()
{
    glEnableClientState(GL_VERTEX_ARRAY);

        glColor3f(0,0,0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 5);

        glColor3f(1,1,1);
        glDrawArrays(GL_TRIANGLE_FAN,5,5);

        glColor3f(1,0,0);
        glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, &index1);
        glColor3f(0,1,0);
        glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, &index2);
        glColor3f(0,0,1);
        glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, &index3);
        glColor3f(1,1,0);
        glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, &index4);
        glColor3f(1,0,1);
        glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, &index5);

        glVertexPointer(3, GL_FLOAT, 0, &plaszcz);
        glColor3f(0,1,1);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
}

void MoveCamera()
{
    if (GetKeyState(VK_UP) < 0) xKat = ++xKat > 180 ? 180 : xKat;
    if (GetKeyState(VK_DOWN) < 0) xKat = --xKat < 0 ? 0 : xKat;
    if (GetKeyState(VK_LEFT) < 0) zKat++;
    if (GetKeyState(VK_RIGHT) < 0) zKat--;

    float kat = -zKat / 180 * M_PI;
    float speed = 0;
    if (GetKeyState('W') < 0) speed = 0.1;
    if (GetKeyState('S') < 0) speed = -0.1;
    if (GetKeyState('A') < 0) { speed = 0.1; kat -= M_PI*0.5; }
    if (GetKeyState('D') < 0) { speed = 0.1; kat += M_PI*0.5; }
    if (speed != 0)
    {
        place.x += sin(kat) * speed;
        place.y += cos(kat) * speed;
    }

    glRotatef(-xKat, 1,0,0);
    glRotatef(-zKat, 0,0,1);
    glTranslatef(-place.x, -place.y, -3);
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          1000,
                          1000,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    glEnable(GL_DEPTH_TEST);

    //glOrtho(-2,2,  -2,2,  -1,1);
    glFrustum(-1,1, -1,1, 2,100);

    //glTranslatef(0,0,-20);

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor(0.3f, 0.3f, 0.7f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glVertexPointer(3, GL_FLOAT, 0, &vert);

            glPushMatrix();
                MoveCamera();
                showEverything();
            glPopMatrix();

            SwapBuffers(hDC);

            Sleep(1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

