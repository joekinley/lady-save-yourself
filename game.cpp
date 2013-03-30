#include "main.h"
#include <SDL/SDL.h>
#include "SDL/SDL_mixer.h"

/*
 Playing sounds: Mix_PlayChannel( -1, sound1, 0 );
 Playing music: Mix_PlayMusic( music, -1 );
 Check for music: Mix_PlayingMusic( );
 Music Handling: Mix_PauseMusic( ), Mix_ResumeMusic( ), Mix_HaltMusic( ), Mix_PausedMusic( )
 */

Game::Game( ) {

  srand( time( NULL ) );
  
  // default values
  started = false;
  running = false;
  paused = false;
  quit = false;
  screen = NULL;
  
  // initialize SDL stuff
  SDL_Init( SDL_INIT_EVERYTHING );
  TTF_Init( );
  
  SDL_WM_SetCaption( GAME_TITLE, NULL );
  screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
  
  Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 );
  //Mix_Init( MIX_INIT_MP3 );
  
  // shall just be initialized once, because although the game is not running, keys could be pressed
  eventCounter = 0;
}

Game::~Game( ) {
  
  TTF_Quit( );
  //Mix_Quit( );
  SDL_Quit( );
}

void Game::initialize( int myDiff ) {
  gameDifficulty = myDiff;
  initialize( );
}

// starts game
void Game::initialize( ) {
    
  running = true;
  selectedMenu = -1;
  
  // camera
  camera.x = 0;
  camera.y = 0;
  camera.w = SCREEN_WIDTH;
  camera.h = SCREEN_HEIGHT;
  
  // initial world options
  worldWidth = 0;
  worldHeight = 0;
  worldMap = NULL;
  gameIsLight = true; // start with light world
  collectibleNuns = 0;
  bgBox.x = 0;
  bgBox.y = SCREEN_HEIGHT;
  bgBox.w = SCREEN_WIDTH;
  bgBox.h = SCREEN_HEIGHT;
  gameWin = false;
  gameLost = false;
  gameShowInfo = false;
  infoTimer.stop( );
  
  // initial player position
  playerVelX = 0;
  playerVelY = 0;
  playerX = 10;
  playerY = worldHeight*TILE_HEIGHT-150;
  playerSprite = 1;
  playerMovement = PM_STILL_RIGHT;
  playerAnimationTimer.start( );
  playerFalling = false;
  playerJumping = false;
  playerInvincible = false;
  playerJumpStartY = 0;
  playerHealth = 0;
  playerScore = 0;
  
  // initialize graphics
  image = load_image( "gfx.png" );
  background = load_image( "background.png" );
  menu = load_image( "menu.png" );

  // initialize music and sounds
  music = Mix_LoadMUS( "LD20.aif" );
  sound1 = Mix_LoadWAV( "jump.wav" );
  sound2 = Mix_LoadWAV( "achieve.wav" );
  sound3 = Mix_LoadWAV( "hurt.wav" );
  sound4 = Mix_LoadWAV( "collect.wav" );
  sound5 = Mix_LoadWAV( "shoot.wav" );
  sound6 = Mix_LoadWAV( "explode.wav" );
  
  // initialize font
  font = TTF_OpenFont( "font.ttf", 12 );
  
  // initialize graphic rects of player
  playerRect[ 0 ] = helper_get_rect_for_image( 2 ); // standing looking left
  playerRect[ 1 ] = helper_get_rect_for_image( 3 ); // standing looking right
  playerRect[ 2 ] = helper_get_rect_for_image( 4 ); // walk right 1
  playerRect[ 3 ] = helper_get_rect_for_image( 5 ); // walk right 2
  playerRect[ 4 ] = helper_get_rect_for_image( 6 ); // walk right 3
  playerRect[ 5 ] = helper_get_rect_for_image( 7 ); // walk left 1
  playerRect[ 6 ] = helper_get_rect_for_image( 8 ); // walk left 2
  playerRect[ 7 ] = helper_get_rect_for_image( 9 ); // walk left 3
  playerRect[ 8 ] = helper_get_rect_for_image( 27 ); // invincible blinker
  
  // initialize world rects
  worldTiles[ 0 ] = helper_get_rect_for_image( 0 ); // first floor tile
  worldTiles[ 1 ] = helper_get_rect_for_image( 1 ); // second floor tile
  worldTiles[ 2 ] = helper_get_rect_for_image( 10 ); // first ground tile
  worldTiles[ 3 ] = helper_get_rect_for_image( 11 ); // second ground tile
  worldTiles[ 4 ] = helper_get_rect_for_image( 12 ); // sky tile
  worldTiles[ 5 ] = helper_get_rect_for_image( 13 ); // grey sky tile
  worldTiles[ 6 ] = helper_get_rect_for_image( 14 ); // platform 1
  worldTiles[ 7 ] = helper_get_rect_for_image( 15 ); // platform 2
  worldTiles[ 8 ] = helper_get_rect_for_image( 16 ); // spikes
  worldTiles[ 9 ] = helper_get_rect_for_image( 17 ); // good robeman left
  worldTiles[ 10 ] = helper_get_rect_for_image( 18 ); // good robeman right
  worldTiles[ 11 ] = helper_get_rect_for_image( 19 ); // evil robeman left
  worldTiles[ 12 ] = helper_get_rect_for_image( 20 ); // evil robeman right
  worldTiles[ 13 ] = helper_get_rect_for_image( 21 ); // collectible nun
  worldTiles[ 14 ] = helper_get_rect_for_image( 22 ); // evil bat particle 1
  worldTiles[ 15 ] = helper_get_rect_for_image( 23 ); // evil bat particle 2
  worldTiles[ 16 ] = helper_get_rect_for_image( 24 ); // player bullet
  worldTiles[ 17 ] = helper_get_rect_for_image( 25 ); // enemy bullet
  worldTiles[ 18 ] = helper_get_rect_for_image( 26 ); // grass tile
  worldTiles[ 19 ] = helper_get_rect_for_image( 27 ); // clear tile
  worldTiles[ 20 ] = helper_get_rect_for_image( 28 ); // leave particle
  worldTiles[ 21 ] = helper_get_rect_for_image( 29 ); // smoke particle 1
  worldTiles[ 22 ] = helper_get_rect_for_image( 30 ); // smoke particle 2
  worldTiles[ 23 ] = helper_get_rect_for_image( 31 ); // smoke particle 3
  worldTiles[ 24 ] = helper_get_rect_for_image( 32 ); // smoke particle 4
  worldTiles[ 25 ] = helper_get_rect_for_image( 33 ); // shelter light world
  worldTiles[ 26 ] = helper_get_rect_for_image( 34 ); // shelter dark world activated
  worldTiles[ 27 ] = helper_get_rect_for_image( 35 ); // shelter dark world deactivated
  worldTiles[ 28 ] = helper_get_rect_for_image( 36 ); // heart (to collect and as stat)
  
  buildWorldMap( );
  
  playerY = worldHeight*TILE_HEIGHT-150;
  
  // spawn robeman
  robemen.clear( );
  robemen.push_back( Robeman( worldWidth*TILE_WIDTH, worldHeight*TILE_HEIGHT-TILE_HEIGHT*2 ) );
  
  // debug
  frame = 0;
  fps.start( );
  frameRate = 0;
  draws = 0;
  
  Mix_PlayMusic( music, -1 );
}

