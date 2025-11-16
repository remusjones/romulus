#include "Transform.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>

#include "imgui.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "Objects/Camera.h"
#include "Objects/Camera.h"

Transform::Transform() : position(0), rotation(glm::identity<glm::quat>()), scale(1), localMatrix(),
                         matrixIsDirtyFlag(true), parent(nullptr)
{
}

glm::vec3 Transform::GetWorldPosition()
{
	return glm::vec3(GetWorldMatrix()[3]);
}

glm::quat Transform::GetWorldRotation()
{
	const glm::mat4 matrix = GetWorldMatrix();
	const glm::mat3 upper_mat3(matrix);
	return glm::quat(upper_mat3);
}

glm::vec3 Transform::GetWorldScale()
{
	glm::mat4 matrix = GetWorldMatrix();
	glm::vec3 vectorScale;
	vectorScale.x = glm::length(glm::vec3(matrix[0]));
	vectorScale.y = glm::length(glm::vec3(matrix[1]));
	vectorScale.z = glm::length(glm::vec3(matrix[2]));
	return vectorScale;
}

glm::vec3 Transform::GetLocalPosition() const
{
	return position;
}

glm::quat Transform::GetLocalRotation() const
{
	return rotation;
}

glm::vec3 Transform::GetLocalEuler() const
{
	return glm::degrees(glm::eulerAngles(rotation));
}

glm::vec3 Transform::GetLocalScale() const
{
	return scale;
}

void Transform::Translate(const glm::vec3& translation)
{
	position += translation;
	SetDirty();
}

void Transform::TranslateLocal(const glm::vec3& translation)
{
	position += rotation * translation;
	SetDirty();
}

void Transform::SetLocalPosition(const glm::vec3& inPosition)
{
	position = inPosition;
	SetDirty();
}

void Transform::SetLocalRotation(const glm::vec3& inRotation)
{
	const glm::vec3 axisRotation = glm::radians(inRotation);
	rotation                    = axisRotation;
	SetDirty();
}

void Transform::SetLocalRotation(const glm::quat& inRotation)
{
	rotation = inRotation;
	SetDirty();
}

void Transform::SetWorldPosition(const glm::vec3& inPositon)
{
	if (parent != nullptr)
	{
		const glm::vec3 parentPosition = parent->GetWorldPosition();
		position = inPositon - parentPosition;
	}
	else
	{
		position = inPositon;
	}
	SetDirty();
}

void Transform::SetWorldRotation(const glm::quat& inRotation)
{
	if (parent != nullptr)
	{
		const glm::quat parentRotation = parent->GetWorldRotation();
		rotation = glm::inverse(parentRotation) * inRotation;
	}
	else
	{
		rotation = inRotation;
	}
	SetDirty();
}

void Transform::SetWorldScale(const glm::vec3& inScale)
{
	if (parent != nullptr)
	{
		const glm::vec3 parentScale = parent->GetWorldScale();
		scale = inScale / parentScale;
	}
	else
	{
		scale = inScale;
	}
	SetDirty();
}

void Transform::RotateAxisLocal(float inAngle, glm::vec3 inRotation)
{
	rotation = rotation * glm::angleAxis(inAngle, inRotation);
	SetDirty();
}

void Transform::RotateAxisLocal(const glm::vec2& inEulerAxisRotation)
{
	const glm::vec2 axisRotation = glm::radians(inEulerAxisRotation);

	rotation = rotation * glm::angleAxis(axisRotation.x, glm::vec3(1, 0, 0));
	rotation = glm::angleAxis(axisRotation.y, glm::vec3(0, 1, 0)) * rotation;
	SetDirty();
}

void Transform::RotateAxisLocal(const glm::vec3& inEulerRotation)
{
	const glm::vec3 axisRotation = glm::radians(inEulerRotation);

	rotation = rotation * glm::angleAxis(axisRotation.x, glm::vec3(1, 0, 0));
	rotation = glm::angleAxis(axisRotation.y, glm::vec3(0, 1, 0)) * rotation;
	rotation = rotation * glm::angleAxis(axisRotation.z, glm::vec3(0, 0, 1));
	SetDirty();
}

