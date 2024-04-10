#include "Application/application.h"
#include "Application/application_data.h"
#include "Engine/Render/world_renderer.h"
#include "Engine/Render/sprite.h"
#include "Engine/transform2d.h"
#include "Engine/input.h"
#include "Engine/time.h"

#include "Engine/imgui/imgui_impl_opengl3.h"
#include "Engine/imgui/imgui_impl_sdl.h"

enum class Animation_Player_Type
{
  Death = 4,
  AFK = 3,
  Walk = 2,
  Shoot = 1,
  Reload = 0
};


constexpr int zombieCount = 500;

static mat3 cameraProjection;
static Transform2D cameraTransform;
static Transform2D mouseTransform;
static WorldRenderer worldRenderer;

bool play_newrank_animation = false;

float rankWidth = 1.f/10.f,  rankHeight =  1.f/8.f;
vec2 rankSize(rankWidth, rankHeight);

float healthbarWidth = 1.f/11.f,  healthbarHeight =  1.f;
vec2 healthbarSize(healthbarWidth, healthbarHeight);

float portalWidth = 1.f/4.f,  portalHeight =  1.f/4.f;
vec2 portalSize(portalWidth, portalHeight);

float weaponWidth = 1.f/5.f,  weaponHeight =  1.f/5.f;
vec2 weaponSize(weaponWidth, weaponHeight);

float floorWidth = 1.f/ 8.f, floorHeight = 1.f/16.f;
vec2 floorSize(floorWidth, floorHeight);

float newrankWidth = 1.f/ 41.f, newrankHeight = 1.f;
vec2 newrankSize(newrankWidth, newrankHeight);



struct Player
{
  Sprite sprite;
  Transform2D transform;

  float health = 100;
  int score = 0;
  bool isAlive = true;
  bool isdamaged = false;

  int sprite_Death_Count = 31;//4
  int sprite_AFK_Count = 20; //3
  int sprite_Walk_Count = 16; //2
  int sprite_Shoot_Count = 5; // 1
  int sprite_Reload_Count = 20; // 0

  double animation_Death_Time_MAX = 2;
  double animation_AFK_Time_MAX = 2;
  double animation_Walk_Time_MAX = 1;
  double animation_Shoot_Time_MAX = 1;
  double animation_Reload_Time_MAX = 1;

  double animation_Death_Time = 0;
  double animation_AFK_Time = 0;
  double animation_Walk_Time = 0;
  double animation_Shoot_Time = 0;
  double animation_Reload_Time = 0;

  int animation_State = int(Animation_Player_Type::AFK);
  int animation_Death_State = 0;
  int animation_AFK_State = 0;
  int animation_Walk_State = 0;
  int animation_Shoot_State = 0;
  int animation_Reload_State = 0;
};
static Player player;
float playerWidth = 1.f/player.sprite_Death_Count,  playerHeight =  1.f/5.f;
vec2 playerSize(playerWidth, playerHeight);


class Object{

  public:
    Sprite sprite;
    Transform2D transform;
};
class Animation: public Object{

  public:
    vec2 animation;
};

class Healthbar: public Animation{

  public:
    void define(){

      Texture2D *healthbarTex = 
        worldRenderer.add_texture(new Texture2D(project_resources_path("healthbar.png"), RGBA));

      //healthbar = Healthbar{ Sprite(healthbarTex, vec2( 0*healthbarWidth, 0*healthbarHeight), healthbarSize),  Transform2D(vec2(0), vec2(2.5,1.0)) , vec2( 0,0) };
      this->sprite.texture = healthbarTex;
      this->sprite.offset = vec2(0,0);
      this->sprite.scale = healthbarSize;

      this->transform.position = vec2(0);
      this->transform.scale = vec2(2.5,1.0);
      this->transform.rotation = 0.;

      this->animation = vec2(0,0);
    }
    void calculate(float health){
      int health_State = health/10.;
      if(health_State > 10){
        this->sprite.offset = vec2(10*healthbarWidth, 0*healthbarHeight);
        return;
      }
      if(health < 10)
      {
        health_State = abs(sin( M_PI*1000*Time::time() ))>0.5 ? 1 : 0;
        this->sprite.offset = vec2(health_State*healthbarWidth, 0*healthbarHeight);
        return;
      }
      this->sprite.offset = vec2(health_State*healthbarWidth, 0*healthbarHeight);
    }
    void render(mat3 viewProjection){
      this->transform.position = vec2(-12.5,7);
      this->sprite.texture->bind(worldRenderer.spriteShader, "sprite", 0);
      worldRenderer.spriteShader.set_mat3x3("trasformViewProjection", viewProjection * this->transform.get_matrix());
      worldRenderer.spriteShader.set_vec4("uvOffsetScale", vec4(this->sprite.offset, this->sprite.scale));
      worldRenderer.spriteShader.set_vec4("color", vec4(1.f));
      worldRenderer.spriteVao.render();
    }
};
class Rank: public Animation{

