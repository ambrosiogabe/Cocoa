#include "core/ImGuiLayer.h"
#include "core/CocoaEditorApplication.h"
#include "editorWindows/InspectorWindow.h"
#include "editorWindows/SceneHeirarchyWindow.h"
#include "editorWindows/ProjectWizard.h"
#include "editorWindows/GameViewport.h"
#include "editorWindows/GameEditorViewport.h"
#include "gui/ImGuiExtended.h"
#include "gui/FontAwesome.h"
#include "util/Settings.h"

#include "cocoa/file/File.h"
#include "cocoa/util/CMath.h"
#include "cocoa/util/JsonExtended.h"

#ifndef _JADE_IMPL
#define _JADE_IMPL
#ifdef IMGUI_IMPL_API
#undef IMGUI_IMPL_API
#endif
#define IMGUI_IMPL_API __declspec(dllexport)
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
#include <examples/imgui_impl_glfw.h>

namespace Cocoa
{
	namespace ImGuiLayer
	{
		// Internal Declarations
		static bool m_HoveringGameEditorWindow;
		static void* m_Window;

		// Forward Declarations
		void SetupDockspace(SceneData& scene);

		void Init(void* window)
		{
			m_Window = window;

			// Setup dear imGui binding
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGui::LoadIniSettingsFromDisk(Settings::General::s_ImGuiConfigPath.Path);

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
			ImFontConfig config;
			Settings::EditorStyle::s_DefaultFont = io.Fonts->AddFontFromFileTTF("assets/fonts/UbuntuMono-Regular.ttf", 24.0f, &config);
			config.MergeMode = true;
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

			LoadStyle(Settings::General::s_EditorStyleData);

			SceneHeirarchyWindow::Init();
		}

		void Destroy()
		{
			SceneHeirarchyWindow::Destroy();
		}

		void OnEvent(SceneData& scene, Event& e)
		{
			// Find way to block events when necessary
			//if (m_BlockEvents)
			//{
			//	ImGuiIO& io = ImGui::GetIO();
			//	//e.m_Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			//	//e.m_Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
			//	e.m_Handled = true;
			//}

			if (!m_HoveringGameEditorWindow && ImGui::GetIO().WantCaptureMouse)
			{
				e.m_Handled = e.IsInCategory(EventCategoryMouse) ? true : false;
			}
		}

		void BeginFrame(SceneData& scene)
		{
			// Start ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			if (CocoaEditor::isProjectLoaded())
			{
				SetupDockspace(scene);
				GameEditorViewport::ImGui(scene, &m_HoveringGameEditorWindow);
				GameViewport::ImGui(scene);
				AssetWindow::ImGui(scene);
				InspectorWindow::ImGui(scene);
				SceneHeirarchyWindow::ImGui(scene);
				if (Settings::Editor::ShowDemoWindow)
				{
					ImGui::ShowDemoWindow(&Settings::Editor::ShowDemoWindow);
				}
			}
			else
			{
				ProjectWizard::ImGui(scene);
			}
		}

		void EndFrame()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glViewport(0, 0, Application::get()->getWindow()->getWidth(), Application::get()->getWindow()->getHeight());
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

		void SetupDockspace(SceneData& scene)
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
			//ImGui::PushStyleColor(ImGuiCol_Header, Settings::EditorStyle::s_AccentDark0);
			//ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Settings::EditorStyle::s_AccentDark0);
			//ImGui::PushStyleColor(ImGuiCol_HeaderActive, Settings::EditorStyle::s_AccentDark0);
			MenuBar::ImGui(scene);
			ImGui::PopStyleVar(4);
			ImGui::PopStyleColor(2);
			ImGui::End();
		}

