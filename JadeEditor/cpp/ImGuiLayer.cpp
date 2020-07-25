#include "externalLibs.h"
#include "jade/core/Core.h"

#include "ImGuiLayer.h"
#include "AssetWizard.h"
#include "jade/file/IFile.h"
#include "jade/core/Application.h"
#include "jade/util/JMath.h"

#include <examples/imgui_impl_glfw.h>
#ifndef _JADE_IMPL_IMGUI
#define _JADE_IMPL_IMGUI
#pragma warning( push )
#pragma warning ( disable : 26812 )
#pragma warning ( disable : 26451 )
#pragma warning ( disable : 6031 )
#pragma warning ( disable : 26495 )
#pragma warning ( disable : 4996 )
#include <examples/imgui_impl_opengl3.h>
#include <imgui/examples/imgui_impl_glfw.cpp>
#include <imgui/examples/imgui_impl_opengl3.cpp>
#pragma warning( pop )
#endif

namespace Jade
{
	static void CustomStyle(bool bStyleDark_ = true, float alpha_ = 0.8f);
	static void LoadStyle(const std::string filepath);
	void HelpMarker(const char* desc);

	void ImGuiLayer::OnAttach()
	{
		Setup(Application::Get()->GetWindow()->GetNativeWindow());
	}

	void ImGuiLayer::Setup(void* window)
	{
		m_Window = window;

		// Setup dear imGui binding
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		// Init GLFW
		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_Window, true);

		// Init OpenGL imgui implementation
		// GL 3.0 + GLSL 130
		const char* glsl_version = "#version 130";
		ImGui_ImplOpenGL3_Init(glsl_version);

		// ImGui style 
		//ImGui::StyleColorsLight();