void Game::destroy( ) {
  
  delete[] worldMap;
  
  TTF_CloseFont( font );
  
  Mix_FreeChunk( sound1 );
  Mix_FreeChunk( sound2 );
  Mix_FreeChunk( sound3 );
  Mix_FreeChunk( sound4 );
  Mix_FreeChunk( sound5 );
  Mix_FreeChunk( sound6 );
  Mix_FreeMusic( music );
  
  SDL_FreeSurface( image );
  SDL_FreeSurface( background );
  SDL_FreeSurface( menu );
}

void Game::buildWorldMap( ) {
  
  int myRandom;
  int cellularAutomataRuns; // floor growing runs, the bigger, the easier the game
  int growRandom;           // percentage for a floor to grow within the cellular automata run, bigger means longer islands -> easier game
  int floorRandom;          // percentage of initial chance to make a ground floor, the higher the easier the game
  int spikeRandom;          // percentage of chance of spike, the higher, the harder the game
  int grassRandom;          // percentage of chance of grass tile, the higher the wilder the look - indicator for game mode
  int heartRandom;          // permille of change of floating heart collectible for health
    
  myRandom = 0;
  switch( gameDifficulty ) {
    
    case 1:
      cellularAutomataRuns = 4;
      growRandom   = 50; 
      floorRandom  = 4; 
      spikeRandom  = 15; 
      grassRandom  = 50;
      playerHealth = 10;
      worldWidth   = 20;
      worldHeight  = 50;
      heartRandom  = 1;
      break;
      
    case 2:
      cellularAutomataRuns = 3;
      growRandom   = 40; 
      floorRandom  = 3; 
      spikeRandom  = 30; 
      grassRandom  = 60;
      playerHealth = 10;
      worldWidth   = 30;
      worldHeight  = 100;
      heartRandom  = 1;
      break;
      
    case 3:
      cellularAutomataRuns = 3;
      growRandom   = 35; 
      floorRandom  = 3; 
      spikeRandom  = 40; 
      grassRandom  = 70;
      playerHealth = 5;
      worldWidth   = 35;
      worldHeight  = 200;
      heartRandom  = 1;
      break;
      
    case 4:
      cellularAutomataRuns = 2;
      growRandom   = 40; 
      floorRandom  = 3; 
      spikeRandom  = 45; 
      grassRandom  = 80;
      playerHealth = 3;
      worldWidth   = 40;
      worldHeight  = 350;
      heartRandom  = 1;
      break;
      
    case 5:
    default:
      cellularAutomataRuns = 1;
      growRandom   = 30; 
      floorRandom  = 5; 
      spikeRandom  = 50; 
      grassRandom  = 90;
      playerHealth = 2;
      worldWidth   = 50;
      worldHeight  = 500;
      heartRandom  = 1;
      break;
  }
  
  // handle world map pointer
  if( worldMap != NULL ) delete [] worldMap;
  
  worldMap = new int [ worldWidth * worldHeight ];
  
  // initialize with 0 values
  for( int i = 0; i < worldHeight * worldWidth; i++ ) {
    worldMap[ i ] = 19; // clear tile
  }
  
  // paste ground at the bottom
  for( int i = (worldHeight-2)*worldWidth; i < worldHeight * worldWidth; i++ ) {
    
    if( i < (worldHeight-1)*worldWidth ) {
      if( i % 2 == 0 ) worldMap[ i ] = 0; // first floor tile
      else worldMap[ i ] = 1; // second floor tile
    } else {
      if( i % 2 == 0 ) worldMap[ i ] = 2; // first ground tile
      else worldMap[ i ] = 3; // second ground tile
    }
  }
  
  // let's start with random creation of floor tiles
  for( int i = 0; i < worldHeight*worldWidth; i++ ) {
    if( worldMap[ i ] == 19 && rand( )%100 < floorRandom ) {
      
      if( i > worldWidth && worldMap[ i-worldWidth ] == 19 ) worldMap[ i ] = 6;
      if( i < worldHeight*worldWidth-worldWidth && worldMap[ i+worldWidth ] == 19 ) worldMap[ i ] = 6;
    }
  }
  
  // grow floors
  for( int j = 0; j < cellularAutomataRuns; j++ ) {
    for( int i = 0; i < worldHeight*worldWidth; i++ ) {
      
      // cellular automata like
      if( worldMap[ i ] == 6 ) {
        myRandom = rand( ) % 100;
        if( myRandom < growRandom ) {
          if( myRandom % 2 == 0 ) {
            worldMap[ i-1 ] = 6;
          } else {
            worldMap[ i+1 ] = 6;
          }
        }
      }
    }
  }
  
  // create overlays, spikes and grass tiles
  for( int i = 0; i < worldHeight*worldWidth; i++ ) {
    
    if( i > worldWidth ) {
      
      if( worldMap[ i ] == 6 && rand( )%100 < spikeRandom ) {
        worldMap[ i - worldWidth ] = 8; // spike
      } else if( worldMap[ i ] == 6 && rand( )%100 < grassRandom ) {
        worldMap[ i -worldWidth ] = 18; // grass
      }
    }
    
    if( worldMap[ i ] == 19 && rand( ) % 1000 < heartRandom ) worldMap[ i ] = 28;
  }
  
  // cleanup
  // no spike or grass over spike or grass
  for( int i = 0; i < worldHeight * worldWidth; i++ ) {
  
    if( i > worldWidth && i < worldHeight*worldWidth-2*worldWidth ) {
      if( worldMap[ i ] == 8 || worldMap[ i ] == 18 ) {
        if( worldMap[ i + worldWidth ] == 8 || worldMap[ i + worldWidth ] == 18 ) {
          worldMap[ i ] = 19; // clear tile
        }
      }
    }
  }
  
  // place win condition at top rightest corner
  int j = 2;
  int i = worldWidth;
  bool set = false;
  do {
    if( worldMap[ (i*j)-1 ] == 6 && worldMap[ ((i*j)-1)-worldWidth ] == 19 ) {
      worldMap[ ((i*j)-1)-worldWidth ] = 25;
      set = true;
    }
    i--;
    
    if( i < 0 ) {
      i = worldWidth;
      j++;
    }
  } while( !set );
  
}

