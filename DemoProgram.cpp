#include <fstream>
#include <iostream>
#include <sstream>
#include <vcruntime.h>
#include <vector>
#include <string>
#include "OpenColorIO/OpenColorABI.h"
#include "OpenColorIO/OpenColorAppHelpers.h"
#include "OpenColorIO/OpenColorTransforms.h"
#include "OpenColorIO/OpenColorTypes.h"
#include <OpenColorIO/OpenColorIO.h>

namespace OCIO = OCIO_NAMESPACE;

int main(int argc, char** argv)
{
	OCIO::ConstConfigRcPtr config = OCIO::GetCurrentConfig();
#if 0
	std::cout << "\nLooks:\n";
	for (int i = 0; i < config->getNumLooks(); ++i)
	{
		std::cout << config->getLookNameByIndex(i) << '\n';
	}
	std::vector<const char*> displays{};
	std::cout << "\nDisplays:\n";
	for (int i = 0; i < config->getNumDisplays(); ++i)
	{
		displays.emplace_back(config->getDisplay(i));
		std::cout << displays.back() << '\n';
	}
#endif
	std::vector<const char*> colorSpaces{};
	std::cout << "\nColorSpaces:\n";
	for (int i = 0; i < config->getNumColorSpaces(); ++i)
	{
		colorSpaces.emplace_back(config->getColorSpaceNameByIndex(i));
		std::cout << colorSpaces.back() << '\n';
	}
#if 0
	std::cout << "\nViews:\n";
	for(auto it = displays.begin(); it != displays.end(); ++it)
	{
		for(int i = 0; i < config->getNumViews(*it); ++it)
		{
			std::cout << config->getView(*it, i) << '\n';
		}
	}
#endif
	//const char* display = config->getDefaultDisplay();
	//const char* view = config->getDefaultView(display);
	//const char* look = config->getDisplayViewLooks(display, view);

	//OCIO::DisplayViewTransformRcPtr transform = OCIO::DisplayViewTransform::Create();
	//transform->setSrc(OCIO::ROLE_SCENE_LINEAR);
	//transform->setDisplay(display);
	//transform->setView(view);

	//OCIO::LegacyViewingPipelineRcPtr vpt = OCIO::LegacyViewingPipeline::Create();
	//vpt->setDisplayViewTransform(transform);
	//vpt->setLooksOverrideEnabled(true);
	//vpt->setLooksOverride(look);

	//OCIO::ConstProcessorRcPtr processor = vpt->getProcessor(config, config->getCurrentContext());
	//OCIO::ConstProcessorRcPtr processor = config->getProcessor("ACES - ACEScg", "Output - sRGB");
	OCIO::ConstProcessorRcPtr processor = config->getProcessor("Input - GoPro - Curve - Protune Flat", "Output - P3D65");
	OCIO::ConstCPUProcessorRcPtr cpu = processor->getDefaultCPUProcessor();
	OCIO::ConstGPUProcessorRcPtr gpu = processor->getDefaultGPUProcessor();

	OCIO::GpuShaderDescRcPtr shaderDesc = OCIO::GpuShaderDesc::CreateShaderDesc();
	shaderDesc->setLanguage(OCIO::GPU_LANGUAGE_HLSL_DX11);
	shaderDesc->setFunctionName("OCIODisplay");
	shaderDesc->setResourcePrefix("ocio_");

	gpu->extractGpuShaderInfo(shaderDesc);


	std::cout << shaderDesc->getShaderText() << '\n';
	std::cout << "Num 3D Textures: " << shaderDesc->getNum3DTextures() << '\n';
	std::cout << "Num 1D Textures: " << shaderDesc->getNumTextures() << '\n';



	const char* textureName = nullptr;
	const char* samplerName = nullptr;
	unsigned int edgeLength = 0;
	OCIO::Interpolation interp = OCIO::INTERP_UNKNOWN;
	shaderDesc->get3DTexture(0, textureName, samplerName, edgeLength, interp);
	std::cout << "Texture: " << textureName << " " << samplerName << " " << +edgeLength << '\n';

	const float* values3d = new float[static_cast<int>(pow(edgeLength, 3))];
	shaderDesc->get3DTextureValues(0, values3d);

	OCIO::BakerRcPtr baker = OCIO::Baker::Create();
	baker->setConfig(config);
	baker->setFormat("resolve_cube");
	baker->setInputSpace("role_scene_linear");
	baker->setTargetSpace("ACES - ACEScg");
	{
		std::ofstream file("resolve.cube");
		baker->bake(file);
	}

	//for (unsigned int x = 0; x < edgeLength; ++x)
	//{
	//	for (unsigned int y = 0; y < edgeLength; ++y)
	//	{
	//		for (unsigned int z = 0; z < edgeLength; ++z)
	//		{
	//			std::cout << values3d[x + edgeLength * (y + edgeLength * z)] << " ";
	//		}
	//		std::cout << '\n';
	//	}
	//}

	return EXIT_SUCCESS;
}