#include "externalLibs.h"
#include "jade/core/Core.h"

#include "jade/core/JWindow.h"

namespace Jade
{
	JWindow* JWindow::Create(uint32 width, uint32 height, const std::string& name)
	{
		return new JWindow(width, height, name);
	}

	JWindow::JWindow(uint32 width, uint32 height, const std::string& name)
	{
		Init(width, height, name);
	}

	void JWindow::Init(uint32 width, uint32 height, const std::string& name)
	{
		Log::Assert(glfwInit(), "Unable to initialize GLFW");

		GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
		m_WindowHandle = window;
		Log::Assert(m_WindowHandle != nullptr, "GLFW unable to create window.");

		glfwMakeContextCurrent(window);

		Log::Assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Unable to initialize GLEW.");
		glfwSetWindowUserPointer(window, this);

		// Set up event callbacks
		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
			{
				JWindow* userWin = static_cast<JWindow*>(glfwGetWindowUserPointer(window));
				Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

				userWin->SetWidth(width);
				userWin->SetHeight(height);

				WindowResizeEvent e(width, height);
				userWin->m_EventCallback(e);
			});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
			{
				JWindow* userWin = static_cast<JWindow*>(glfwGetWindowUserPointer(window));
				Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

				WindowCloseEvent e;
				userWin->m_EventCallback(e);
			});

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				JWindow* userWin = static_cast<JWindow*>(glfwGetWindowUserPointer(window));
				Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

				Input::KeyCallback(key, scancode, action, mods);
				switch (action)
				{
				case JADE_PRESS: {
					KeyPressedEvent e(key, 0);
					userWin->m_EventCallback(e);
					break;
				}
				case JADE_RELEASE: {
					KeyReleasedEvent e(key);
					userWin->m_EventCallback(e);
					break;
				}
				case JADE_REPEAT: {
					KeyPressedEvent e(key, 1);
					userWin->m_EventCallback(e);
					break;
				}
				}
			});

		glfwSetCharCallback(window, [](GLFWwindow* window, uint32 keycode)
			{
				JWindow* userWin = static_cast<JWindow*>(glfwGetWindowUserPointer(window));
				Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

				// Event& e = KeyTypedEvent(keycode);
				// userWin->EventCallback(e);
			});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
			{
				JWindow* userWin = static_cast<JWindow*>(glfwGetWindowUserPointer(window));
				Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

				Input::MouseButtonCallback(button, action, mods);
				switch (action)
				{
				case JADE_PRESS: {
					MouseButtonPressedEvent e(button);
					userWin->m_EventCallback(e);
					break;
				}
				case JADE_RELEASE: {
					MouseButtonReleasedEvent e(button);
					userWin->m_EventCallback(e);
					break;
				}
				}
			});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				JWindow* userWin = static_cast<JWindow*>(glfwGetWindowUserPointer(window));
				Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

				Input::ScrollCallback(xoffset, yoffset);
				MouseScrolledEvent e((float)xoffset, (float)yoffset);
				userWin->m_EventCallback(e);
			});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
			{
				JWindow* userWin = static_cast<JWindow*>(glfwGetWindowUserPointer(window));
				Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

				Input::CursorCallback(xpos, ypos);
				MouseMovedEvent e((float)xpos, (float)ypos);
				userWin->m_EventCallback(e);
			});

		SetVSync(true);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void JWindow::SetEventCallback(const EventCallbackFn& e)
	{
		m_EventCallback = e;
	}

	void JWindow::OnUpdate()
	{
		glfwPollEvents();
	}

	void JWindow::Render()
	{
		glfwSwapBuffers((GLFWwindow*)m_WindowHandle);
	}

	// Window Attributes
	void JWindow::SetVSync(bool enabled)
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

	bool JWindow::IsVSync() const
	{
		return m_VSync;
	}

	void* JWindow::GetNativeWindow() const
	{
		return m_WindowHandle;
	}

	glm::vec2 JWindow::GetWindowPos()
	{
		GLFWwindow* win = static_cast<GLFWwindow*>(m_WindowHandle);
		int x, y;
		glfwGetWindowPos(win, &x, &y);
		return {x, y};
	}

	void JWindow::SetSize(const glm::vec2& size)
	{
		GLFWwindow* win = static_cast<GLFWwindow*>(m_WindowHandle);
		Log::Assert(size.x >= 0 && size.y >= 0, "Window width or height cannot be 0.");
		glfwSetWindowSize(win, (int)size.x, (int)size.y);
	}

	glm::vec2 JWindow::GetMonitorSize()
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		return { mode->width, mode->height };
	}

	void JWindow::Destroy()
	{
		glfwDestroyWindow((GLFWwindow*)m_WindowHandle);
	}

	bool JWindow::IsRunning()
	{
		return m_Running;
	}
}