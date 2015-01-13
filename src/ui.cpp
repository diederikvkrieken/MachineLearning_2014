#include "ui.h"

#include "master.h"
#include "simulation.h"

void UI::init(Master* master_ptr)
{
  master = master_ptr;
  simulation = master->getSimulation();

  renderer = master->getRenderer();

  status = SELECTING_FILE;

  // Load UI input box
  box_surface = master->loadImage("data/input_file.png");
  input_font = TTF_OpenFont("fonts/cour.ttf", 14);

  int width, height;
  SDL_QueryTexture(box_surface, NULL, NULL, &width, &height);
  input_box_pos = master->getResolution() * 0.5f;
  // Adjust to center the box
  input_box_pos.x -= width / 2;
  input_box_pos.y -= height / 2;
  text_offset.set(18,50);
  save_button.x = input_box_pos.x + 317;
  save_button.y = input_box_pos.y + 159;
  save_button.h = 34;
  save_button.w = 76;
  ignore_button.x = input_box_pos.x + 235;
  ignore_button.y = input_box_pos.y + 159;
  ignore_button.h = 34;
  ignore_button.w = 76;

  input_text = "";
}

void UI::update(input *inputs)
{
  if(status == SELECTING_FILE)
  {
    handleFileSelecting(inputs);
    drawFileSelector(false);
  }
  else if(status == STORING_WALLS)
  {
    handleWallStoring(inputs);
    drawFileSelector(true);
  }
}

void UI::readConfigFile(string path)
{
  ifstream file;
  string line;

  file.open(path.c_str());
  if(!file.is_open())
  {
    fprintf(stderr, "[UI.readConfigFile] Error opening file: %s.\n", path.c_str());
  }

  while(getline(file, line))
  {
    // Split the name variable from rest of the line
    vector<string> words = splitString(line, ':');
    for(unsigned int i=0; i < words.size(); i++)
    {
      printf("word[%d]: %s\n", i, words[i].c_str());
    }
    printf("--------------\n");
    parseParameter(words);
  }
  file.close();
}

void UI::readWalls(string path)
{
  printf("start loading walls\n");
  ifstream file;
  string line;
  vector<pixel> vertices;

  file.open(path.c_str());
  if(!file.is_open())
  {
    fprintf(stderr, "[UI.readWalls] Error opening file: %s.\n", path.c_str());
  }

  while(getline(file, line))
  {
    // Split line up into (x,y) coordinates
    vector<string> points = splitString(line, ';');
    for(unsigned int i=0; i < points.size(); i++)
    {
      pixel p;
      // Split coordinate up into x and y
      vector<string> point = splitString(points[i], ',');
      int x = atoi(point[0].c_str());
      int y = atoi(point[1].c_str());
      p.set(x,y);
      vertices.push_back(p);
    }
  }
  file.close();

  simulation->setWalls(vertices);
  printf("stopped loading walls\n");
}

void UI::exportWalls(string path)
{
  ofstream file;
  stringstream output;

  vector<pixel> walls = simulation->getWalls();

  file.open(path.c_str());
  if(!file.is_open())
  {
    fprintf(stderr, "[UI.exportWalls] Error opening file: %s.\n", path.c_str());
  }

  for(unsigned int i=0; i < walls.size(); i++)
  {
    output.str("");
    printf("walls x: %d -- y: %d\n", walls[i].x, walls[i].y);
    output << walls[i].x << "," << walls[i].y;
    file.write(output.str().c_str(), output.str().size());
    file.write(";", 1);
  }
  file.close();
}

void UI::parseParameter(vector<string> words)
{
  // First word is the parameter name
  if(words[0] == "n_people")
  { simulation->setPeopleAmount(atoi(words[1].c_str())); }
  if(words[0] == "min_radius")
  { simulation->setMinRadius(atoi(words[1].c_str())); }
  if(words[0] == "max_radius")
  { simulation->setMaxRadius(atoi(words[1].c_str())); }
  if(words[0] == "min_vision_range")
  { simulation->setMinVision(atof(words[1].c_str())); }
  if(words[0] == "max_vision_range")
  { simulation->setMaxVision(atof(words[1].c_str())); }
  if(words[0] == "min_height")
  { simulation->setMinHeight(atoi(words[1].c_str())); }
  if(words[0] == "max_height")
  { simulation->setMaxHeight(atoi(words[1].c_str())); }
  if(words[0] == "min_age")
  { simulation->setMinAge(atoi(words[1].c_str())); }
  if(words[0] == "max_age")
  { simulation->setMaxAge(atoi(words[1].c_str())); }
  if(words[0] == "min_speed")
  { simulation->setMinSpeed(atof(words[1].c_str())); }
  if(words[0] == "max_speed")
  { simulation->setMaxSpeed(atof(words[1].c_str())); }
  if(words[0] == "max_simulation_frames")
  { simulation->setMaxFrames(atoi(words[1].c_str())); }
  if(words[0] == "walls_file")
  {
    if(words.size() == 1 || words[1] == "" || words[1] == " " || words[1] == "\n")
    {
      // Don't use a walls configuration
      simulation->setLoadWalls(false);
    }
    else
    {
      simulation->setLoadWalls(true);
      readWalls("config/walls/" + words[1]);
      walls_file = words[1];
    }
  }
  if(words[0] == "single_cone")
  {
    if(atoi(words[1].c_str()) == 1)
    { simulation->setConeDrawing(true); }
    else
    { simulation->setConeDrawing(false); }
  }
}

