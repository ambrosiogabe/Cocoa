#include "externalLibs.h"
#include "jade/core/Core.h"

#include "ImGuiLayer.h"
#include "AssetWizard.h"
#include "EditorWindows/InspectorWindow.h"
#include "EditorWindows/SceneHeirarchyWindow.h"
#include "Gui/ImGuiExtended.h"
#include "Util/Settings.h"
#include "FontAwesome.h"

#include "jade/file/IFile.h"
#include "jade/core/Application.h"
#include "jade/util/JMath.h"
#include "JadeEditorApplication.h"
#include "jade/util/JsonExtended.h"

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
#include <examples/imgui_impl_glfw.cpp>
#include <examples/imgui_impl_opengl3.cpp>
#pragma warning( pop )
#endif

namespace Jade
{
	void HelpMarker(const char* desc);

	ImVec4 From(const glm::vec4& vec4)
	{
		return ImVec4(vec4.x, vec4.y, vec4.z, vec4.w);
	}

	ImVec2 From(const glm::vec2& vec2)
	{
		return ImVec2(vec2.x, vec2.y);
	}

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
		ImGui::LoadIniSettingsFromDisk(Settings::General::s_ImGuiConfigPath.Filepath());

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
		//io.Fonts->AddFontDefault();

		ImFontConfig config;
		Settings::EditorStyle::s_DefaultFont = io.Fonts->AddFontFromFileTTF("assets/fonts/UbuntuMono-Regular.ttf", 24.0f, &config);
		config.MergeMode = true;
		//config.GlyphMinAdvanceX = 30.0f;
		const ImWchar iconRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome-webfont.ttf", 24.0f, &config, iconRanges);
		io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome-webfont2.ttf", 24.0f, &config, iconRanges);
		io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome-webfont3.ttf", 24.0f, &config, iconRanges);
		io.Fonts->Build();

		config.MergeMode = false;
		const ImWchar customIconRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		Settings::EditorStyle::s_LargeIconFont = io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome-webfont.ttf", 64.0f, &config, customIconRanges);
		config.MergeMode = true;
		io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome-webfont2.ttf", 64.0f, &config, customIconRanges);
		io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome-webfont3.ttf", 64.0f, &config, customIconRanges);
		io.Fonts->Build();

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