		void LoadStyle(const CPath& filepath)
		{
			FileHandle* styleData = File::OpenFile(filepath);
			if (styleData->m_Size > 0)
			{
				json j = json::parse(styleData->m_Data);
				if (j.contains("Colors"))
				{
					json& subJ = j["Colors"];
					JsonExtended::AssignIfNotNull(subJ, "MainBgLight0", Settings::EditorStyle::s_MainBgLight0);
					JsonExtended::AssignIfNotNull(subJ, "MainBg", Settings::EditorStyle::s_MainBg);
					JsonExtended::AssignIfNotNull(subJ, "MainBgDark0", Settings::EditorStyle::s_MainBgDark0);
					JsonExtended::AssignIfNotNull(subJ, "MainBgDark1", Settings::EditorStyle::s_MainBgDark1);
					JsonExtended::AssignIfNotNull(subJ, "MainBgDark2", Settings::EditorStyle::s_MainBgDark2);
					JsonExtended::AssignIfNotNull(subJ, "Accent", Settings::EditorStyle::s_Accent);
					JsonExtended::AssignIfNotNull(subJ, "AccentDark0", Settings::EditorStyle::s_AccentDark0);
					JsonExtended::AssignIfNotNull(subJ, "AccentDark1", Settings::EditorStyle::s_AccentDark1);
					JsonExtended::AssignIfNotNull(subJ, "Button", Settings::EditorStyle::s_Button);
					JsonExtended::AssignIfNotNull(subJ, "ButtonHovered", Settings::EditorStyle::s_ButtonHovered);
					JsonExtended::AssignIfNotNull(subJ, "Header", Settings::EditorStyle::s_Header);
					JsonExtended::AssignIfNotNull(subJ, "HeaderHovered", Settings::EditorStyle::s_HeaderHovered);
					JsonExtended::AssignIfNotNull(subJ, "HeaderActive", Settings::EditorStyle::s_HeaderActive);
					JsonExtended::AssignIfNotNull(subJ, "Font", Settings::EditorStyle::s_Font);
					JsonExtended::AssignIfNotNull(subJ, "FontDisabled", Settings::EditorStyle::s_FontDisabled);
					JsonExtended::AssignIfNotNull(subJ, "HighlightColor", Settings::EditorStyle::s_HighlightColor);
				}

				if (j.contains("Sizing"))
				{
					json& subJ = j["Sizing"];
					JsonExtended::AssignIfNotNull(subJ, "WindowPadding", Settings::EditorStyle::s_WindowPadding);
					JsonExtended::AssignIfNotNull(subJ, "FramePadding", Settings::EditorStyle::s_FramePadding);
					JsonExtended::AssignIfNotNull(subJ, "ItemSpacing", Settings::EditorStyle::s_ItemSpacing);
					JsonExtended::AssignIfNotNull(subJ, "ScrollbarSize", Settings::EditorStyle::s_ScrollbarSize);
					JsonExtended::AssignIfNotNull(subJ, "ScrollbarRounding", Settings::EditorStyle::s_ScrollbarRounding);
					JsonExtended::AssignIfNotNull(subJ, "FrameRounding", Settings::EditorStyle::s_FrameRounding);
					JsonExtended::AssignIfNotNull(subJ, "GrabRounding", Settings::EditorStyle::s_TabRounding);
					JsonExtended::AssignIfNotNull(subJ, "TabRounding", Settings::EditorStyle::s_GrabRounding);
				}

			}
			else
			{
				CPath defaultStylePath = Settings::General::s_StylesDirectory;
				defaultStylePath.Join(CPath::Create("Default.json"));
				ExportCurrentStyle(defaultStylePath);
			}
			File::CloseFile(styleData);

			ApplyStyle();
			ExportCurrentStyle(Settings::General::s_EditorStyleData);
		}