void Game::switchDarkWorld( ) {
  
  gameShowInfo = true;
  infoTimer.start( );
  
  int maxNuns; // max amount of nuns to spawn / collect
  
  switch( gameDifficulty ) {
      
    case 1:
      maxNuns = 10;
      break;
      
    case 2:
      maxNuns = 20;
      break;
      
    case 3:
      maxNuns = 40;
      break;
      
    case 4:
      maxNuns = 50;
      break;
      
    case 5:
    default:
      maxNuns = 80;
      break;
  }
  
  
  Mix_PlayChannel( -1, sound2, 0 );
  gameIsLight = false;

  // change background
  SDL_FreeSurface( background );
  background = load_image( "background2.png" );
  
  // switch platform tiles
  for( int i = 0; i < worldWidth*worldHeight; i++ ) {
    if( worldMap[ i ] == 6 ) worldMap[ i ] = 7;
    if( worldMap[ i ] == 25 ) worldMap[ i ] = 27; // deactivated shelter, collect nuns first
  }
  
  // spawn nuns
  int itterations = 0;
  while( collectibleNuns < maxNuns && itterations < 1000 ) {
    for( int i = 0; i < worldWidth*worldHeight; i++ ) {
     
      // spawn nuns
      if( i > worldWidth ) {
        if( worldMap[ i ] == 7 ) {
          if( worldMap[ i-worldWidth ] == 19 ) {
            if( rand( ) % 10000 < 10 && collectibleNuns < maxNuns ) { 
              worldMap[ i-worldWidth ] = 13;
              collectibleNuns++;
            }
          }
        }
      }
    }
    itterations++;
  }
  
  // remove the light robe
  robemen.clear( );
}

void Game::run( ) {

  Timer delta;
  delta.start( );
  Mix_PlayMusic( music, -1 );
  
  while( !quit ) {
    
    drawTimer.start( );
    
    handleEvent();
    if( !paused && running ) update( delta.getTicks() );
    
    delta.start( );
    draw( );
    
    frame++;
  }
}

