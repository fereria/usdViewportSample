#define NOMINMAX

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include <stdio.h>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include "GLFW/glfw3.h"

#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usdGeom/sphere.h"
#include "pxr/usd/sdf/path.h"

#include "pxr/usdImaging/usdImagingGL/engine.h"
#include "pxr/usdImaging/usdImagingGL/renderParams.h"
#include "pxr/base/gf/vec4d.h"
#include "pxr/base/gf/vec3d.h"
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/gf/rotation.h"
#include "pxr/imaging/glf/simpleLight.h"
#include "pxr/imaging/glf/simpleMaterial.h"

#include <iostream>

#include "SimpleCamera.h"

using namespace std;
using namespace pxr;

enum class CameraMode
{
  OFF = 0,
  TUMBLE = 1,
  TRUCK = 2,
  ZOOM = 3,
  PICK = 4
};

class MainWindow
{
public:
  MainWindow();  // コンストラクタ
  ~MainWindow(); // デストラクタ

  SimpleCamera _cam;
  GLFWwindow *window;
  UsdStageRefPtr stage;
  UsdImagingGLEngine *renderer;
  UsdImagingGLRenderParams renderParams;

  void paintGL();
  void initGL();
  void show();
  void initImGui();
  void paintImGui();

  double pan;
  double tilt;
  double dist;

  // 現在のマウス位置
  double mouse_pos_X;
  double mouse_pos_Y;

  // Altを押しているときにONにする
  bool cameraActive;
  CameraMode camMode;

private:
  double _lastX;
  double _lastY;
};