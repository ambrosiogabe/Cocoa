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
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		{
			return;
		}

		// Suppress some useless warnings
		switch (id)
		{
		case 131218: // NVIDIA: "shader will be recompiled due to GL state mismatches"
			return;
		default:
			break;
		}

		Logger::Warning("---------------------opengl-callback-start------------\n");
		Logger::Warning("message: %s\n", message);
		Logger::Warning("type: ");
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			Logger::Warning("ERROR\n");
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			Logger::Warning("DEPRECATED_BEHAVIOR\n");
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			Logger::Warning("UNDEFINED_BEHAVIOR\n");
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			Logger::Warning("PORTABILITY\n");
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			Logger::Warning("PERFORMANCE\n");
			break;
		case GL_DEBUG_TYPE_OTHER:
			Logger::Warning("OTHER\n");
			break;
		}

		Logger::Warning("id: %u\n", id);
		Logger::Warning("severity: ");
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_LOW:
			Logger::Warning("LOW\n");
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			Logger::Warning("MEDIUM\n");
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			Logger::Warning("HIGH\n");
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			Logger::Warning("NOTIFICATION\n");
			break;
		default:
			Logger::Warning("Unkown\n");
			break;
		}
		Logger::Warning("---------------------opengl-callback-end--------------\n");
	}

	CWindow* CWindow::create(uint32 width, uint32 height, const std::string& name)
	{
		return new CWindow(width, height, name);
	}

	CWindow::CWindow(uint32 width, uint32 height, const std::string& name)
	{
		init(width, height, name);
	}

	void CWindow::init(uint32 width, uint32 height, const std::string& name)
	{
		Logger::Assert(glfwInit(), "Unable to initialize GLFW");

		GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
		mWindowHandle = window;
		Logger::Assert(mWindowHandle != nullptr, "GLFW unable to create window.");

		glfwMakeContextCurrent(window);

		Logger::Assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Unable to initialize GLAD.");
		glfwSetWindowUserPointer(window, this);

		// Set up event callbacks
		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
			{
				CWindow* userWin = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
				Logger::Assert(userWin != nullptr, "CWindow is nullpointer in callback.");

				userWin->setWidth(width);
				userWin->setHeight(height);

				WindowResizeEvent e(width, height);
				userWin->mEventCallback(e);
			});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
			{
				CWindow* userWin = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
				Logger::Assert(userWin != nullptr, "CWindow is nullpointer in callback.");

				WindowCloseEvent e;
				userWin->mEventCallback(e);
			});

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				CWindow* userWin = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
				Logger::Assert(userWin != nullptr, "CWindow is nullpointer in callback.");

				Input::keyCallback(key, scancode, action, mods);
				switch (action)
				{
				case COCOA_PRESS: {
					KeyPressedEvent e(key, 0);
					userWin->mEventCallback(e);
					break;
				}
				case COCOA_RELEASE: {
					KeyReleasedEvent e(key);
					userWin->mEventCallback(e);
					break;
				}
				case COCOA_REPEAT: {
					KeyPressedEvent e(key, 1);
					userWin->mEventCallback(e);
					break;
				}
				}
			});

		glfwSetCharCallback(window, [](GLFWwindow* window, uint32 keycode)
			{
				CWindow* userWin = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
				Logger::Assert(userWin != nullptr, "CWindow is nullpointer in callback.");

				// Event& e = KeyTypedEvent(keycode);
				// userWin->EventCallback(e);
			});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
			{
				CWindow* userWin = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
				Logger::Assert(userWin != nullptr, "CWindow is nullpointer in callback.");

				Input::mouseButtonCallback(button, action, mods);
				switch (action)
				{
				case COCOA_PRESS: {
					MouseButtonPressedEvent e(button);
					userWin->mEventCallback(e);
					break;
				}
				case COCOA_RELEASE: {
					MouseButtonReleasedEvent e(button);
					userWin->mEventCallback(e);
					break;
				}
				}
			});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				CWindow* userWin = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
				Logger::Assert(userWin != nullptr, "CWindow is nullpointer in callback.");

				Input::scrollCallback(xoffset, yoffset);
				MouseScrolledEvent e((float)xoffset, (float)yoffset);
				userWin->mEventCallback(e);
			});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
			{
				CWindow* userWin = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
				Logger::Assert(userWin != nullptr, "CWindow is nullpointer in callback.");

				Input::cursorCallback(xpos, ypos);
				MouseMovedEvent e((float)xpos, (float)ypos);
				userWin->mEventCallback(e);
			});

		// During init, enable debug output
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);

		setVSync(true);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void CWindow::setEventCallback(const EventCallbackFn& callback)
	{
		mEventCallback = callback;
	}

	void CWindow::onUpdate()
	{
		glfwPollEvents();
	}

	void CWindow::render()
	{
		glfwSwapBuffers((GLFWwindow*)mWindowHandle);
	}

	// Window Attributes
	void CWindow::setVSync(bool enabled)
	{
		if (enabled)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}

		mVSync = enabled;
	}

	bool CWindow::isVSync() const
	{
		return mVSync;
	}

	void* CWindow::getNativeWindow() const
	{
		return mWindowHandle;
	}

	glm::vec2 CWindow::getWindowPos()
	{
		GLFWwindow* win = static_cast<GLFWwindow*>(mWindowHandle);
		int x, y;
		glfwGetWindowPos(win, &x, &y);
		return {x, y};
	}

	void CWindow::setSize(const glm::vec2& size)
	{
		GLFWwindow* win = static_cast<GLFWwindow*>(mWindowHandle);
		Logger::Assert(size.x >= 0 && size.y >= 0, "Window width or height cannot be 0.");
		glfwSetWindowSize(win, (int)size.x, (int)size.y);
	}

	void CWindow::setTitle(const char* newTitle)
	{
		GLFWwindow* win = static_cast<GLFWwindow*>(mWindowHandle);
		glfwSetWindowTitle(win, newTitle);
	}

	glm::vec2 CWindow::getMonitorSize()
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		return { mode->width, mode->height };
	}

	void CWindow::destroy()
	{
		glfwDestroyWindow((GLFWwindow*)mWindowHandle);
	}

	bool CWindow::isRunning()
	{
		return mRunning;
	}
}