void Game::handleEvent( ) {
    
  // handle events
  while( SDL_PollEvent( &event ) ) {
    
    // X out of application
    if( event.type == SDL_QUIT ) {
      quit = true;
    }
    
    if( event.type == SDL_KEYDOWN ) { // key is pressed
      eventCounter++;
      // any key
      if( !running && !gameMenu ) initialize( );
      
      switch( event.key.keysym.sym ) {
          
        case SDLK_F1:
          if( Mix_PausedMusic( ) ) Mix_ResumeMusic( );
          else Mix_PauseMusic( );
          break;
          
        case SDLK_a:
          shoot( );
          break;
          
        case SDLK_q:
          if( !gameMenu ) initialize( );
          break;
          
        case SDLK_p:
          paused = !paused;
          break;
          
        case SDLK_RETURN:
          if( gameMenu ) {
            gameMenu = false;
            running = true;
            destroy( );
            initialize( selectedMenu+1 );
          }
          break;
          
        case SDLK_ESCAPE:
          if( gameMenu ) quit = true;
          else showMenu( );
          break;
          
        case SDLK_UP:
          
          if( gameMenu ) {
            selectedMenu--;
          }
          
          if( paused ) continue;
          
          if( running && !playerFalling && !playerJumping ) {
            Mix_PlayChannel( -1, sound1, 0 );
            playerJumping = true;
            playerJumpStartY = playerY;
          }
          break;
          
        case SDLK_DOWN:
          if( gameMenu ) {
            selectedMenu++;
          }
          //switchDarkWorld( );
          break;
        
        case SDLK_RIGHT:
          if( paused ) continue;
          
          playerVelX += PLAYER_SPEED;
          playerMovement = PM_RIGHT;
          break;
          
        case SDLK_LEFT:
          if( paused ) continue;
          
          playerVelX -= PLAYER_SPEED;
          playerMovement = PM_LEFT;
          break;
      }
    } else if( event.type == SDL_KEYUP ) { // key is released
      eventCounter--;
      
      switch( event.key.keysym.sym ) {
          
        case SDLK_UP:
          playerJumping = false;
          break;
          
        case SDLK_RIGHT:
          playerVelX -= PLAYER_SPEED;
          playerMovement = PM_STILL_RIGHT;
          break;
          
        case SDLK_LEFT:
          playerVelX += PLAYER_SPEED;
          playerMovement = PM_STILL_LEFT;
          break;
      }
    } else if( event.type == SDL_MOUSEBUTTONDOWN ) { // mouse is clicked
      
      //int x = event.button.x;
      //int y = event.button.y;
      
      //int tileClicked = ((x+camera.x)/TILE_WIDTH)+(((y+camera.y)/TILE_HEIGHT)*worldWidth);
      //std::cerr << "Raw click at: " << x << " " << y << " that is: " << ((x+camera.x)/TILE_WIDTH) << " " << ((y+camera.y)/TILE_HEIGHT) << std::endl;
      //std::cerr << "Clicked Tile No: " << tileClicked << std::endl;
      //worldMap[ tileClicked ] = 4;
    } else if( event.type == SDL_MOUSEBUTTONUP ) { // mouse is released
      
    }
  }
}

void Game::shoot( ) {
  Particle myBullet( playerX+TILE_WIDTH/2, playerY );
  myBullet.image = 16;
  
  if( playerMovement == PM_LEFT || playerMovement == PM_STILL_LEFT ) myBullet.vX = -PLAYER_BULLET_SPEED;
  else if( playerMovement == PM_RIGHT || playerMovement == PM_STILL_RIGHT ) myBullet.vX = PLAYER_BULLET_SPEED;
    
  bullets.push_back( myBullet );
  Mix_PlayChannel( -1, sound5, 0 );
}

void Game::update( Uint32 delta ) {
  float ticksInSeconds = ( delta / 1000.f );
    
  updateParticles( ticksInSeconds );
  updateRobemen( ticksInSeconds );
  updatePlayerMovement( ticksInSeconds );
  updatePlayerAnimation( ticksInSeconds );
  updatePlayerBullets( ticksInSeconds );
  updateCamera( ticksInSeconds );    
  
  // update message
  if( gameShowInfo && infoTimer.getTicks( ) > 3000 ) gameShowInfo = false;
}

void Game::updateParticles( float ticks ) {
  
  // randomly spawn particles
  // bat particles
  if( rand( ) % 100 < 1 ) {
    
    Particle myParticle( rand( ) % worldWidth*TILE_WIDTH, playerY - rand( ) % 100 );
    myParticle.image = 14;
    
    if( myParticle.x > playerX ) myParticle.vX = rand( ) % PARTICLE_SPEED;
    else myParticle.vX = -( rand( ) % PARTICLE_SPEED );
    
    if( rand( ) % 2 == 0 ) myParticle.vY = rand( ) % PARTICLE_SPEED;
    else myParticle.vY = -( rand( ) % PARTICLE_SPEED );
    
    // security
    while( myParticle.vX < 50 && myParticle.vX > 50 ) myParticle.vX = rand( ) % PARTICLE_SPEED;
    while( myParticle.vY < 50 && myParticle.vY > 50 ) myParticle.vY = rand( ) % PARTICLE_SPEED;
    
    myParticle.spriteTimer.start( );
    
    particles.push_back( myParticle );
    
    // also smoke particles at creation point
    Particle smokeParticle( myParticle.x, myParticle.y );
    smokeParticle.vX = 0;
    smokeParticle.vY = 0;
    smokeParticle.image = 21;
    smokeParticle.spriteTimer.start( );
    
    particles.push_back( smokeParticle );
  }
  
  // leave particles
  int leaveRandom = 1;
  if( !gameIsLight ) leaveRandom = 3;
  if( rand( ) % 100 < leaveRandom ) {
    Particle myParticle( camera.x, camera.y );
    myParticle.image = 20;
    myParticle.vX = rand( ) % PARTICLE_SPEED;
    if( !gameIsLight ) myParticle.vX += rand( ) % 200;
    myParticle.vY = PARTICLE_SPEED;
    
    particles.push_back( myParticle );
  }
  
  // handle proper movement
  // handle proper animation - 14 - 15 for bats
  for( int i = 0; i < (int)particles.size( ); i++ ) {
    particles[ i ].x += particles[ i ].vX * ticks;
    particles[ i ].y += particles[ i ].vY * ticks;
    
    if( particles[ i ].spriteTimer.isStarted( ) && particles[ i ].spriteTimer.getTicks( ) > PLAYER_ANIMATION_TIME ) {
      // bat particle
      if( particles[ i ].image == 14 ) particles[ i ].image = 15;
      else if( particles[ i ].image == 15 ) particles[ i ].image = 14;
      
      // smoke particle
      if( particles[ i ].image >= 21 && particles[ i ].image <= 24 ) {
       
        particles[ i ].image++;
        if( particles[ i ].image > 24 ) {
          particles.erase( particles.begin( ) + i );
          continue;
        }
      }
      
      particles[ i ].spriteTimer.start( );
    }
    
    // handle removal
    if( particles[ i ].x < 0 || particles[ i ].x > worldWidth*TILE_WIDTH ) {
      particles.erase( particles.begin( ) + i );
    }
  }
}

