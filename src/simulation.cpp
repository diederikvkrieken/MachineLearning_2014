#include "simulation.h"

#include "master.h"
#include "ui.h"
#include "machine.h"

void Simulation::init(Master *master_ptr, Machine *machine_ptr)
{
  master = master_ptr;
  machine = machine_ptr;
  ui = machine->getUI();

  network = machine->getNetwork();
  renderer = master->getRenderer();

  total_escape_time = 0;
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
  chance_collision_fall = 0.5f;
  standup_time = 4000;
  trample_constant = 0.00001f;
  push_rate = 2000;
  max_frames = 100000;
  frame_counter = 0;
  action_rate = 10;
  frames_since_action = action_rate;

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
  drawing = true;

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

  // Reset humans
  people.clear();

  vector<dim2> walls_vector = convertPixelToDim2(wall_vertices);
  while(people.size() < (unsigned int)n_people)
  {
    human new_human;
    vector<human *> collided;
    do
    {
      /** TODO: gender is still static **/
      new_human.ID = people.size();
      new_human.gender = MALE;
      new_human.status = HEALTHY;
      new_human.escaped = false;
      new_human.escape_time = 0;
      new_human.push_time_out.start();
      new_human.trample_status = 0.0f;
      new_human.panic = default_panic;
      new_human.age = randInt(min_age, max_age);
      new_human.height = randInt(min_height, max_height);
      new_human.radius = randInt(min_radius, max_radius);
      new_human.vision_range = randInt(min_vision, max_vision);
      new_human.fov = default_fov;
      new_human.direction.set(randInt(-100, 100), randInt(-100, 100));
      new_human.direction = normalise(new_human.direction);
      // Find a position in the building
      do
      {
        // Find a position without collision with other humans
        new_human.position.set(randInt(0 + new_human.radius, master->getResolution().x - 1 - new_human.radius),
                               randInt(0 + new_human.radius, master->getResolution().y - 1 - new_human.radius));
      } while(!pointInPolygon(new_human.position, walls_vector) || hitsWall(&new_human, true));
      new_human.previous_position = new_human.position;

      // Direction towards the exit
      /*new_human.direction = normalise(exit_location - new_human.position);*/
      // Speed
      float speed = min_speed + (randInt(0, 100) / 100.0f) * (max_speed - min_speed);
      new_human.direction = new_human.direction * speed;

      float distance;
      collided = humanCollision(&new_human, &distance);

      tries++;
    } while(tries < max_placement_tries && collided.size() > 0);

    people.push_back(new_human);

    tries = 0;
  }

  // Pick a focus human
  int idx = randInt(0, people.size() - 1);
  focus_human = &people[idx];
}

void Simulation::update(int frame_time, input inputs)
{
  handleInput(frame_time, inputs);

  if(drawing)
  {
    // Reset background surface
    SDL_FillRect(screen, NULL, SDL_MapRGBA(screen->format, 255,255,255, 255));
  }

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
    total_escape_time = 0;
    frame_counter = 0;
    start_time = SDL_GetTicks();
    status = RUNNING;
  }
  else if(status == RUNNING)
  {
    if(frame_counter >= max_frames)
    {
      // Simulation has ended
      status = STOPPED;
      calculateTotalTime();
      return;
    }
    updateActions();
    updateFallen();
    moveHumans(frame_time);

    frame_counter++;
  }

  if(drawing)
  {
    // Draw walls
    drawWalls();

    // Draw humans
    for(unsigned int i=0; i < people.size(); i++)
    {
      if(!people[i].escaped )
      { drawHuman(people[i]); }
    }

    // Convert surface to texture
    SDL_UpdateTexture(master->getMasterTexture(), NULL, screen->pixels, screen->pitch);

    // Update the texture
    SDL_RenderCopy(renderer, master->getMasterTexture(), NULL, NULL);

    // Draw the (scaled and rotated) vision cones
    drawVision();
  }
}

