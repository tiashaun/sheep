#include "CApp.h"
#include <iostream>
#include <math.h>
#include <algorithm>


// move all entity, report all collision, then resolve the list of collision even (for loop)
void CApp::OnLoop() {

  // switch time on and monitor framerate
  CFPS::FPSControl.OnLoop();
  MultitouchEvent::Controller.OnLoop(CFPS::FPSControl.getFrameDuration());

  // time between two loops
  double dt = CFPS::FPSControl.GetSpeedFactor();

  // apply gravity and controls on all entities
  for(std::vector<CEntity*>::iterator itEntity = CEntity::EntityList.begin(); itEntity != CEntity::EntityList.end(); ++itEntity) {
    if((*itEntity) == NULL) continue;
    //(*itEntity)->OnLoopApplyGravity();
    (*itEntity)->OnLoopApplyControls();
    (*itEntity)->OnLoopDeriveAndCapSpeed(dt);
  }

  // apply gravity and controls on all entities
  for(std::vector<CEntity*>::iterator itEntity = CEntity::EntityList.begin(); itEntity != CEntity::EntityList.end(); ++itEntity) {
    if((*itEntity) == NULL) continue;
    (*itEntity)->OnLoopMotionBounds();
  }

  for(std::vector<CEntity*>::iterator itEntity = CEntity::EntityList.begin(); itEntity != CEntity::EntityList.end(); ++itEntity) {
    if((*itEntity) == NULL) continue;
    (*itEntity)->OnMove((*itEntity)->getSpeed(), dt);
  }

  SolveCollisions(3, dt);

  // fill collision container by checking every speculative collisions
  for(std::vector<CEntity*>::iterator itEntity = CEntity::EntityList.begin(); itEntity != CEntity::EntityList.end(); ++itEntity) {
    if((*itEntity) == NULL) continue;
    (*itEntity)->OnLoopRealizeMotion();
  }

  int nbCollisionCheck = CEntityCol::EntityColList.size();;
  bool allCollisionResolved = false;
  CEntityCol::EntityColList.clear();
}

double sign(double value) {
  if(value >= 0) return 1;
  else return -1;
}


void CApp::SolveCollisions(int numIterations, double& dt) {



  for (int j=0; j<numIterations; j++) {

    // fill collision container by checking every speculative collisions
    CEntityCol::EntityColList.clear();
    for(std::vector<CEntity*>::iterator itEntity = CEntity::EntityList.begin(); itEntity != CEntity::EntityList.end(); ++itEntity) {
      if((*itEntity) == NULL) continue;
      (*itEntity)->PosValidOnEntities();
    }
    if(CEntityCol::EntityColList.empty()) { return; }

    for (std::map<std::pair<int, int>, CEntityCol>::iterator itEntityCol = CEntityCol::EntityColList.begin();
        itEntityCol != CEntityCol::EntityColList.end(); ++itEntityCol) {

      CEntityCol& contact = itEntityCol->second;

      if(contact._rectangle.isVoid()) { continue; }
      CEntity& EntityA = *(contact.EntityA);
      CEntity& EntityB = *(contact.EntityB);

      if(contact._rectangle.getWidth() >= contact._rectangle.getHeight()) { // vertical collision
        PointDouble repulsA = PointDouble(0, (sign(EntityA.getNextPosition().getY() - EntityB.getNextPosition().getY()) * contact._rectangle.getHeight())/2);
        PointDouble repulsB = PointDouble(0, -(sign(EntityA.getNextPosition().getY() - EntityB.getNextPosition().getY()) * contact._rectangle.getHeight())/2);
        EntityA.OnMove(repulsA, dt);
        EntityB.OnMove(repulsB, dt);
      } else { // latteral collision
        PointDouble repulsA = PointDouble((sign(EntityA.getNextPosition().getX() - EntityB.getNextPosition().getX()) * contact._rectangle.getWidth())/2, 0);
        PointDouble repulsB = PointDouble(-(sign(EntityA.getNextPosition().getX() - EntityB.getNextPosition().getX()) * contact._rectangle.getWidth())/2, 0);
        EntityA.OnMove( repulsA, dt);
        EntityB.OnMove( repulsB, dt);
      }
      contact.updateContactSize();
    }

  }

}

