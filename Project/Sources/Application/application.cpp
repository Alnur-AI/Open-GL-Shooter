#include "application_data.h"
#include "application.h"
#include "glad/glad.h"
#include "Engine/imgui/imgui_impl_opengl3.h"
#include "Engine/imgui/imgui_impl_sdl.h"
#include <SDL2/SDL.h>

void compile_shaders();

void start();
void update();
void imgui_update();
void before_exit_game();

bool game_exit_statement();

float pressedW(float w);
float pressedA(float a);
float pressedS(float s);
float pressedD(float d);


bool left_mouse_status(bool isPressed);
void playerAttack();

void movePlayer(float w, float a, float s, float d);
void use_knife();
void use_grenade();

void notWASDpressed();

Application::Application(const string &window_name, const string &project_path):
context(window_name), timer(),
projectPath(project_path),
projectResourcesPath(project_path + "/Resources"),
projectShaderPath(project_path + "/Shaders")
{
  application = this;
  compile_shaders();
}
void Application::start_application()
{
  start();
}
bool Application::sdl_event_handler()
{
  SDL_Event event;
  bool running = true;
  Input &input = Input::input();
  while (SDL_PollEvent(&event))
  {
    ImGui_ImplSDL2_ProcessEvent(&event);

    switch(event.type){
      case SDL_QUIT: running = false; break;
      

      case SDL_KEYDOWN:  input.event_process(event.key, Time::time());

        movePlayer(input.get_key_derivative(SDLK_w ), input.get_key_derivative(SDLK_a ), input.get_key_derivative(SDLK_s ), input.get_key_derivative(SDLK_d ));
        
        switch( event.key.keysym.sym )
        {
          /*
          case SDLK_w:
            pressedW( input.get_key(SDLK_w) );           
          break;
          case SDLK_a:
            pressedA( input.get_key(SDLK_a) );
          break;
          case SDLK_s:
            pressedS( input.get_key(SDLK_s) );
          break;
          case SDLK_d:
            pressedD( input.get_key(SDLK_d) );
          break;
          */
          case SDLK_q:
            use_knife();
          break;
          case SDLK_g:
            use_grenade();
          break;
        }

        /*
        //movePlayer(input.get_key(SDLK_w ), input.get_key(SDLK_a ), input.get_key(SDLK_s ), input.get_key(SDLK_d ));
        movePlayer(input.get_key_derivative(SDLK_w ), input.get_key_derivative(SDLK_a ), input.get_key_derivative(SDLK_s ), input.get_key_derivative(SDLK_d ));
        
        if(event.key.keysym.sym == SDLK_q){
          use_knife();
        }
        if(event.key.keysym.sym == SDLK_g){
          use_grenade();
        }
        */
        //std::cout << "W " << input.get_key_derivative(SDLK_w ) << std::endl;
        //std::cout << "A " << input.get_key_derivative(SDLK_a) << std::endl;
        //std::cout << "S " << input.get_key_derivative(SDLK_s) << std::endl;
        //std::cout << "D " << input.get_key_derivative(SDLK_d) << std::endl << std::endl;
      break;
      
      case SDL_KEYUP: input.event_process(event.key, Time::time());

        if(event.key.keysym.sym == SDLK_ESCAPE){
          return running = false;
        }
        notWASDpressed();
      break;


      case SDL_MOUSEBUTTONDOWN: input.event_process(event.button, Time::time());

        if(event.button.button ==  SDL_BUTTON_LEFT ){
          left_mouse_status(true);
          //std::cout << "PRESSED LEFT MOUSE " << std::endl << std::endl;
        }

      break;

      case SDL_MOUSEBUTTONUP: input.event_process(event.button, Time::time());

        if(event.button.button ==  SDL_BUTTON_LEFT ){
          left_mouse_status(false);
          //std::cout << "NOT PRESSED LEFT MOUSE " << std::endl << std::endl;
        }

      break;

      case SDL_MOUSEMOTION: input.event_process(event.motion, Time::time()); break;

      case SDL_MOUSEWHEEL: input.event_process(event.wheel, Time::time()); break;
      
      case SDL_WINDOWEVENT: break;
    }
  } 
  running = game_exit_statement();
  return running;
}
void Application::main_loop()
{
  bool running = true;
  while(running){
    timer.update();
		running = sdl_event_handler();

    if (running)
    {

      update();
      context.swap_buffer();
      
      context.start_imgui();


      imgui_update();
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
	}
}
void Application::exit()
{
  before_exit_game();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_Quit();
}
string project_resources_path(const string &path)
{
  return Application::instance().projectResourcesPath.string() + "/" + path;
}
string project_resources_path()
{
  return Application::instance().projectResourcesPath.string();
}