		if (JadeEditor::IsProjectLoaded())
		{
			SetupDockspace();
			RenderGameViewport();
			AssetWizard::ImGui();
			m_AssetWindow.ImGui();
			InspectorWindow::ImGui();
			SceneHeirarchyWindow::ImGui();
		}
		else
		{
			ProjectWizard::ImGui();
		}
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
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 1));
		ImGui::Begin("Game Viewport", &open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			static bool isPlaying = false;
			if (ImGui::BeginMenu("Play", !isPlaying))
			{
				if (!isPlaying)
				{
					m_Scene->Save(Settings::General::s_EngineAssetsPath + "tmp.jade");
					m_Scene->Play();
					isPlaying = true;
				}
				ImGui::EndMenu();
			}
			else if (ImGui::BeginMenu("Stop", isPlaying))
			{
				if (isPlaying)
				{
					m_Scene->Stop();
					m_Scene->Load(Settings::General::s_EngineAssetsPath + "tmp.jade");
					isPlaying = false;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

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

		ImGui::SetCursorPos(ImVec2(vpX + 8, vpY + 8));

		ImVec2 topLeft = ImGui::GetCursorScreenPos();
		m_GameviewPos.x = topLeft.x;
		m_GameviewPos.y = topLeft.y + aspectHeight;
		Input::SetGameViewPos(m_GameviewPos);
		m_GameviewSize.x = aspectWidth - 16;
		m_GameviewSize.y = aspectHeight - 16;
		Input::SetGameViewSize(m_GameviewSize);

		ImVec2 mousePos = ImGui::GetMousePos() - ImGui::GetCursorScreenPos() - ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
		m_GameviewMousePos.x = mousePos.x;
		m_GameviewMousePos.y = mousePos.y;
		Input::SetGameViewMousePos(m_GameviewMousePos);

		JadeEditor* editor = static_cast<JadeEditor*>(Application::Get());
		//uint32 texId = editor->GetEditorLayer()->GetPickingTextureID();
		uint32 texId = Application::Get()->GetFramebuffer()->GetTexture()->GetId();
		ImGui::Image(reinterpret_cast<void*>(texId), ImVec2(aspectWidth - 16, aspectHeight - 16), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleColor();

		m_BlockEvents = m_GameviewMousePos.x < 0 || m_GameviewMousePos.x > aspectWidth || m_GameviewMousePos.y < 0 || m_GameviewMousePos.y > aspectHeight;
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
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_TitleBg, Settings::EditorStyle::s_Accent);
		ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, Settings::EditorStyle::s_Accent);
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, Settings::EditorStyle::s_Accent);
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, Settings::EditorStyle::s_Accent);
		ImGui::Begin("DockSpace Demo", &p_open, window_flags);

		ImGui::PopStyleVar(5);
		ImGui::PopStyleColor(4);

		// DockSpace
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::PushStyleColor(ImGuiCol_Border, Settings::EditorStyle::s_MainBgDark2);
		ImGui::PushStyleColor(ImGuiCol_PopupBg, Settings::EditorStyle::s_AccentDark0);
		ImGui::PushStyleColor(ImGuiCol_Header, Settings::EditorStyle::s_AccentDark0);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Settings::EditorStyle::s_AccentDark0);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, Settings::EditorStyle::s_AccentDark0);
		m_MenuBar->ImGui();
		ImGui::PopStyleVar(4);
		ImGui::PopStyleColor(5);
		ImGui::End();
	}
	
	void ImGuiLayer::LoadStyle(const JPath& filepath)
	{
		File* styleData = IFile::OpenFile(filepath);
		if (styleData->m_Data.size() > 0)
		{
			json j = json::parse(styleData->m_Data);
			JsonExtended::AssignIfNotNull(j["Colors"]["MainBgLight0"], Settings::EditorStyle::s_MainBgLight0);
			JsonExtended::AssignIfNotNull(j["Colors"]["MainBg"], Settings::EditorStyle::s_MainBg);
			JsonExtended::AssignIfNotNull(j["Colors"]["MainBgDark0"], Settings::EditorStyle::s_MainBgDark0);
			JsonExtended::AssignIfNotNull(j["Colors"]["MainBgDark1"], Settings::EditorStyle::s_MainBgDark1);
			JsonExtended::AssignIfNotNull(j["Colors"]["MainBgDark2"], Settings::EditorStyle::s_MainBgDark2);
			JsonExtended::AssignIfNotNull(j["Colors"]["Accent"], Settings::EditorStyle::s_Accent);
			JsonExtended::AssignIfNotNull(j["Colors"]["AccentDark0"], Settings::EditorStyle::s_AccentDark0);
			JsonExtended::AssignIfNotNull(j["Colors"]["AccentDark1"], Settings::EditorStyle::s_AccentDark1);
			JsonExtended::AssignIfNotNull(j["Colors"]["Button"], Settings::EditorStyle::s_Button);
			JsonExtended::AssignIfNotNull(j["Colors"]["ButtonHovered"], Settings::EditorStyle::s_ButtonHovered);
			JsonExtended::AssignIfNotNull(j["Colors"]["Font"], Settings::EditorStyle::s_Font);
			JsonExtended::AssignIfNotNull(j["Colors"]["FontDisabled"], Settings::EditorStyle::s_FontDisabled);
			JsonExtended::AssignIfNotNull(j["Colors"]["HighlightColor"], Settings::EditorStyle::s_HighlightColor);

			JsonExtended::AssignIfNotNull(j["Sizing"]["WindowPadding"], Settings::EditorStyle::s_WindowPadding);
			JsonExtended::AssignIfNotNull(j["Sizing"]["FramePadding"], Settings::EditorStyle::s_FramePadding);
			JsonExtended::AssignIfNotNull(j["Sizing"]["ItemSpacing"], Settings::EditorStyle::s_ItemSpacing);
			JsonExtended::AssignIfNotNull(j["Sizing"]["ScrollbarSize"], Settings::EditorStyle::s_ScrollbarSize);
			JsonExtended::AssignIfNotNull(j["Sizing"]["ScrollbarRounding"], Settings::EditorStyle::s_ScrollbarRounding);
			JsonExtended::AssignIfNotNull(j["Sizing"]["FrameRounding"], Settings::EditorStyle::s_FrameRounding);
			JsonExtended::AssignIfNotNull(j["Sizing"]["GrabRounding"], Settings::EditorStyle::s_TabRounding);
			JsonExtended::AssignIfNotNull(j["Sizing"]["TabRounding"], Settings::EditorStyle::s_GrabRounding);
		}
		else
		{
			json styles = {
				{"Colors",
					{
						JMath::Serialize("MainBgLight0",    Settings::EditorStyle::s_MainBgLight0),
						JMath::Serialize("MainBg",          Settings::EditorStyle::s_MainBg),
						JMath::Serialize("MainBgDark0",     Settings::EditorStyle::s_MainBgDark0),
						JMath::Serialize("MainBgDark1",     Settings::EditorStyle::s_MainBgDark1),
						JMath::Serialize("MainBgDark2",     Settings::EditorStyle::s_MainBgDark2),
						JMath::Serialize("Accent",          Settings::EditorStyle::s_Accent),
						JMath::Serialize("AccentDark0",     Settings::EditorStyle::s_AccentDark0),
						JMath::Serialize("AccentDark1",     Settings::EditorStyle::s_AccentDark1),
						JMath::Serialize("Button",          Settings::EditorStyle::s_Button),
						JMath::Serialize("ButtonHovered",   Settings::EditorStyle::s_ButtonHovered),
						JMath::Serialize("Font",            Settings::EditorStyle::s_Font),
						JMath::Serialize("FontDisabled",    Settings::EditorStyle::s_FontDisabled),
						JMath::Serialize("Highlight",       Settings::EditorStyle::s_HighlightColor)
					}
				},
				{"Sizing",
					{
						JMath::Serialize("WindowPadding",  Settings::EditorStyle::s_WindowPadding),
						JMath::Serialize("FramePadding",   Settings::EditorStyle::s_FramePadding),
						JMath::Serialize("ItemSpacing",    Settings::EditorStyle::s_ItemSpacing),
						{"ScrollbarSize",                  Settings::EditorStyle::s_ScrollbarSize},
						{"ScrollbarRounding",              Settings::EditorStyle::s_ScrollbarRounding},
						{"FrameRounding",                  Settings::EditorStyle::s_FrameRounding},
						{"GrabRounding",                   Settings::EditorStyle::s_GrabRounding},
						{"TabRounding",                    Settings::EditorStyle::s_TabRounding}
					}
				}
			};
			IFile::WriteFile(styles.dump(4).c_str(), Settings::General::s_EditorStyleData);
		}
		IFile::CloseFile(styleData);

		ImGuiStyle* style = &ImGui::GetStyle();

		// Apply Sizing
		style->WindowPadding = From(Settings::EditorStyle::s_WindowPadding);
		style->WindowBorderSize = 1;
		style->ChildBorderSize = 1;
		style->FramePadding = From(Settings::EditorStyle::s_FramePadding);
		style->ItemSpacing = From(Settings::EditorStyle::s_ItemSpacing);
		style->ScrollbarSize = Settings::EditorStyle::s_ScrollbarSize;
		style->ScrollbarRounding = Settings::EditorStyle::s_ScrollbarRounding;
		style->FrameRounding = Settings::EditorStyle::s_FrameRounding;
		style->GrabRounding = Settings::EditorStyle::s_GrabRounding;
		style->TabRounding = Settings::EditorStyle::s_TabRounding;
		style->FrameBorderSize = 1.0f;

		// Apply Colors
		ImVec4* colors = style->Colors;
		colors[ImGuiCol_WindowBg] = From(Settings::EditorStyle::s_MainBg);
		colors[ImGuiCol_ChildBg] = From(Settings::EditorStyle::s_MainBg);

		colors[ImGuiCol_Text] = From(Settings::EditorStyle::s_Font);
		colors[ImGuiCol_TextDisabled] = From(Settings::EditorStyle::s_FontDisabled);
		colors[ImGuiCol_TextInverted] = From(Settings::EditorStyle::s_MainBgDark1);

		colors[ImGuiCol_FrameBg] = From(Settings::EditorStyle::s_MainBgDark1);
		colors[ImGuiCol_FrameBgHovered] = From(Settings::EditorStyle::s_MainBgDark0);
		colors[ImGuiCol_FrameBgActive] = From(Settings::EditorStyle::s_MainBgDark2);

		colors[ImGuiCol_TitleBg] = From(Settings::EditorStyle::s_MainBgDark0);
		colors[ImGuiCol_TitleBgCollapsed] = From(Settings::EditorStyle::s_MainBgDark0);
		colors[ImGuiCol_TitleBgActive] = From(Settings::EditorStyle::s_MainBgDark0);
		colors[ImGuiCol_MenuBarBg] = From(Settings::EditorStyle::s_MainBgDark0);
		colors[ImGuiCol_MenuBarButtonBg] = From(Settings::EditorStyle::s_AccentDark0);
		colors[ImGuiCol_MenuBarButtonBgHover] = From(Settings::EditorStyle::s_AccentDark1);
		colors[ImGuiCol_MenuBarButtonBgActive] = From(Settings::EditorStyle::s_AccentDark1);

		colors[ImGuiCol_Tab] = From(Settings::EditorStyle::s_MainBgDark0);
		colors[ImGuiCol_TabUnfocused] = From(Settings::EditorStyle::s_MainBgDark0);
		colors[ImGuiCol_TabHovered] = From(Settings::EditorStyle::s_MainBgDark1);
		colors[ImGuiCol_TabActive] = From(Settings::EditorStyle::s_MainBgDark1);
		colors[ImGuiCol_TabUnfocusedActive] = From(Settings::EditorStyle::s_MainBgDark1);

		colors[ImGuiCol_ScrollbarBg] = From(Settings::EditorStyle::s_MainBgDark1);
		colors[ImGuiCol_ScrollbarGrab] = From(Settings::EditorStyle::s_Font);
		colors[ImGuiCol_ScrollbarGrabActive] = From(Settings::EditorStyle::s_FontDisabled);
		colors[ImGuiCol_ScrollbarGrabHovered] = From(Settings::EditorStyle::s_FontDisabled);
		colors[ImGuiCol_CheckMark] = From(Settings::EditorStyle::s_Font);
		colors[ImGuiCol_SliderGrab] = From(Settings::EditorStyle::s_Font);
		colors[ImGuiCol_SliderGrabActive] = From(Settings::EditorStyle::s_FontDisabled);

		colors[ImGuiCol_Button] = From(Settings::EditorStyle::s_Button);
		colors[ImGuiCol_ButtonHovered] = From(Settings::EditorStyle::s_ButtonHovered);
		colors[ImGuiCol_ButtonActive] = From(Settings::EditorStyle::s_ButtonHovered);

		colors[ImGuiCol_Header] = From(Settings::EditorStyle::s_MainBg);
		colors[ImGuiCol_HeaderHovered] = From(Settings::EditorStyle::s_MainBgDark0);
		colors[ImGuiCol_HeaderActive] = From(Settings::EditorStyle::s_MainBgDark1);

		colors[ImGuiCol_Separator] = From(Settings::EditorStyle::s_MainBgLight0);
		colors[ImGuiCol_SeparatorHovered] = From(Settings::EditorStyle::s_MainBgLight0);
		colors[ImGuiCol_SeparatorActive] = From(Settings::EditorStyle::s_MainBgLight0);
		colors[ImGuiCol_Border] = From(Settings::EditorStyle::s_MainBgLight0);

		colors[ImGuiCol_ResizeGrip] = From(Settings::EditorStyle::s_MainBg);
		colors[ImGuiCol_ResizeGripHovered] = From(Settings::EditorStyle::s_MainBg);
		colors[ImGuiCol_ResizeGripActive] = From(Settings::EditorStyle::s_MainBg);

		colors[ImGuiCol_DockingPreview] = From(Settings::EditorStyle::s_AccentDark0);
		colors[ImGuiCol_TextSelectedBg] = From(Settings::EditorStyle::s_HighlightColor);
		colors[ImGuiCol_NavHighlight] = From(Settings::EditorStyle::s_AccentDark0);
	}
}