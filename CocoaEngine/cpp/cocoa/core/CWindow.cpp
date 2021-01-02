#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/core/CWindow.h"

namespace Cocoa
{
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