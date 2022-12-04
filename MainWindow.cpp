#define NOMINMAX

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

#include "GLFW/glfw3.h"

#include "MainWindow.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif
#define IM_ARRAYSIZE(_ARR) ((int)(sizeof(_ARR) / sizeof(*(_ARR)))) // Size of a static C-style array. Don't use on pointers!

MainWindow::MainWindow()
{
    // 初期値の設定
    this->dist = 1000;
    this->pan = 0;
    this->tilt = 0;

    this->cameraActive = false;
    this->camMode = CameraMode::OFF;

    this->initGL();

    this->renderer = new UsdImagingGLEngine();

    this->renderParams.drawMode = UsdImagingGLDrawMode::DRAW_WIREFRAME_ON_SURFACE;
    this->renderParams.enableLighting = true;
    this->renderParams.enableIdRender = false;
    this->renderParams.frame = 0;
    this->renderParams.complexity = 1;
    this->renderParams.cullStyle = UsdImagingGLCullStyle::CULL_STYLE_BACK_UNLESS_DOUBLE_SIDED;
    this->renderParams.enableSceneMaterials = false;
    this->renderParams.highlight = true;

    this->stage = UsdStage::Open("D:/Kitchen_set/Kitchen_set.usd");
}

MainWindow::~MainWindow()
{
    delete this->renderer;
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    MainWindow *_this = static_cast<MainWindow *>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_UP)
        _this->dist += 10;
    if (key == GLFW_KEY_DOWN)
        _this->dist -= 10;
    if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS)
        _this->cameraActive = true;
    if (key == GLFW_KEY_LEFT_ALT && action == GLFW_RELEASE)
        _this->cameraActive = false;
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    MainWindow *_this = static_cast<MainWindow *>(glfwGetWindowUserPointer(window));

    _this->mouse_pos_X = xpos;
    _this->mouse_pos_Y = ypos;
}

void mouseClickCallback(GLFWwindow *window, int button, int action, int mods)
{
    MainWindow *_this = static_cast<MainWindow *>(glfwGetWindowUserPointer(window));
    _this->camMode = CameraMode::OFF;
    if (_this->cameraActive)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            _this->camMode = CameraMode::TUMBLE;
        if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
            _this->camMode = CameraMode::TRUCK;
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
            _this->camMode = CameraMode::ZOOM;
    }
}

void MainWindow::initGL()
{

    glfwInit();
    this->window = glfwCreateWindow(1280, 720, "Hydra Sample", NULL, NULL);
    glfwSetWindowUserPointer(window, this);
    glfwMakeContextCurrent(this->window);
    glfwSwapInterval(1); // Enable vsync

    glfwSetWindowUserPointer(window, this);
    // Key/Mouse入力取得
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseClickCallback);
    glfwSetCursorPosCallback(window, mouseCallback);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();
}

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void MainWindow::show()
{
    glfwSetErrorCallback(glfw_error_callback);

    // 前フレームの値
    double currentPan = 0;
    double currentTilt = 0;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
            ImGui::Begin("Debug");

            ImGui::InputDouble("Distance", &this->dist);
            ImGui::InputDouble("Tilt", &this->tilt);
            ImGui::InputDouble("Pan", &this->pan);

            if (this->camMode == CameraMode::OFF)
                ImGui::Text("CamMode::OFF");
            if (this->camMode == CameraMode::TUMBLE)
                ImGui::Text("CamMode::TUMBLE");
            if (this->camMode == CameraMode::TRUCK)
                ImGui::Text("CamMode::TRUCK");
            if (this->camMode == CameraMode::ZOOM)
                ImGui::Text("CamMode::ZOOM");

            ImGui::InputDouble("MousePosX", &this->mouse_pos_X);
            ImGui::InputDouble("MousePosY", &this->mouse_pos_Y);

            ImGui::End();
        }

        ImGui::Render();

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // USD 関連

        double moveX = this->_lastX - this->mouse_pos_X;
        double moveY = this->_lastY - this->mouse_pos_Y;

        // カメラの移動
        if (this->camMode == CameraMode::TUMBLE)
            this->_cam.Tumble(moveX * 0.25, moveY * 0.25);

        if (this->camMode == CameraMode::TRUCK)
            this->_cam.Track(moveX * 0.25, moveY * -0.25);

        if (this->camMode == CameraMode::ZOOM)
            this->dist += -0.5 * (moveX + moveY);

        // 前のカメラの数値を保存
        this->_lastX = this->mouse_pos_X;
        this->_lastY = this->mouse_pos_Y;

        this->_cam.SetDist(this->dist);
        this->_cam.PanTilt(this->pan - currentPan, this->tilt - currentTilt);

        currentPan = this->pan;
        currentTilt = this->tilt;

        GfFrustum frustum = this->_cam.GetFrustum();
        GfVec3d cam_pos = frustum.GetPosition();

        GlfSimpleLightVector lights;
        GlfSimpleMaterial material;
        GfVec4f ambient(GfVec4f(.1f, .1f, .1f, 1.f));
        GlfSimpleLight l;
        l.SetAmbient(GfVec4f(0, 0, 0, 0));
        l.SetPosition(GfVec4f(cam_pos[0], cam_pos[1], cam_pos[2], 1.f));
        lights.push_back(l);

        material.SetAmbient(GfVec4f(.1f, .1f, .1f, 1.f));
        material.SetSpecular(GfVec4f(.6f, .6f, .6f, .6f));
        material.SetShininess(16.0);

        renderer->SetRenderViewport(GfVec4d(0, 0, display_w, display_h));
        renderer->SetCameraState(frustum.ComputeViewMatrix(),
                                 frustum.ComputeProjectionMatrix());
        renderer->SetLightingState(lights, material, ambient);

        UsdPrim root = stage->GetPseudoRoot();
        renderer->Render(root, renderParams);

        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }
}