
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "GL/glew.h"
#include "QWindow.h"
#include "ErrorTracingDefinitions.h"
#include "QResourceManager.h"
#include "QGraphicsEngine.h"
#include "QKeyboard.h"
#include "QCamera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_projection.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_operation.hpp"
#include "glm/gtx/transform2.hpp"

// [TODO]: Get opengl debugging log
// [TODO]: Be able to save binary compilation of shaders
// [TODO]: Bindless textures
// [TODO]: Use Direct State Access commands

QResourceManager* QE_RESOURCE_MANAGER = null_q;
QGraphicsEngine* QE_GRAPHICS_ENGINE = null_q;
QKeyboard* QE_KEYBOARD = null_q;
QCamera* QE_CAMERA = null_q;
QStopwatchEnclosed MAIN_TIMER(QTimeSpan(0, 0, 0, 1, 0, 0, 0), EQStopwatchEnclosedBehavior::E_Proportional);
QStopwatchEnclosed ANIMATION_TIMER(QTimeSpan(0, 0, 0, 1, 0, 0, 0), EQStopwatchEnclosedBehavior::E_Proportional);

void SetupInputDevices();
void InitializeMainWindow(HINSTANCE hInstance, QWindow** ppWindow);
void InitializeRenderingContext(QDeviceContext &dc);
void InitializeGlew();
void SetupCoreProfileRenderingContext(QDeviceContext &dc);
int MainLoop();
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void Cleanup();
void SetupEngine();
void SetupShaders();
void SetupGeometry();
void SetupTextures();
void SetTexture(GLuint programId, GLuint textureId, const char* samplerName, int textureIndex);
void SetupScene();
QWindow* pMainWindow;
void CheckInputs();
void OnKeyPressed(const QKeyboard* pKeyboard, const QKeyboard::KeyInfo &info);
void OnKeyKept(const QKeyboard* pKeyboard, const QKeyboard::KeyInfo &info);
void OnKeyReleased(const QKeyboard* pKeyboard, const QKeyboard::KeyInfo &info);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR    lpCmdLine,
                     int       nCmdShow)
{
    MAIN_TIMER.Set();
    ANIMATION_TIMER.Set();

    SetupInputDevices();
    InitializeMainWindow(hInstance, &pMainWindow);
    InitializeRenderingContext(pMainWindow->GetDeviceContext());
    InitializeGlew();
    SetupCoreProfileRenderingContext(pMainWindow->GetDeviceContext());
    InitializeGlew(); // Every time glew is initialized, functions are associated to the current rendering context, so it should be initialized every time the context changes
    SetupEngine();
    SetupShaders();
    SetupGeometry();
    SetupTextures();
    QE_GRAPHICS_ENGINE->SetAspect("Aspect1");
    SetupScene();

    int uResult = MainLoop();

    std::exit(uResult);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
    wchar_t msg[32];

    QE_KEYBOARD->Update(message, wParam, lParam);

	switch (message)
	{
    /*case WM_SYSKEYDOWN:
        swprintf_s(msg, L"WM_SYSKEYDOWN: 0x%x\n", wParam);
        OutputDebugString(msg);
        break;

    case WM_SYSCHAR:
        swprintf_s(msg, L"WM_SYSCHAR: %c\n", (wchar_t)wParam);
        OutputDebugString(msg);
        break;

    case WM_SYSKEYUP:
        swprintf_s(msg, L"WM_SYSKEYUP: 0x%x\n", wParam);
        OutputDebugString(msg);
        break;

    case WM_KEYDOWN:
        swprintf_s(msg, L"WM_KEYDOWN: 0x%x\n", wParam);
        OutputDebugString(msg);
        break;

    case WM_KEYUP:
        swprintf_s(msg, L"WM_KEYUP: 0x%x\n", wParam);
        OutputDebugString(msg);
        break;

    case WM_CHAR:
        swprintf_s(msg, L"WM_CHAR: %c\n", (wchar_t)wParam);
        OutputDebugString(msg);
        break;*/
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int MainLoop()
{
    QE_LOG("LOG: Entering de main loop...\n");

    MSG msg;


    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    //glCullFace(GL_FRONT); // LH -> RH

    QTransformationMatrix<QMatrix4x4> transformation;

    glClearColor(0.4f, 0.6f, 0.9f, 0.0f);
    glViewport(0, 0, 800, 600); // Set the viewport size to fill the window
    float_q fTranslation = 0;
    float_q fRotation = 0;
	// Main message loop:

    QColor cubeColor = QColor::GetVectorOfOnes();

	while (true)
	{
        CheckInputs();
        ANIMATION_TIMER.Set();

        QVector4 position = QVector4(QE_CAMERA->LocalPosition.x, QE_CAMERA->LocalPosition.y, QE_CAMERA->LocalPosition.z, QE_CAMERA->LocalPosition.w);
        //position.z = -position.z;
        QVector4 target = QVector4(QE_CAMERA->LocalPosition.x, QE_CAMERA->LocalPosition.y, QE_CAMERA->LocalPosition.z, QE_CAMERA->LocalPosition.w) + (-QVector4::GetUnitVectorZ()).Transform(QE_CAMERA->LocalOrientation.Invert());
        //target.z = -target.z;
        /////////target = -target;
        QVector4 up = QVector4::GetUnitVectorY().Transform(QE_CAMERA->LocalOrientation.Invert());
       // up.z = -up.z;

        glm::mat4 viewM = glm::lookAt(glm::vec3(QE_CAMERA->LocalPosition.x, QE_CAMERA->LocalPosition.y, QE_CAMERA->LocalPosition.z),
                                      glm::vec3(target.x, target.y, target.z),
                                      glm::vec3(up.x, up.y, up.z));
        glm::mat4 projM = glm::perspective(SQAngle::RadiansToDegrees(QE_CAMERA->Frustum.Fov), QE_CAMERA->Frustum.AspectRatio, QE_CAMERA->Frustum.NearPlaneDistance, QE_CAMERA->Frustum.FarPlaneDistance);
        
        QSpaceConversionMatrix worldMatrix, viewMatrix, projectionMatrix;
        worldMatrix.SetWorldSpaceMatrix(QVector4(0, 0, 0, 1.0f), QQuaternion::GetIdentity(), QVector3(1, 1, 1));
        viewMatrix = QE_CAMERA->GetViewSpaceMatrix();
        /*viewMatrix.SetViewSpaceMatrix(QVector4(QE_CAMERA->LocalPosition.x, QE_CAMERA->LocalPosition.y, QE_CAMERA->LocalPosition.z, QE_CAMERA->LocalPosition.w),
                                      QVector4(QE_CAMERA->LocalPosition.x, QE_CAMERA->LocalPosition.y, QE_CAMERA->LocalPosition.z, QE_CAMERA->LocalPosition.w) + (QVector4::GetUnitVectorZ()).Transform(QE_CAMERA->LocalOrientation.Invert()),
                                      QVector4::GetUnitVectorY().Transform(QE_CAMERA->LocalOrientation.Invert()));*/
        viewMatrix.SetViewSpaceMatrix(position, target, up);
        viewMatrix.ij[0][0] = -viewMatrix.ij[0][0];
        viewMatrix.ij[1][0] = -viewMatrix.ij[1][0];
        viewMatrix.ij[2][0] = -viewMatrix.ij[2][0];
        viewMatrix.ij[3][0] = -viewMatrix.ij[3][0];/*
        viewMatrix.ij[0][1] = -viewMatrix.ij[0][1];
        viewMatrix.ij[1][1] = -viewMatrix.ij[1][1];
        viewMatrix.ij[2][1] = -viewMatrix.ij[2][1];
        viewMatrix.ij[3][1] = -viewMatrix.ij[3][1];*/
        /*viewMatrix.ij[0][2] = -viewMatrix.ij[0][2];
        viewMatrix.ij[1][2] = -viewMatrix.ij[1][2];
        viewMatrix.ij[2][2] = -viewMatrix.ij[2][2];
        viewMatrix.ij[3][2] = -viewMatrix.ij[3][2];
        //viewMatrix = viewMatrix.SwitchHandConventionViewSpaceMatrix(); // LH -> RH
        //viewMatrix = *(QSpaceConversionMatrix*)&viewM;
        //viewMatrix.ij[0][0] = -viewMatrix.ij[0][0];
        //viewMatrix.ij[3][1] = -viewMatrix.ij[3][1];
        //viewMatrix.ij[2][2] = -viewMatrix.ij[2][2];
        /*
        QVector3 vPOV = QVector3(-(viewMatrix.ij[3][0] * viewMatrix.ij[0][0] + viewMatrix.ij[3][1] * viewMatrix.ij[0][1] + viewMatrix.ij[3][2] * viewMatrix.ij[0][2]),
            -(viewMatrix.ij[3][0] * viewMatrix.ij[1][0] + viewMatrix.ij[3][1] * viewMatrix.ij[1][1] + viewMatrix.ij[3][2] * viewMatrix.ij[1][2]),
            -(viewMatrix.ij[3][0] * viewMatrix.ij[2][0] + viewMatrix.ij[3][1] * viewMatrix.ij[2][1] + viewMatrix.ij[3][2] * viewMatrix.ij[2][2]));

        QVector3 vUp(viewMatrix.ij[0][1], viewMatrix.ij[1][1], viewMatrix.ij[2][1]);

        QVector3 vZAxis = QVector3(-viewMatrix.ij[0][2], -viewMatrix.ij[1][2], -viewMatrix.ij[2][2]);

        QVector3 vXAxis = vZAxis.CrossProduct(vUp);

        QVector3 vYAxis = vZAxis.CrossProduct(vXAxis);

        viewMatrix = QSpaceConversionMatrix(QMatrix4x4(vXAxis.x, vYAxis.x, vZAxis.x, SQFloat::_0,
            vXAxis.y, vYAxis.y, vZAxis.y, SQFloat::_0,
            vXAxis.z, vYAxis.z, vZAxis.z, SQFloat::_0,
            -vXAxis.DotProduct(vPOV), -vYAxis.DotProduct(vPOV), -vZAxis.DotProduct(vPOV), SQFloat::_1));
            */
        
        projectionMatrix = QE_CAMERA->Frustum.GetProjectionSpaceMatrix();
        projectionMatrix.SetProjectionSpaceMatrix(QE_CAMERA->Frustum.NearPlaneDistance, QE_CAMERA->Frustum.FarPlaneDistance, QE_CAMERA->Frustum.AspectRatio, QE_CAMERA->Frustum.Fov);
        projectionMatrix = projectionMatrix.SwitchHandConventionProjectionSpaceMatrix();
        //projectionMatrix = *(QSpaceConversionMatrix*)&projM;
        //projectionMatrix.ij[0][0] = -projectionMatrix.ij[0][0];
        
        fTranslation = MAIN_TIMER.GetProgression();
        fRotation = MAIN_TIMER.GetProgression();

        QTranslationMatrix<QMatrix4x4> translation(0, 0, -10);
        QRotationMatrix3x3 rotation(fRotation, fRotation, fRotation);
        
        QQuaternion quat(0, 0, SQAngle::_HalfPi * MAIN_TIMER.GetProgression());
        rotation = QRotationMatrix3x3(quat);

        //worldMatrix.SetWorldSpaceMatrix(QVector4::GetNullVector(), quat, QVector3::GetVectorOfOnes());
        
        transformation = worldMatrix * viewMatrix * projectionMatrix;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear required buffers

        QE_GRAPHICS_ENGINE->UpdateVertexShaderData("VS1", "uColor", cubeColor.x, cubeColor.y, cubeColor.z, cubeColor.w);
        QE_GRAPHICS_ENGINE->UpdateVertexShaderData("VS1", "transformationMatrix", transformation.Transpose());

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, null_q);

        if (pMainWindow->GetDeviceContext().GetCurrentRenderingContext() != null_q)
            pMainWindow->GetDeviceContext().SwapBuffers();

        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // Translate the message and dispatch it to WindowProc()
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(msg.message == WM_QUIT)
            break;
	}

    QE_LOG("LOG: Exiting de main loop...\n");

	return (int) msg.wParam;
}

void InitializeMainWindow(HINSTANCE hInstance, QWindow** ppWindow)
{
    *ppWindow = new QWindow(hInstance);
    (*ppWindow)->SetMessageDispatcher(&WndProc);
    (*ppWindow)->SetHeight(600);
    (*ppWindow)->SetWidth(800);
    (*ppWindow)->SetTitle("OpenGL test application");

    QE_LOG("LOG: Window initialized.\n");

    (*ppWindow)->Show();

    QE_LOG("LOG: Window shown.\n");
}

void InitializeRenderingContext(QDeviceContext &dc)
{
    dc.SetPixelFormat(QDeviceContext::E_R8G8B8A8D24S8);
    QDeviceContext::NativeRenderingContext renderingContext = dc.CreateRenderingContext();
    dc.MakeRenderingContextCurrent();

    QE_LOG("LOG: Pixel format set.\n");
}

void InitializeGlew()
{
    glewExperimental = GL_TRUE;
    GLenum eInitResult = glewInit();
    
    QE_ASSERT_ERROR(eInitResult == GLEW_OK, string_q("An error occurred when attempting to initialize GLEW. ") + rcast_q(glewGetErrorString(eInitResult), const i8_q*));
    const GLubyte* u = glewGetErrorString(eInitResult);
    QE_LOG(string_q("LOG: GLEW Initialized (") + rcast_q(glewGetString(GLEW_VERSION), const i8_q*) + ")\n");
    QE_LOG(string_q("LOG: Open GL Version ") + rcast_q(glGetString(GL_VERSION), const i8_q*) + "\n");
}

void SetupCoreProfileRenderingContext(QDeviceContext &dc)
{
    QDeviceContext::NativeRenderingContext rc = dc.GetCurrentRenderingContext();
    dc.CreateAdvancedRenderingContext(QDeviceContext::E_R8G8B8A8D24S8, true);
    dc.MakeRenderingContextCurrent();
    dc.DeleteRenderingContext(rc);
}

void Cleanup()
{
    QDeviceContext::NativeRenderingContext rc = pMainWindow->GetDeviceContext().GetCurrentRenderingContext();
    pMainWindow->GetDeviceContext().ResetCurrentRenderingContext();
    pMainWindow->GetDeviceContext().DeleteRenderingContext(rc);
    delete pMainWindow;

    QE_LOG("LOG: Clean up performed.\n");
}

void SetupEngine()
{
    QE_RESOURCE_MANAGER = new QResourceManager(new QShaderCompositor());
    QE_GRAPHICS_ENGINE = new QGraphicsEngine(QE_RESOURCE_MANAGER);
}

void SetupShaders()
{
    QE_RESOURCE_MANAGER->CreateVertexShader("VS1", QPath("./Resources/VertexShader.glsl"));
    QE_RESOURCE_MANAGER->CreateFragmentShader("FS1", QPath("./Resources/FragmentShader.glsl"));
    QE_GRAPHICS_ENGINE->SetVertexShader("VS1");
    QE_GRAPHICS_ENGINE->SetFragmentShader("FS1");
}

void SetupGeometry()
{
    struct QVertex
    {
        QVector4 Position;
        QVector2 TexCoords0;
        QVector2 TexCoords1;
    };

    QHexahedron<QVector4> hexahedron = QHexahedron<QVector4>::GetUnitCube();

    QVertex arVertices[] = {
        // Images are flipped vertically, texture coordinates had to be reverted for Y axis
        { QVector4(-0.5f, -0.5f, -0.5f, 1.0f),        QVector2(1.0f/*0.0f*/, 0.0f),        QVector2(0.0f, 0.0f)
        },
        
        { QVector4(0.5f, -0.5f, -0.5f, 1.0f),        QVector2(1.0f, 0.0f),          QVector2(1.0f, 0.0f)
        },
        
        { QVector4(0.5f, 0.5f, -0.5f, 1.0f),        QVector2(1.0f, 1.0f),        QVector2(1.0f, 1.0f)
        },
        
        { QVector4(0.5f, 0.5f, -0.5f, 1.0f),        QVector2(1.0f, 1.0f),        QVector2(1.0f, 1.0f)
        },

        { QVector4(-0.9f, 0.5f, -0.5f, 1.0f),        QVector2(0.0f, 1.0f),        QVector2(0.0f, 1.0f)
        },

        { QVector4(-0.5f, -0.5f, -0.5f, 1.0f),        QVector2(0.0f, 0.0f),        QVector2(0.0f, 0.0f)
        },

        { QVector4(-0.5f, -0.5f, 0.5f, 1.0f),        QVector2(0.0f, 0.0f),        QVector2(0.0f, 0.0f)
        },

        { QVector4(0.5f, -0.5f, 0.5f, 1.0f),        QVector2(1.0f, 0.0f),        QVector2(1.0f, 0.0f)
        },

        { QVector4(0.5f, 0.5f, 0.5f, 1.0f),        QVector2(1.0f, 1.0f),        QVector2(1.0f, 1.0f)
        },

        { QVector4(0.5f, 0.5f, 0.5f, 1.0f),        QVector2(1.0f, 1.0f),        QVector2(1.0f, 1.0f)
        },

        { QVector4(-0.5f, 0.5f, 0.5f, 1.0f),        QVector2(0.0f, 1.0f),        QVector2(0.0f, 1.0f)
        },

        { QVector4(-0.5f, -0.5f, 0.5f, 1.0f),        QVector2(0.0f, 0.0f),        QVector2(0.0f, 0.0f)
        },

        { QVector4(-0.5f, 0.5f, 0.5f, 1.0f), QVector2(1.0f, 0.0f), QVector2(1.0f, 0.0f)
        },

        { QVector4(-0.5f, 0.5f, -0.5f, 1.0f), QVector2(1.0f, 1.0f), QVector2(1.0f, 1.0f)
        },

        { QVector4(-0.5f, -0.5f, -0.5f, 1.0f), QVector2(0.0f, 1.0f), QVector2(0.0f, 1.0f)
        },

        { QVector4(-0.5f, -0.5f, -0.5f, 1.0f), QVector2(0.0f, 1.0f), QVector2(0.0f, 1.0f)
        },

        { QVector4(-0.5f, -0.5f, 0.5f, 1.0f), QVector2(0.0f, 0.0f), QVector2(0.0f, 0.0f)
        },

        { QVector4(-0.5f, 0.5f, 0.5f, 1.0f), QVector2(1.0f, 0.0f), QVector2(1.0f, 0.0f)
        },

        { QVector4(0.5f, 0.5f, 0.5f, 1.0f), QVector2(1.0f, 0.0f), QVector2(1.0f, 0.0f)
        },

        { QVector4(0.5f, 0.5f, -0.5f, 1.0f), QVector2(1.0f, 1.0f), QVector2(1.0f, 1.0f)
        },

        { QVector4(0.5f, -0.5f, -0.5f, 1.0f), QVector2(0.0f, 1.0f), QVector2(0.0f, 1.0f)
        },

        { QVector4(0.5f, -0.5f, -0.5f, 1.0f), QVector2(0.0f, 1.0f), QVector2(0.0f, 1.0f)
        },

        { QVector4(0.5f, -0.5f, 0.5f, 1.0f), QVector2(0.0f, 0.0f), QVector2(0.0f, 0.0f)
        },

        { QVector4(0.5f, 0.5f, 0.5f, 1.0f), QVector2(1.0f, 0.0f), QVector2(1.0f, 0.0f)
        },

        { QVector4(-0.5f, -0.5f, -0.5f, 1.0f), QVector2(0.0f, 1.0f), QVector2(0.0f, 1.0f)
        },

        { QVector4(0.5f, -0.5f, -0.5f, 1.0f), QVector2(1.0f, 1.0f), QVector2(1.0f, 1.0f)
        },

        { QVector4(0.5f, -0.5f, 0.5f, 1.0f), QVector2(1.0f, 0.0f), QVector2(1.0f, 0.0f)
        },

        { QVector4(0.5f, -0.5f, 0.5f, 1.0f), QVector2(1.0f, 0.0f), QVector2(1.0f, 0.0f)
        },

        { QVector4(-0.5f, -0.5f, 0.5f, 1.0f), QVector2(0.0f, 0.0f), QVector2(0.0f, 0.0f)
        },

        { QVector4(-0.5f, -0.5f, -0.5f, 1.0f), QVector2(0.0f, 1.0f), QVector2(0.0f, 1.0f)
        },

        { QVector4(-0.5f, 0.5f, -0.5f, 1.0f), QVector2(0.0f, 1.0f), QVector2(0.0f, 1.0f)
        },

        { QVector4(0.5f, 0.5f, -0.5f, 1.0f), QVector2(1.0f, 1.0f), QVector2(1.0f, 1.0f)
        },

        { QVector4(0.5f, 0.5f, 0.5f, 1.0f), QVector2(1.0f, 0.0f), QVector2(1.0f, 0.0f)
        },

        { QVector4(0.5f, 0.5f, 0.5f, 1.0f), QVector2(1.0f, 0.0f), QVector2(1.0f, 0.0f)
        },

        { QVector4(-0.5f, 0.5f, 0.5f, 1.0f), QVector2(0.0f, 0.0f), QVector2(0.0f, 0.0f)
        },

        { QVector4(-0.5f, 0.5f, -0.5f, 1.0f), QVector2(0.0f, 1.0f), QVector2(0.0f, 1.0f)
        },
    };

    // Indices
    GLuint arIndices[] = { 
        0, 1, 2,
        3, 4, 5,
        8, 7, 6,
        11, 10, 9,
        14, 13, 12,
        17, 16, 15,
        18, 19, 20,
        21, 22, 23,
        26, 25, 24,
        29, 28, 27,
        30, 31, 32,
        33, 34, 35
    };

    // create attribs
    // create VAO
    GLuint vaoTriangleID = 0;
    glGenVertexArrays(1, &vaoTriangleID);
    glBindVertexArray(vaoTriangleID);

    // Set up VAO

    // create VBO
    GLuint vboTriangleID = 0;
    glGenBuffers(1, &vboTriangleID);
    glBindBuffer(GL_ARRAY_BUFFER, vboTriangleID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(arVertices), &arVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(QVertex), (GLvoid*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QVertex), (GLvoid*)(sizeof(QVector4)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(QVertex), (GLvoid*)(sizeof(QVector4) + sizeof(QVector2)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // create EBO
    GLuint eboTriangleID = 0;
    glGenBuffers(1, &eboTriangleID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboTriangleID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(arIndices), arIndices, GL_STATIC_DRAW);
    
}

void SetupTextures()
{
    QMaterial* pMaterial = QE_RESOURCE_MANAGER->CreateMaterial("DEFAULT");
    pMaterial->Ambient = QColor(0, 0, 0, 1);
    pMaterial->Diffuse = QColor(0.5, 0.5, 0.5, 1);

    QE_RESOURCE_MANAGER->CreateImage("Image1", QPath("./Resources/qe.png"), QImage::E_RGBA);
    QE_RESOURCE_MANAGER->CreateImage("Image2", QPath("./Resources/wall.jpg"), QImage::E_RGB);
    QTexture2D* pTex1 = QE_RESOURCE_MANAGER->CreateTexture2D("Texture1", "Image1", QImage::E_RGBA);

    pTex1->GenerateMipmaps();
    pTex1->SetWrapModeS(QTexture2D::E_Repeat);
    pTex1->SetWrapModeT(QTexture2D::E_Repeat);
    pTex1->SetMinificationFilter(QTexture2D::E_LinearMipmaps);
    pTex1->SetMagnificationFilter(QTexture2D::E_MagLinear);

    QTexture2D* pTex2 = QE_RESOURCE_MANAGER->CreateTexture2D("Texture2", "Image2", QImage::E_RGB);

    pTex2->GenerateMipmaps();
    pTex2->SetWrapModeS(QTexture2D::E_Repeat);
    pTex2->SetWrapModeT(QTexture2D::E_Repeat);
    pTex2->SetMinificationFilter(QTexture2D::E_LinearMipmaps);
    pTex2->SetMagnificationFilter(QTexture2D::E_MagLinear);

    QAspect* pAspect = QE_RESOURCE_MANAGER->CreateAspect("Aspect1");
    pAspect->AddTexture("Texture2", "DEFAULT", "sampler2");
    pAspect->AddTexture("Texture1", "DEFAULT", "sampler1");
    pAspect->SetVertexShader("VS1");
    pAspect->SetFragmentShader("FS1");

}

void SetupInputDevices()
{
    QE_KEYBOARD = new QKeyboard();
    //QE_KEYBOARD->KeyPressedEvent += &OnKeyPressed;
    //QE_KEYBOARD->KeyKeptEvent += &OnKeyKept;
    //QE_KEYBOARD->KeyReleasedEvent += &OnKeyReleased;
}

void SetupScene()
{
    QE_CAMERA = new QCamera();
    QE_CAMERA->Frustum.AspectRatio = 800.0f / 600.0f;
    QE_CAMERA->Frustum.FarPlaneDistance = 1000.0f;
    QE_CAMERA->Frustum.Fov = SQAngle::_QuarterPi;
    QE_CAMERA->Frustum.NearPlaneDistance = 0.1f;
    QE_CAMERA->SetPosition(QVector4(0.0f, 0.0f, 0.0f, 1.0f));
    QE_CAMERA->SetOrientation(QQuaternion::GetIdentity());
    QE_CAMERA->SetScale(QVector3::GetVectorOfOnes());
}

void CheckInputs()
{
    const float_q SPEED = 1.0f;

    if (QE_KEYBOARD->IsDown(QKeyboard::E_LEFT))
    {
        QE_CAMERA->Move(QVector3(-ANIMATION_TIMER.GetProgression() * SPEED, 0, 0));
    }

    if (QE_KEYBOARD->IsDown(QKeyboard::E_RIGHT))
    {
        QE_CAMERA->Move(QVector3(ANIMATION_TIMER.GetProgression() * SPEED, 0, 0));
    }

    if (QE_KEYBOARD->IsDown(QKeyboard::E_UP))
    {
        QE_CAMERA->Move(QVector3(0, 0, ANIMATION_TIMER.GetProgression() * SPEED));
    }

    if (QE_KEYBOARD->IsDown(QKeyboard::E_DOWN))
    {
        QE_CAMERA->Move(QVector3(0, 0, -ANIMATION_TIMER.GetProgression() * SPEED));
    }
    
    if (QE_KEYBOARD->IsDown(QKeyboard::E_Z))
    {
        QE_CAMERA->RotateYaw(-SQAngle::_QuarterPi * ANIMATION_TIMER.GetProgression());
    }
    
    if (QE_KEYBOARD->IsDown(QKeyboard::E_X))
    {
        QE_CAMERA->RotateYaw(SQAngle::_QuarterPi * ANIMATION_TIMER.GetProgression());
    }
    
    if (QE_KEYBOARD->IsDown(QKeyboard::E_F))
    {
        QE_CAMERA->RotatePitch(-SQAngle::_QuarterPi * ANIMATION_TIMER.GetProgression());
    }

    if (QE_KEYBOARD->IsDown(QKeyboard::E_V))
    {
        QE_CAMERA->RotatePitch(SQAngle::_QuarterPi * ANIMATION_TIMER.GetProgression());
    }

    if (QE_KEYBOARD->IsDown(QKeyboard::E_Q))
    {
        QE_CAMERA->RotateRoll(SQAngle::_QuarterPi * ANIMATION_TIMER.GetProgression());
    }

    if (QE_KEYBOARD->IsDown(QKeyboard::E_W))
    {
        QE_CAMERA->RotateRoll(-SQAngle::_QuarterPi * ANIMATION_TIMER.GetProgression());
    }

    if (QE_KEYBOARD->IsDown(QKeyboard::E_D))
    {
        QE_CAMERA->Move(QVector3(0, ANIMATION_TIMER.GetProgression() * SPEED, 0));
    }
    
    if (QE_KEYBOARD->IsDown(QKeyboard::E_C))
    {
        QE_CAMERA->Move(QVector3(0, -ANIMATION_TIMER.GetProgression() * SPEED, 0));
    }

    if (QE_KEYBOARD->IsDown(QKeyboard::E_ESCAPE))
    {
        exit(0);
    }
}

void OnKeyPressed(const QKeyboard* pKeyboard, const QKeyboard::KeyInfo &info)
{
    if (info.Code == QKeyboard::E_LEFT)
    {
        QE_CAMERA->Move(QVector3(-ANIMATION_TIMER.GetProgression() * 0.0001f, 0, 0));
    }
    else if (info.Code == QKeyboard::E_RIGHT)
    {
        QE_CAMERA->Move(QVector3(ANIMATION_TIMER.GetProgression() * 0.0001f, 0, 0));
    }
    else if (info.Code == QKeyboard::E_UP)
    {
        QE_CAMERA->Move(QVector3(0, 0, ANIMATION_TIMER.GetProgression() * 0.0001f));
    }
    else if (info.Code == QKeyboard::E_DOWN)
    {
        QE_CAMERA->Move(QVector3(0, 0, -ANIMATION_TIMER.GetProgression() * 0.0001f));
    }
    else if (info.Code == QKeyboard::E_Z)
    {
        QE_CAMERA->RotateYaw(-SQAngle::_QuarterPi * ANIMATION_TIMER.GetProgression());
    }
    else if (info.Code == QKeyboard::E_X)
    {
        QE_CAMERA->RotateYaw(SQAngle::_QuarterPi * ANIMATION_TIMER.GetProgression());
    }
    else if (info.Code == QKeyboard::E_D)
    {
        QE_CAMERA->Move(QVector3(0, ANIMATION_TIMER.GetProgression() * 0.0001f, 0));
    }
    else if (info.Code == QKeyboard::E_C)
    {
        QE_CAMERA->Move(QVector3(0, -ANIMATION_TIMER.GetProgression() * 0.0001f, 0));
    }
    else if (info.Code == QKeyboard::E_ESCAPE)
    {
        exit(0);
    }
    

    /*QE_LOG("KEY PRESSED\n");
    QE_LOG(string_q("Code: 0x") + string_q::FromIntegerToHexadecimal(info.Code) + "\n");
    QE_LOG(string_q("TimePressed: ") + info.TimePressed.GetMilliseconds() + "\n");
    QE_LOG(string_q("Timestamp: ") + info.Timestamp.GetMilliseconds() + "\n");
    QE_LOG(string_q("TimeElapsedSinceLastEvent: ") + info.TimeElapsedSinceLastEvent.GetMilliseconds() + "\n");

    if (info.HasCharacter)
    {
        QE_LOG(string_q("Character: ") + info.Character + "\n");
    }*/
}

void OnKeyKept(const QKeyboard* pKeyboard, const QKeyboard::KeyInfo &info)
{
    const float_q SPEED = 0.1f;

    if (info.Code == QKeyboard::E_LEFT)
    {
        QE_CAMERA->Move(QVector3(-ANIMATION_TIMER.GetElapsedTimeAsFloat() * SPEED, 0, 0));
    }
    else if (info.Code == QKeyboard::E_RIGHT)
    {
        QE_CAMERA->Move(QVector3(ANIMATION_TIMER.GetElapsedTimeAsFloat() * SPEED, 0, 0));
    }
    else if (info.Code == QKeyboard::E_UP)
    {
        QE_CAMERA->Move(QVector3(0, 0, ANIMATION_TIMER.GetElapsedTimeAsFloat() * SPEED));
    }
    else if (info.Code == QKeyboard::E_DOWN)
    {
        QE_CAMERA->Move(QVector3(0, 0, -ANIMATION_TIMER.GetElapsedTimeAsFloat() * SPEED));
    }
    else if (info.Code == QKeyboard::E_Z)
    {
        QE_CAMERA->RotateYaw(-SQAngle::_QuarterPi * ANIMATION_TIMER.GetElapsedTimeAsFloat());
    }
    else if (info.Code == QKeyboard::E_X)
    {
        QE_CAMERA->RotateYaw(SQAngle::_QuarterPi * ANIMATION_TIMER.GetElapsedTimeAsFloat());
    }
    else if (info.Code == QKeyboard::E_D)
    {
        QE_CAMERA->Move(QVector3(0, ANIMATION_TIMER.GetElapsedTimeAsFloat() * SPEED, 0));
    }
    else if (info.Code == QKeyboard::E_C)
    {
        QE_CAMERA->Move(QVector3(0, -ANIMATION_TIMER.GetElapsedTimeAsFloat() * SPEED, 0));
    }

    /*QE_LOG("KEY KEPT\n");
    QE_LOG(string_q("Code: 0x") + string_q::FromIntegerToHexadecimal(info.Code) + "\n");
    QE_LOG(string_q("TimePressed: ") + info.TimePressed.GetMilliseconds() + "\n");
    QE_LOG(string_q("Timestamp: ") + info.Timestamp.GetMilliseconds() + "\n");
    QE_LOG(string_q("TimeElapsedSinceLastEvent: ") + info.TimeElapsedSinceLastEvent.GetMilliseconds() + "\n");

    if (info.HasCharacter)
    {
        QE_LOG(string_q("Character: ") + info.Character + "\n");
    }*/
}

void OnKeyReleased(const QKeyboard* pKeyboard, const QKeyboard::KeyInfo &info)
{
    /*QE_LOG("KEY RELEASED\n");
    QE_LOG(string_q("Code: 0x") + string_q::FromIntegerToHexadecimal(info.Code) + "\n");
    QE_LOG(string_q("TimePressed: ") + info.TimePressed.GetMilliseconds() + "\n");
    QE_LOG(string_q("Timestamp: ") + info.Timestamp.GetMilliseconds() + "\n");
    QE_LOG(string_q("TimeElapsedSinceLastEvent: ") + info.TimeElapsedSinceLastEvent.GetMilliseconds() + "\n");*/
}