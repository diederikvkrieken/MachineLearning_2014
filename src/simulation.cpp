#include "simulation.h"

#include "master.h"
#include "ui.h"

void Simulation::init(Master *master_ptr)
{
  master = master_ptr;
  ui = master->getUI();
  renderer = master->getRenderer();

  min_radius = 4;
  max_radius = 12;
  min_age = 2;
  max_age = 98;
  min_height = 160;
  max_height = 180;
  min_speed = 0.001f;
  max_speed = 0.01f;
  n_people = 100;
  max_placement_tries = 1000;
  default_vision_range = 60.0f;
  min_vision = default_vision_range;
  max_vision = default_vision_range;
  default_fov = 90.0f;
  default_panic = 0.2f;
  vision_alpha = 50;
  exit_location.set(0,0);

  colour_healthy = makeRGB(GREEN);
  colour_fallen = makeRGB(BLUE);
  colour_dead = makeRGB(RED);
  vision_colour = makeRGB(.29f,.87f,.97f);
  wall_colour = makeRGB(BLACK);
  exit_colour = makeRGB(0.8f,0.5f,0.5f);
  inside_bg_colour = makeRGB(0.8f, 0.8f, 0.8f);

  status = MAIN_SCREEN;
  load_walls = false;
  single_cone = false;

  // Set up base surface/texture
  screen = master->createEmptySurface(master->getResolution().x, master->getResolution().y);
  // Set up surface for walls
  walls = master->createEmptySurface(master->getResolution().x, master->getResolution().y);
  makeColourTransparent(walls, makeRGB(WHITE));

  setupIcons();
  setupVisionCone();
  setupStatusText();
}

void Simulation::fillBuilding()
{
  int tries = 0;

  vector<dim2> walls_vector = convertPixelToDim2(wall_vertices);
  while(people.size() < (unsigned int)n_people)
  {
    human new_human;
    human *collided;
    // Find a position without collision with other humans
    do
    {
      /** TODO: gender is still static **/
      new_human.ID = people.size();
      new_human.gender = MALE;
      new_human.status = HEALTHY;
      new_human.panic = default_panic;
      new_human.age = randInt(min_age, max_age);
      new_human.height = randInt(min_height, max_height);
      new_human.radius = randInt(min_radius, max_radius);
      new_human.vision_range = randInt(min_vision, max_vision);
      new_human.fov = default_fov;
      /*new_human.direction.set(randInt(-100, 100), randInt(-100, 100));
      new_human.direction = normalise(new_human.direction);*/
      // Find a position in the building
      do
      {
        new_human.position.set(randInt(0 + new_human.radius, master->getResolution().x - 1 - new_human.radius),
                               randInt(0 + new_human.radius, master->getResolution().y - 1 - new_human.radius));
      } while(!pointInPolygon(new_human.position, walls_vector) || hitsWall(&new_human));

      // Direction towards the exit
      new_human.direction = normalise(exit_location - new_human.position);
      // Speed
      float speed = min_speed + (randInt(0, 100) / 100.0f) * (max_speed - min_speed);
      new_human.direction = new_human.direction * speed;

      float distance;
      collided = humanCollision(&new_human, &distance);

      tries++;
    } while(tries < max_placement_tries && collided != NULL);

    people.push_back(new_human);

    tries = 0;
  }

  // Pick a focus human
  int idx = randInt(0, people.size() - 1);
  focus_human = &people[idx];
}

void Simulation::update(int frame_time, input inputs)
{
  // Reset background surface
  SDL_FillRect(screen, NULL, SDL_MapRGBA(screen->format, 255,255,255, 255));

  if(status == MAIN_SCREEN || status == STORING_WALLS)
  {
    ui->update(&inputs);
    return;
  }
  else if(status == DRAWING_WALLS)
  {
    if(load_walls)
    {
      status = SPAWNING;
      updateWallSurface();
      fillWallBackground();
      exit_location = determineExit();
      return;
    }
    createWalls(&inputs);
  }
  else if(status == SPAWNING)
  {
    fillBuilding();
    status = RUNNING;
  }
  else if(status == RUNNING)
  {
    /*// debug
    focus_human->direction.x = 0.02;
    focus_human->direction.y = 0.02;
    visible_information test = applyPerception(focus_human);
    printf("seeing %d walls.\n", test.n_walls);
    printf("seeing %d humans.\n", test.n_people);*/
    moveHumans(frame_time);
  }

  // Draw walls
  drawWalls();

  for(unsigned int i=0; i < people.size(); i++)
  {
    drawHuman(people[i]);
  }

  // Convert surface to texture
  SDL_UpdateTexture(master->getMasterTexture(), NULL, screen->pixels, screen->pitch);

  // Update the texture
  SDL_RenderCopy(renderer, master->getMasterTexture(), NULL, NULL);

  // Draw the (scaled and rotated) vision cones
  drawVision();
}

