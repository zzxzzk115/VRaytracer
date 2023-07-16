#pragma once

struct GLFWwindow;

namespace VRaytracer
{
	struct WindowConfiguration
	{
        int  Height;
        int  Width;
        bool EnableVSync;
	};

	class Window
	{
    public:
        Window(WindowConfiguration config) : m_Config(config) {}
        ~Window() { Release(); }

		bool        Create();
        void        Update();
        bool        ShouldClose();
        int         GetWidth() const { return m_Config.Width; }
        int         GetHeight() const { return m_Config.Height; }
        bool        IsEnableVSync() const { return m_Config.EnableVSync; }
        GLFWwindow* GetNativeWindow() const { return m_NativeWindow; }

	private:
        void Release();

	private:
        WindowConfiguration m_Config;
        GLFWwindow*         m_NativeWindow = nullptr;
	};
}