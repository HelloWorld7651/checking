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

    // Add points.
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