visible_information Simulation::applyPerception(human *h)
{
  visible_information view;

  vector<human *> visible_humans = visibleHumans(h);

  view.n_people = visible_humans.size();
  getAgeMeanVariance(visible_humans, &view.mean_age, &view.var_age);
  getHeightMeanVariance(visible_humans, &view.mean_height, &view.var_height);
  getRadiusMeanVariance(visible_humans, &view.mean_radius, &view.var_radius);
  getPanicMeanVariance(visible_humans, &view.mean_panic, &view.var_panic);
  getDirectionMeanVariance(visible_humans, &view.mean_direction, &view.var_direction);
  view.exit_distance = computeDistance(exit_location, h->position); // Straight-line distance

  view.n_walls = 0;
  view.closest_wall_distance = 500000.0f;  // No wall in sight
  float facing_angle = toDegrees(radiansPositiveOnly(computeAngle(h->direction, makeDim2(0.0f, 0.0f))));
  // Check walls
  for(unsigned int i=0; i < wall_vertices.size(); i++)
  {
    dim2 current = convertPixelToDim2(wall_vertices[i]);
    pixel n = (i < wall_vertices.size() - 1) ? wall_vertices[i+1] : wall_vertices[0];
    dim2 next = convertPixelToDim2(n);

    // See if the person can see this wall section
    float distance;
    if(detectCollisionLineCone(current, next, 200, h->position, h->vision_range, h->fov, facing_angle, &distance))
    {
      view.n_walls++;
      // Check if this wall is closer than others
      if(distance < view.closest_wall_distance)
      { view.closest_wall_distance = distance; }
    }
  }

  return view;
}

/*void Simulation::testSim()
{
  vector<dim2> test_pos, in_polygon, polygon;
  polygon = convertPixelToDim2(wall_vertices);
  test_pos.push_back(makeDim2(400,400));
  test_pos.push_back(makeDim2(800,400));
  test_pos.push_back(makeDim2(700,500));
  test_pos.push_back(makeDim2(20,80));

  for(unsigned int i=0; i < test_pos.size(); i++)
  {
    if(pointInPolygon(test_pos[i], polygon))
    { placeHuman(test_pos[i], MALE, 40, 125, 7, HEALTHY); }
  }
}*/

void Simulation::handleInput(int frame_time, input inputs)
{
}

void Simulation::moveHumans(int frame_time)
{
  for(unsigned int i=0; i < people.size(); i++)
  {
    human *h = &people[i];

    // Detect collisions
    float distance; /** Use this to simulate pushing **/
    if(humanCollision(h, &distance) != NULL || hitsWall(h))
    {
      /*h->position = h->previous_position;*/
      continue;
    }

    // Calculate new position based on frame time and direction
    h->previous_position = h->position;
    h->position = h->position + h->direction * frame_time;
  }
}

void Simulation::createWalls(input *inputs)
{
  if(inputs->mouse_down == LEFT)  // Left click
  {
    pixel point;
    point.set(inputs->mouse_pos.x, inputs->mouse_pos.y);
    if(wall_vertices.size() > 2)
    {
      dim2 first_point;
      // Check if close to starting point
      first_point.set(wall_vertices[0].x, wall_vertices[0].y);
      if(computeDistance(first_point, inputs->mouse_pos) < 6.0f)
      {
        // Walls form an enclosed area
        // Determine exit
        exit_location = determineExit();
        // Save walls
        status = STORING_WALLS;
        ui->setStatus(STORING_WALLS);
      }
    }

    // Add point
    if(status == DRAWING_WALLS)
    { wall_vertices.push_back(point); }
  }
  else if(inputs->mouse_down == RIGHT) // Right click
  {
    if(wall_vertices.size() > 0)
    { wall_vertices.pop_back(); } // Remove last point
  }
  updateWallSurface();
  fillWallBackground();
}

