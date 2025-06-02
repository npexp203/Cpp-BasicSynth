// src/main.cpp
#include <SDL.h>
#include <SDL_opengl.h>      // pour le contexte OpenGL
#include <imgui.h>
#include <Dear/backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>
#include <stdio.h>

static const int WINDOW_WIDTH  = 1280;
static const int WINDOW_HEIGHT = 720;

int main(int argc, char* argv[])
{
    // 1) Initialiser SDL (vidéo + audio + timer)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        printf("Erreur SDL_Init : %s\n", SDL_GetError());
        return -1;
    }

    // 2) Configurer le contexte OpenGL 3.2
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // 3) Créer la fenêtre SDL (OpenGL + ImGui)
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(
        SDL_WINDOW_OPENGL |
        SDL_WINDOW_RESIZABLE |
        SDL_WINDOW_ALLOW_HIGHDPI
    );
    SDL_Window* window = SDL_CreateWindow(
        "Mon Synthe - V1",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        window_flags
    );
    if (!window) {
        printf("Erreur SDL_CreateWindow : %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // 4) Créer le contexte OpenGL associé
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        printf("Erreur SDL_GL_CreateContext : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // active le VSync

    // 5) Initialiser ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // 6) Initialiser les backends SDL & OpenGL3 pour ImGui
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 7) Boucle principale
    bool run = true;
    while (run)
    {
        // 7.1) Gérer les événements SDL
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                run = false;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
              run = false;
        }

        // 7.2) Démarrer une frame ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        // --- Ici on dessine avec ImGui ---
        ImGui::Begin("Hello, ImGui!");
        ImGui::Text("Fenêtre de test MonSynthe");
        ImGui::End();

        // 7.3) Rendu ImGui + swap
        ImGui::Render();
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // 8) Nettoyer ImGui & SDL
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
