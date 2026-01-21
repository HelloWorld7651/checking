//
// bomb.cpp
//

// Engine includes.
#include "EventCollision.h"
#include "EventOut.h"
#include "EventView.h"
#include "GameManager.h"
#include "LogManager.h"
#include "WorldManager.h"

// Game includes.
#include "game.h"
#include "bomb.h"
#include "Points.h"
#include "Sword.h"
#include "util.h"

Bomb::Bomb() {
    //setup the bomb
    setType("bomb");
    if (setSprite("bomb") != 0)
        LM.writeLog("BOMB::BOMB(): Error! Unable to find sprite: bomb");

    first_spawn = true; // To ignore first out of bounds (when spawning).
    setSolidness(df::SOFT);
}

// Handle event.
int Bomb::eventHandler(const df::Event *p_e) {

  // Out of bounds event.
  if (p_e -> getType() == df::OUT_EVENT)
    return out((df::EventOut *) p_e);

  // Collision event.
  if (p_e -> getType() == df::COLLISION_EVENT)
    return collide((df::EventCollision *) p_e);

  // Not handled.
  return 0;
}

// Handle out events.
int Bomb::out(const df::EventOut *p_e) {

  if (first_spawn) { // Ignore first out (when spawning).
    first_spawn = false;
    return 1;
  }

  // Destroy this bomb.
  WM.markForDelete(this);

  // Handled.
  return 1;
}

// Handle collision events.
int Bomb::collide(const df::EventCollision *p_e) {

  // Sword collision means ninja sliced this bomb.
  if (p_e -> getObject1() -> getType() == SWORD_STRING) {

    // delete points.
    df::EventView ev(POINTS_STRING, -25, true);
    WM.onEvent(&ev);

    // Destroy this bomb.
    WM.markForDelete(this);
  }

  // Handled.
  return 1;
}

// Destructor.
Bomb::~Bomb() {

  // If inside the game world and engine not shutting down,
  // create explosion and play sound.
  if (df::boxContainsPosition(WM.getBoundary(), getPosition()) &&
      GM.getGameOver() == false) {
    df::explode(getAnimation().getSprite(), getAnimation().getIndex(), getPosition(),
                EXPLOSION_AGE, EXPLOSION_SPEED, EXPLOSION_ROTATE);

  }
}

// Setup starting conditions.
void Bomb::start(float speed) {

  df::Vector begin, end;

  // Get world boundaries.
  int world_x = (int) WM.getBoundary().getHorizontal();
  int world_y = (int) WM.getBoundary().getVertical();
  df::Vector world_center(world_x/2.0f, world_y/2.0f);

  // Pick random side (Top, Right, Bottom, Left) to spawn.
  switch (rand() % 4) {

  case 0: // Top.
    begin.setX((float) (rand()%world_x));
    begin.setY(0 - 3.0f);
    end.setX((float) (rand()%world_x));
    end.setY(world_y + 3.0f);
    break;

  case 1: // Right.
    begin.setX(world_x + 3.0f);
    begin.setY((float) (rand()%world_y));
    end.setX(0 - 3.0f);
    end.setY((float) (rand()%world_y));
    break;

  case 2: // Bottom.
    begin.setX((float) (rand()%world_x));
    begin.setY(world_y + 3.0f);
    end.setX((float) (rand()%world_x));
    end.setY(0 - 3.0f);
    break;
    
  case 3: // Left.
    begin.setX(0 - 3.0f);
    begin.setY((float) (rand()%world_y));
    end.setX(world_x + 3.0f);
    end.setY((float) (rand()%world_y));
    break;

  default:
    break;
  }

  // Move Object into position.
  WM.moveObject(this, begin);

  // Set velocity towards opposite side.
  df::Vector velocity = end - begin;
  velocity.normalize();
  setDirection(velocity);
  setSpeed(speed);
}