		void ApplyStyle()
		{
			ImGuiStyle& style = ImGui::GetStyle();

			// Apply Sizing
			style.WindowPadding = CImGui::From(Settings::EditorStyle::s_WindowPadding);
			style.WindowBorderSize = 1;
			style.ChildBorderSize = 1;
			style.FramePadding = CImGui::From(Settings::EditorStyle::s_FramePadding);
			style.ItemSpacing = CImGui::From(Settings::EditorStyle::s_ItemSpacing);
			style.ScrollbarSize = Settings::EditorStyle::s_ScrollbarSize;
			style.ScrollbarRounding = Settings::EditorStyle::s_ScrollbarRounding;
			style.FrameRounding = Settings::EditorStyle::s_FrameRounding;
			style.GrabRounding = Settings::EditorStyle::s_GrabRounding;
			style.TabRounding = Settings::EditorStyle::s_TabRounding;
			style.FrameBorderSize = 1.0f;

			// Apply Colors
			style.Colors[ImGuiCol_WindowBg] = CImGui::From(Settings::EditorStyle::s_MainBg);
			style.Colors[ImGuiCol_ChildBg] = CImGui::From(Settings::EditorStyle::s_MainBg);

			style.Colors[ImGuiCol_Text] = CImGui::From(Settings::EditorStyle::s_Font);
			style.Colors[ImGuiCol_TextDisabled] = CImGui::From(Settings::EditorStyle::s_FontDisabled);
			style.Colors[ImGuiCol_TextInverted] = CImGui::From(Settings::EditorStyle::s_MainBgDark1);

			style.Colors[ImGuiCol_FrameBg] = CImGui::From(Settings::EditorStyle::s_MainBgDark1);
			style.Colors[ImGuiCol_FrameBgHovered] = CImGui::From(Settings::EditorStyle::s_MainBgDark0);
			style.Colors[ImGuiCol_FrameBgActive] = CImGui::From(Settings::EditorStyle::s_MainBgDark2);

			style.Colors[ImGuiCol_TitleBg] = CImGui::From(Settings::EditorStyle::s_MainBgDark0);
			style.Colors[ImGuiCol_TitleBgCollapsed] = CImGui::From(Settings::EditorStyle::s_MainBgDark0);
			style.Colors[ImGuiCol_TitleBgActive] = CImGui::From(Settings::EditorStyle::s_MainBgDark0);
			style.Colors[ImGuiCol_MenuBarBg] = CImGui::From(Settings::EditorStyle::s_MainBgDark0);
			style.Colors[ImGuiCol_MenuBarButtonBg] = CImGui::From(Settings::EditorStyle::s_AccentDark0);
			style.Colors[ImGuiCol_MenuBarButtonBgHover] = CImGui::From(Settings::EditorStyle::s_AccentDark1);
			style.Colors[ImGuiCol_MenuBarButtonBgActive] = CImGui::From(Settings::EditorStyle::s_AccentDark1);

			style.Colors[ImGuiCol_Tab] = CImGui::From(Settings::EditorStyle::s_MainBgDark0);
			style.Colors[ImGuiCol_TabUnfocused] = CImGui::From(Settings::EditorStyle::s_MainBgDark0);
			style.Colors[ImGuiCol_TabHovered] = CImGui::From(Settings::EditorStyle::s_MainBgDark1);
			style.Colors[ImGuiCol_TabActive] = CImGui::From(Settings::EditorStyle::s_MainBgDark1);
			style.Colors[ImGuiCol_TabUnfocusedActive] = CImGui::From(Settings::EditorStyle::s_MainBgDark1);

			style.Colors[ImGuiCol_ScrollbarBg] = CImGui::From(Settings::EditorStyle::s_MainBgDark1);
			style.Colors[ImGuiCol_ScrollbarGrab] = CImGui::From(Settings::EditorStyle::s_Font);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = CImGui::From(Settings::EditorStyle::s_FontDisabled);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = CImGui::From(Settings::EditorStyle::s_FontDisabled);
			style.Colors[ImGuiCol_CheckMark] = CImGui::From(Settings::EditorStyle::s_Font);
			style.Colors[ImGuiCol_SliderGrab] = CImGui::From(Settings::EditorStyle::s_Font);
			style.Colors[ImGuiCol_SliderGrabActive] = CImGui::From(Settings::EditorStyle::s_FontDisabled);

			style.Colors[ImGuiCol_Button] = CImGui::From(Settings::EditorStyle::s_Button);
			style.Colors[ImGuiCol_ButtonHovered] = CImGui::From(Settings::EditorStyle::s_ButtonHovered);
			style.Colors[ImGuiCol_ButtonActive] = CImGui::From(Settings::EditorStyle::s_ButtonHovered);

			style.Colors[ImGuiCol_Header] = CImGui::From(Settings::EditorStyle::s_Header);
			style.Colors[ImGuiCol_HeaderHovered] = CImGui::From(Settings::EditorStyle::s_HeaderHovered);
			style.Colors[ImGuiCol_HeaderActive] = CImGui::From(Settings::EditorStyle::s_HeaderActive);

			style.Colors[ImGuiCol_Separator] = CImGui::From(Settings::EditorStyle::s_MainBgLight0);
			style.Colors[ImGuiCol_SeparatorHovered] = CImGui::From(Settings::EditorStyle::s_MainBgLight0);
			style.Colors[ImGuiCol_SeparatorActive] = CImGui::From(Settings::EditorStyle::s_MainBgLight0);
			style.Colors[ImGuiCol_Border] = CImGui::From(Settings::EditorStyle::s_MainBgLight0);

			style.Colors[ImGuiCol_ResizeGrip] = CImGui::From(Settings::EditorStyle::s_MainBg);
			style.Colors[ImGuiCol_ResizeGripHovered] = CImGui::From(Settings::EditorStyle::s_MainBg);
			style.Colors[ImGuiCol_ResizeGripActive] = CImGui::From(Settings::EditorStyle::s_MainBg);

			style.Colors[ImGuiCol_DockingPreview] = CImGui::From(Settings::EditorStyle::s_AccentDark0);
			style.Colors[ImGuiCol_TextSelectedBg] = CImGui::From(Settings::EditorStyle::s_HighlightColor);
			style.Colors[ImGuiCol_NavHighlight] = CImGui::From(Settings::EditorStyle::s_AccentDark0);
		}

