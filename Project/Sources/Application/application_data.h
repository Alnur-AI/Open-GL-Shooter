#pragma once
#include <filesystem>
#include "common.h"
#include "context.h"
#include "Engine/input.h"
#include "Engine/time.h"

class Application
{
private:
  inline static Application *application;
  Context context;
  Time timer;
public:
  filesystem::path projectPath, projectResourcesPath, projectShaderPath;
  Application(const string &window_name, const string &project_path);
  bool sdl_event_handler();
  void start_application();
  void main_loop();
  void exit();
  static Application& instance()
  {
    return *application;
  }
  static Context& get_context()
  {
    return application->context;
  }
};