void UI::handleTextInput(bool *done, input *inputs)
{
  *done = false;

  string text = input_text;
  if(inputs->key == SDL_SCANCODE_BACKSPACE)
  {
    // Delete last character
    if(text.size() > 0)
    {
      text = text.substr(0, text.size() - 1);
    }
  }
  else if(inputs->key == SDL_SCANCODE_RETURN || inputs->key == SDL_SCANCODE_KP_ENTER)
  {
    // End text input (Enter)
    *done = true;
    return;
  }
  else if(inputs->key == SDL_SCANCODE_SPACE)
  {
    // Space
    text = text + " ";
  }
  else if((inputs->key_state[SDL_SCANCODE_LSHIFT] || inputs->key_state[SDL_SCANCODE_RSHIFT]) &&
          (inputs->key == SDL_SCANCODE_MINUS || inputs->key == SDL_SCANCODE_KP_MINUS))
  {
    // Underscore
    text = text + "_";
  }
  else if((inputs->key_state[SDL_SCANCODE_LCTRL] || inputs->key_state[SDL_SCANCODE_RCTRL]) &&
          inputs->key == SDL_SCANCODE_C)
  {
    // CTRL + C
    SDL_SetClipboardText(text.c_str());
  }
  else if((inputs->key_state[SDL_SCANCODE_LCTRL] || inputs->key_state[SDL_SCANCODE_RCTRL]) &&
          inputs->key == SDL_SCANCODE_V)
  {
    // CTRL + V
    string copied(SDL_GetClipboardText());
    text = text + copied;
  }
  else
  {
    // Typing a character
    const char *key = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)inputs->key));
    string addition(key);
    if(addition.size() == 1)
    { text = text + addition; }
  }

  input_text = text;
}

void UI::handleFileSelecting(input *inputs)
{
  bool finished = false;
  handleTextInput(&finished, inputs);
  if(finished)
  {
    readConfigFile("config/" + input_text);
    // Reset text for next time
    input_text = "";
    simulation->setStatus(DRAWING_WALLS);
  }
}

void UI::handleWallStoring(input *inputs)
{
  bool finished = false;
  handleTextInput(&finished, inputs);
  if(finished || clickedOnButton(save_button, inputs))
  {
    printf("save button clicked\n");
    exportWalls("config/walls/" + input_text);
    input_text = "";
    simulation->setStatus(SPAWNING);
  }
  else if(clickedOnButton(ignore_button, inputs))
  {
    printf("ignore button clicked\n");
    // Don't save walls
    input_text = "";
    simulation->setStatus(SPAWNING);
  }
}

void UI::drawFileSelector(bool draw_buttons)
{
  SDL_Rect d;
  int width, height;
  SDL_Color text_colour = {0,0,0};
  /** inefficient to allocate every time **/
  SDL_Surface *text_surface;
  SDL_Texture *text_texture;

  // Box
  SDL_QueryTexture(box_surface, NULL, NULL, &width, &height);
  d.x = input_box_pos.x;
  d.y = input_box_pos.y;
  d.w = width;
  d.h = height;
  SDL_RenderCopy(renderer, box_surface, NULL, &d);

  // Text
  text_surface = TTF_RenderText_Solid(input_font, input_text.c_str(), text_colour);
  text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
  SDL_QueryTexture(text_texture, NULL, NULL, &width, &height);
  d.x = input_box_pos.x + text_offset.x;
  d.y = input_box_pos.y + text_offset.y;
  d.w = width;
  d.h = height;
  SDL_RenderCopy(renderer, text_texture, NULL, &d);
  SDL_FreeSurface(text_surface);
  SDL_DestroyTexture(text_texture);

  if(!draw_buttons)
  { return; }

  // Button text
  text_surface = TTF_RenderText_Solid(input_font, "SAVE", text_colour);
  text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
  SDL_QueryTexture(text_texture, NULL, NULL, &width, &height);
  d.x = input_box_pos.x + 340;
  d.y = input_box_pos.y + 170;
  d.w = width;
  d.h = height;
  SDL_RenderCopy(renderer, text_texture, NULL, &d);
  SDL_FreeSurface(text_surface);
  SDL_DestroyTexture(text_texture);

  text_surface = TTF_RenderText_Solid(input_font, "IGNORE", text_colour);
  text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
  SDL_QueryTexture(text_texture, NULL, NULL, &width, &height);
  d.x = input_box_pos.x + 250;
  d.y = input_box_pos.y + 170;
  d.w = width;
  d.h = height;
  SDL_RenderCopy(renderer, text_texture, NULL, &d);
  SDL_FreeSurface(text_surface);
  SDL_DestroyTexture(text_texture);
}

bool UI::clickedOnButton(SDL_Rect button, input *inputs)
{
  if(inputs->mouse_down != LEFT)
  { return false; }
  if(inputs->mouse_pos.x >= button.x && inputs->mouse_pos.x < button.x + button.w &&
     inputs->mouse_pos.y >= button.y && inputs->mouse_pos.y < button.y + button.h)
  { return true; }
  return false;
}






