/** Normalises the values between [-1,1] **/
visible_information Simulation::applyPerception(human *h)
{
  visible_information view;

  vector<human *> visible_humans = visibleHumans(h);

  view.n_people = (visible_humans.size() / (float)n_people - 0.5f) * 2.0f;
  getAgeMeanVariance(visible_humans, &view.mean_age, &view.var_age);
  getHeightMeanVariance(visible_humans, &view.mean_height, &view.var_height);
  getRadiusMeanVariance(visible_humans, &view.mean_radius, &view.var_radius);
  getPanicMeanVariance(visible_humans, &view.mean_panic, &view.var_panic);
  getDirectionMeanVariance(visible_humans, &view.mean_direction, &view.var_direction);

  view.exit_distance = (computeDistance(exit_location, h->position) / (float)master->getResolution().x - 0.5f) * 2.0f; // Straight-line distance

  view.n_walls = 0;
  view.closest_wall_distance = -1.0f;  // No wall in sight
  float facing_angle = toDegrees(radiansPositiveOnly(computeAngle(h->direction, makeDim2(0.0f, 0.0f))));
  // Check walls
  for(unsigned int i=0; i < wall_vertices.size() - 1; i++)
  {
    dim2 current = convertPixelToDim2(wall_vertices[i]);
    dim2 next = convertPixelToDim2(wall_vertices[i+1]);

    // See if the person can see this wall section
    float distance;
    if(detectCollisionLineCone(current, next, 200, h->position, h->vision_range, h->fov, facing_angle, &distance))
    {
      view.n_walls++;
      // Check if this wall is closer than others
      if(distance < view.closest_wall_distance && distance >= 0.0f)
      { view.closest_wall_distance = distance; }
    }
  }
  view.n_walls = (view.n_walls / (float)(wall_vertices.size() - 1) - 0.5f) * 2.0f;
  if(view.closest_wall_distance < 0.0f)
  { view.closest_wall_distance = 0.0f; }
  else
  { view.closest_wall_distance = (view.closest_wall_distance / (float)master->getResolution().x - 0.5f) * 2.0f; }

  return view;
}

int Simulation::getResult()
{
  if(status != STOPPED)
  { return -1; }
  return total_escape_time;
}

void Simulation::calculateTotalTime()
{
  Uint32 end_time = SDL_GetTicks();
  // See which humans have not escaped and give them max time
  for(unsigned int i=0; i < people.size(); i++)
  {
    human *h = &people[i];

    if(!h->escaped)
    {
      h->escape_time = end_time - start_time;
    }
    // Add to total
    total_escape_time += h->escape_time;
  }
}

void Simulation::handleInput(int frame_time, input inputs)
{
  if(inputs.key == SDL_SCANCODE_D)  // D
  {
    // Toggle drawing
    drawing = !drawing;
  }
  if((inputs.key_state[SDL_SCANCODE_LCTRL] || inputs.key_state[SDL_SCANCODE_RCTRL]) &&
     inputs.key == SDL_SCANCODE_S)  // CTRL + S
  {
    // Save the state
    printf("saving\n");
    machine->saveState();
  }
}

void Simulation::updateActions()
{
  frames_since_action++;

  if(frames_since_action <= action_rate)
  { return; } // Too soon to change actions from network
  frames_since_action = 0;

  for(unsigned int i=0; i < people.size(); i++)
  {
    human *h = &people[i];

    vector<float> nn_inputs = createNNInputs(h, applyPerception(h));
    /*printf("human idx: %d\n", i);
    for(unsigned int j=0; j < nn_inputs.size(); j++)
    {
      printf("input %d = %.2f\n", j, nn_inputs[j]);
    }*/
    human_action action = machine->queryNetwork(nn_inputs);
    h->direction = action.direction;
    h->panic = action.panic;
  }
}

