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
  while(getline(file, line))
  {
    // Split the name variable from rest of the line
    vector<string> words = splitString(line, ':');
    for(unsigned int i=0; i < words.size(); i++)
    {
      printf("word[%d]: %s\n", i, words[i].c_str());
    }
    readParameter(words);
  }
  file.close();

  /** Set variables in simulation **/

  /** skipping this now **/
  simulation->setStatus(DRAWING_WALLS);
}

void UI::readParameter(vector<string> words)
{
  // First word is the parameter name
  if(words[0] == "n_people")
  { simulation->setPeopleAmount(atoi(words[1].c_str())); }
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
  else if(inputs->key == SDL_SCANCODE_RETURN)
  {
    // End text input
    *done = true;
    return;
  }
  else if(inputs->key == SDL_SCANCODE_SPACE)
  {
    // Space
    text = text + " ";
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
    status = READING_CONFIG;
    readConfigFile("io/" + input_text);
    // Reset text for next time
    input_text = "";
  }
}

void UI::handleWallStoring(input *inputs)
{
  bool finished = false;
  handleTextInput(&finished, inputs);
  if(finished)
  {
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






