void Simulation::updateWallSurface()
{
  // Reset background
  SDL_FillRect(walls, NULL, SDL_MapRGBA(screen->format, 255,255,255, 255));

  if(wall_vertices.size() < 2)
  { return; }

  for(unsigned int i=0; i < wall_vertices.size() - 1; i++)
  {
    dim2 p1, p2;
    p1.set(wall_vertices[i].x, wall_vertices[i].y);
    p2.set(wall_vertices[i+1].x, wall_vertices[i+1].y);
    dim2 direction = normalise(p2 - p1);
    // Length between two points
    // Draw all pixels of a line segment
    dim2 current = p1;
    while(computeDistance(current, p2) > 0.5f)
    {
      colourPixel(walls, current.x, current.y, wall_colour, 1.0f);
      current = current + direction;
    }
    // To create lines without gaps
    colourPixel(walls, current.x, current.y, wall_colour, 1.0f);

    // Draw the exit
    if(i+1 == wall_vertices.size() - 1 &&
       status != DRAWING_WALLS)
    {
      p1.set(wall_vertices[i+1].x, wall_vertices[i+1].y);
      p2.set(wall_vertices[0].x, wall_vertices[0].y);
      direction = normalise(p2 - p1);
      current = p1;
      while(computeDistance(current, p2) > 0.5f)
      {
        colourPixel(walls, current.x, current.y, exit_colour, 1.0f);
        current = current + direction;
      }
      colourPixel(walls, current.x, current.y, exit_colour, 1.0f);
    }
  }
}

void Simulation::fillWallBackground()
{
  if(status != DRAWING_WALLS)
  {
    dim2 point;
    pixel start;
    // Find pixel inside polygon
    do
    {
      point.set(randInt(0, master->getResolution().x - 1),
                randInt(0, master->getResolution().y - 1));
    } while(!pointInPolygon(point, convertPixelToDim2(wall_vertices)));
    start.set(point.x, point.y);
    floodFillInside(inside_bg_colour, makeRGB(WHITE), start);
  }
}

void Simulation::floodFillInside(rgb target, rgb overwrite, pixel start)
{
  pixel resolution;
  vector<pixel> pixel_stack;

  resolution.set(master->getResolution().x, master->getResolution().y);
  pixel_stack.push_back(start);

  while(pixel_stack.size() > 0)
  {
    pixel last = pixel_stack[pixel_stack.size()-1];
    pixel_stack.pop_back(); // Delete inspected pixel

    if(getPixelColour(walls, last.x, last.y) == overwrite)
    {
      pixel next;

      colourPixel(walls, last.x, last.y, target, 1.0f);
      next.set(last.x+1, last.y);
      if(next.x >= 0 && next.x < resolution.x-1 && next.y >= 0 && next.y < resolution.y)
      { pixel_stack.push_back(next); }
      next.set(last.x-1, last.y);
      if(next.x >= 0+1 && next.x < resolution.x && next.y >= 0 && next.y < resolution.y)
      { pixel_stack.push_back(next); }
      next.set(last.x, last.y+1);
      if(next.x >= 0 && next.x < resolution.x && next.y >= 0 && next.y < resolution.y-1)
      { pixel_stack.push_back(next); }
      next.set(last.x, last.y-1);
      if(next.x >= 0 && next.x < resolution.x && next.y >= 0+1 && next.y < resolution.y)
      { pixel_stack.push_back(next); }
    }
  }
}

void Simulation::setupIcons()
{
  circle_surface circle;

  // Size 0 is not used
  circle.radius = -1;
  circle.healthy = NULL;
  circle.fallen = NULL;
  circle.dead = NULL;
  icons.push_back(circle);
  // Fill vector up to max size
  for(int i=1; i <= max_radius; i++)
  {
    circle.radius = i;
    circle.healthy = master->createEmptySurface(i*2+2, i*2+2);
    circle.fallen = master->createEmptySurface(i*2+2, i*2+2);
    circle.dead = master->createEmptySurface(i*2+2, i*2+2);
    createCircle(circle.healthy, colour_healthy);
    createCircle(circle.fallen, colour_fallen);
    createCircle(circle.dead, colour_dead);
    icons.push_back(circle);
  }
}

