#include "core/ImGuiLayer.h"
#include "core/CocoaEditorApplication.h"
#include "editorWindows/InspectorWindow.h"
#include "editorWindows/SceneHierarchyWindow.h"
#include "editorWindows/ProjectWizard.h"
#include "editorWindows/GameViewport.h"
#include "editorWindows/GameEditorViewport.h"
#include "editorWindows/AssetWindow.h"
#include "gui/ImGuiExtended.h"
#include "gui/FontAwesome.h"
#include "util/Settings.h"

#include "cocoa/file/File.h"
#include "cocoa/util/CMath.h"
#include "cocoa/util/JsonExtended.h"

#ifndef _COCOA_IMPL
#define _COCOA_IMPL
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

#include "cocoa/util/Settings.h"
#include "editorWindows/MenuBar.h"

namespace Cocoa
{
	namespace ImGuiLayer
	{
		// Internal Declarations
		static bool m_HoveringGameEditorWindow;
		static void* m_Window;

		// Forward Declarations
		void SetupDockspace(SceneData& scene);

		void init(void* window)
		{
			m_Window = window;

			// Setup dear imGui binding
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGui::LoadIniSettingsFromDisk(Settings::General::imGuiConfigPath.c_str());

			ImGuiIO& io = ImGui::GetIO(); (void)io;

			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

			// Init GLFW
			ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_Window, true);

			// Init OpenGL imgui implementation
			// GL 3.0 + GLSL 130
			const char* glsl_version = "#version 130";
			ImGui_ImplOpenGL3_Init(glsl_version);

			// imgui style 
			ImFontConfig config;
			Settings::EditorStyle::defaultFont = io.Fonts->AddFontFromFileTTF("assets/fonts/UbuntuMono-Regular.ttf", 24.0f, &config);
			config.MergeMode = true;
			const ImWchar iconRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
			io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome-webfont.ttf", 24.0f, &config, iconRanges);
			io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome-webfont2.ttf", 24.0f, &config, iconRanges);
			io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome-webfont3.ttf", 24.0f, &config, iconRanges);
			io.Fonts->Build();

			config.MergeMode = false;
			const ImWchar customIconRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
			Settings::EditorStyle::largeIconFont = io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome-webfont.ttf", 64.0f, &config, customIconRanges);
			config.MergeMode = true;
			io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome-webfont2.ttf", 64.0f, &config, customIconRanges);
			io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome-webfont3.ttf", 64.0f, &config, customIconRanges);
			io.Fonts->Build();

			loadStyle(Settings::General::editorStyleData.createTmpPath());

			SceneHierarchyWindow::init();
		}

		void destroy()
		{
			SceneHierarchyWindow::destroy();
		}

		void onEvent(SceneData& scene, Event& e)
		{
			// Find way to block events when necessary
			//if (m_BlockEvents)
			//{
			//	ImGuiIO& io = imgui::GetIO();
			//	//e.m_Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			//	//e.m_Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
			//	e.m_Handled = true;
			//}

			if (!m_HoveringGameEditorWindow && ImGui::GetIO().WantCaptureMouse)
			{
				e.handled = e.isInCategory(EventCategoryMouse) ? true : false;
			}
		}

		void beginFrame(SceneData& scene)
		{
			// Start imgui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			if (CocoaEditor::isProjectLoaded())
			{
				SetupDockspace(scene);
				GameEditorViewport::imgui(scene, &m_HoveringGameEditorWindow);
				GameViewport::imgui(scene);
				AssetWindow::imgui(scene);
				InspectorWindow::imgui(scene);
				SceneHierarchyWindow::imgui(scene);
				if (Settings::Editor::showDemoWindow)
				{
					ImGui::ShowDemoWindow(&Settings::Editor::showDemoWindow);
				}
			}
			else
			{
				ProjectWizard::imgui(scene);
			}
		}

