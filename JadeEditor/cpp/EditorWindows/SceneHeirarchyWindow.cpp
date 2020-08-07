#include "EditorWindows/SceneHeirarchyWindow.h"
#include "Gui/ImGuiExtended.h"

#include "jade/core/Entity.h"
#include "jade/components/Transform.h"
#include "FontAwesome.h"

namespace Jade
{
	Scene* SceneHeirarchyWindow::s_CurrentScene = nullptr;

	void SceneHeirarchyWindow::SetScene(Scene* scene)
	{
		s_CurrentScene = scene;
	}

	void SceneHeirarchyWindow::ImGui()
	{
		ImGui::Begin(ICON_FA_PROJECT_DIAGRAM " Scene");
		//int index = 0;
		//auto view = s_CurrentScene->GetRegistry().view<Transform>();
		//for (Entity entity : view)
		//{
		//	Transform& transform = entity.GetComponent<Transform>();
		//	if (transform.m_Parent.IsNull())
		//	{
		//		DoTreeNode(index, transform);
		//	}
		//}
		ImGui::End();
	}

	void SceneHeirarchyWindow::DoTreeNode(int& index, const Transform& transform)
	{
		//static bool isDark = true;
		//std::string str = transform.m_Name;
		//std::string res = str + "##" + std::to_string(index);
		//index++;

		//Entity next = transform.m_Next;
		//while (!next.IsNull())
		//{
		//	const Transform& transform2 = next.GetComponent<Transform>();
		//	std::string str2 = transform2.m_Name;
		//	std::string res2 = str2 + "##" + std::to_string(index);
		//	index++;

		//	if (ImGui::TreeNodeEx(res2.c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth))
		//	{
		//		if (!transform2.m_First.IsNull())
		//		{
		//			const Transform& childTrans = transform2.m_First.GetComponent<Transform>();
		//			DoTreeNode(index, childTrans);
		//		}
		//		ImGui::TreePop();
		//	}
		//	next = transform2.m_Next;
		//}

		//if (ImGui::TreeNodeEx(res.c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth))
		//{
		//	if (!transform.m_First.IsNull())
		//	{
		//		const Transform& firstTransform = transform.m_First.GetComponent<Transform>();
		//		DoTreeNode(index, firstTransform);
		//	}
		//	ImGui::TreePop();
		//}
	}
}