// ImGui SDL2 binding with OpenGL3
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui
#pragma once
#include <imgui.h>

struct SDL_Window;
typedef union SDL_Event SDL_Event;

// ImGui_RE_Init must be called before usage (usually in a setup step)
IMGUI_API bool        ImGui_Renderer_Init(SDL_Window *window);
// ImGui_RE_NewFrame must be invoked in the beginning of each frame before any other ImGui calls
IMGUI_API void        ImGui_Renderer_NewFrame(SDL_Window *window);
// ImGui_RE_ProcessEvent must be invoked in the beginning of each frame before any other ImGui calls
IMGUI_API bool        ImGui_Renderer_ProcessEvent(SDL_Event *event);
// ImGui_RE_Shutdown destroys and releases resources owned by ImGui
IMGUI_API void        ImGui_Renderer_Shutdown();


// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void        ImGui_Renderer_InvalidateDeviceObjects();
IMGUI_API bool        ImGui_Renderer_CreateDeviceObjects();