void Game::updateRobemen( float ticks ) {
  
  if( gameIsLight ) { // just my single light robe man
    // follow player
    if( robemen[ 0 ].x > playerX ) {
      robemen[ 0 ].x -= ENEMY_SPEED * ticks;
      robemen[ 0 ].image = 9;
    } else if( robemen[ 0 ].x < playerX ) {
      robemen[ 0 ].x += ENEMY_SPEED * ticks;
      robemen[ 0 ].image = 10;
    }
    
    if( robemen[ 0 ].y > playerY ) robemen[ 0 ].y -= ENEMY_SPEED * ticks;
    else if( robemen[ 0 ].y < playerY ) robemen[ 0 ].y += ENEMY_SPEED * ticks;
    
    // check for collision to turn to dark world
    int ax, bx, ay, by;
    ax = abs( playerX-robemen[ 0 ].x );
    bx = abs( (playerX+TILE_WIDTH) - (robemen[ 0 ].x+TILE_WIDTH) );
    ay = abs( playerY-robemen[ 0 ].y );
    by = abs( (playerY+TILE_HEIGHT) - (robemen[ 0 ].y+TILE_HEIGHT) );
    
    // TILE_WIDTH/2 tolerance
    int tolerance = TILE_WIDTH / 2;
    if( ax < tolerance && ay < tolerance ) switchDarkWorld( );
    if( bx < tolerance && by < tolerance ) switchDarkWorld( );
    
  } else { // dark robemen
    
    // remove shot robemen
    int bx, by, ra, rb, rc, rd;
    for( int i = 0; i < (int)bullets.size( ); i++ ) {
      bx = bullets[ i ].x+16; 
      by = bullets[ i ].y+16;
      
      for( int j = 0; j < (int)robemen.size( ); j++ ) {
        ra = robemen[ j ].x;
        rb = robemen[ j ].y;
        rc = robemen[ j ].x+TILE_WIDTH;
        rd = robemen[ j ].y+TILE_HEIGHT;
        
        if( ra > bx ) continue;
        if( rb > by ) continue;
        if( rc < bx ) continue;
        if( rd < by ) continue;
        
        // else hit
        robemen.erase( robemen.begin( ) + i );
        bullets.erase( bullets.begin( ) + i );
        Mix_PlayChannel( -1, sound6, 0 );
        break;
      }
    }
    
    // spawn robemen randomly
    int myRandom = rand( ) % 1000;
    if( robemen.size( ) < (5*gameDifficulty) && myRandom < 1 ) { // % chance of spawning a robeman
      
      Robeman myRobeman( camera.x, camera.y+SCREEN_HEIGHT-rand( ) % SCREEN_HEIGHT/2 ); // always spawn from under the player
      if( myRandom % 2 == 0 ) myRobeman.x += SCREEN_WIDTH; // either left or right spawning
      myRobeman.image = 11;
      
      robemen.push_back( myRobeman );
      
    }
    
    // update robemen movement
    for( int i = 0; i < (int)robemen.size( ); i++ ) {
     
      // follow player
      if( robemen[ i ].x > playerX ) {
        robemen[ i ].x -= ENEMY_SPEED * ticks;
        robemen[ i ].image = 11;
      } else if( robemen[ 0 ].x < playerX ) {
        robemen[ i ].x += ENEMY_SPEED * ticks;
        robemen[ i ].image = 12;
      }
      
      if( robemen[ i ].y > playerY ) robemen[ i ].y -= ENEMY_SPEED * ticks;
      else if( robemen[ i ].y < playerY ) robemen[ i ].y += ENEMY_SPEED * ticks;
    }
    
    
    // spawn robemen bullets
  }
}

