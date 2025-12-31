#include "Transform.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "imgui.h"

Transform::Transform()
{
    children.reserve(5);
}

Transform::~Transform()
{
    if (parent)
    {
        parent->RemoveChild(this);
    }

    for (auto* child : children)
    {
        child->parent = nullptr;
        child->MarkWorldDirty();
    }
}


glm::vec3 Transform::GetWorldPosition()
{
    return glm::vec3(GetWorldMatrix()[3]);
}

glm::quat Transform::GetWorldRotation()
{
    glm::vec3 decompScale;
    glm::quat decompRotation;
    glm::vec3 decompTranslation;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(GetWorldMatrix(), decompScale, decompRotation, decompTranslation, skew, perspective);
    return decompRotation;
}

glm::vec3 Transform::GetWorldScale()
{
    const glm::mat4& mat = GetWorldMatrix();
    return {
        glm::length(glm::vec3(mat[0])),
        glm::length(glm::vec3(mat[1])),
        glm::length(glm::vec3(mat[2]))
    };
}

// ==================================================================================
// Setters
// ==================================================================================

void Transform::Translate(const glm::vec3& translation)
{
    position += translation;
    MarkLocalDirty();
}

void Transform::TranslateLocal(const glm::vec3& translation)
{
    position += rotation * translation;
    MarkLocalDirty();
}

void Transform::SetLocalPosition(const glm::vec3& inPosition)
{
    if (position == inPosition) return;

    position = inPosition;
    MarkLocalDirty();
}

void Transform::SetLocalRotation(const glm::vec3& inEulerAngles)
{
    glm::quat newRot = glm::quat(inEulerAngles);

    if (rotation == newRot) return;
    rotation = newRot;
    MarkLocalDirty();
}

void Transform::SetLocalRotation(const glm::quat& inRotation)
{
    if (rotation == inRotation) return;
    rotation = inRotation;
    MarkLocalDirty();
}

void Transform::SetLocalScale(const glm::vec3& inScale)
{
    if (scale == inScale) return;
    scale = inScale;
    MarkLocalDirty();
}

void Transform::SetWorldPosition(const glm::vec3& inPosition)
{
    if (parent)
    {
        glm::mat4 parentInverse = glm::inverse(parent->GetWorldMatrix());
        position = glm::vec3(parentInverse * glm::vec4(inPosition, 1.0f));
    }
    else
    {
        position = inPosition;
    }
    MarkLocalDirty();
}

void Transform::SetWorldRotation(const glm::quat& inRotation)
{
    if (parent)
    {
        glm::quat parentWorldRot = parent->GetWorldRotation();
        rotation = glm::inverse(parentWorldRot) * inRotation;
    }
    else
    {
        rotation = inRotation;
    }
    MarkLocalDirty();
}

void Transform::RotateLocal(const glm::vec3& axis, float angleRadians)
{
    glm::quat rot = glm::angleAxis(angleRadians, axis);
    rotation = rotation * rot;
    MarkLocalDirty();
}

void Transform::RotateLocal(const glm::quat& inRotation)
{
    rotation = rotation * inRotation;
    MarkLocalDirty();
}

void Transform::SetLocalMatrix(const glm::mat4& inMatrix)
{
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(inMatrix, scale, rotation, position, skew, perspective);

    MarkLocalDirty();
}

void Transform::SetParent(Transform* inParent, bool keepWorldTransform)
{
    if (parent == inParent) return;

    glm::mat4 newMatrix;
    if (keepWorldTransform)
    {
        newMatrix = GetWorldMatrix();
    }

    if (parent)
    {
        parent->RemoveChild(this);
    }

    parent = inParent;

    if (parent)
    {
        parent->AddChild(this);
    }

    if (keepWorldTransform)
    {
        if(parent)
        {
            glm::mat4 parentInverse = glm::inverse(parent->GetWorldMatrix());
            SetLocalMatrix(parentInverse * newMatrix);
        }
        else
        {
            SetLocalMatrix(newMatrix);
        }
    }

    MarkWorldDirty();
}

void Transform::AddChild(Transform* child)
{
    if (eastl::find(children.begin(), children.end(), child) == children.end())
    {
        children.push_back(child);
    }
}

void Transform::RemoveChild(Transform* child)
{
    auto it = eastl::find(children.begin(), children.end(), child);
    if (it != children.end())
    {
        children.erase(it);
    }
}

void Transform::MarkLocalDirty()
{
    isLocalDirty = true;
    MarkWorldDirty();
}

void Transform::MarkWorldDirty()
{
    if (isWorldDirty) return;

    isWorldDirty = true;

    for (Transform* child : children)
    {
        child->MarkWorldDirty();
    }
}

void Transform::RecalculateLocalMatrix()
{
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

    // T * R * S
    localMatrix = translationMatrix * rotationMatrix * scaleMatrix;
    isLocalDirty = false;
}

const glm::mat4& Transform::GetLocalMatrix()
{
    if (isLocalDirty)
    {
        RecalculateLocalMatrix();
    }
    return localMatrix;
}

void Transform::RecalculateWorldMatrix()
{
    const glm::mat4& local = GetLocalMatrix();

    if (parent)
    {
        worldMatrix = parent->GetWorldMatrix() * local;
    }
    else
    {
        worldMatrix = local;
    }

    isWorldDirty = false;
}

const glm::mat4& Transform::GetWorldMatrix()
{
    if (isWorldDirty)
    {
        RecalculateWorldMatrix();
    }
    return worldMatrix;
}

void Transform::OnDebugGui()
{
    ImGui::SeparatorText("Transform");

    glm::vec3 pos = position;
    glm::vec3 rotEuler = glm::degrees(glm::eulerAngles(rotation));
    glm::vec3 scl = scale;

    if (ImGui::DragFloat3("Position", &pos[0], 0.1f))
    {
        SetLocalPosition(pos);
    }

    if (ImGui::DragFloat3("Rotation", &rotEuler[0], 0.1f))
    {
        SetLocalRotation(glm::radians(rotEuler));
    }

    if (ImGui::DragFloat3("Scale", &scl[0], 0.1f))
    {
        SetLocalScale(scl);
    }

    ImGui::Text("Children: %zu", children.size());
    ImGui::Text(isWorldDirty ? "Dirty: Yes" : "Dirty: No");
}