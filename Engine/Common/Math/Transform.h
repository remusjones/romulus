//
// Created by Remus on 22/12/2023.
//

#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Objects/ImGuiDebugLayer.h"


class Transform final : public ImGuiDebugLayer
{
public:
	Transform();

	//TODO: Make vectors constant
	glm::vec3 Forward() const { return rotation * glm::vec3(0, 0, 1); }
	glm::vec3 Up() const { return rotation * glm::vec3(0, 1, 0); }
	glm::vec3 Right() const { return rotation * glm::vec3(1, 0, 0); }

	// todo: all these should be cached for multiple requests per changed vector
	// - can we hash the matrix and do cache to avoid recalculating all these per get?
	glm::vec3 GetLocalPosition() const;
	glm::quat GetLocalRotation() const;
	glm::vec3 GetLocalEuler() const;
	glm::vec3 GetLocalScale() const;
	glm::vec3 GetWorldPosition();
	glm::quat GetWorldRotation();
	glm::vec3 GetWorldScale();

	void Translate(const glm::vec3& translation);
	void TranslateLocal(const glm::vec3& translation);

	void SetLocalPosition(const glm::vec3& inPosition);
	void SetWorldPosition(const glm::vec3& inPositon);

	void SetLocalRotation(const glm::vec3& inRotation);
	void SetLocalRotation(const glm::quat& inRotation);
	void SetWorldRotation(const glm::quat& inRotation);

	void RotateAxisLocal(float inAngle, glm::vec3 inRotation);
	void RotateAxisLocal(const glm::vec2& inEulerAxisRotation);
	void RotateAxisLocal(const glm::vec3& inEulerRotation);
	void LocalRotate(const glm::quat& inRotation);

	void SetLocalScale(const glm::vec3& inScale);
	void SetWorldScale(const glm::vec3& inScale);

	void SetMatrix(const glm::mat4& inMatrix);

	void SetDirty();
	bool GetDirty() const;

	void SetParent(Transform* inParent);
	Transform* GetParent() const;
	std::vector<Transform*> GetChildren() const;
	size_t GetChildCount() const;

	glm::mat4 GetWorldMatrix();
	glm::mat4 GetLocalMatrix();

	glm::mat4 GetRotationMatrix() const { return glm::mat4_cast(rotation); }
	glm::mat4 GetTranslationMatrix() const { return glm::translate(glm::identity<glm::mat4>(), position); }
	glm::mat4 GetScaleMatrix() const { return glm::scale(glm::identity<glm::mat4>(), scale); }

	void OnImGuiRender() override;

private:
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	glm::mat4 localMatrix;
	bool matrixIsDirtyFlag;

	Transform* parent;
	std::vector<Transform*> children;

	void RemoveChild(Transform* child);
	void AddChild(Transform* child);
	void UpdateLocalMatrix();
};