  public:
    bool changeRang = false;
    int currentRang = 0;
    float timer = 0;
  public:
    void define(){
      Texture2D *rankTex = 
        worldRenderer.add_texture(new Texture2D(project_resources_path("rank.png"), RGBA));

      //player_rank = Rank{ Sprite(rankTex, vec2( 0*rankWidth, 7*rankHeight), rankSize),  Transform2D(vec2(0), vec2(1.0)), vec2(0,7)};
      this->sprite.texture = rankTex;
      this->sprite.offset = vec2(0,7*rankHeight);
      this->sprite.scale = rankSize;

      this->transform.position = vec2(0);
      this->transform.scale = vec2(1.0);
      this->transform.rotation = 0.;

      this->animation = vec2(0,7);
    }
    void calculate(int score){
      if(score % 60 == 0 && score != 0 ){
        if(this->currentRang == 79){
          return;
        }

        if(this->currentRang != score/60){
          this->changeRang = true;
          play_newrank_animation = true;
          this->currentRang++;
        }
        if(this->changeRang){
          this->animation.x++;
          if(this->animation.x == 10){
            this->animation.x = 0;
            this->animation.y--;
            if(this->animation.y < 0){
              this->animation.x = 0;
              this->animation.y = 7;
            }
          }
          this->changeRang = false;
        }
        this->sprite.offset = vec2( this->animation.x *rankWidth, this->animation.y *rankHeight);
      }
    }
    void render(mat3 viewProjection){
      this->transform.position = vec2(14,7);
      this->sprite.texture->bind(worldRenderer.spriteShader, "sprite", 0);
      worldRenderer.spriteShader.set_mat3x3("trasformViewProjection", viewProjection * this->transform.get_matrix());
      worldRenderer.spriteShader.set_vec4("uvOffsetScale", vec4(this->sprite.offset, this->sprite.scale));
      worldRenderer.spriteShader.set_vec4("color", vec4(1.f));
      worldRenderer.spriteVao.render();
    }
};
class Portal: public Animation{

  public:
    bool isZombieDead = false;
  public:
    void define(Texture2D *portalTex){
      
      //portal = Portal{ Sprite(portalTex, vec2( 0*portalWidth, 0*portalHeight), portalSize),  Transform2D(vec2(0), vec2(1.)) };
      this->sprite.texture = portalTex;
      this->sprite.offset = vec2(0.,0.);
      this->sprite.scale = portalSize;

      this->transform.position = vec2(0.);
      this->transform.scale = vec2(1.0);
      this->transform.rotation = 0.;

      this->animation = vec2(0.,0.);
    }
    void calculate(bool isAlive){
      // if zombie is dead - play portal animation
      if(isAlive == false)  
        this->isZombieDead = true;

      // play portal animation
      if(this->isZombieDead == true){
        this->sprite.offset = vec2( this->animation.x *portalWidth, this->animation.y *portalHeight);
        
        this->animation.x++;
        if(this->animation.x == 4){
          this->animation.x = 0;
          this->animation.y++;
          if(this->animation.y == 4){
            this->animation.y = 0;
            this->isZombieDead = false;
          }
        }
      }
    }
    void render(mat3 viewProjection, Transform2D zombie_transform, Transform2D player_transform){
      Transform2D portalT;
      portalT.position = ( zombie_transform.position - player_transform.position);
      portalT.rotation = zombie_transform.rotation + M_PI*(5./4.);

      this->sprite.texture->bind(worldRenderer.spriteShader, "sprite", 0);
      worldRenderer.spriteShader.set_mat3x3("trasformViewProjection", viewProjection * portalT.get_matrix());
      worldRenderer.spriteShader.set_vec4("uvOffsetScale", vec4(this->sprite.offset, this->sprite.scale));
      worldRenderer.spriteShader.set_vec4("color", vec4(1.f));
      worldRenderer.spriteVao.render();
    }
};

class Bullet: public Object{
  public:

  public:
    void define(Texture2D *bulletTex){

      //myBullet[i] = Bullet{ Sprite(bulletTex, vec2( 0 ), vec2(1.)),  Transform2D(vec2(0), vec2(0.25)) };
      this->sprite.texture = bulletTex;
      this->sprite.offset = vec2(0.);
      this->sprite.scale =  vec2(1.);

      this->transform.position = vec2(0.);
      this->transform.scale = vec2(0.25);
      this->transform.rotation = 0.;
    }
    void calculate(Player &player){
      // calculate trajectory
      if( pow(this->transform.position.x- player.transform.position.x,2) +  pow(this->transform.position.y-player.transform.position.y,2) >= 0.1 ){
        this->transform.position.x -= 0.15*(cos(this->transform.rotation) - sin(this->transform.rotation));
        this->transform.position.y += 0.15*(sin(this->transform.rotation) + cos(this->transform.rotation));
      }
      else{
        this->transform.position = player.transform.position;
      }
    }
    void render(mat3 viewProjection, vec2 player_transform_position){
      Transform2D bullet;
      bullet.position = ( this->transform.position - player_transform_position);
      bullet.rotation = this->transform.rotation + M_PI*(5./4.);
      bullet.scale = vec2(0.25);
      this->sprite.texture->bind(worldRenderer.spriteShader, "sprite", 0);
      worldRenderer.spriteShader.set_mat3x3("trasformViewProjection", viewProjection * bullet.get_matrix());
      worldRenderer.spriteShader.set_vec4("uvOffsetScale", vec4(this->sprite.offset, this->sprite.scale));
      worldRenderer.spriteShader.set_vec4("color", vec4(1.f));
      worldRenderer.spriteVao.render();
    }
};




