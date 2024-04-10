#pragma once
#include <string>
#include <vector>
#include "Texture/texture2d.h"
#include "Shader/shader.h"
#include "vertex_array_object.h"

class WorldRenderer
{
public:
  mat3 screenToWorld;
  ivec2 resolution;
  std::vector<Texture2D*> textures;
  Shader spriteShader;
  VertexArrayObject spriteVao;
  void init();

  Texture2D *add_texture(Texture2D *texture);
  Texture2D *get_texture(const std::string &name);
  vec2 screen_to_world(int x, int y) const;
};