void Game::updatePlayerMovement( float ticks ) {

  // reset everything
  if( !playerJumping && eventCounter == 0 ) {
    playerVelX = 0;
    playerVelY = 0;
  }
  
  // check player states

  // correct fall speed
  if( playerVelY > PLAYER_GRAVITATION ) {
    playerJumping = false;
    playerVelY = PLAYER_GRAVITATION;
  }

  if( playerJumping && playerJumpStartY-playerY < PLAYER_JUMP_DIST ) {
    playerVelY += -PLAYER_JUMP_SPEED;
  } else {
    // apply gravitation
    playerVelY += PLAYER_GRAVITATION;
    playerJumping = false;
  }
  
  if( !playerFalling && !playerJumping ) playerVelY = 0; // fallback
  
  // falling
  int bottomTileLeft = (((playerX+8)/TILE_WIDTH))+(((playerY+TILE_HEIGHT)/TILE_HEIGHT)*worldWidth);
  int bottomTileRight = (((playerX+TILE_WIDTH-8)/TILE_WIDTH))+(((playerY+TILE_HEIGHT)/TILE_HEIGHT)*worldWidth);
  if( ( worldMap[ bottomTileLeft ] != 0 && worldMap[ bottomTileLeft ] != 1 && worldMap[ bottomTileLeft ] != 6 && worldMap[ bottomTileLeft ] != 7  ) && ( worldMap[ bottomTileRight ] != 0 && worldMap[ bottomTileRight ] != 1 && worldMap[ bottomTileRight ] != 6 && worldMap[ bottomTileRight ] != 7 ) ) {
    playerFalling = true;
  } else {
    playerFalling = false;
    if( worldMap[ bottomTileLeft ] == 0 || worldMap[ bottomTileLeft ] == 1 || worldMap[ bottomTileLeft ] == 6 || worldMap[ bottomTileLeft ] == 7 ) playerY = ((bottomTileLeft/worldWidth)*TILE_HEIGHT)-TILE_HEIGHT; // align player on top of tile
    else if( worldMap[ bottomTileRight ] == 0 || worldMap[ bottomTileRight ] == 1 || worldMap[ bottomTileRight ] == 6 || worldMap[ bottomTileRight ] == 7 ) playerY = ((bottomTileRight/worldWidth)*TILE_HEIGHT)-TILE_HEIGHT; // align player on top of tile
  }
  
  //
  // player movement
  //
  playerX += playerVelX * ticks;
  playerY += playerVelY * ticks;
  
  // special tile collisions
  int tileLeft = (((playerX+8)/TILE_WIDTH))+(((playerY)/TILE_HEIGHT)*worldWidth);
  int tileRight = (((playerX+TILE_WIDTH-8)/TILE_WIDTH))+(((playerY)/TILE_HEIGHT)*worldWidth);  
  
  // check collision with tiles around yourself    
  if( !playerInvincible && !playerFalling && !playerJumping && ( worldMap[ tileLeft ] == 8 || worldMap[ tileRight ] == 8 ) ) { // hit by spike
    playerInvincibleTimer.start( );
    playerInvincible = true;
    Mix_PlayChannel( -1, sound3, 0 );
    playerHealth--;
  }
    
  if( worldMap[ tileLeft ] == 13 ) { // nun collected
    Mix_PlayChannel( -1, sound4, 0 );
    playerScore++;
    worldMap[ tileLeft ] = 19;
    if( playerScore == collectibleNuns ) {
      for( int i = 0; i < worldWidth*worldHeight; i++ ) {
        if( worldMap[ i ] == 27 ) {
          worldMap[ i ] = 26;
          break;
        }
      }
    }
  }
  if( worldMap[ tileRight ] == 13 ) { // nun collected
    Mix_PlayChannel( -1, sound4, 0 );
    playerScore++;
    worldMap[ tileRight ] = 19;
    if( playerScore == collectibleNuns ) {
      for( int i = 0; i < worldWidth*worldHeight; i++ ) {
        if( worldMap[ i ] == 27 ) {
          worldMap[ i ] = 26;
          break;
        }
      }
    }
  }
  
  if( worldMap[ tileLeft ] == 26 || worldMap[ tileRight ] == 26 || worldMap[ tileLeft ] == 25 || worldMap[ tileRight ] == 25 ) gameWin = true; 
  
  if( worldMap[ tileLeft ] == 28 ) { // heart collected
    Mix_PlayChannel( -1, sound4, 0 );
    playerHealth++;
    worldMap[ tileLeft ] = 19;
  }
  if( worldMap[ tileRight ] == 28 ) { // heart collected
    Mix_PlayChannel( -1, sound4, 0 );
    playerHealth++;
    worldMap[ tileRight ] = 19;
  }

  // max value for health
  if( playerHealth > PLAYER_MAX_HEALTH ) playerHealth = PLAYER_MAX_HEALTH;
  else if( playerHealth < 0 ) playerHealth = 0;
  
  if( playerHealth == 0 ) gameLost = true;
  
  if( playerInvincibleTimer.getTicks( ) > 3000 ) {
    playerInvincible = false;
    playerInvincibleTimer.stop( );
  }
  
  // check collision with robemen
  int ra, rb, rc, rd, pa, pb, pc, pd;
  pa = playerX;
  pb = playerY;
  pc = playerX+TILE_WIDTH;
  pd = playerY+TILE_HEIGHT;
  if( !playerInvincible ) {
    
    for( int i = 0; i < (int)robemen.size( ); i++ ) {
      ra = robemen[ i ].x;
      rb = robemen[ i ].y;
      rc = robemen[ i ].x+TILE_WIDTH;
      rd = robemen[ i ].y+TILE_HEIGHT;
      
      if( ra > pc ) continue;
      if( rb > pd ) continue;
      if( rc < pa ) continue;
      if( rd < pb ) continue;
      
      // robeman hit player
      playerInvincibleTimer.start( );
      playerInvincible = true;
      Mix_PlayChannel( -1, sound3, 0 );
      playerHealth--;
      break;
    } 
  }
  
  // check level borders collision
  if( playerX > worldWidth*TILE_WIDTH - TILE_WIDTH ) {
    playerX = worldWidth*TILE_WIDTH - TILE_WIDTH;
  } else if( playerX < 0 ) {
    playerX = 0;
  }
  
  if( playerY > worldHeight*TILE_HEIGHT - TILE_HEIGHT ) {
    playerY = worldHeight*TILE_HEIGHT - TILE_HEIGHT;
  } else if( playerY < 0 ) {
    playerY = 0;
  }
  
  if( gameLost || gameWin ) running = false;
}

