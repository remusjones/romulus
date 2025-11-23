//
// Created by Remus on 22/12/2023.
//

#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "EASTL/vector.h"
#include "Objects/IDebuggable.h"

class Transform final : public IDebuggable
{
public:
    Transform();
    ~Transform();

    [[nodiscard]] glm::vec3 Forward() const { return glm::rotate(rotation, WorldForward); }
    [[nodiscard]] glm::vec3 Up() const      { return glm::rotate(rotation, WorldUp); }
    [[nodiscard]] glm::vec3 Right() const   { return glm::rotate(rotation, WorldRight); }

    [[nodiscard]] const glm::vec3& GetLocalPosition() const { return position; }
    [[nodiscard]] const glm::quat& GetLocalRotation() const { return rotation; }
    [[nodiscard]] const glm::vec3& GetLocalScale() const    { return scale; }
    [[nodiscard]] glm::vec3 GetLocalEuler() const           { return glm::eulerAngles(rotation); }

    [[nodiscard]] glm::vec3 GetWorldPosition();
    [[nodiscard]] glm::quat GetWorldRotation();
    [[nodiscard]] glm::vec3 GetWorldScale();

    // ==================================================================================
    // Setters (Triggers Dirty Flags)
    // ==================================================================================

    void Translate(const glm::vec3& translation);
    void TranslateLocal(const glm::vec3& translation);

    void SetLocalPosition(const glm::vec3& inPosition);
    void SetWorldPosition(const glm::vec3& inPosition);

    void SetLocalRotation(const glm::vec3& inEulerAngles);
    void SetLocalRotation(const glm::quat& inRotation);
    void SetWorldRotation(const glm::quat& inRotation);

    void RotateLocal(const glm::vec3& axis, float angleRadians);
    void RotateLocal(const glm::quat& rotation);

    void SetLocalScale(const glm::vec3& inScale);
    void SetLocalMatrix(const glm::mat4& inMatrix);

    void SetParent(Transform* inParent, bool keepWorldTransform = true);
    void AddChild(Transform* child);
    void RemoveChild(Transform* child);
    [[nodiscard]] Transform* GetParent() const { return parent; }
    [[nodiscard]] const eastl::vector<Transform*>& GetChildren() const { return children; }
    [[nodiscard]] size_t GetChildCount() const { return children.size(); }

    // Returns the cached world matrix, recalculating if dirty
    [[nodiscard]] const glm::mat4& GetWorldMatrix();

    // Returns the cached local matrix, recalculating if dirty
    [[nodiscard]] const glm::mat4& GetLocalMatrix();

    void OnDebugGui() override;

private:
    void MarkWorldDirty();
    void MarkLocalDirty();

    void RecalculateLocalMatrix();
    void RecalculateWorldMatrix();

private:
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity
    glm::vec3 scale    = glm::vec3(1.0f);

    // Caches
    glm::mat4 localMatrix = glm::mat4(1.0f);
    glm::mat4 worldMatrix = glm::mat4(1.0f);

    // Hierarchy
    Transform* parent = nullptr;
    eastl::vector<Transform*> children;

    // Dirty Flags
    bool isLocalDirty = true;
    bool isWorldDirty = true;

    static constexpr glm::vec3 WorldUp      = glm::vec3(0, 1, 0);
    static constexpr glm::vec3 WorldForward = glm::vec3(0, 0, 1);
    static constexpr glm::vec3 WorldRight   = glm::vec3(1, 0, 0);
};