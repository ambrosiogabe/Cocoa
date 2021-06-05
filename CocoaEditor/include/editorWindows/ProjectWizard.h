#ifndef COCOA_EDITOR_PROJECT_WIZARD_H
#define COCOA_EDITOR_PROJECT_WIZARD_H
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	namespace ProjectWizard
	{
		void init();
		void imgui(SceneData& scene);

		void createProjectImGui(SceneData& scene, bool& windowOpen);
	};
}

#endif