struct Weapon
{
  Sprite sprite;
  Transform2D transform;

  int curr_x = 0;
  int curr_y = 1;

  bool isClickPressed = false;

  // (Pistol)
  // fire_rate - интервал между выстрелами
  // damage - сколько зомби уничтожит пуля
  // magazine - количество патронов в обоиме 
  // ammobag - общее количество патронов

  // (Shotgun)
  // fire_rate - интервал между выстрелами
  // bullets - количество вылетаемых снарядов (не патронов) за раз
  // damage - сколько зомби уничтожит пуля
  // magazine - количество патронов в обоиме 
  // ammobag - общее количество патронов

  // (submachine gun)
  // fire_rate - интервал между выстрелами
  // damage - сколько зомби уничтожит пуля
  // magazine - количество патронов в обоиме 
  // ammobag - общее количество патронов

  // (machine gun)
  // fire_rate - интервал между выстрелами
  // damage - сколько зомби уничтожит пуля
  // magazine - количество патронов в обоиме 
  // ammobag - общее количество патронов

  // (rifle)
  // fire_rate - интервал между выстрелами
  // damage - сколько зомби уничтожит пуля
  // magazine - количество патронов в обоиме 
  // ammobag - общее количество патронов

   // (grenade)
  // fire_rate - интервал между выстрелами
  // range  - радиус поражения
  // magazine - количество патронов в обоиме 
  // ammobag - общее количество патронов

  // 0 0 - (Pistol) Glock 19

  // 0 1 - (Pistol) Taurus 

  // 0 2 - (Pistol) Five-Seven

  // 0 3 - (Pistol) Makarov

  // 0 4 - (Pistol) Luger P08

  // 1 0 - (Pistol) Tokarev

  // 1 1 - (Pistol) Walther P38

  // 1 2 - (Pistol) Nagant M1895

  // 1 3 - (Shotgun) Spas-12

  // 1 4 - (submachine gun) Mp5

  // 2 0 - (submachine gun) ump 45

  // 2 1 - (submachine gun) uzi

  // 2 2 - (submachine gun) FN P90

  // 2 3 - (submachine gun) PP-19 Bizon

  // 2 4 - (machine gun) Steyr aug

  // 3 0 - (machine gun) m4a1

  // 3 1 - (machine gun)ak-47

  // 3 2 - (machine gun) fn scar

  // 3 3 - (rifle) mosin-nagant

  // 3 4 - (rifle) awp

  // 4 0 - (rifle) steyr ssg 08

  // 4 1 - (rifle) dragunov

  // 4 2 - (rifle) barrett m82

  // 4 3 - (grenade) rpg-7

  // 4 4 - (grenade) m79
};






static Player zombie[zombieCount];
static Portal portal[zombieCount];

static Weapon weapon;
static Healthbar healthbar;
static Object grass;
static Object wall;
static Object crosshair;
static Object newrank;
static Object loading;
static Object reload;
static Rank player_rank;

static Bullet myBullet[100];
int myShootedBullet = 0;

constexpr int max_Scoreboard = 60*80;









constexpr int newrank_sprite_count = 41;
int newrank_sprite = 0;
float newrank_timer = 0;
//bool play_newrank_animation = false;

float loadingWidth = 1.0f/ 10.0f, loadingHeight = 1.0f/20.0f;
vec2 loadingSize(loadingWidth, loadingHeight);
int loading_x = 0;
int loading_y = 19;

float reloadWidth = 1.f/ 20.f, reloadHeight = 1.f/7.f;
vec2 reloadSize(reloadWidth, reloadHeight);
float reload_timer;
int reload_x = 0;
int reload_y = 6;






