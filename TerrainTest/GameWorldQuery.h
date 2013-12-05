/********************************************************************
 * File   : GameWorldQuery.h
 * Project: Multiple
 *
 ********************************************************************
 * Created on 12/5/13 By Nonlinear Ideas Inc.
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

#ifndef __GameWorldQuery__
#define __GameWorldQuery__

#include "CommonSTL.h"
#include "CommonProject.h"

// This class is meant to be used for
// responding to AABB queries of the gameworld and
// returning the results found in that query.
//
// The class contains utility functions to
// return specific information, such as
// the "closest" body to a point or
// filter out duplicates.
//
// NOTE:  This class will automatically filter
// out duplicate bodies found during the
// query (which returns fixtures, not bodies).
class GameWorldQuery : public b2QueryCallback
{
private:
   vector<b2Body*> _bodies;
   vector<b2Fixture*> _fixtures;
   set<b2Body*> _bodySet;
public:
   
   void Reset()
   {
      _bodies.clear();
      _bodySet.clear();
      _fixtures.clear();
   }
   
   GameWorldQuery()
   {
      Reset();
   }
   
   b2Body* FindBodyNearest(const Vec2& pos)
   {
      b2Body* result = NULL;
      switch(_bodies.size())
      {
         case 0:
            return NULL;
         case 1:
            return _bodies[0];
            break;
         default:
         {
            float32 diffSq = FLT_MAX;
            for(int idx = 0; idx < _bodies.size(); idx++)
            {
               Vec2 toTarget = pos - _bodies[idx]->GetPosition();
               float32 tempDiffSq = toTarget.LengthSquared();
               if(tempDiffSq < diffSq)
               {
                  diffSq = tempDiffSq;
                  result = _bodies[idx];
               }
            }
         }
      }
      return result;
   }
   
   
   const vector<b2Body*>& GetBodies()
   {
      return _bodies;
   }

   const vector<b2Fixture*>& GetFixtures()
   {
      return _fixtures;
   }
   
   // Called by the physics engine to add bodies/fixtures
   // to this container.
   bool ReportFixture(b2Fixture* fixture)
   {
      b2Body* body = fixture->GetBody();
      _fixtures.push_back(fixture);
      
      // Some weak optimization.
      switch(_bodies.size())
      {
         case 0:
            _bodies.push_back(body);
            break;
         case 1:
            if(_bodies[0] != body)
            {
               _bodies.push_back(body);
               _bodySet.insert(body);
            }
            break;
         case 2:
            if(_bodies[0] != body &&
               _bodies[1] != body)
            {
               _bodies.push_back(body);
               _bodySet.insert(body);
            }
            break;
         case 3:
            if(_bodies[0] != body &&
               _bodies[1] != body &&
               _bodies[2] != body
               )
            {
               _bodies.push_back(body);
               _bodySet.insert(body);
            }
            break;
         default:
         {
            if(_bodySet.find(body) == _bodySet.end())
            {
               _bodies.push_back(body);
               _bodySet.insert(body);               
            }
         }
            break;
      }
      return true;
   }
};

#endif /* defined(__GameWorldQuery__) */
