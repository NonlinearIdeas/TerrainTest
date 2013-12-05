/********************************************************************
 * File   : MainScene.cpp
 * Project: Multiple
 *
 ********************************************************************
 * Created on 9/21/13 By Nonlinear Ideas Inc.
 * Copyright (c) 2013 Nonlinear Ideas Inc. All rights reserved.
 ********************************************************************
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must
 *    not claim that you wrote the original software. If you use this
 *    software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and
 *    must not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 */

#include "MainScene.h"
#include "Box2DDebugDrawLayer.h"
#include "GridLayer.h"
#include "DebugLinesLayer.h"
#include "Notifier.h"
#include "Viewport.h"
#include "MathUtilities.h"
#include "RanNumGen.h"
#include "GameWorldQuery.h"


MainScene::MainScene()
{
}

MainScene::~MainScene()
{
}


void MainScene::CreatePhysics()
{
   // Set up the viewport
   static const float32 worldSizeMeters = 100.0;
   
   // Initialize the Viewport
   Viewport::Instance().Init(worldSizeMeters);
   Viewport::Instance().SetScale(1.5);
   
   Vec2 gravity(0.0,-9.8);
   _world = new b2World(gravity);
   // Do we want to let bodies sleep?
   // No for now...makes the debug layer blink
   // which is annoying.
   //   _world->SetAllowSleeping(false);
   //   _world->SetContinuousPhysics(true);
}

bool MainScene::init()
{
   
   // Create physical world
   CreatePhysics();

   // Create the terrain
   CreateTerrain();
   
   // Adding the debug lines so that we can draw the path followed.
   addChild(DebugLinesLayer::create());
   
   // Box2d Debug
   addChild(Box2DDebugDrawLayer::create(_world));
   
   // Grid
   addChild(GridLayer::create());
   
   // Add User Input
   TapDragPinchInput* input = TapDragPinchInput::create(this);
   addChild(input);
   
   return true;
}

void MainScene::CreateTerrain()
{
   Viewport& vp = Viewport::Instance();
   
   // Get positions:
   CCSize worldSize = vp.GetWorldSizeMeters();
   float32 wWidth = worldSize.width;
   float32 wHeight = worldSize.height;
   float32 wallHeight = wHeight/8;
   b2BodyDef groundBodyDef;
   groundBodyDef.position.Set(0, -wallHeight); // bottom-left corner
   groundBodyDef.type = b2_staticBody;

   b2Body* groundBody = _world->CreateBody(&groundBodyDef);
   groundBody->SetUserData(NULL);
   
   // Now create the actual body fixtures.
   b2PolygonShape shape;
   
   b2FixtureDef fixtureDef;
   fixtureDef.shape = &shape;
   fixtureDef.density = 1.0;
   fixtureDef.friction = 1.0;
   fixtureDef.restitution = 0.1;

   vector<Vec2> vertices;
   // Bottom
   vertices.push_back(Vec2(-wWidth/2,0));
   vertices.push_back(Vec2(-wWidth/2,-.1));
   vertices.push_back(Vec2(wWidth/2,-.1));
   vertices.push_back(Vec2(wWidth/2,0));
   shape.Set(&vertices[0], vertices.size());
   groundBody->CreateFixture(&fixtureDef);
   vertices.clear();
   
   // Left
   vertices.push_back(Vec2(-wWidth/2 ,0));
   vertices.push_back(Vec2(-wWidth/2 ,wallHeight));
   vertices.push_back(Vec2(-wWidth/2-0.1 ,wallHeight));
   vertices.push_back(Vec2(-wWidth/2-0.1 ,0));
   shape.Set(&vertices[0], vertices.size());
   groundBody->CreateFixture(&fixtureDef);
   vertices.clear();
   
   // Right
   vertices.push_back(Vec2(wWidth/2 ,0));
   vertices.push_back(Vec2(wWidth/2+0.1 ,0));
   vertices.push_back(Vec2(wWidth/2+0.1 ,wallHeight));
   vertices.push_back(Vec2(wWidth/2 ,wallHeight));
   shape.Set(&vertices[0], vertices.size());
   groundBody->CreateFixture(&fixtureDef);
   vertices.clear();
}