void Game::updatePlayerAnimation( float ticks ) {
  if( playerAnimationTimer.getTicks( ) >= PLAYER_ANIMATION_TIME ) {
    
    // check for correct animation for special cases: jump, multiple keys pressed
    if( playerJumping ) {
      if( playerVelX > 0 )      playerMovement = PM_JUMP_RIGHT;
      else if( playerVelX < 0 ) playerMovement = PM_JUMP_LEFT;
      else                      playerMovement = PM_STILL_RIGHT;
    }
    
    playerAnimationTimer.start( ); // restart timer
    
    switch( playerMovement ) {
      case PM_LEFT:
        playerSprite++;
        if( playerSprite < 5 || playerSprite > 7 ) playerSprite = 5; // beginning animation
        break;
        
      case PM_RIGHT:
        playerSprite++;
        if( playerSprite < 2 || playerSprite > 4 ) playerSprite = 2; // beginning animation 
        break;
        
      case PM_STILL_LEFT:
        playerSprite = 0;
        break;
        
      case PM_STILL_RIGHT:
        playerSprite = 1;
        break;
        
      case PM_JUMP_RIGHT:
        playerSprite = 3;
        break;
        
      case PM_JUMP_LEFT:
        playerSprite = 6;
        break;
    }
    
    if( playerInvincible ) {
      if( playerInvincibleTimer.getTicks( )%2 == 0 ) {
        playerSprite = 8;
      }
    }
  }
}

void Game::updatePlayerBullets( float ticks ) {
  
  for( int i = 0; i < (int)bullets.size( ); i++ ) {
    bullets[ i ].x += bullets[ i ].vX * ticks;
    //bullets[ i ].y += bullets[ i ].vY;
    
    if( bullets[ i ].x < 0 || bullets[ i ].x > worldWidth*TILE_WIDTH ) {
      bullets.erase( bullets.begin( ) + i );
    }
  }
}

void Game::updateCamera( float ticks ) {
  // camera should center player on screen
  camera.x = ( playerX + TILE_WIDTH/2 ) - SCREEN_WIDTH / 2;
  camera.y = ( playerY + TILE_HEIGHT/2 ) - SCREEN_HEIGHT / 2;
  
  if( camera.x < 0 ) camera.x = 0;
  if( camera.y < 0 ) camera.y = 0;
  if( camera.x > worldWidth*TILE_WIDTH - camera.w ) camera.x = worldWidth*TILE_WIDTH - camera.w;
  if( camera.y > worldHeight*TILE_HEIGHT - camera.h ) camera.y = worldHeight*TILE_HEIGHT - camera.h;
  
  // also we could update the background scrolling here
  bgBox.y = playerY/((worldHeight*TILE_HEIGHT)/480);
}

void Game::draw( ) {
  
  // clear screen
  SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0x00, 0x00, 0x00 ) );
  

  // draw each entity in proper order
  if( gameMenu ) {
    
    drawMenu( );
  } else {
    
    drawWorld( );
    drawParticles( );
    drawPlayer( );
    drawRobemen( );
    //drawDebug( );
    drawStats( );
    
    if( gameLost ) drawMessage( 1 ); // game lost
    else if( gameWin ) drawMessage( 2 ); // game won 
    if( gameShowInfo ) drawMessage( 3 ); // dangerous message
  }
  
  
  // final drawing
  SDL_Flip( screen );
}

void Game::drawPlayer( ) {
  
  drawThat( playerX, playerY, image, &playerRect[ playerSprite ] );
}

void Game::drawWorld( ) {
  
  int drawPosX = 0;
  int drawPosY = 0;
  
  // draw background image
  drawThat( 0+camera.x, 0+camera.y, background, &bgBox );
  
  // initial sky box
  for( int i = 0; i < worldHeight * worldWidth; i++ ) {
    
    // tiles
    //drawThat( drawPosX, drawPosY, image, &worldTiles[ worldBackground[ i ] ] );
    drawThat( drawPosX, drawPosY, image, &worldTiles[ worldMap[ i ] ] );
    
    drawPosX += TILE_WIDTH;
    if( drawPosX >= TILE_WIDTH * worldWidth ) {
      drawPosX = 0;
      drawPosY += TILE_HEIGHT;
    }
  }
}

void Game::drawParticles( ) {
  
  // world particles
  for( int i = 0; i < (int)particles.size( ); i++ ) {
    drawThat( particles[ i ].x, particles[ i ].y, image, &worldTiles[ particles[ i ].image ] );
  }
  
  // player bullets
  for( int i = 0; i < (int)bullets.size( ); i++ ) {
    drawThat( bullets[ i ].x, bullets[ i ].y, image, &worldTiles[ bullets[ i ].image ] );
  }
}

void Game::drawRobemen( ) {
  
  for( int i = 0; i < (int)robemen.size( ); i++ ) {
    drawThat( robemen[ i ].x, robemen[ i ].y, image, &worldTiles[ robemen[ i ].image ] );
  }
}

void Game::drawDebug( ) {
  
  if( fps.getTicks( ) >= 1000 ) {
    frameRate = frame / ( fps.getTicks() / 1000.f );
    fps.start( );
    frame = 0;
  }
  SDL_Surface *message;
    
  std::stringstream ss;
  ss << frameRate;
  SDL_Color textColor = { 0, 0, 0 };
  std::string statString( "FPS: " );
  statString += ss.str( );
  message = TTF_RenderText_Solid( font, statString.c_str( ), textColor );
  drawThat( 2+camera.x, 2+camera.y, message, NULL );
  SDL_FreeSurface( message );
  
  if( drawTimer.getTicks( ) >= 1000 ) {
    draws = 0;
    drawTimer.start( );
  }
  
  ss << draws;
  statString = "DrawsPS: ";
  statString += ss.str( );
  message = TTF_RenderText_Solid( font, statString.c_str( ), textColor );
  drawThat( 2+camera.x, 12+camera.y, message, NULL );
  SDL_FreeSurface( message );
}