void Simulation::setupVisionCone()
{
  dim2 cone_start;
  float min_angle, max_angle;

  SDL_Surface *cone = master->createEmptySurface(2 * default_vision_range, default_vision_range);
  // Enable alpha blending
  SDL_SetSurfaceBlendMode(cone, SDL_BLENDMODE_BLEND);
  SDL_SetSurfaceAlphaMod(cone, vision_alpha);
  makeColourTransparent(cone, makeRGB(WHITE));
  SDL_FillRect(cone, NULL, SDL_MapRGBA(screen->format, 255,255,255, 255));

  cone_start.set(cone->w / 2, cone->h);
  min_angle = 90.0f - default_fov / 2.0f;
  max_angle = 90.0f + default_fov / 2.0f;

  dim2 point;
  for(int i=0; i < cone->w; i++)
  {
    for(int j=0; j < cone->h; j++)
    {
      point.set(i,j);
      float distance = computeDistance(point, cone_start);
      float angle = toDegrees(computeAngle(point, cone_start));
      if(distance <= default_vision_range && angle > min_angle && angle < max_angle)
      { colourPixel(cone, i, j, vision_colour, 1.0); }
    }
  }

  vision_cone = SDL_CreateTextureFromSurface(renderer, cone);
  SDL_FreeSurface(cone);
}

void Simulation::createCircle(SDL_Surface *circle, rgb colour)
{
  dim2 current, center;
  rgb black = makeRGB(BLACK);
  rgb white = makeRGB(WHITE);

  center.set(circle->w * 0.5f,
             circle->h * 0.5f);
  // Colour a circle
  for(int i=0; i < circle->w; i++)
  {
    for(int j=0; j < circle->h; j++)
    {
      current.set(i,j);
      if(computeDistance(current, center) <= circle->w * 0.5f)
      { colourPixel(circle, i, j, colour, 1.0f); }
      else
      { colourPixel(circle, i, j, white, 1.0f); }
    }
  }
  // Make the edges black
  for(int i=0; i < circle->w; i++)
  {
    for(int j=0; j < circle->h; j++)
    {
      rgb pixel = getPixelColour(circle, i, j);
      if(pixel == colour) // Part of circle
      {
        // Edges of surface
        if(i == 0 || i == circle->w -1 || j == 0 || j == circle->h - 1)
        { colourPixel(circle, i, j, black, 1.0f); }
        // If any neighbour pixel is white, make it black
        else if(getPixelColour(circle, i+1, j) == white ||
                getPixelColour(circle, i-1, j) == white ||
                getPixelColour(circle, i, j+1) == white ||
                getPixelColour(circle, i, j-1) == white)
        { colourPixel(circle, i, j, black, 1.0f); }
      }
    }
  }

  // Colour key on white
  makeColourTransparent(circle, white);
}

void Simulation::setupStatusText()
{
  status_text[MAIN_SCREEN] = "MAIN_SCREEN";
  status_text[SELECTING_FILE] = "SELECTING_FILE";
  status_text[READING_CONFIG] = "READING_CONFIG";
  status_text[DRAWING_WALLS] = "DRAWING_WALLS";
  status_text[STORING_WALLS] = "STORING_WALLS";
  status_text[SPAWNING] = "SPAWNING";
  status_text[RUNNING] = "RUNNING";
  status_text[PAUSED] = "PAUSED";
  status_text[STOPPED] = "STOPPED";
}

void Simulation::drawHuman(human individual)
{
  SDL_Rect d;
  // Center the surface on an individual's position
  d.x = individual.position.x - individual.radius;
  d.y = individual.position.y - individual.radius;

  switch(individual.status)
  {
    case HEALTHY:
      SDL_BlitSurface(icons[individual.radius].healthy, NULL, screen, &d);
      break;
    case FALLEN:
      SDL_BlitSurface(icons[individual.radius].fallen, NULL, screen, &d);
      break;
    case DEAD:
      SDL_BlitSurface(icons[individual.radius].dead, NULL, screen, &d);
      break;
  }
}

void Simulation::drawVision()
{
  float angle;
  SDL_Point center;
  SDL_Rect d;

  for(unsigned int i=0; i < people.size(); i++)
  {
    human *indiv = &people[i];

    if(single_cone && indiv != focus_human)
    { continue; } // Only draw focus human's cone

    if(indiv->status != HEALTHY)
    { continue; } // Don't draw vision cone

    d.w = indiv->vision_range * 2;
    d.h = indiv->vision_range;
    d.x = indiv->position.x - indiv->vision_range;
    d.y = indiv->position.y - indiv->vision_range;
    angle = 360 - toDegrees(radiansPositiveOnly(computeAngle(indiv->direction, makeDim2(0,0)))) + 90;
    center.x = d.w / 2;
    center.y = d.h;

    SDL_RenderCopyEx(renderer, vision_cone, NULL, &d, angle, &center, SDL_FLIP_NONE);
  }
}

void Simulation::drawWalls()
{
  SDL_BlitSurface(walls, NULL, screen, NULL);
}