void MainScene::UpdateTerrain()
{
   Viewport& vp = Viewport::Instance();
   CCSize worldSize = vp.GetWorldSizeMeters();
   float32 wWidth = worldSize.width;
   float32 wHeight = worldSize.height;
   
   const uint32 BALLS_MAX = 1000;
   const float32 BALL_RAD_MIN = 0.1;
   const float32 BALL_RAD_MAX = 0.9;
   
   static uint32 ballsLeft = BALLS_MAX;
   
   if(ballsLeft > 0)
   {  // If we have some left to create, then create them
      // in random positions and sizes.
      --ballsLeft;
      
      float32 xPos = RanNumGen::RandFloat(-wWidth*0.9/2, wWidth*0.9/2);
      float32 yPos = RanNumGen::RandFloat(wHeight/10, wHeight/2);
      
      b2BodyDef ballBodyDef;
      ballBodyDef.position = Vec2(xPos,yPos);
      ballBodyDef.type = b2_dynamicBody;
      
      b2Body* ballBody = _world->CreateBody(&ballBodyDef);
      
      // We'll just set the userdata for a ball to "this" so that
      // we can distinguish it later.
      ballBody->SetUserData(this);
      
      b2FixtureDef ballFixture;
      b2CircleShape ballShape;
      ballShape.m_radius = RanNumGen::RandFloat(BALL_RAD_MIN, BALL_RAD_MAX);
      ballFixture.density = 1.0;
      ballFixture.friction = 1.0;
      ballFixture.restitution = 0.1;
      ballFixture.shape = &ballShape;
      ballBody->CreateFixture(&ballFixture);
   }
}

void MainScene::DestroyTerrain(const Vec2& position)
{
   
}

MainScene* MainScene::create()
{
   MainScene *pRet = new MainScene();
   if (pRet && pRet->init())
   {
      pRet->autorelease();
      return pRet;
   }
   else
   {
      CC_SAFE_DELETE(pRet);
      return NULL;
   }
}

void MainScene::onEnter()
{
   CCScene::onEnter();
}

void MainScene::onExit()
{
   CCScene::onExit();
}

void MainScene::onEnterTransitionDidFinish()
{
   CCScene::onEnterTransitionDidFinish();
   // Schedule Updates
   scheduleUpdate();
}

void MainScene::onExitTransitionDidStart()
{
   CCScene::onExitTransitionDidStart();
   
   // Turn off updates
   unscheduleUpdate();
}


static void AdjustNodeScale(CCNode* node, float32 entitySizeMeters, float32 ptmRatio)
{
   CCSize nodeSize = node->getContentSize();
   float32 maxSizePixels = max(nodeSize.width,nodeSize.height);
   assert(maxSizePixels >= 1.0);
   float32 scale = (entitySizeMeters*ptmRatio/maxSizePixels);
   
   node->setScale(scale);
}


void MainScene::UpdatePhysics()
{
   const int velocityIterations = 8;
   const int positionIterations = 1;
   float32 fixedDT = SECONDS_PER_TICK;
   // Instruct the world to perform a single step of simulation. It is
   // generally best to keep the time step and iterations fixed.
   _world->Step(fixedDT, velocityIterations, positionIterations);
}

void MainScene::update(float dt)
{
   UpdatePhysics();
   UpdateTerrain();
}

void MainScene::TapDragPinchInputPinchBegin(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
   _viewportCenterOrg = Viewport::Instance().GetCenterMeters();
   _viewportScaleOrg = Viewport::Instance().GetScale();
   PinchViewport(GetPinchPoint0().pos, GetPinchPoint1().pos, point0.pos, point1.pos);
}
void MainScene::TapDragPinchInputPinchContinue(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
   PinchViewport(GetPinchPoint0().pos, GetPinchPoint1().pos, point0.pos, point1.pos);
}
void MainScene::TapDragPinchInputPinchEnd(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
}

void MainScene::TapDragPinchInputTap(const TOUCH_DATA_T& point)
{
   // Get the position in the physics engine.
   Viewport& vp = Viewport::Instance();
   Vec2 posPhysics = vp.Convert(point.pos);
   
   // Create an AABB around the point.
   AABB aabb;
   Vec2 bound(0.5,0.5);
   aabb.upperBound = posPhysics + bound;
   aabb.lowerBound = posPhysics - bound;
   
   // Create a query.
   GameWorldQuery query;
   _world->QueryAABB(&query,aabb);
   vector<b2Body*> bodies = query.GetBodies();
   for(int idx = 0; idx < bodies.size(); idx++)
   {
      // A bit of a hack here.  Usually, the userdata member
      // would be set to a controller class (e.g. Entity).
      // However, here, all the balls have a non-NULL userdata
      // and the static bodies have a NULL.  This lets us
      // know what we should NOT delete.
      if(bodies[idx]->GetUserData() != NULL)
      {
         _world->DestroyBody(bodies[idx]);
      }
   }
}


void MainScene::PinchViewport(const CCPoint& p0Org,const CCPoint& p1Org,
                              const CCPoint& p0,const CCPoint& p1)
{
   Viewport& vp = Viewport::Instance();
   float32 distOrg = ccpDistance(p0Org, p1Org);
   float32 distNew = ccpDistance(p0, p1);
   
   if(distOrg < 1)
      distOrg = 1;
   if(distNew < 1)
      distNew = 1;
   
   float32 scaleAdjust = distNew/distOrg;
   Vec2 centerOld = vp.Convert(ccpMidpoint(p0Org, p1Org));
   Vec2 centerNew = vp.Convert(ccpMidpoint(p0, p1));
   
   vp.SetCenter(_viewportCenterOrg-centerNew+centerOld);
   vp.SetScale(scaleAdjust*_viewportScaleOrg);
}