void Simulation::moveHumans(int frame_time)
{
  vector< vector<human *> > checked_collisions; // Keep track of when two humans collide to avoid double pushing/trampling

  for(unsigned int i=0; i < people.size(); i++)
  {
    human *h = &people[i];

    // Don't do anything with disabled people
    if(h->escaped || h->status == FALLEN || h->status == DEAD)
    { continue; }

    // Detect collisions
    float overlap;  /** TODO: doesn't make sense with multiple collisions **/
    vector<human *> collisions = humanCollision(h, &overlap);
    if(collisions.size() > 0)
    {
      // Has collided with a human
      for(unsigned int j=0; j < collisions.size(); j++)
      {
        human *collided = collisions[j];
        // Handle pushing and trampling
        if(!collisionChecked(checked_collisions, h, collided))
        {
          if(collided->status == FALLEN)
          {
            // Hurt the human lying on the ground
            trample(frame_time, collided, h, -overlap);
          }
          else if(collided->status == HEALTHY &&
                  ((computeChance(getPushChance(h), 100) && h->push_time_out.getTime() > push_rate) ||  // Check if one of them wants to push
                   (computeChance(getPushChance(collided), 100) && collided->push_time_out.getTime() > push_rate)))
          {
            push(h, collided);
          }
        }
      }

      // Update collision tracking
      vector<human *> human_pair;
      for(unsigned int j=0; j < collisions.size(); j++)
      {
        human_pair.clear();
        human_pair.push_back(h);
        human_pair.push_back(collisions[j]);
        checked_collisions.push_back(human_pair);
      }

      // Check which human is in front
      if(!isFrontHuman(h, collisions))
      /*if(dot(normalise(h->direction), normalise(collided->position - h->position)) >= 0 &&
         collided->status == HEALTHY)*/
      {
        h->position = h->previous_position;
        // One of [collisions] is in front of [h]: don't move [h]
        continue;
      }
    }
    if(hitsWall(h, false))
    {
       h->position = h->previous_position;
       // Don't move [h]
       continue;
    }

    // Calculate new position based on frame time and direction
    h->previous_position = h->position;
    h->position = h->position + h->direction * frame_time;
    // Check if still in the building
    if(!humanInBuilding(h))
    {
      h->escaped = true;
      h->escape_time = SDL_GetTicks() - start_time;
    }
  }
}

void Simulation::updateFallen()
{
  bool alive_collisions;

  for(unsigned int i=0; i < people.size(); i++)
  {
    human *h = &people[i];

    if(h->status == FALLEN)
    {
      alive_collisions = false;

      float distance;
      vector<human *> collisions = humanCollision(h, &distance);
      // See if any of the colliding humans are alive
      for(unsigned int j=0; j < collisions.size(); j++)
      {
        if(collisions[j]->status == HEALTHY)
        { alive_collisions = true; }
      }
      if(h->lying.isStarted() && h->lying.getTime() > standup_time &&
         (collisions.size() == 0 || !alive_collisions))  // The area is clear
      {
        h->lying.stop();
        h->lying.reset();
        h->trample_status = 0.0f;
        h->status = HEALTHY;
      }
    }
  }
}

void Simulation::push(human *a, human *b)
{
  /** TODO: add speed factor **/
  float a_score, b_score; // The human with the highest score wins
  float a_advantage, b_advantage; // Which human has advantage related to position and direction
  float height_weight, radius_weight, panic_weight, direction_weight;

  // Check if a fall should occur
  bool should_fall = computeChance(0.5f, 100);
  if(!should_fall)
  { return; }

  // Reset time-outs
  a->push_time_out.reset();
  b->push_time_out.reset();

  // Constants to normalize score factors
  direction_weight = 1.0f;
  height_weight = 0.003f;
  radius_weight = 0.125f;
  panic_weight = 0.3f;

  float dot_product = dot(normalise(a->direction), normalise(b->direction));
  if(dot(normalise(a->direction), normalise(b->position - a->position)) >= 0) // if a is facing b
  { a_advantage = (dot_product + 1.0f) / 2.0f; }
  else
  { a_advantage = 1.0f - (dot_product + 1.0f) / 2.0f; }
  b_advantage = 1.0f - a_advantage;

  a_score = (a->height - b->height) * height_weight +
            (a->radius - b->radius) * radius_weight +
            a->panic * panic_weight +
            a_advantage * direction_weight;
  b_score = (b->height - a->height) * height_weight +
            (b->radius - a->radius) * radius_weight +
            b->panic * panic_weight +
            b_advantage * direction_weight;

  if(a_score > b_score)
  {
    b->status = FALLEN;
    b->lying.start(); // Start counting down stand-up timer
  }
  else
  {
    a->status = FALLEN;
    a->lying.start();
  }
}

