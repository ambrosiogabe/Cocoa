#pragma once
#include "cocoa/scenes/Scene.h"

namespace Cocoa
{
	class SceneHeirarchyWindow
	{
	public:
		static void SetScene(Scene* scene);

		static void ImGui();

	private:
		static void DoTreeNode(int& index, const Transform& transform);

	private:
		static Scene* s_CurrentScene;
	};
}