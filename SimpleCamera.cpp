#include "SimpleCamera.h"
#include <iostream>

using namespace std;

SimpleCamera::SimpleCamera()
{
    this->YZUpMatrix = GfMatrix4d().SetRotate(GfRotation(GfVec3d::XAxis(), -90));
    this->YZUpInvMatrix = this->YZUpMatrix.GetInverse();
    this->cameraTransformDirty = false;
}

GfFrustum SimpleCamera::GetFrustum()
{
    return this->_cam.GetFrustum();
}

void SimpleCamera::SetDist(double value)
{
    this->_dist = value;
    this->_pushFromCameraTransform();
    this->cameraTransformDirty = true;
}

void SimpleCamera::_pushFromCameraTransform()
{
    if(this->cameraTransformDirty == false) return;
    
    GfMatrix4d viewMatrix;
    viewMatrix.SetIdentity();

    viewMatrix *= GfMatrix4d().SetTranslate(GfVec3d::ZAxis() * this->_dist);
    viewMatrix *= this->RotMatrix(GfVec3d::ZAxis(), _rotPsi);
    viewMatrix *= this->RotMatrix(GfVec3d::XAxis(), _rotPhi);
    viewMatrix *= this->RotMatrix(GfVec3d::YAxis(), _rotTheta);
    viewMatrix *= this->YZUpInvMatrix;
    viewMatrix *= GfMatrix4d().SetTranslate(this->_center);

    this->_cam.SetTransform(viewMatrix);
    this->_cam.SetFocusDistance(this->_dist);
    
    this->cameraTransformDirty = false;
}

void SimpleCamera::_pullFromCameraTransform()
{
    GfMatrix4d cam_transform = this->_cam.GetTransform();
    float dist = this->_cam.GetFocusDistance();
    GfFrustum frustum = this->_cam.GetFrustum();
    GfVec3d cam_pos = frustum.GetPosition();
    GfVec3d cam_axis = frustum.ComputeViewDirection();

    GfMatrix4d transform = cam_transform * this->YZUpMatrix;
    transform.Orthonormalize();
    GfRotation rotation = transform.ExtractRotation();
    GfVec3d result = -rotation.Decompose(GfVec3d::YAxis(),
                                         GfVec3d::XAxis(),
                                         GfVec3d::ZAxis());
                                         
    this->_dist = dist;
    this->_center = cam_pos + dist * cam_axis;
    
    this->cameraTransformDirty = true;
}

void SimpleCamera::Tumble(float dTheta,float dPhi)
{
    //左ボタン
    this->_rotTheta += dTheta;
    this->_rotPhi += dPhi;
    this->cameraTransformDirty = true;
}

void SimpleCamera::Track(float deltaRight, float deltaUp)
{
    // 中ボタン
    this->_pushFromCameraTransform();
    
    GfFrustum frustum = this->_cam.GetFrustum();
    GfVec3d cam_up = frustum.ComputeUpVector();
    GfVec3d cam_right = GfCross(frustum.ComputeViewDirection(),cam_up);
    this->_center += (deltaRight * cam_right + deltaUp * cam_up);
    this->cameraTransformDirty = true;
}

void SimpleCamera::PanTilt(double dPan, double dTilt)
{
    this->_cam.SetTransform(
        RotMatrix(GfVec3d::XAxis(), dTilt) *
        RotMatrix(GfVec3d::YAxis(), dPan) *
        this->_cam.GetTransform());
    this->_pullFromCameraTransform();
    
    this->_rotPsi = 0.0;
}

void SimpleCamera::Walk(float dForward, float dRight)
{
}

GfMatrix4d SimpleCamera::RotMatrix(GfVec3d vec, double angle)
{
    return GfMatrix4d(1.0).SetRotate(GfRotation(vec, angle));
}