void Transform::LocalRotate(const glm::quat& inRotation)
{
	rotation *= inRotation;
	SetDirty();
}


void Transform::SetLocalScale(const glm::vec3& inScale)
{
	scale = inScale;
	SetDirty();
}

void Transform::SetMatrix(const glm::mat4& inMatrix)
{
	position = glm::vec3(inMatrix[3][0], inMatrix[3][1], inMatrix[3][2]);

	scale.x = glm::length(glm::vec3(inMatrix[0][0], inMatrix[0][1], inMatrix[0][2]));
	scale.y = glm::length(glm::vec3(inMatrix[1][0], inMatrix[1][1], inMatrix[1][2]));
	scale.z = glm::length(glm::vec3(inMatrix[2][0], inMatrix[2][1], inMatrix[2][2]));

	glm::mat3 rotationMatrix;
	rotationMatrix[0] = glm::normalize(glm::vec3(inMatrix[0]));
	rotationMatrix[1] = glm::normalize(glm::vec3(inMatrix[1]));
	rotationMatrix[2] = glm::normalize(glm::vec3(inMatrix[2]));
	rotation          = glm::quat(rotationMatrix);

	SetDirty();
}

void Transform::SetDirty()
{
	matrixIsDirtyFlag = true;
}

bool Transform::GetDirty() const
{
	return matrixIsDirtyFlag;
}

void Transform::SetParent(Transform* inParent)
{
	if (parent != nullptr)
	{
		parent->RemoveChild(this);
	}

	parent = inParent;

	if (parent != nullptr)
	{
		parent->AddChild(this);
		parent->SetDirty();
	}


	SetDirty();
}

Transform* Transform::GetParent() const
{
	return parent;
}

std::vector<Transform*> Transform::GetChildren() const
{
	return children;
}

size_t Transform::GetChildCount() const
{
	return children.size();
}

void Transform::RemoveChild(Transform* child)
{
	if (const auto it = std::ranges::find(children, child); it != children.end())
	{
		children.erase(it);
	}

	child->SetParent(nullptr);
	parent->SetDirty();
}

void Transform::AddChild(Transform* child)
{
	children.push_back(child);
	child->SetDirty();
}

void Transform::UpdateLocalMatrix()
{
	localMatrix = glm::mat4(1.0f);
	localMatrix = glm::translate(localMatrix, position);
	localMatrix = localMatrix * glm::mat4_cast(rotation);
	localMatrix = glm::scale(localMatrix, scale);
}

glm::mat4 Transform::GetWorldMatrix()
{
	if (matrixIsDirtyFlag)
	{
		UpdateLocalMatrix();
		for (const auto child : children)
		{
			child->SetDirty();
		}
		matrixIsDirtyFlag = false;
	}
	if (parent)
		return parent->GetWorldMatrix() * localMatrix;

	return localMatrix;
}

glm::mat4 Transform::GetLocalMatrix()
{
	if (matrixIsDirtyFlag)
	{
		UpdateLocalMatrix();
		for (const auto child : children)
		{
			child->SetDirty();
		}
		matrixIsDirtyFlag = false;
	}

	return localMatrix;
}

void Transform::OnImGuiRender()
{
	ImGui::SeparatorText("Transform");
	glm::vec3 rot = GetLocalEuler();
	if (ImGui::DragFloat3("Position", &position[0], 0.1f))
	{
		SetDirty();
	}
	if (ImGui::DragFloat3("Rotation", &rot[0], 0.1f))
	{
		SetLocalRotation(rot);
	}
	if (ImGui::DragFloat3("Scale", &scale[0], 0.1f))
	{
		SetDirty();
	}
}
