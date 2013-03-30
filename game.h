#ifndef __GAME__
#define __GAME__

class Timer;
struct Robeman;
struct Particle;

class Game {
  
private:
  bool started;
  bool running;
  bool paused;
  bool quit;
  
  // main game screen
  SDL_Surface *screen;
  
  // game camera
  SDL_Rect camera;
  
  // event catcher
  SDL_Event event;
  int eventCounter;
  
  // ressources
  Mix_Music *music;
  Mix_Chunk *sound1; // jump
  Mix_Chunk *sound2; // achieve
  Mix_Chunk *sound3; // hurt
  Mix_Chunk *sound4; // collect
  Mix_Chunk *sound5; // shoot
  Mix_Chunk *sound6; // explode
  SDL_Surface *image;
  SDL_Surface *background;
  SDL_Surface *menu;
  SDL_Rect bgBox;
  
  // font
  TTF_Font *font;
  
  // player
  SDL_Rect playerRect[ 9 ];
  int playerX;
  int playerY;
  int playerVelX;
  int playerVelY;
  int playerSprite;
  int playerMovement; // actually PLAYER_MOVEMENT
  Timer playerAnimationTimer;
  Timer playerInvincibleTimer;
  bool playerFalling;
  bool playerJumping;
  bool playerInvincible;
  int playerJumpStartY;
  int playerScore;
  int playerHealth;
  std::vector<Particle> bullets;
  
  // world
  SDL_Rect worldTiles[ 29 ];
  int *worldMap; // array of tiles that build the world
  int worldWidth; // in tiles
  int worldHeight; // in tiles
  bool gameIsLight;
  int gameDifficulty;
  int collectibleNuns;
  bool gameWin;
  bool gameLost;
  bool gameMenu;
  int selectedMenu;
  bool gameShowInfo;
  Timer infoTimer;
  
  // robemen - enemies
  std::vector<Robeman> robemen;
  // particles
  std::vector<Particle> particles;
  
  // DEBUG
  Timer fps;
  Timer drawTimer;
  int draws;
  int frame;
  float frameRate;
  
public:
  Game( );
  ~Game( );
  
  void buildWorldMap( );
  
  void initialize( );
  void initialize( int );
  void run( );
  void destroy( );
  void showMenu( );
  
  void shoot( );
  
  void handleEvent( );
  
  void update( Uint32 );
  void updatePlayerMovement( float );
  void updatePlayerAnimation( float );
  void updatePlayerBullets( float );
  void updateCamera( float );
  void updateRobemen( float );
  void updateParticles( float );
  
  void draw( );
  void drawPlayer( );
  void drawWorld( );
  void drawDebug( );
  void drawRobemen( );
  void drawParticles( );
  void drawStats( );
  void drawMessage( int );
  void drawMenu( );
  
  void switchDarkWorld( );
  
  SDL_Surface *load_image( std::string );
  void drawThat( int, int, SDL_Surface *, SDL_Rect * );
  
  // helper
  SDL_Rect helper_get_rect_for_image( int );
  SDL_Rect helper_get_rect_for_images( int, int, int );
};

#endif