		void endFrame()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glViewport(0, 0, Application::get()->getWindow()->getWidth(), Application::get()->getWindow()->getHeight());
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			// Render imgui frame
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
			ImGui::PushStyleColor(ImGuiCol_TitleBg, Settings::EditorStyle::accent);
			ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, Settings::EditorStyle::accent);
			ImGui::PushStyleColor(ImGuiCol_TitleBgActive, Settings::EditorStyle::accent);
			ImGui::PushStyleColor(ImGuiCol_MenuBarBg, Settings::EditorStyle::accent);
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

			ImGui::PushStyleColor(ImGuiCol_Border, Settings::EditorStyle::mainBgDark2);
			ImGui::PushStyleColor(ImGuiCol_PopupBg, Settings::EditorStyle::accentDark0);
			//imgui::PushStyleColor(ImGuiCol_Header, Settings::EditorStyle::s_AccentDark0);
			//imgui::PushStyleColor(ImGuiCol_HeaderHovered, Settings::EditorStyle::s_AccentDark0);
			//imgui::PushStyleColor(ImGuiCol_HeaderActive, Settings::EditorStyle::s_AccentDark0);
			MenuBar::imgui(scene);
			ImGui::PopStyleVar(4);
			ImGui::PopStyleColor(2);
			ImGui::End();
		}

		void loadStyle(const Path& filepath)
		{
			FileHandle* styleData = File::openFile(filepath);
			if (styleData->size > 0)
			{
				json j = json::parse(styleData->data);
				if (j.contains("Colors"))
				{
					json& subJ = j["Colors"];
					JsonExtended::assignIfNotNull(subJ, "MainBgLight0", Settings::EditorStyle::mainBgLight0);
					JsonExtended::assignIfNotNull(subJ, "MainBg", Settings::EditorStyle::mainBg);
					JsonExtended::assignIfNotNull(subJ, "MainBgDark0", Settings::EditorStyle::mainBgDark0);
					JsonExtended::assignIfNotNull(subJ, "MainBgDark1", Settings::EditorStyle::mainBgDark1);
					JsonExtended::assignIfNotNull(subJ, "MainBgDark2", Settings::EditorStyle::mainBgDark2);
					JsonExtended::assignIfNotNull(subJ, "Accent", Settings::EditorStyle::accent);
					JsonExtended::assignIfNotNull(subJ, "AccentDark0", Settings::EditorStyle::accentDark0);
					JsonExtended::assignIfNotNull(subJ, "AccentDark1", Settings::EditorStyle::accentDark1);
					JsonExtended::assignIfNotNull(subJ, "Button", Settings::EditorStyle::button);
					JsonExtended::assignIfNotNull(subJ, "ButtonHovered", Settings::EditorStyle::buttonHovered);
					JsonExtended::assignIfNotNull(subJ, "Header", Settings::EditorStyle::header);
					JsonExtended::assignIfNotNull(subJ, "HeaderHovered", Settings::EditorStyle::headerHovered);
					JsonExtended::assignIfNotNull(subJ, "HeaderActive", Settings::EditorStyle::headerActive);
					JsonExtended::assignIfNotNull(subJ, "Font", Settings::EditorStyle::font);
					JsonExtended::assignIfNotNull(subJ, "FontDisabled", Settings::EditorStyle::fontDisabled);
					JsonExtended::assignIfNotNull(subJ, "HighlightColor", Settings::EditorStyle::highlightColor);
				}

				if (j.contains("Sizing"))
				{
					json& subJ = j["Sizing"];
					JsonExtended::assignIfNotNull(subJ, "WindowPadding", Settings::EditorStyle::windowPadding);
					JsonExtended::assignIfNotNull(subJ, "FramePadding", Settings::EditorStyle::framePadding);
					JsonExtended::assignIfNotNull(subJ, "ItemSpacing", Settings::EditorStyle::itemSpacing);
					JsonExtended::assignIfNotNull(subJ, "ScrollbarSize", Settings::EditorStyle::scrollbarSize);
					JsonExtended::assignIfNotNull(subJ, "ScrollbarRounding", Settings::EditorStyle::scrollbarRounding);
					JsonExtended::assignIfNotNull(subJ, "FrameRounding", Settings::EditorStyle::frameRounding);
					JsonExtended::assignIfNotNull(subJ, "GrabRounding", Settings::EditorStyle::tabRounding);
					JsonExtended::assignIfNotNull(subJ, "TabRounding", Settings::EditorStyle::grabRounding);
				}

			}
			else
			{
				exportCurrentStyle(
					PathBuilder(Settings::General::stylesDirectory)
						.join("Default.json")
						.createTmpPath()
				);
			}
			File::closeFile(styleData);

			applyStyle();
			exportCurrentStyle(Settings::General::editorStyleData.createTmpPath());
		}

		void applyStyle()
		{
			ImGuiStyle& style = ImGui::GetStyle();

			// Apply Sizing
			style.WindowPadding = CImGui::from(Settings::EditorStyle::windowPadding);
			style.WindowBorderSize = 1;
			style.ChildBorderSize = 1;
			style.FramePadding = CImGui::from(Settings::EditorStyle::framePadding);
			style.ItemSpacing = CImGui::from(Settings::EditorStyle::itemSpacing);
			style.ScrollbarSize = Settings::EditorStyle::scrollbarSize;
			style.ScrollbarRounding = Settings::EditorStyle::scrollbarRounding;
			style.FrameRounding = Settings::EditorStyle::frameRounding;
			style.GrabRounding = Settings::EditorStyle::grabRounding;
			style.TabRounding = Settings::EditorStyle::tabRounding;
			style.FrameBorderSize = 1.0f;

			// Apply Colors
			style.Colors[ImGuiCol_WindowBg] = CImGui::from(Settings::EditorStyle::mainBg);
			style.Colors[ImGuiCol_ChildBg] = CImGui::from(Settings::EditorStyle::mainBg);

			style.Colors[ImGuiCol_Text] = CImGui::from(Settings::EditorStyle::font);
			style.Colors[ImGuiCol_TextDisabled] = CImGui::from(Settings::EditorStyle::fontDisabled);
			style.Colors[ImGuiCol_TextInverted] = CImGui::from(Settings::EditorStyle::mainBgDark1);

			style.Colors[ImGuiCol_FrameBg] = CImGui::from(Settings::EditorStyle::mainBgDark1);
			style.Colors[ImGuiCol_FrameBgHovered] = CImGui::from(Settings::EditorStyle::mainBgDark0);
			style.Colors[ImGuiCol_FrameBgActive] = CImGui::from(Settings::EditorStyle::mainBgDark2);

			style.Colors[ImGuiCol_TitleBg] = CImGui::from(Settings::EditorStyle::mainBgDark0);
			style.Colors[ImGuiCol_TitleBgCollapsed] = CImGui::from(Settings::EditorStyle::mainBgDark0);
			style.Colors[ImGuiCol_TitleBgActive] = CImGui::from(Settings::EditorStyle::mainBgDark0);
			style.Colors[ImGuiCol_MenuBarBg] = CImGui::from(Settings::EditorStyle::mainBgDark0);
			style.Colors[ImGuiCol_MenuBarButtonBg] = CImGui::from(Settings::EditorStyle::accentDark0);
			style.Colors[ImGuiCol_MenuBarButtonBgHover] = CImGui::from(Settings::EditorStyle::accentDark1);
			style.Colors[ImGuiCol_MenuBarButtonBgActive] = CImGui::from(Settings::EditorStyle::accentDark1);

			style.Colors[ImGuiCol_Tab] = CImGui::from(Settings::EditorStyle::mainBgDark0);
			style.Colors[ImGuiCol_TabUnfocused] = CImGui::from(Settings::EditorStyle::mainBgDark0);
			style.Colors[ImGuiCol_TabHovered] = CImGui::from(Settings::EditorStyle::mainBgDark1);
			style.Colors[ImGuiCol_TabActive] = CImGui::from(Settings::EditorStyle::mainBgDark1);
			style.Colors[ImGuiCol_TabUnfocusedActive] = CImGui::from(Settings::EditorStyle::mainBgDark1);

			style.Colors[ImGuiCol_ScrollbarBg] = CImGui::from(Settings::EditorStyle::mainBgDark1);
			style.Colors[ImGuiCol_ScrollbarGrab] = CImGui::from(Settings::EditorStyle::font);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = CImGui::from(Settings::EditorStyle::fontDisabled);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = CImGui::from(Settings::EditorStyle::fontDisabled);
			style.Colors[ImGuiCol_CheckMark] = CImGui::from(Settings::EditorStyle::font);
			style.Colors[ImGuiCol_SliderGrab] = CImGui::from(Settings::EditorStyle::font);
			style.Colors[ImGuiCol_SliderGrabActive] = CImGui::from(Settings::EditorStyle::fontDisabled);

			style.Colors[ImGuiCol_Button] = CImGui::from(Settings::EditorStyle::button);
			style.Colors[ImGuiCol_ButtonHovered] = CImGui::from(Settings::EditorStyle::buttonHovered);
			style.Colors[ImGuiCol_ButtonActive] = CImGui::from(Settings::EditorStyle::buttonHovered);

			style.Colors[ImGuiCol_Header] = CImGui::from(Settings::EditorStyle::header);
			style.Colors[ImGuiCol_HeaderHovered] = CImGui::from(Settings::EditorStyle::headerHovered);
			style.Colors[ImGuiCol_HeaderActive] = CImGui::from(Settings::EditorStyle::headerActive);

			style.Colors[ImGuiCol_Separator] = CImGui::from(Settings::EditorStyle::mainBgLight0);
			style.Colors[ImGuiCol_SeparatorHovered] = CImGui::from(Settings::EditorStyle::mainBgLight0);
			style.Colors[ImGuiCol_SeparatorActive] = CImGui::from(Settings::EditorStyle::mainBgLight0);
			style.Colors[ImGuiCol_Border] = CImGui::from(Settings::EditorStyle::mainBgLight0);

			style.Colors[ImGuiCol_ResizeGrip] = CImGui::from(Settings::EditorStyle::mainBg);
			style.Colors[ImGuiCol_ResizeGripHovered] = CImGui::from(Settings::EditorStyle::mainBg);
			style.Colors[ImGuiCol_ResizeGripActive] = CImGui::from(Settings::EditorStyle::mainBg);

			style.Colors[ImGuiCol_DockingPreview] = CImGui::from(Settings::EditorStyle::accentDark0);
			style.Colors[ImGuiCol_TextSelectedBg] = CImGui::from(Settings::EditorStyle::highlightColor);
			style.Colors[ImGuiCol_NavHighlight] = CImGui::from(Settings::EditorStyle::accentDark0);
		}

		void exportCurrentStyle(const Path& outputPath)
		{
			json styles = {
				{"Colors",
					{
						CMath::serialize("MainBgLight0",    Settings::EditorStyle::mainBgLight0),
						CMath::serialize("MainBg",          Settings::EditorStyle::mainBg),
						CMath::serialize("MainBgDark0",     Settings::EditorStyle::mainBgDark0),
						CMath::serialize("MainBgDark1",     Settings::EditorStyle::mainBgDark1),
						CMath::serialize("MainBgDark2",     Settings::EditorStyle::mainBgDark2),
						CMath::serialize("Accent",          Settings::EditorStyle::accent),
						CMath::serialize("AccentDark0",     Settings::EditorStyle::accentDark0),
						CMath::serialize("AccentDark1",     Settings::EditorStyle::accentDark1),
						CMath::serialize("Button",          Settings::EditorStyle::button),
						CMath::serialize("ButtonHovered",   Settings::EditorStyle::buttonHovered),
						CMath::serialize("Header",          Settings::EditorStyle::header),
						CMath::serialize("HeaderHovered",   Settings::EditorStyle::headerHovered),
						CMath::serialize("HeaderActive",    Settings::EditorStyle::headerActive),
						CMath::serialize("Font",            Settings::EditorStyle::font),
						CMath::serialize("FontDisabled",    Settings::EditorStyle::fontDisabled),
						CMath::serialize("Highlight",       Settings::EditorStyle::highlightColor)
					}
				},
				{"Sizing",
					{
						CMath::serialize("WindowPadding",  Settings::EditorStyle::windowPadding),
						CMath::serialize("FramePadding",   Settings::EditorStyle::framePadding),
						CMath::serialize("ItemSpacing",    Settings::EditorStyle::itemSpacing),
						{"ScrollbarSize",                  Settings::EditorStyle::scrollbarSize},
						{"ScrollbarRounding",              Settings::EditorStyle::scrollbarRounding},
						{"FrameRounding",                  Settings::EditorStyle::frameRounding},
						{"GrabRounding",                   Settings::EditorStyle::grabRounding},
						{"TabRounding",                    Settings::EditorStyle::tabRounding}
					}
				}
			};
			File::writeFile(styles.dump(4).c_str(), outputPath);
		}
	}
}