void Simulation::placeHuman(dim2 position, dim2 direction, human_gender gender, int age, int height, int radius, float vision_range, health_status status)
{
  human individual;
  individual.position = position;
  individual.direction = makeDim2(0, -1); // Facing north
  individual.gender = gender;
  individual.age = age;
  individual.height = height;
  individual.radius = radius;
  individual.status = status;
  individual.vision_range = vision_range;
  individual.fov = default_fov; // Not visualized

  people.push_back(individual);
}

human *Simulation::humanCollision(human *target, float *distance)
{
  for(unsigned int i=0; i < people.size(); i++)
  {
    // Don't check self-collisions
    if(target->ID == people[i].ID)
    { continue; }

    *distance = detectCollisionCircle(people[i].position, people[i].radius, target->position, target->radius);
    if(*distance < 0.0f)
    { return &people[i]; }
  }
  return NULL;
}

bool Simulation::hitsWall(human *target)
{
  dim2 w1, w2;  // Wall points

  w1.set(wall_vertices[wall_vertices.size()-1].x, wall_vertices[wall_vertices.size()-1].y);
  w2.set(wall_vertices[0].x, wall_vertices[0].y);
  if(distancePointToLineSegment(target->position, w1, w2) < (float)target->radius)
  { return true; }
  for(unsigned int i=0; i < wall_vertices.size() - 1; i++)
  {
    w1.set(wall_vertices[i].x, wall_vertices[i].y);
    w2.set(wall_vertices[i+1].x, wall_vertices[i+1].y);
    if(distancePointToLineSegment(target->position, w1, w2) < (float)target->radius)
    { return true; }
  }
  return false;
}

vector<human *> Simulation::visibleHumans(human *h)
{
  vector<human *> visible;

  float facing_angle = toDegrees(radiansPositiveOnly(computeAngle(h->direction, makeDim2(0.0f, 0.0f))));

  for(unsigned int i=0; i < people.size(); i++)
  {
    human *compare = &people[i];

    if(compare == h)
    { continue; } // Don't check against self

    if(!detectCollisionPointCone(compare->position, h->position, h->vision_range, h->fov, facing_angle))
    { continue; } // Human is not in the field of view

    visible.push_back(compare);
  }
  return visible;
}

void Simulation::getAgeMeanVariance(vector<human *> humans, float *mean, float *variance)
{
  vector<float> numbers;
  for(unsigned int i=0; i < humans.size(); i++)
  {
    numbers.push_back(humans[i]->age);
  }
  *mean = calculateMean(numbers);
  *variance = calculateVariance(numbers, *mean);
}

void Simulation::getRadiusMeanVariance(vector<human *> humans, float *mean, float *variance)
{
  vector<float> numbers;
  for(unsigned int i=0; i < humans.size(); i++)
  {
    numbers.push_back(humans[i]->radius);
  }
  *mean = calculateMean(numbers);
  *variance = calculateVariance(numbers, *mean);
}

void Simulation::getHeightMeanVariance(vector<human *> humans, float *mean, float *variance)
{
  vector<float> numbers;
  for(unsigned int i=0; i < humans.size(); i++)
  {
    numbers.push_back(humans[i]->height);
  }
  *mean = calculateMean(numbers);
  *variance = calculateVariance(numbers, *mean);
}

void Simulation::getPanicMeanVariance(vector<human *> humans, float *mean, float *variance)
{
  vector<float> numbers;
  for(unsigned int i=0; i < humans.size(); i++)
  {
    numbers.push_back(humans[i]->panic);
  }
  *mean = calculateMean(numbers);
  *variance = calculateVariance(numbers, *mean);
}

void Simulation::getDirectionMeanVariance(vector<human *> humans, dim2 *mean, dim2 *variance)
{
  vector<float> numbers_x;
  vector<float> numbers_y;
  for(unsigned int i=0; i < humans.size(); i++)
  {
    numbers_x.push_back(humans[i]->direction.x);
    numbers_y.push_back(humans[i]->direction.y);
  }
  (*mean).x = calculateMean(numbers_x);
  (*mean).y = calculateMean(numbers_y);
  (*variance).x = calculateVariance(numbers_x, (*mean).x);
  (*variance).y = calculateVariance(numbers_y, (*mean).y);
}


/** Returns the exit point, which is represented by the center of the line of the the last wall section **/
dim2 Simulation::determineExit()
{
  dim2 start_point, end_point;

  start_point = convertPixelToDim2(wall_vertices[wall_vertices.size() - 1]);
  end_point = convertPixelToDim2(wall_vertices[0]);
  dim2 direction = end_point - start_point;
  return start_point + direction * 0.5f;
}