		void ExportCurrentStyle(const CPath& outputPath)
		{
			json styles = {
				{"Colors",
					{
						CMath::Serialize("MainBgLight0",    Settings::EditorStyle::s_MainBgLight0),
						CMath::Serialize("MainBg",          Settings::EditorStyle::s_MainBg),
						CMath::Serialize("MainBgDark0",     Settings::EditorStyle::s_MainBgDark0),
						CMath::Serialize("MainBgDark1",     Settings::EditorStyle::s_MainBgDark1),
						CMath::Serialize("MainBgDark2",     Settings::EditorStyle::s_MainBgDark2),
						CMath::Serialize("Accent",          Settings::EditorStyle::s_Accent),
						CMath::Serialize("AccentDark0",     Settings::EditorStyle::s_AccentDark0),
						CMath::Serialize("AccentDark1",     Settings::EditorStyle::s_AccentDark1),
						CMath::Serialize("Button",          Settings::EditorStyle::s_Button),
						CMath::Serialize("ButtonHovered",   Settings::EditorStyle::s_ButtonHovered),
						CMath::Serialize("Header",          Settings::EditorStyle::s_Header),
						CMath::Serialize("HeaderHovered",   Settings::EditorStyle::s_HeaderHovered),
						CMath::Serialize("HeaderActive",    Settings::EditorStyle::s_HeaderActive),
						CMath::Serialize("Font",            Settings::EditorStyle::s_Font),
						CMath::Serialize("FontDisabled",    Settings::EditorStyle::s_FontDisabled),
						CMath::Serialize("Highlight",       Settings::EditorStyle::s_HighlightColor)
					}
				},
				{"Sizing",
					{
						CMath::Serialize("WindowPadding",  Settings::EditorStyle::s_WindowPadding),
						CMath::Serialize("FramePadding",   Settings::EditorStyle::s_FramePadding),
						CMath::Serialize("ItemSpacing",    Settings::EditorStyle::s_ItemSpacing),
						{"ScrollbarSize",                  Settings::EditorStyle::s_ScrollbarSize},
						{"ScrollbarRounding",              Settings::EditorStyle::s_ScrollbarRounding},
						{"FrameRounding",                  Settings::EditorStyle::s_FrameRounding},
						{"GrabRounding",                   Settings::EditorStyle::s_GrabRounding},
						{"TabRounding",                    Settings::EditorStyle::s_TabRounding}
					}
				}
			};
			File::WriteFile(styles.dump(4).c_str(), outputPath);
		}
	}
}