#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/renderer/Texture.h"

namespace Cocoa
{
	namespace ProjectWizard
	{
		void Init();
		void ImGui();

		void CreateProjectImGui(bool& windowOpen);
	};
}