void Game::drawStats( ) {
  
  SDL_Surface *message;
  
  if( !gameIsLight ) {
   
    std::stringstream ss;
    ss << playerScore;
    SDL_Color textColor = { 0xFF, 0xFF, 0xFF };
    std::string statString( "Nuns: " );
    statString += ss.str( );
    statString += " / ";
    ss.str( std::string( ) );
    ss << collectibleNuns;
    statString += ss.str( );
    message = TTF_RenderText_Solid( font, statString.c_str( ), textColor );
    drawThat( (SCREEN_WIDTH-100)+camera.x, 2+camera.y, message, NULL );
    SDL_FreeSurface( message );
  }
  
  int offsetX = 2;
  int offsetY = 2;
  
  for( int i = 0; i < playerHealth; i++ ) {
    drawThat( offsetX+camera.x, offsetY+camera.y, image, &worldTiles[ 28 ] );
    offsetX += 17;
    if( i == 9 ) {
      offsetX = 2;
      offsetY = 19;
    }
  }
}

void Game::drawMessage( int msg ) {
  
  if( msg == 1 ) { // lost
    SDL_Rect myMessage = helper_get_rect_for_images(50, 6, 1);
    drawThat( SCREEN_WIDTH/2-TILE_WIDTH*3 + camera.x, SCREEN_HEIGHT / 2 + camera.y, image, &myMessage );
  } else if( msg == 2 ) { // win
    SDL_Rect myMessage = helper_get_rect_for_images(60, 6, 1);
    drawThat( SCREEN_WIDTH/2-TILE_WIDTH*3 + camera.x, SCREEN_HEIGHT / 2 + camera.y, image, &myMessage );
  } else if( msg == 3 ) { // dangerous message
    SDL_Rect myMessage = helper_get_rect_for_images(80, 6, 2);
    drawThat( SCREEN_WIDTH/2-TILE_WIDTH*3 + camera.x, 200 + camera.y, image, &myMessage );
  }
}

void Game::drawMenu( ) {
  
  if( selectedMenu < 0 ) selectedMenu = 0;
  if( selectedMenu > 4 ) selectedMenu = 4;
  
  // draw menu background
  SDL_Rect menuRect;
  menuRect.x = 0;
  menuRect.y = 0;
  menuRect.w = SCREEN_WIDTH;
  menuRect.h = SCREEN_HEIGHT;
  drawThat( 0+camera.x, 0+camera.y, menu, &menuRect );
  
  // get accessed difficulty from file
  int availableDiff = 5;
  
  int offsetX = SCREEN_WIDTH/2-50;
  int offsetY = 200;
  for( int i = 0; i < availableDiff; i++ ) {
    
    SDL_Rect myButton = helper_get_rect_for_images( 56+i*10, 3, 1 );
    drawThat( offsetX+camera.x, offsetY+camera.y, image, &myButton );
    
    if( i == selectedMenu ) {
      SDL_Rect mySelection = helper_get_rect_for_images( 73, 3, 1 );
      drawThat( offsetX+camera.x, offsetY+camera.y, image, &mySelection );
    }
    
    offsetY += TILE_WIDTH;
  }
}

void Game::showMenu( ) {
   
  // just initialize for once
  initialize( 1 );
  running = false;
  gameMenu = true;
  selectedMenu = 0;
  
  Mix_HaltMusic( );
  Mix_HaltMusic( );
}

SDL_Surface *Game::load_image( std::string filename ) {
  
  SDL_Surface *myImage = NULL;
  SDL_Surface *returnImage = NULL;
  
  myImage = IMG_Load( filename.c_str( ) );
  if( myImage != NULL ) {
    
    returnImage = SDL_DisplayFormat( myImage );
    SDL_FreeSurface( myImage );
    
    if( returnImage != NULL ) {
      SDL_SetColorKey( returnImage, SDL_SRCCOLORKEY, SDL_MapRGB( returnImage->format, 0xFF, 0xFF, 0xFF ) );
    }
  }
  
  return returnImage; 
}

void Game::drawThat( int x, int y, SDL_Surface *source, SDL_Rect *clip = NULL ) {
  SDL_Rect offset;
  offset.x = x - camera.x;
  offset.y = y - camera.y;
  
  // TODO: maybe do not draw stuff when not within camera anyways; please mind clip width with screen_width and height
  if( offset.x > SCREEN_WIDTH ) return;
  if( offset.y > SCREEN_HEIGHT ) return;
  if( offset.x + TILE_WIDTH < 0 ) return;
  if( offset.y + TILE_HEIGHT < 0 ) return;

  draws++;
  SDL_BlitSurface( source, clip, screen, &offset );
}

// helper functions

// return proper rectangle for given image no
// count starts at 0 and goes left to right, top to bottom
SDL_Rect Game::helper_get_rect_for_image( int img ) {
  SDL_Rect myRect;
  
  myRect.x = TILE_WIDTH*img;
  myRect.y = 0;
  myRect.w = TILE_WIDTH;
  myRect.h = TILE_HEIGHT;
  
  while( myRect.x >= IMAGE_WIDTH ) {
    myRect.x -= IMAGE_WIDTH;
    myRect.y += TILE_HEIGHT;
  }
  
  return myRect;
}

SDL_Rect Game::helper_get_rect_for_images( int start, int cols, int rows ) {

  SDL_Rect myRect;
  myRect = helper_get_rect_for_image( start );
  
  myRect.w = TILE_WIDTH*(cols);
  myRect.h = TILE_HEIGHT*(rows);
  
  return myRect;
}