void start()
{

  worldRenderer.init();

  // Define Textures
  Texture2D *playerTex = 
  worldRenderer.add_texture(new Texture2D(project_resources_path("player.png"), RGBA));
  Texture2D *floorTex = 
  worldRenderer.add_texture(new Texture2D(project_resources_path("floor.png"), RGBA));
  
  
  
  
  Texture2D *crosshairTex = 
  worldRenderer.add_texture(new Texture2D(project_resources_path("crosshair.png"), RGBA));
  Texture2D *loadingTex = 
  worldRenderer.add_texture(new Texture2D(project_resources_path("loading.png"), RGBA));
  Texture2D *reloadTex = 
  worldRenderer.add_texture(new Texture2D(project_resources_path("reload.png"), RGBA));
  Texture2D *newrankTex = 
  worldRenderer.add_texture(new Texture2D(project_resources_path("newrank.png"), RGBA));
  Texture2D *weaponTex = 
  worldRenderer.add_texture(new Texture2D(project_resources_path("weapon.png"), RGBA));

  //std::cout << "Textures loaded" << std::endl;
  

  // Define Player
  player = Player{ Sprite(playerTex, vec2( 0*playerWidth, 0*playerHeight), playerSize), Transform2D(vec2(0), vec2(1))};

  // Define weapon
  weapon = Weapon{ Sprite(weaponTex, vec2( 0*weaponWidth, 0*weaponHeight), weaponSize), Transform2D(vec2(0), vec2(1))};
  
  // Define zombies
  for(int i = 0; i < zombieCount; i++){
    int x = rand() % 30 -15;
      int y = rand() % 30 -15;
      if(abs(x) < 4 ){

        if(x==0)
          x += 4;

        x += sign(x)*4;
      }
      if(abs(y) < 4){
        
        if(y==0)
          y += 4;
        
        y += sign(y)*4;
      }
    zombie[i] = Player{ Sprite(playerTex, vec2( 0*playerWidth, 0*playerHeight), playerSize), Transform2D(vec2(x ,y), vec2(1))};
    zombie[i].animation_State = int(Animation_Player_Type::Walk);
  }
  // Define blocks
  grass = Object{  Sprite(floorTex, vec2( 0*floorWidth, 14*floorHeight), floorSize),  Transform2D(vec2(0), vec2(0.5)) };

  wall = Object{ Sprite(floorTex, vec2( 5*floorWidth, 15*floorHeight), floorSize),  Transform2D(vec2(0), vec2(0.5)) };

  // Define crosshair
  crosshair = Object{ Sprite(crosshairTex, vec2( 0, 0), vec2(1.0)),  Transform2D(vec2(0), vec2(0.5)) };
  reload = Object{ Sprite(reloadTex, vec2( reload_x*reloadWidth, reload_y*reloadHeight), reloadSize),  Transform2D(vec2(0), vec2(2)) };
  loading = Object{ Sprite(loadingTex, vec2( 0, 19*loadingHeight), loadingSize),  Transform2D(vec2(0,4), vec2( 1.5 )) };
  
  // Define rank
  player_rank.define();
  //player_rank = Rank{ Sprite(rankTex, vec2( 0*rankWidth, 7*rankHeight), rankSize),  Transform2D(vec2(0), vec2(1.0)), vec2(0,7)};
  newrank = Object{ Sprite(newrankTex, vec2( 0, 0), newrankSize),  Transform2D(vec2(0), vec2( 1.536*5, .864*5)) };
  //std::cout << "Good rank done" << std::endl;

  // Define healthbar
  healthbar.define();
  //std::cout << "Good healthbar done" << std::endl;

  // Define Portal
  Texture2D *portalTex = 
        worldRenderer.add_texture(new Texture2D(project_resources_path("portal.png"), RGBA));
  for(int i = 0; i < zombieCount; i++)
    portal[i].define(portalTex);

  //std::cout << "Good portal done" << std::endl;
    
  // Define bullets
  Texture2D *bulletTex = 
    worldRenderer.add_texture(new Texture2D(project_resources_path("bullet.png"), RGBA));
  for(int i = 0; i < 100; i++)
    myBullet[i].define(bulletTex);
    

  // define world and camera
  worldRenderer.spriteShader = get_shader("standart_sprite");
  Resolution r = Application::get_context().get_screen_resolution();
  worldRenderer.resolution = vec2(r.x, r.y);
  float orthoScale = 1.f/100.f;//100 pixel per unit
  cameraProjection = mat3(0.f);
  cameraProjection[0][0] = 1.f / r.x;
  cameraProjection[1][1] = 1.f / r.y;
  cameraTransform = Transform2D(vec2(0), vec2(orthoScale));
}
bool game_exit_statement(){
  if(player.health <= 0)
    return false;
  if(player.score >= max_Scoreboard)
    return false;
  return true;
}

void calculate_Player_AFK_Animation( struct Player &player){
  player.animation_AFK_Time += Time::delta_time();
  if( player.animation_AFK_Time  >  player.animation_AFK_Time_MAX / player.sprite_AFK_Count )
  {
    player.animation_AFK_State++;
    if(player.animation_AFK_State == player.sprite_AFK_Count ){
      player.animation_AFK_State = 0;
    }
    //std::cout << player.animation_AFK_Time << std::endl;
    //std::cout << player.animation_AFK_State << std::endl;
    player.sprite.offset = vec2( player.animation_AFK_State*playerWidth, int(Animation_Player_Type::AFK)*playerHeight );
    player.animation_AFK_Time = 0;
  }
}
void calculate_Player_Walk_Animation( struct Player &player){
  player.animation_Walk_Time += Time::delta_time();
  if( player.animation_Walk_Time  >  player.animation_Walk_Time_MAX / player.sprite_Walk_Count )
  {
    player.animation_Walk_State++;
    if(player.animation_Walk_State == player.sprite_Walk_Count ){
      player.animation_Walk_State = 0;
    }
    //std::cout << player.animation_Walk_Time << std::endl;
    //std::cout << player.animation_Walk_State << std::endl;
    player.sprite.offset = vec2( player.animation_Walk_State*playerWidth, int(Animation_Player_Type::Walk)*playerHeight );
    player.animation_Walk_Time = 0;
  }
}
void calculate_Player_Shoot_Animation( struct Player &player){
  player.animation_Shoot_Time += Time::delta_time();
  if( player.animation_Shoot_Time  >  player.animation_Shoot_Time_MAX / player.sprite_Shoot_Count )
  {
    player.animation_Shoot_State++;
    if(player.animation_Shoot_State == player.sprite_Shoot_Count ){

      player.animation_State = int(Animation_Player_Type::AFK);
      player.sprite.offset = vec2( player.animation_AFK_State*playerWidth, int(Animation_Player_Type::AFK)*playerHeight );

      player.animation_Death_Time = 0;
      player.animation_AFK_Time = 0;
      player.animation_Walk_Time = 0;
      player.animation_Shoot_Time = 0;
      player.animation_Reload_Time = 0;

      player.animation_Shoot_State = 0;
      return;
      //player.animation_Shoot_State = 0;
    }
    //std::cout << player.animation_Walk_Time << std::endl;
    //std::cout << player.animation_Walk_State << std::endl;
    player.sprite.offset = vec2( player.animation_Shoot_State*playerWidth, int(Animation_Player_Type::Shoot)*playerHeight );
    player.animation_Shoot_Time = 0;
  }
}