void Simulation::trample(int frame_time, human *fallen, human *treading, float overlap)
{
  // Trampling goes faster if the two humans overlap more
  // weight is between [0,1]
  float overlap_weight = overlap / (float)(fallen->radius + treading->radius);
  /** [overlap_weight] is disabled **/

  fallen->trample_status += frame_time * treading->radius * trample_constant/* * overlap_weight*/;
  if(fallen->trample_status >= 1.0f)
  {
    fallen->status = DEAD;
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

    if(indiv->escaped)
    { continue; } // Don't draw cones when human has disappeared

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

vector<human *>  Simulation::humanCollision(human *target, float *distance)
{
  SDL_Rect bounding_box;

  vector<human *> collided;
  for(unsigned int i=0; i < people.size(); i++)
  {
    // Only check active humans
    if(target->escaped)
    { continue; }

    // Don't check self-collisions
    if(target->ID == people[i].ID)
    { continue; }

    // Check within minimum bounding box for possible collisions
    bounding_box.x = target->position.x - max_radius * 2;
    bounding_box.y = target->position.y - max_radius * 2;
    bounding_box.w = max_radius * 4;
    bounding_box.h = bounding_box.w;
    if(!detectCollisionPointBoundingBox(people[i].position, bounding_box))
    { continue; }

    /** TODO: distance doesn't make sense with multiple collisions **/
    *distance = detectCollisionCircle(people[i].position, people[i].radius, target->position, target->radius);
    // Check if circles overlap
    if(*distance < 0.0f)
    { collided.push_back(&people[i]); }
  }
  return collided;
}

bool Simulation::collisionChecked(vector< vector<human *> > checked_collisions, human *a, human *b)
{
  for(unsigned int i=0; i < checked_collisions.size(); i++)
  {
    if((checked_collisions[i][0]->ID == a->ID && checked_collisions[i][1]->ID == b->ID) ||
       (checked_collisions[i][0]->ID == b->ID && checked_collisions[i][1]->ID == a->ID))
    { return true; }
  }
  return false;
}

bool Simulation::hitsWall(human *target, bool include_exit)
{
  dim2 w1, w2;  // Wall points

  if(include_exit)
  {
    w1.set(wall_vertices[wall_vertices.size()-1].x, wall_vertices[wall_vertices.size()-1].y);
    w2.set(wall_vertices[0].x, wall_vertices[0].y);
    if(distancePointToLineSegment(target->position, w1, w2) < (float)target->radius)
    { return true; }
  }

  for(unsigned int i=0; i < wall_vertices.size() - 1; i++)
  {
    w1.set(wall_vertices[i].x, wall_vertices[i].y);
    w2.set(wall_vertices[i+1].x, wall_vertices[i+1].y);
    if(distancePointToLineSegment(target->position, w1, w2) < (float)target->radius)
    { return true; }
  }
  return false;
}

bool Simulation::isFrontHuman(human *h, vector<human *> collisions)
{
  for(unsigned int i=0; i < collisions.size(); i++)
  {
    human *collided = collisions[i];

    if(dot(normalise(h->direction), normalise(collided->position - h->position)) >= 0 &&
       collided->status == HEALTHY)
    {
      // [collided] is in front
      return false;
    }
  }
  return true;
}

bool Simulation::humanInBuilding(human *h)
{
  vector<dim2> walls_vector = convertPixelToDim2(wall_vertices);
  if(pointInPolygon(h->position, walls_vector))
  { return true; }
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


float Simulation::getPushChance(human *h)
{
  float chance = h->panic - 0.15f;
  clamp(&chance, 0.0f, 1.0f);
  return chance;
}

int Simulation::getNN()
{
  return NeuralNetwork;
}

void Simulation::getAgeMeanVariance(vector<human *> humans, float *mean, float *variance)
{
  vector<float> numbers;
  for(unsigned int i=0; i < humans.size(); i++)
  {
    // Normalize value between [-1,1]
    float norm_val = ((humans[i]->age - min_age) / (float)(max_age - min_age) - 0.5f) * 2.0f;
    numbers.push_back(norm_val);
  }
  *mean = calculateMean(numbers);
  *variance = calculateVariance(numbers, *mean);
}

void Simulation::getRadiusMeanVariance(vector<human *> humans, float *mean, float *variance)
{
  vector<float> numbers;
  for(unsigned int i=0; i < humans.size(); i++)
  {
    float norm_val = ((humans[i]->radius - min_radius) / (float)(max_radius - min_radius) - 0.5f) * 2.0f;
    numbers.push_back(norm_val);
  }
  *mean = calculateMean(numbers);
  *variance = calculateVariance(numbers, *mean);
}

void Simulation::getHeightMeanVariance(vector<human *> humans, float *mean, float *variance)
{
  vector<float> numbers;
  for(unsigned int i=0; i < humans.size(); i++)
  {
    float norm_val = ((humans[i]->height - min_height) / (max_height - min_height) - 0.5f) * 2.0f;
    numbers.push_back(norm_val);
  }
  *mean = calculateMean(numbers);
  *variance = calculateVariance(numbers, *mean);
}

void Simulation::getPanicMeanVariance(vector<human *> humans, float *mean, float *variance)
{
  vector<float> numbers;
  for(unsigned int i=0; i < humans.size(); i++)
  {
    float norm_val = (humans[i]->panic - 0.5f) * 2.0f;
    numbers.push_back(norm_val);
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
    float norm_x = humans[i]->direction.x * 100.0f;
    float norm_y = humans[i]->direction.y * 100.0f;
    numbers_x.push_back(norm_x);
    numbers_y.push_back(norm_y);
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

vector<float> Simulation::createNNInputs(human *h, visible_information info)
{
  vector<float> result;
  result.push_back(info.closest_wall_distance);
  result.push_back(info.exit_distance);
  result.push_back(info.mean_age);
  result.push_back(info.mean_direction.x);
  result.push_back(info.mean_direction.y);
  result.push_back(info.mean_height);
  result.push_back(info.mean_panic);
  result.push_back(info.mean_radius);
  result.push_back(info.n_people);
  result.push_back(info.n_walls);
  result.push_back(info.var_age);
  result.push_back(info.var_direction.x);
  result.push_back(info.var_direction.y);
  result.push_back(info.var_height);
  result.push_back(info.var_panic);
  result.push_back(info.var_radius);
  result.push_back(h->direction.x * 100.0f);
  result.push_back(h->direction.y * 100.0f);
  result.push_back(((h->age - min_age) / (float)(max_age - min_age) - 0.5f) * 2.0f);
  result.push_back(((h->height - min_height) / (max_height - min_height) - 0.5f) * 2.0f);
  result.push_back((h->panic - 0.5f) * 2.0f);
  result.push_back(((h->radius - min_radius) / (float)(max_radius - min_radius) - 0.5f) * 2.0f);

  return result;
}

/** Normalises the visible information based on mix and max values from the config file to the range [-1,1] **/
/*visible_information Simulation::normaliseVision(visible_information info)
{
  info.n_people = ((info.n_people / (float)n_people) - 0.5f) * 2.0f;
  info.mean_height = ((info.mean_height - min_height) / (max_height - min_height) - 0.5f) * 2.0f;
  info.var_height = (info.var_height )
}*/