		//io.Fonts->AddFontFromFileTTF("assets/fonts/segoeui.ttf", 32);
		io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-Regular.ttf", 32.0f);
		//CustomStyle();
		LoadStyle(Settings::General::s_EditorStyleData);
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			//e.m_Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			//e.m_Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
			e.m_Handled = true;
		}
	}

	void ImGuiLayer::BeginFrame()
	{
		// Start ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		SetupDockspace();
		RenderGameViewport();
		AssetWizard::ImGui();
	}

	void ImGuiLayer::EndFrame()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, Application::Get()->GetWindow()->GetWidth(), Application::Get()->GetWindow()->GetHeight());
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		// Render ImGui frame
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		GLFWwindow* backupCurrentContext = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backupCurrentContext);
	}

	void ImGuiLayer::RenderGameViewport()
	{
		static bool open = true;
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1));
		ImGui::Begin("Game Viewport", &open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			static bool isPlaying = false;
			if (ImGui::BeginMenu("Play", !isPlaying))
			{
				if (!isPlaying)
				{
					Application::Get()->GetScene()->Save("tmp.jade");
					Application::Get()->GetScene()->Play();
					isPlaying = true;
				}
				ImGui::EndMenu();
			}
			else if (ImGui::BeginMenu("Stop", isPlaying))
			{
				if (isPlaying)
				{
					Application::Get()->GetScene()->Stop();
					Application::Get()->GetScene()->Load("tmp.jade");
					isPlaying = false;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		m_BlockEvents = !ImGui::IsWindowFocused() || !ImGui::IsWindowHovered();

		ImVec2 windowSize = ImGui::GetWindowContentRegionMax();
		// Figure out the largest area that fits this target aspect ratio
		float aspectWidth = windowSize.x;
		float aspectHeight = (float)aspectWidth / Application::Get()->GetWindow()->GetTargetAspectRatio();
		if (aspectHeight > windowSize.y)
		{
			// It doesn't fit our height, we must switch to pillarbox
			aspectHeight = windowSize.y;
			aspectWidth = (float)aspectHeight * Application::Get()->GetWindow()->GetTargetAspectRatio();
		}

		// Center rectangle
		float vpX = ((float)windowSize.x / 2.0f) - ((float)aspectWidth / 2.0f);
		float vpY = ((float)windowSize.y / 2.0f) - ((float)aspectHeight / 2.0f);

		ImGui::SetCursorPos(ImVec2(vpX, vpY));

		ImVec2 topLeft = ImGui::GetCursorScreenPos();
		m_GameviewPos.x = topLeft.x;
		m_GameviewPos.y = topLeft.y + aspectHeight;
		Input::SetGameViewPos(m_GameviewPos);
		m_GameviewSize.x = aspectWidth;
		m_GameviewSize.y = aspectHeight;
		Input::SetGameViewSize(m_GameviewSize);

		ImVec2 mousePos = ImGui::GetMousePos() - ImGui::GetCursorScreenPos() - ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
		m_GameviewMousePos.x = mousePos.x;
		m_GameviewMousePos.y = mousePos.y;
		Input::SetGameViewMousePos(m_GameviewMousePos);

		ImGui::Image(reinterpret_cast<void*>(Application::Get()->GetFramebuffer()->GetId()), ImVec2(aspectWidth, aspectHeight), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleColor();
	}

	void ImGuiLayer::SetupDockspace()
	{
		static bool p_open = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &p_open, window_flags);
		ImGui::PopStyleVar();

		ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

		m_MenuBar.ImGui();
		ImGui::End();
	}


	ImVec4 From(const glm::vec4& vec4)
	{
		return ImVec4(vec4.x, vec4.y, vec4.z, vec4.w);
	}

	void AssignIfNotNull(json& j, glm::vec4& vector)
	{
		if (!j.is_null())
		{
			bool success = true;
			glm::vec4 tmp = JMath::DeserializeVec4(j, success);
			if (success)
			{
				vector = tmp;
			}
		}
	}

	void LoadStyle(const std::string filepath)
	{
		File* styleData = IFile::OpenFile(Settings::General::s_EditorStyleData.c_str());
		if (styleData->m_Data.size() > 0)
		{
			json j = json::parse(styleData->m_Data);
			AssignIfNotNull(j["DarkBgColor"], Settings::EditorStyle::s_DarkBgColor);
			AssignIfNotNull(j["LighBgColor"], Settings::EditorStyle::s_LightBgColor);
			AssignIfNotNull(j["DarkInset"], Settings::EditorStyle::s_DarkInset);
			AssignIfNotNull(j["DarkAccent"], Settings::EditorStyle::s_DarkAccentColor);
			AssignIfNotNull(j["LightAccent"], Settings::EditorStyle::s_LightAccentColor);
		}
		else
		{
			json styles = {
				JMath::Serialize("DarkBgColor", Settings::EditorStyle::s_DarkBgColor),
				JMath::Serialize("LightBgColor", Settings::EditorStyle::s_LightBgColor),
				JMath::Serialize("DarkInset", Settings::EditorStyle::s_DarkInset),
				JMath::Serialize("DarkAccent", Settings::EditorStyle::s_DarkAccentColor),
				JMath::Serialize("LightAccent", Settings::EditorStyle::s_LightAccentColor)
			};
			IFile::WriteFile(styles.dump(4).c_str(), Settings::General::s_EditorStyleData.c_str());
		}
		IFile::CloseFile(styleData);

		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;
		colors[ImGuiCol_WindowBg] = From(Settings::EditorStyle::s_DarkBgColor);
		colors[ImGuiCol_FrameBg] = From(Settings::EditorStyle::s_DarkInset);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.58f, 0.58f, 0.58f, 0.67f);
		colors[ImGuiCol_TitleBg] = From(Settings::EditorStyle::s_DarkBgColor);
		colors[ImGuiCol_TitleBgActive] = From(Settings::EditorStyle::s_LightBgColor);
		colors[ImGuiCol_MenuBarBg] = From(Settings::EditorStyle::s_DarkBgColor);
		colors[ImGuiCol_ScrollbarBg] = From(Settings::EditorStyle::s_DarkBgColor);
		colors[ImGuiCol_CheckMark] = From(Settings::EditorStyle::s_LightAccentColor);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = From(Settings::EditorStyle::s_DarkAccentColor);
		colors[ImGuiCol_Button] = From(Settings::EditorStyle::s_LightBgColor);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.15f, 0.55f, 0.38f, 1.00f);
		colors[ImGuiCol_ButtonActive] = From(Settings::EditorStyle::s_DarkAccentColor);
		colors[ImGuiCol_Header] = From(Settings::EditorStyle::s_LightBgColor);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_HeaderActive] = From(Settings::EditorStyle::s_LightBgColor);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.43f, 0.43f, 0.43f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.21f, 0.21f, 0.21f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.39f, 0.39f, 0.39f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.44f, 0.44f, 0.44f, 0.95f);
		colors[ImGuiCol_Tab] = From(Settings::EditorStyle::s_DarkBgColor);
		colors[ImGuiCol_TabHovered] = From(Settings::EditorStyle::s_LightBgColor);
		colors[ImGuiCol_TabActive] = From(Settings::EditorStyle::s_LightBgColor);
		colors[ImGuiCol_TabUnfocusedActive] = From(Settings::EditorStyle::s_LightBgColor);
		colors[ImGuiCol_DockingPreview] = From(Settings::EditorStyle::s_DarkAccentColor);
		colors[ImGuiCol_TextSelectedBg] = From(Settings::EditorStyle::s_DarkAccentColor);
		colors[ImGuiCol_NavHighlight] = From(Settings::EditorStyle::s_DarkAccentColor);
	}

	void CustomStyle(bool bStyleDark_, float alpha_)
	{
		ImGuiStyle* style = &ImGui::GetStyle();

		style->WindowPadding = ImVec2(15, 15);
		style->WindowRounding = 5.0f;
		style->FramePadding = ImVec2(5, 5);
		style->FrameRounding = 4.0f;
		style->ItemSpacing = ImVec2(12, 8);
		style->ItemInnerSpacing = ImVec2(8, 6);
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 9.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 3.0f;

		style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		//style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		//style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
		style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		//style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		//style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		//style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		//style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
		//style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
		//style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
		style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

		//style->ChildRounding = 4.0f;
		//style->FrameBorderSize = 1.0f;
		//style->FrameRounding = 2.0f;
		//style->GrabMinSize = 7.0f;
		//style->PopupRounding = 2.0f;
		//style->ScrollbarRounding = 12.0f;
		//style->ScrollbarSize = 13.0f;
		//style->TabBorderSize = 1.0f;
		//style->TabRounding = 0.0f;
		//style->WindowRounding = 4.0f;
	}
}