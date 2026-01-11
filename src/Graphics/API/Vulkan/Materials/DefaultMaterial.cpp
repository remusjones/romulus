#include <Vulkan/Materials/DefaultMaterial.h>

#include <FileManagement.h>
#include <imgui.h>
#include <LoadUtilities.h>
#include <VulkanGraphicsImpl.h>
#include <Base/Common/Buffers/Texture.h>
#include <Vulkan/RomulusVulkanRenderer.h>

DefaultMaterial::DefaultMaterial(const eastl::string_view& inMaterialName) : Material(inMaterialName)
{
}

void DefaultMaterial::Create()
{
	AddBinding(SCENE_INFORMATION, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL);
	AddBinding(PROPERTIES, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL);
	AddBinding(TEXTURE, 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

	Material::Create();

	for (int i = 0; i < RomulusVulkanRenderer::MAX_FRAMES_IN_FLIGHT; i++)
	{
		SetBuffers(gGraphics->vulkanRenderer->GetFrame(i).sceneBuffer, SCENE_INFORMATION, 0);
	}

	CreateProperties(PROPERTIES, MaterialProperties());
	MakeDefaults();
}

void DefaultMaterial::OnDebugGui()
{
	Material::OnDebugGui();

	ImGui::SeparatorText("Material");
	ImGui::ColorEdit4("Color", &materialProperties.color[0]);
	ImGui::DragFloat("Shininess", &materialProperties.shininess, 0.1f);
	ImGui::DragFloat("Specular", &materialProperties.specularStrength, 0.1f);

	int tmp = materialProperties.debugRenderState;
	if (ImGui::Combo("Debug", &tmp, debugColors.data(), debugColors.size()))
	{
		materialProperties.debugRenderState = static_cast<float>(tmp);
	}
}

static std::unique_ptr<Texture> gDefaultTexture = nullptr;

void DefaultMaterial::MakeDefaults() const
{
	if (gDefaultTexture == nullptr)
	{
		gDefaultTexture = std::make_unique<Texture>();
		eastl::vector<Color_RGBA> defaultColors;
		eastl::vector<eastl::string> paths;

		paths.push_back(FileManagement::GetWorkingDirectory() + DefaultAssetPaths[ALBEDO]);
		paths.push_back(FileManagement::GetWorkingDirectory() + DefaultAssetPaths[NORMAL]);

		gDefaultTexture->LoadImagesFromDisk(paths);
		gDefaultTexture->Create();
	}

	BindTexture(gDefaultTexture->imageBufferInfos, TEXTURE);
}

void DefaultMaterial::Destroy()
{
	if (gDefaultTexture != nullptr)
	{
		gDefaultTexture->Destroy();
		gDefaultTexture.reset();
	}
	Material::Destroy();
}