bool w_is_pressed = false;
bool a_is_pressed = false;
bool s_is_pressed = false;
bool d_is_pressed = false;

float pressedW(float w){
  //player.transform.position.x += 0.0;
  player.transform.position.y += .1*w;
  player.animation_State = int(Animation_Player_Type::Walk);
  w_is_pressed = true;
  return w;
}
float pressedA(float a){
  player.transform.position.x -= .1*a;
  //player.transform.position.y += 0.0;
  player.animation_State = int(Animation_Player_Type::Walk);
  a_is_pressed = true;
  return a;
}
float pressedS(float s){  
  //player.transform.position.x += 0.0;
  player.transform.position.y -= .1*s;
  player.animation_State = int(Animation_Player_Type::Walk);
  s_is_pressed = true;
  return s;
}
float pressedD(float d){  
  player.transform.position.x += .1*d;
  //player.transform.position.y += 0.0;
  player.animation_State = int(Animation_Player_Type::Walk);
  d_is_pressed = true;
  return d;
}
void movePlayer(float w, float a, float s, float d){
  
  ///player.transform.position.x += (a - d)/10;
  //player.transform.position.y += (s - w)/10;
  if(int(w)==1)
  player.transform.position.y += 0.1;
  if(int(a)==1)
  player.transform.position.x -= 0.1;
  if(int(s)==1)
  player.transform.position.y -= 0.1;
  if(int(d)==1)
  player.transform.position.x += 0.1;
  

  player.animation_State = int(Animation_Player_Type::Walk);

  //std::cout << player.transform.position.x << " " << player.transform.position.y << std::endl;
}


bool left_mouse_status(bool isPressed){
  weapon.isClickPressed = isPressed;
  return isPressed;
}

void playerAttack(){
  //if(q) 
  player.animation_State = int(Animation_Player_Type::Shoot);
  myBullet[myShootedBullet].transform.position = player.transform.position;
  myBullet[myShootedBullet].transform.rotation = player.transform.rotation+M_PI*(3./4.);

  myBullet[myShootedBullet].transform.position.x -= .5*(cos(myBullet[myShootedBullet].transform.rotation) - sin(myBullet[myShootedBullet].transform.rotation));
  myBullet[myShootedBullet].transform.position.y += .5*(sin(myBullet[myShootedBullet].transform.rotation) + cos(myBullet[myShootedBullet].transform.rotation));

  myShootedBullet++;
  if(myShootedBullet==100)
    myShootedBullet = 0;
}
void notWASDpressed(){

  w_is_pressed = false;
  a_is_pressed = false;
  s_is_pressed = false;
  d_is_pressed = false;

  player.animation_State = int(Animation_Player_Type::AFK);
  return;
}
void use_knife(){
  for(int i = 0; i < zombieCount; i++){
    if(pow( zombie[i].transform.position.x -  player.transform.position.x,2) + pow( zombie[i].transform.position.y -player.transform.position.y,2) < 2){
      zombie[i].isAlive = false;
      player.score++;

      int x = rand() % 30 -15;
      int y = rand() % 30 -15;
      if(abs(x) < 4 || abs(y) < 4){

        if(x==0)
          x += 4;
        
        if(y==0)
          y += 4;
        
        x += sign(x)*4;
        y += sign(y)*4;
      }
      zombie[i].transform.position = vec2(x ,y);

      // if zombie is dead - play portal animation
    if(zombie[i].isAlive == false)  
      portal[i].isZombieDead = true;
      
    }
  }
}
void use_grenade(){

  MousePosition mousePos = Input::get_mouse_position();
  mouseTransform.position = worldRenderer.screen_to_world(mousePos.x, mousePos.y);

  for(int i = 0; i < zombieCount; i++){
    if(pow( zombie[i].transform.position.x -  player.transform.position.x - mouseTransform.position.x,2) + pow( zombie[i].transform.position.y -player.transform.position.y - mouseTransform.position.y,2) < 4){
      zombie[i].isAlive = false;
      player.score++;

      int x = rand() % 30 -15;
      int y = rand() % 30 -15;
      if(abs(x) < 4 || abs(y) < 4){

        if(x==0)
          x += 4;
        
        if(y==0)
          y += 4;
        
        x += sign(x)*4;
        y += sign(y)*4;
      }
      zombie[i].transform.position = vec2(x ,y);

      // if zombie is dead - play portal animation
    if(zombie[i].isAlive == false)  
      portal[i].isZombieDead = true;
      
    }
  }
}

