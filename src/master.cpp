#include "master.h"

bool Master::init()
{
  // Seed random generator
  srand(time(NULL));

  // Maximum framerate
  max_fps = 10000;
  output_fps = true;
  fps_output_rate = 2000;
  frame_counter = 0;
  frame_time_counter = 0;
  total_fps = 0;

  // Set resolution
  resolution.set(800,800);
  running = false;

  // Stop SDL modules when exiting
  atexit(SDL_Quit);
  atexit(IMG_Quit);

  // Initialize SDL modules
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    fprintf(stderr, "[Master.init] Failed to initialize SDL. Message: %s\n", SDL_GetError());
    return false;
  }
  // Initialize TTF
  if( TTF_Init() == -1 )
  {
    printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
    return false;
  }
  // Set texture filtering mode
  if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
  {
    fprintf(stderr, "[Master.init] Failed to set texture filtering mode.\n");
    return false;
  }
  // Create window
  window = SDL_CreateWindow("Panic Simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resolution.x, resolution.y, SDL_WINDOW_SHOWN);
  if(window == NULL)
  {
    fprintf(stderr, "[Master.init] Failed to create window. Message: %s\n", SDL_GetError());
    return false;
  }
  // Create renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
  if(renderer == NULL)
  {
    fprintf(stderr, "[Master.init] Failed to create renderer. Message: %s\n", SDL_GetError());
    return false;
  }
  /*// Set up screen texture
  screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, resolution.x, resolution.y);
  if(screen == 0)
  {
    fprintf(stderr, "[Master.init] Failed to initialize screen texture: %s\n", SDL_GetError());
    return false;
  }*/
  /*// Set render target
  if(SDL_SetRenderTarget(renderer, screen) < 0)
  {
    fprintf(stderr, "[Master.init] Failed to set screen texture as render target: %s\n", SDL_GetError());
    return false;
  }*/

  // Init image loading
  int flags = IMG_INIT_JPG | IMG_INIT_PNG;
  if((IMG_Init(flags) & flags) != flags)
  {
    fprintf(stderr, "[Master.init] Failed to set up png/jpg image loading. Message: %s\n", IMG_GetError());
    return false;
  }

  // Set surface masks
  if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
  {
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
  }
  else
  {
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
  }

  return true;
}

void Master::run()
{
  machine.init(this);
  /*simulation.init(this);
  ui.init(this);*/

  // Make main texture from main surface
  screen = SDL_CreateTextureFromSurface(renderer, machine.getScreen());

  running = true;

  //debug
  /*bool test = detectCollisionPointCone(makeDim2(50,0), makeDim2(45,10), 30, 90, 90);
  if(test)
  { printf("success!\n"); }*/

  while(running)
  {
    // Handle timer
    if(!frame_timer.isStarted())
    { frame_timer.start(); }
    int last_frame_time = (int)frame_timer.getTime();
    frame_timer.reset();
    Uint32 frametime = SDL_GetTicks();

    resetInputs();
    handleInput();

    if(!running)
    { return; }

    SDL_SetRenderTarget(renderer, NULL);
    // Set background color
    SDL_SetRenderDrawColor(renderer, 255,255,255, 255);
    // Clear screen
    SDL_RenderClear(renderer);

    running = machine.run(last_frame_time, inputs);
    /*simulation.update(last_frame_time, inputs);*/

    // Update screen
    SDL_RenderPresent(renderer);

    if(output_fps)
    { outputFPS(last_frame_time); }

    wait(SDL_GetTicks() - frametime);
  }

  quit();
}

void Master::wait(Uint32 frame_length)
{
  Uint32 min_time = 1000 / max_fps;
  if(frame_length < min_time)
  { SDL_Delay(min_time - frame_length); }
}

void Master::outputFPS(Uint32 frame_length)
{
  frame_counter++;
  frame_time_counter += frame_length;
  if(frame_time_counter >= fps_output_rate)
  {
    int fps = (frame_counter / (float)frame_time_counter) * 1000;
    total_fps += frame_counter;
    frame_counter = 0;
    frame_time_counter = 0;
    printf("FPS:\t %d.\n", fps);
  }
}

void Master::quit()
{
  SDL_DestroyTexture(screen);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

void Master::handleInput()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    if(event.type == SDL_QUIT)  // Quit button on window was clicked
    { running = false; }
    if(event.type == SDL_KEYDOWN)
    {
      inputs.key = event.key.keysym.scancode;
      if(inputs.key == SDL_SCANCODE_ESCAPE)
      {
        running = false;
      }
    }
    if(event.type == SDL_MOUSEMOTION)
    {
      inputs.mouse_pos.x = event.motion.x;
      inputs.mouse_pos.y = event.motion.y;
      // Filter out extreme values of relative motion
      if(abs(event.motion.xrel) < resolution.x * 0.4 && abs(event.motion.yrel) < resolution.y * 0.4)
      {
        inputs.mouse_relative.x = event.motion.xrel;
        inputs.mouse_relative.y = event.motion.yrel;
      }
    }
    if(event.type == SDL_MOUSEBUTTONDOWN)
    {
      if(event.button.button == SDL_BUTTON_LEFT)
      { inputs.mouse_down = LEFT; }
      if(event.button.button == SDL_BUTTON_RIGHT)
      { inputs.mouse_down = RIGHT; }
    }
    if(event.type == SDL_MOUSEBUTTONUP)
    {
      if(event.button.button == SDL_BUTTON_LEFT)
      { inputs.mouse_up = LEFT; }
      if(event.button.button == SDL_BUTTON_RIGHT)
      { inputs.mouse_up = RIGHT; }
    }
    if(event.type == SDL_MOUSEWHEEL)
    {
      if(event.wheel.y > 0)
      { inputs.mouse_down = WHEEL_UP; }
      else if(event.wheel.y < 0)
      { inputs.mouse_down = WHEEL_DOWN; }
    }
  }
  inputs.key_state = SDL_GetKeyboardState(NULL);
}

void Master::resetInputs()
{
  inputs.key = -1;
  inputs.key_state = NULL;
  inputs.mouse_relative.set(0,0);
  inputs.mouse_down = -1;
  inputs.mouse_up = -1;
}

SDL_Surface *Master::createEmptySurface(int width, int height)
{
  SDL_Surface *surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);

  if(surface == NULL)
  { fprintf(stderr, "Failed to create empty surface: %s\n", SDL_GetError()); }

  return surface;
}

SDL_Texture *Master::loadImage(string path)
{
  // The final texture
  SDL_Texture* newTexture = NULL;

  // Load image at specified path
  SDL_Surface* loadedSurface = IMG_Load(path.c_str());
  if(loadedSurface == NULL)
  {
    fprintf(stderr, "Unable to load image: %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
  }
  else
  {
    // Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if(newTexture == NULL)
    { fprintf(stderr, "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError()); }

    // Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);
  }

  return newTexture;
}



















