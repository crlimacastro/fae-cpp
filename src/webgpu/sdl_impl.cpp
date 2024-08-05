#include "fae/webgpu/sdl_impl.hpp"

#include <SDL3/SDL.h>
#include <webgpu/webgpu_cpp.h>

#ifdef FAE_PLATFORM_WINDOWS
#include <Windows.h>
#endif


namespace fae
{
    [[nodiscard]] auto get_sdl_webgpu_surface(wgpu::Instance instance, SDL_Window* window) noexcept -> wgpu::Surface
    {
        auto surface_descriptor = wgpu::SurfaceDescriptor{};
#if defined(FAE_PLATFORM_WEB)
        wgpu::SurfaceDescriptorFromCanvasHTMLSelector canvas_desc{};
        canvas_desc.selector = "#canvas";
        surface_descriptor.nextInChain = (const wgpu::ChainedStruct*)&canvas_desc;
#elif defined(FAE_PLATFORM_WINDOWS)

        auto hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        auto hinstance = (HINSTANCE)GetModuleHandle(nullptr);

        auto windows_surface_descriptor = wgpu::SurfaceDescriptorFromWindowsHWND{};
        windows_surface_descriptor.hinstance = hinstance;
        windows_surface_descriptor.hwnd = hwnd;
        surface_descriptor.nextInChain = (const wgpu::ChainedStruct*)&windows_surface_descriptor;
        surface_descriptor.label = "windows_sdl_webgpu_surface";
#elif defined(FAE_PLATFORM_MACOS)
        NSWindow* nswindow = (__bridge NSWindow*)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
        auto macos_surface_descriptor = wgpu::SurfaceDescriptorFromMetalLayer{};
        if (nswindow)
        {
            macos_surface_descriptor.layer = CAMetalLayer * nswindow.contentView.layer;
        }
        surface_descriptor.nextInChain = (const wgpu::ChainedStruct*)&macos_surface_descriptor;
        surface_descriptor.label = "macos_sdl_webgpu_surface";
#elif defined(FAE_PLATFORM_LINUX)
        if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0)
        {
            Display* xdisplay = (Display*)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
            Window xwindow = (Window)SDL_GetNumberProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
            auto linux_x11_surface_descriptor = wgpu::SurfaceDescriptorFromXlib{};
            if (xdisplay && xwindow)
            {
                linux_x11_surface_descriptor.display = xdisplay;
                linux_x11_surface_descriptor.window = xwindow;
            }
            surface_descriptor.nextInChain = (const wgpu::ChainedStruct*)&linux_x11_surface_descriptor;
            surface_descriptor.label = "linux_x11_sdl_webgpu_surface";
        }
        else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0)
        {
            struct wl_display* display = (struct wl_display*)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
            struct wl_surface* surface = (struct wl_surface*)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);
            auto linux_wayland_surface_descriptor = wgpu::SurfaceDescriptorFromWayland {}
            if (display && surface)
            {
                linux_wayland_surface_descriptor.display = display;
                linux_wayland_surface_descriptor.surface = surface;
            }
            surface_descriptor.nextInChain = (const wgpu::ChainedStruct*)&linux_wayland_surface_descriptor;
            surface_descriptor.label = "linux_wayland_sdl_webgpu_surface";
        }
#else
#error "unsupported platform"
#endif
        return instance.CreateSurface(&surface_descriptor);
    }
} // namespace fae