void update(){
  //logic
  worldRenderer.screenToWorld = cameraTransform.get_matrix();
  MousePosition mousePos = Input::get_mouse_position();
  mouseTransform.position = worldRenderer.screen_to_world(mousePos.x, mousePos.y);

  crosshair.transform.position = worldRenderer.screen_to_world(mousePos.x, mousePos.y);

  // set angle view
  player.transform.rotation = -atan2f( (mouseTransform.position.y ), (mouseTransform.position.x ) ) ;

  // if left is pressed
  if(weapon.isClickPressed )playerAttack();


  /*PLAYER ANIMATION*/
  switch (player.animation_State)
  {
  case int(Animation_Player_Type::Walk):
    calculate_Player_Walk_Animation(player);
    player.animation_Death_Time = 0;
    player.animation_AFK_Time = 0;
    //player.animation_Walk_Time = 0;
    player.animation_Shoot_Time = 0;
    player.animation_Reload_Time = 0;
    break;
  case int(Animation_Player_Type::AFK):
    calculate_Player_AFK_Animation(player);
    player.animation_Death_Time = 0;
    //player.animation_AFK_Time = 0;
    player.animation_Walk_Time = 0;
    player.animation_Shoot_Time = 0;
    player.animation_Reload_Time = 0;
    break;
  case int(Animation_Player_Type::Shoot):
    calculate_Player_Shoot_Animation(player);
    player.animation_Death_Time = 0;
    player.animation_AFK_Time = 0;
    player.animation_Walk_Time = 0;
    //player.animation_Shoot_Time = 0;
    player.animation_Reload_Time = 0;
    break;
  
  default:
    break;
  }
  
  /*ZOMBIE ANIMATION*/
  for(int i = 0 ; i < zombieCount; i++)
  switch (zombie[i].animation_State)
  {
  case int(Animation_Player_Type::Walk):
    calculate_Player_Walk_Animation(zombie[i]);
    zombie[i].animation_Death_Time = 0;
    zombie[i].animation_AFK_Time = 0;
    //player.animation_Walk_Time = 0;
    zombie[i].animation_Shoot_Time = 0;
    zombie[i].animation_Reload_Time = 0;
    break;
  case int(Animation_Player_Type::AFK):
    calculate_Player_AFK_Animation(zombie[i]);
    zombie[i].animation_Death_Time = 0;
    //player.animation_AFK_Time = 0;
    zombie[i].animation_Walk_Time = 0;
    zombie[i].animation_Shoot_Time = 0;
    zombie[i].animation_Reload_Time = 0;
    break;
  case int(Animation_Player_Type::Shoot):
    calculate_Player_Shoot_Animation(zombie[i]);
    zombie[i].animation_Death_Time = 0;
    zombie[i].animation_AFK_Time = 0;
    zombie[i].animation_Walk_Time = 0;
    //player.animation_Shoot_Time = 0;
    zombie[i].animation_Reload_Time = 0;
    break;
  
  default:
    break;
  }
  
  // respawn dead zombies
  for(int i = 0; i < zombieCount; i++){
    if(zombie[i].isAlive == false){
      zombie[i].isAlive = true;

      int x = rand() % 30 -15;
      int y = rand() % 30 -15;
      if(abs(x) < 4 || abs(y) < 4){

        if(x==0)
          x += 4;
        
        if(y==0)
          y += 4;
        
        x += sign(x)*4;
        y += sign(y)*4;
      }
      zombie[i].transform.position = vec2(x + player.transform.position.x ,y + player.transform.position.y);
    }
  }

  // calculate bullets
  for(int i = 0; i < 100; i++){
    myBullet[i].calculate(player);
    // calculate zombie hit
    for (int j = 0; j < zombieCount; j++){
      bool bullet_out_of_player =  pow(myBullet[i].transform.position.x- player.transform.position.x,2) +  pow(myBullet[i].transform.position.y-player.transform.position.y,2) >= 0.1;
      bool buller_inside_zombie = pow(myBullet[i].transform.position.x- zombie[j].transform.position.x,2) +  pow(myBullet[i].transform.position.y-zombie[j].transform.position.y,2) <= 0.4;
      if( bullet_out_of_player  && zombie[j].isAlive && buller_inside_zombie ){
        zombie[j].isAlive = false;
        player.score++;
        zombie[j].transform.position = vec2(rand() % 30 -15 ,rand() % 30 -15);
      }
    }
  }
  
  // calculate player health
  for(int i = 0 ; i < zombieCount; i++){
    if( zombie[i].isAlive && pow(zombie[i].transform.position.x - player.transform.position.x,2) + pow(zombie[i].transform.position.y - player.transform.position.y,2) <= 0.3 )
    {
      if(player.health < 10)
        player.health -= 0.03;
      else
        player.health -= 0.1;
      player.isdamaged = true;
    }
  }
  
  // output debug
  {
  //std::cout << "x: " << player.transform.position.x << "\n";
  //std::cout << "y: " << player.transform.position.y << "\n";
  //std::cout << "angle: " << player.transform.rotation << "\n";
  //std::cout << "score: " << player.score << "\n";
  //std::cout << "Health: " << player.health << "\n\n";
  }

  // calculate zombie movement
  for(int i = 0 ; i < zombieCount; i++){
    //zombie[i].transform.rotation = atan2f( (zombie[i].transform.position.y - player.transform.position.y ), (zombie[i].transform.position.x - player.transform.position.x ) );
    zombie[i].transform.rotation = -atan2f( (-zombie[i].transform.position.y +player.transform.position.y ), (-zombie[i].transform.position.x +player.transform.position.x) ) - M_PI*(5./4.);

    zombie[i].transform.position.x -= Time::delta_time()*(cos(zombie[i].transform.rotation) - sin(zombie[i].transform.rotation));
    zombie[i].transform.position.y += Time::delta_time()*(sin(zombie[i].transform.rotation) + cos(zombie[i].transform.rotation));
  }
  
  // Calculate portal animation
  for(int i = 0 ; i < zombieCount; i++)
    portal[i].calculate(zombie[i].isAlive);
  

  // Calculate rang 
  player_rank.calculate(player.score);

  // Calculate newrank animation
  if(play_newrank_animation){

    newrank_timer += Time::delta_time();
    if(newrank_timer  > 0.1f){
      newrank_timer = 0;
      newrank_sprite++;
      newrank.sprite.offset = vec2( newrank_sprite*newrankWidth, 0);
      if(newrank_sprite == newrank_sprite_count){
        newrank_sprite = 0;
        play_newrank_animation = false;
        newrank_timer = 0;
      }
    }
  }
  
  // Calculate loading
  int success_measure = int(float(player.score) / float(max_Scoreboard) * 100);

  loading_x = success_measure%10;  
  loading_y = 19 - success_measure/10;
  //std::cout << player.score << " " << success_measure << " " << loading_x << " " << loading_y << "\n";
  if(loading_y == -1)
    loading_y=19;

  loading.sprite.offset = vec2( loading_x*loadingWidth, loading_y*(loadingHeight));
  

  // Calculate reload
  reload_timer += Time::delta_time();
  if(reload_timer > 0.01){
    reload_timer = 0;
    reload_x++;
    if(reload_x == 20){
      reload_x = 0;
      reload_y--;
      if(reload_y < 0){
        reload_x = 0;
        reload_y = 6;
      }
    }
  }
  //std::cout << reload_x << " " << reload_y << std::endl;
  reload.sprite.offset = vec2( reload_x*reloadWidth, reload_y*reloadHeight);
  

  // Calculate healthbar
  healthbar.calculate(player.health);

  //Start Render
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mat3 viewProjection = cameraProjection * glm::inverse(cameraTransform.get_matrix());
  worldRenderer.spriteShader.use();
  glEnable(GL_BLEND); 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

  // Render blocks
  
      // flat grass
      for(int i = -80; i < 80; i++ )
      for(int j = -80; j < 80; j++ ){
        Transform2D grassT;
        grassT.position = (vec2( i,j ) - player.transform.position);
        grassT.scale = vec2(0.5);
        if( -16 < grassT.position.x && grassT.position.x < 16 && -10 < grassT.position.y && grassT.position.y < 10  ){
          grass.sprite.texture->bind(worldRenderer.spriteShader, "sprite", 0);
          worldRenderer.spriteShader.set_mat3x3("trasformViewProjection", viewProjection * grassT.get_matrix());
          worldRenderer.spriteShader.set_vec4("uvOffsetScale", vec4(grass.sprite.offset, grass.sprite.scale));
          worldRenderer.spriteShader.set_vec4("color", vec4(1.f));
          worldRenderer.spriteVao.render();
        }
      }
      // wall
      for(int i = -80; i < 80; i++ ){
        Transform2D wallT;
        wallT.position = (vec2( i,40 ) - player.transform.position);
        wallT.scale = vec2(0.5);
        if( -16 < wallT.position.x && wallT.position.x < 16 && -10 < wallT.position.y && wallT.position.y < 10  ){
          wall.sprite.texture->bind(worldRenderer.spriteShader, "sprite", 0);
          worldRenderer.spriteShader.set_mat3x3("trasformViewProjection", viewProjection * wallT.get_matrix());
          worldRenderer.spriteShader.set_vec4("uvOffsetScale", vec4(wall.sprite.offset, wall.sprite.scale));
          worldRenderer.spriteShader.set_vec4("color", vec4(1.f));
          worldRenderer.spriteVao.render();
      }
    }
  

  // Render Bullets
  for(int i = 0; i < 100; i++ )
    myBullet[i].render(viewProjection, player.transform.position);
  

  // Render Zombie
  for(int i = 0; i < zombieCount; i++ ){
    if(zombie[i].isAlive){
      Transform2D zombieT;
      zombieT.position = ( zombie[i].transform.position - player.transform.position);
      zombieT.rotation = zombie[i].transform.rotation + M_PI*(5./4.);
      if( -16 < zombieT.position.x && zombieT.position.x < 16 && -10 < zombieT.position.y && zombieT.position.y < 10  ){
        zombie[i].sprite.texture->bind(worldRenderer.spriteShader, "sprite", 0);
        worldRenderer.spriteShader.set_mat3x3("trasformViewProjection", viewProjection * zombieT.get_matrix());
        worldRenderer.spriteShader.set_vec4("uvOffsetScale", vec4(zombie[i].sprite.offset, zombie[i].sprite.scale));
        worldRenderer.spriteShader.set_vec4("color", vec4(1.f,1.f,0.f,1.f));
        worldRenderer.spriteVao.render();
      }
    }

    if(portal[i].isZombieDead)
      portal[i].render(viewProjection, zombie[i].transform, player.transform);
  }

  

  // Render Weapon
  weapon.sprite.offset = vec2(weapon.curr_x*weaponWidth,weapon.curr_y*weaponHeight);
  weapon.transform.position = vec2(-7,6);
  weapon.transform.scale = vec2(2);
  weapon.sprite.texture->bind(worldRenderer.spriteShader, "sprite", 0);
  worldRenderer.spriteShader.set_mat3x3("trasformViewProjection", viewProjection * weapon.transform.get_matrix());
  worldRenderer.spriteShader.set_vec4("uvOffsetScale", vec4(weapon.sprite.offset, weapon.sprite.scale));
  worldRenderer.spriteShader.set_vec4("color", vec4(1.f));
  worldRenderer.spriteVao.render();
  
  // Render Player
  Transform2D playerT = vec2(0.);
  playerT.rotation = player.transform.rotation;
  player.sprite.texture->bind(worldRenderer.spriteShader, "sprite", 0);
  worldRenderer.spriteShader.set_mat3x3("trasformViewProjection", viewProjection * playerT.get_matrix());
  worldRenderer.spriteShader.set_vec4("uvOffsetScale", vec4(player.sprite.offset, player.sprite.scale));

  if(player.isdamaged ){
    worldRenderer.spriteShader.set_vec4("color", vec4(abs(sin( M_PI*2000*Time::time() ))/2. + 0.5,0.f,0.f,1.f));
    player.isdamaged = false;  
  }
  else{
    worldRenderer.spriteShader.set_vec4("color", vec4(1.f));
  }
  worldRenderer.spriteVao.render();
  

  // render Rank
  player_rank.render(viewProjection);
  
  
  // render Healthbar
  healthbar.render(viewProjection);


   // render crosshair
  crosshair.sprite.texture->bind(worldRenderer.spriteShader, "sprite", 0);
  worldRenderer.spriteShader.set_mat3x3("trasformViewProjection", viewProjection * crosshair.transform.get_matrix());
  worldRenderer.spriteShader.set_vec4("uvOffsetScale", vec4(crosshair.sprite.offset, crosshair.sprite.scale));
  worldRenderer.spriteShader.set_vec4("color", vec4(1.f));
  worldRenderer.spriteVao.render();

   // render reload
  reload.transform = crosshair.transform;
  reload.transform.scale = vec2(1.2);
  reload.sprite.texture->bind(worldRenderer.spriteShader, "sprite", 0);
  worldRenderer.spriteShader.set_mat3x3("trasformViewProjection", viewProjection * reload.transform.get_matrix());
  worldRenderer.spriteShader.set_vec4("uvOffsetScale", vec4(reload.sprite.offset, reload.sprite.scale));
  worldRenderer.spriteShader.set_vec4("color", vec4(1.f,1.f,1.f,.75f));
  worldRenderer.spriteVao.render();

  // render newrank
  if(play_newrank_animation){
    newrank.transform.position = vec2(0,4);
    newrank.sprite.texture->bind(worldRenderer.spriteShader, "sprite", 0);
    worldRenderer.spriteShader.set_mat3x3("trasformViewProjection", viewProjection * newrank.transform.get_matrix());
    worldRenderer.spriteShader.set_vec4("uvOffsetScale", vec4(newrank.sprite.offset, newrank.sprite.scale));
    worldRenderer.spriteShader.set_vec4("color", vec4(1.f,1.f,1.f,0.75f));
    worldRenderer.spriteVao.render();
  }

  // render loading
  loading.transform.position = vec2(0,7);
  loading.sprite.texture->bind(worldRenderer.spriteShader, "sprite", 0);
  worldRenderer.spriteShader.set_mat3x3("trasformViewProjection", viewProjection * loading.transform.get_matrix());
  worldRenderer.spriteShader.set_vec4("uvOffsetScale", vec4(loading.sprite.offset, loading.sprite.scale));
  worldRenderer.spriteShader.set_vec4("color", vec4(1.f));
  worldRenderer.spriteVao.render();
  
}




void imgui_update()
{
  //ImGui::Begin("Sample window"); 

  //ImGui::End(); // end window
}

void before_exit_game()
{

}