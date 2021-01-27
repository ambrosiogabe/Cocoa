#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/core/CWindow.h"

namespace Cocoa
{
	// OpenGL message callback
	static void GLAPIENTRY MessageCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		//if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		//{
		//	return;
		//}

		//Log::Warning("---------------------opengl-callback-start------------\n");
		//Log::Warning("message: %s\n", message);
		//Log::Warning("type: ");
		//switch (type)
		//{
		//case GL_DEBUG_TYPE_ERROR:
		//	Log::Warning("ERROR\n");
		//	break;
		//case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		//	Log::Warning("DEPRECATED_BEHAVIOR\n");
		//	break;
		//case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		//	Log::Warning("UNDEFINED_BEHAVIOR\n");
		//	break;
		//case GL_DEBUG_TYPE_PORTABILITY:
		//	Log::Warning("PORTABILITY\n");
		//	break;
		//case GL_DEBUG_TYPE_PERFORMANCE:
		//	Log::Warning("PERFORMANCE\n");
		//	break;
		//case GL_DEBUG_TYPE_OTHER:
		//	Log::Warning("OTHER\n");
		//	break;
		//}

		//Log::Warning("id: %u\n", id);
		//Log::Warning("severity: ");
		//switch (severity)
		//{
		//case GL_DEBUG_SEVERITY_LOW:
		//	Log::Warning("LOW\n");
		//	break;
		//case GL_DEBUG_SEVERITY_MEDIUM:
		//	Log::Warning("MEDIUM\n");
		//	break;
		//case GL_DEBUG_SEVERITY_HIGH:
		//	Log::Warning("HIGH\n");
		//	break;
		//case GL_DEBUG_SEVERITY_NOTIFICATION:
		//	Log::Warning("NOTIFICATION\n");
		//	break;
		//default:
		//	Log::Warning("Unkown\n");
		//	break;
		//}
		//Log::Warning("---------------------opengl-callback-end--------------\n");
	}

	CWindow* CWindow::Create(uint32 width, uint32 height, const std::string& name)
	{
		return new CWindow(width, height, name);
	}

	CWindow::CWindow(uint32 width, uint32 height, const std::string& name)
	{
		Init(width, height, name);
	}

	void CWindow::Init(uint32 width, uint32 height, const std::string& name)
	{
		Log::Assert(glfwInit(), "Unable to initialize GLFW");

		GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
		m_WindowHandle = window;
		Log::Assert(m_WindowHandle != nullptr, "GLFW unable to create window.");

		glfwMakeContextCurrent(window);

		Log::Assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Unable to initialize GLAD.");
		glfwSetWindowUserPointer(window, this);

		// Set up event callbacks
		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
			{
				CWindow* userWin = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
				Log::Assert(userWin != nullptr, "CWindow is nullpointer in callback.");

				userWin->SetWidth(width);
				userWin->SetHeight(height);

				WindowResizeEvent e(width, height);
				userWin->m_EventCallback(e);
			});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
			{
				CWindow* userWin = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
				Log::Assert(userWin != nullptr, "CWindow is nullpointer in callback.");

				WindowCloseEvent e;
				userWin->m_EventCallback(e);
			});

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				CWindow* userWin = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
				Log::Assert(userWin != nullptr, "CWindow is nullpointer in callback.");

				Input::KeyCallback(key, scancode, action, mods);
				switch (action)
				{
				case COCOA_PRESS: {
					KeyPressedEvent e(key, 0);
					userWin->m_EventCallback(e);
					break;
				}
				case COCOA_RELEASE: {
					KeyReleasedEvent e(key);
					userWin->m_EventCallback(e);
					break;
				}
				case COCOA_REPEAT: {
					KeyPressedEvent e(key, 1);
					userWin->m_EventCallback(e);
					break;
				}
				}
			});

		glfwSetCharCallback(window, [](GLFWwindow* window, uint32 keycode)
			{
				CWindow* userWin = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
				Log::Assert(userWin != nullptr, "CWindow is nullpointer in callback.");

				// Event& e = KeyTypedEvent(keycode);
				// userWin->EventCallback(e);
			});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
			{
				CWindow* userWin = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
				Log::Assert(userWin != nullptr, "CWindow is nullpointer in callback.");

				Input::MouseButtonCallback(button, action, mods);
				switch (action)
				{
				case COCOA_PRESS: {
					MouseButtonPressedEvent e(button);
					userWin->m_EventCallback(e);
					break;
				}
				case COCOA_RELEASE: {
					MouseButtonReleasedEvent e(button);
					userWin->m_EventCallback(e);
					break;
				}
				}
			});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				CWindow* userWin = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
				Log::Assert(userWin != nullptr, "CWindow is nullpointer in callback.");

				Input::ScrollCallback(xoffset, yoffset);
				MouseScrolledEvent e((float)xoffset, (float)yoffset);
				userWin->m_EventCallback(e);
			});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
			{
				CWindow* userWin = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
				Log::Assert(userWin != nullptr, "CWindow is nullpointer in callback.");

				Input::CursorCallback(xpos, ypos);
				MouseMovedEvent e((float)xpos, (float)ypos);
				userWin->m_EventCallback(e);
			});

		// During init, enable debug output
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);

		SetVSync(true);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void CWindow::SetEventCallback(const EventCallbackFn& e)
	{
		m_EventCallback = e;
	}

	void CWindow::OnUpdate()
	{
		glfwPollEvents();
	}

	void CWindow::Render()
	{
		glfwSwapBuffers((GLFWwindow*)m_WindowHandle);
	}

	// Window Attributes
	void CWindow::SetVSync(bool enabled)
	{
		if (enabled)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}

		m_VSync = enabled;
	}

	bool CWindow::IsVSync() const
	{
		return m_VSync;
	}

	void* CWindow::GetNativeWindow() const
	{
		return m_WindowHandle;
	}

	glm::vec2 CWindow::GetWindowPos()
	{
		GLFWwindow* win = static_cast<GLFWwindow*>(m_WindowHandle);
		int x, y;
		glfwGetWindowPos(win, &x, &y);
		return {x, y};
	}

	void CWindow::SetSize(const glm::vec2& size)
	{
		GLFWwindow* win = static_cast<GLFWwindow*>(m_WindowHandle);
		Log::Assert(size.x >= 0 && size.y >= 0, "Window width or height cannot be 0.");
		glfwSetWindowSize(win, (int)size.x, (int)size.y);
	}

	void CWindow::SetTitle(const char* newTitle)
	{
		GLFWwindow* win = static_cast<GLFWwindow*>(m_WindowHandle);
		glfwSetWindowTitle(win, newTitle);
	}

	glm::vec2 CWindow::GetMonitorSize()
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		return { mode->width, mode->height };
	}

	void CWindow::Destroy()
	{
		glfwDestroyWindow((GLFWwindow*)m_WindowHandle);
	}

	bool CWindow::IsRunning()
	{
		return m_Running;
	}
}