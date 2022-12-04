#include "pxr/base/gf/camera.h"
#include "pxr/base/gf/frustum.h"
#include "pxr/base/gf/vec3d.h"
#include "pxr/base/gf/rotation.h"

using namespace std;
using namespace pxr;

class SimpleCamera
{
public:
    void Track(float deltaRight, float deltaUp);
    void PanTilt(double dPan, double dTilt);
    void Walk(float dForward, float dRight);
    // MouseCamera
    void Tumble(float dTheta,float dPhi);
    
    GfMatrix4d RotMatrix(GfVec3d vec, double angle);
    
    void SetDist(double value);
    GfFrustum GetFrustum();
    
    SimpleCamera();

private:
    GfCamera _cam;
    GfVec3d _center;
    GfMatrix4d YZUpMatrix;
    GfMatrix4d YZUpInvMatrix;
    bool cameraTransformDirty;

    float _dist;
    double _rotPsi;
    double _rotPhi;
    double _rotTheta;

    void _pushFromCameraTransform();
    void _pullFromCameraTransform();
};
