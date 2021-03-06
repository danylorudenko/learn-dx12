#include <Math\Transform.hpp>

namespace Math
{

using namespace DirectX;

Transform::Transform() :
    position_(0.0f, 0.0f, 0.0f),
    rotation_(1.0f, 0.0f, 0.0f, 0.0f),
    scale_(1.0f, 1.0f, 1.0f)
{ }

Transform::Transform(Transform const& rhs) = default;

Transform::Transform(Transform&& rhs) = default;

Transform& Transform::operator=(Transform const& rhs) = default;

Transform& Transform::operator=(Transform&& rhs) = default;

DirectX::XMVECTOR Transform::PositionSIMD() const
{
    return DirectX::XMLoadFloat3A(&position_);
}

DirectX::XMVECTOR Transform::RotationSIMD() const
{
    return DirectX::XMLoadFloat4A(&rotation_);
}

DirectX::XMVECTOR Transform::ScaleSIMD() const
{
    return DirectX::XMLoadFloat3A(&scale_);
}

DirectX::XMFLOAT3A const& Transform::Position() const
{
    return position_;
}

DirectX::XMFLOAT4A const& Transform::Rotation() const
{
    return rotation_;
}

DirectX::XMFLOAT3A const& Transform::Scale() const
{
    return scale_;
}

void Transform::Position(DirectX::FXMVECTOR src)
{
    DirectX::XMStoreFloat3A(&position_, src);
    worldMatrixDirty_ = true;
}

void Transform::Rotation(DirectX::FXMVECTOR rotation)
{
    DirectX::XMStoreFloat4A(&rotation_, rotation);
    worldMatrixDirty_ = true;
}

void Transform::RotationRollPitchYaw(DirectX::FXMVECTOR eulerAngles)
{
    XMVECTOR eulerRad = DirectX::XMVectorMultiply(eulerAngles, DirectX::XMVectorReplicate(DEGREE_TO_RAD));
    DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYawFromVector(eulerRad);
    DirectX::XMStoreFloat4A(&rotation_, quat);
    worldMatrixDirty_ = true;
}

void Transform::Scale(DirectX::FXMVECTOR src)
{
    DirectX::XMStoreFloat3A(&scale_, src);
    worldMatrixDirty_ = true;
}

void Transform::Position(DirectX::XMFLOAT3A const& pos)
{
    position_ = pos;
    worldMatrixDirty_ = true;
}

void Transform::Rotation(DirectX::XMFLOAT4A const& orientation)
{
    rotation_ = orientation;
    worldMatrixDirty_ = true;
}

void Transform::RotationRollPitchYaw(DirectX::XMFLOAT3A const& eulerAngles)
{
    DirectX::XMVECTOR eulerRad = DirectX::XMVectorMultiply(DirectX::XMLoadFloat3A(&eulerAngles), DirectX::XMVectorReplicate(DEGREE_TO_RAD));
    DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYawFromVector(eulerRad);
    DirectX::XMStoreFloat4A(&rotation_, quat);
    worldMatrixDirty_ = true;
}

void Transform::Scale(DirectX::XMFLOAT3A const& scale)
{
    scale_ = scale;
    worldMatrixDirty_ = true;
}

DirectX::XMFLOAT3A Transform::Forward() const
{
    DirectX::XMFLOAT3A forward{};
    DirectX::XMStoreFloat3A(&forward, ForwardSIMD());
    return forward;
}

DirectX::XMVECTOR Transform::ForwardSIMD() const
{
    DirectX::XMVECTOR unitForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    return DirectX::XMVector4Transform(unitForward, DirectX::XMMatrixRotationQuaternion(RotationSIMD()));
}

DirectX::XMFLOAT3A Transform::Right() const
{
    DirectX::XMFLOAT3A right{};
    DirectX::XMStoreFloat3A(&right, RightSIMD());
    return right;
}

DirectX::XMVECTOR Transform::RightSIMD() const
{
    DirectX::XMVECTOR unitRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
    return DirectX::XMVector4Transform(unitRight, DirectX::XMMatrixRotationQuaternion(RotationSIMD()));
}

DirectX::XMFLOAT3A Transform::Up() const
{
    DirectX::XMFLOAT3A up{};
    DirectX::XMStoreFloat3A(&up, UpSIMD());
    return up;
}

DirectX::XMVECTOR Transform::UpSIMD() const
{
    DirectX::XMVECTOR unitUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    return DirectX::XMVector2Transform(unitUp, DirectX::XMMatrixRotationQuaternion(RotationSIMD()));
}

DirectX::XMFLOAT4X4A const& Transform::WorldMatrix()
{
    if (worldMatrixDirty_) {
        DirectX::XMStoreFloat4x4A(&worldMatrix_, WorldMatrixSIMD());
        worldMatrixDirty_ = false;
    }

    return worldMatrix_;
}

DirectX::XMMATRIX Transform::WorldMatrixSIMD() const
{
    return DirectX::XMMatrixAffineTransformation(ScaleSIMD(), DirectX::XMVectorReplicate(0.0f), RotationSIMD(), PositionSIMD());
}

}