#include <Vulkan/Materials/DefaultMaterial.h>

#include <FileManagement.h>
#include <imgui.h>
#include <LoadUtilities.h>
#include <VulkanGraphicsImpl.h>
#include <Base/Common/Buffers/Texture.h>
#include <Vulkan/RomulusVulkanRenderer.h>

DefaultMaterial::DefaultMaterial(const char* inMaterialName)
{
	materialName = inMaterialName;
}

void DefaultMaterial::Create(MaterialBase* baseMaterial)
{
	AddBinding(SCENE_INFORMATION, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL);
	AddBinding(PROPERTIES, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL);
	AddBinding(TEXTURE, 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

	Material::Create(this);

	for (int i = 0; i < RomulusVulkanRenderer::MAX_FRAMES_IN_FLIGHT; i++)
	{
		SetBuffers(gGraphics->vulkanRenderer->GetFrame(i).sceneBuffer, SCENE_INFORMATION, 0);
	}

	CreateProperties(PROPERTIES, MaterialProperties());
	MakeDefaults();
}

void DefaultMaterial::OnImGuiRender()
{
	Material::OnImGuiRender();

	ImGui::SeparatorText("Material");
	ImGui::ColorEdit4(GetUniqueLabel("Color"), &materialProperties.color[0]);
	ImGui::DragFloat(GetUniqueLabel("Shininess"), &materialProperties.shininess, 0.1f);
	ImGui::DragFloat(GetUniqueLabel("Specular"), &materialProperties.specularStrength, 0.1f);

	int tmp = materialProperties.debugRenderState;
	if (ImGui::Combo(GetUniqueLabel("Debug"), &tmp, debugColors.data(), debugColors.size()))
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
		std::vector<Color_RGBA> defaultColors;
		std::vector<std::string> paths;

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
