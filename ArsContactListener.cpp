/*MIT License
*
*Copyright (c) 2018 Alysson Ribeiro da Silva
*
*Permission is hereby granted, free of charge, to any person obtaining a copy 
*of this software and associated documentation files (the "Software"), to deal 
*in the Software without restriction, including *without limitation the rights 
*to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
*copies of the Software, and to permit persons to whom the Software is furnished 
*to do so, subject *to the following conditions:
*
*The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
*EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. *IN NO EVENT SHALL THE AUTHORS 
*OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN 
*AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH 
*THE *SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//--------------------------LOCAL INCLUDES------------------------

//--------------------------GLOBAL INCLUDES-----------------------

//--------------------------MACROS--------------------------------

//--------------------------DIRECTIVES----------------------------
#include "ArsContactListener.h"
#include "2DObject.h"
#include "Ars.h"

void ArsContactListener::BeginContact( b2Contact * contact )
{
    _2DObject* obj;
    _2DObject* collider;
    void     * userData         = contact->GetFixtureA()->GetBody()->GetUserData();
    void     * colliderUserData = contact->GetFixtureB()->GetBody()->GetUserData();

    if(userData != NULL && colliderUserData != NULL){
        obj      = static_cast<_2DObject*>(userData);
        collider = static_cast<_2DObject*>(colliderUserData);

        obj     ->onCollisionEnter(collider);
        collider->onCollisionEnter(obj     );
    }
}
//----------------------------------------------------------------------------------------------
void ArsContactListener::EndContact  ( b2Contact * contact )
{
    _2DObject* obj;
    _2DObject* collider;
    void     * userData         = contact->GetFixtureA()->GetBody()->GetUserData();
    void     * colliderUserData = contact->GetFixtureB()->GetBody()->GetUserData();

    if(userData != NULL && colliderUserData != NULL){
        obj      = static_cast<_2DObject*>(userData);
        collider = static_cast<_2DObject*>(colliderUserData);

        obj     ->onCollisionEnd(collider);
        collider->onCollisionEnd(obj     );
    }
}
//----------------------------------------------------------------------------------------------
void ArsContactListener::PreSolve    ( b2Contact * contact, const b2Manifold      * oldManifold )
{
    _2DObject* obj;
    _2DObject* collider;
    void     * userData         = contact->GetFixtureA()->GetBody()->GetUserData();
    void     * colliderUserData = contact->GetFixtureB()->GetBody()->GetUserData();

    if(userData != NULL && colliderUserData != NULL){
        obj      = static_cast<_2DObject*>(userData);
        collider = static_cast<_2DObject*>(colliderUserData);

        obj     ->onPreSolve(collider, oldManifold);
        collider->onPreSolve(obj     , oldManifold);
    }
}
//----------------------------------------------------------------------------------------------
void ArsContactListener::PostSolve   ( b2Contact * contact, const b2ContactImpulse* impulse     )
{
    _2DObject* obj;
    _2DObject* collider;
    void     * userData         = contact->GetFixtureA()->GetBody()->GetUserData();
    void     * colliderUserData = contact->GetFixtureB()->GetBody()->GetUserData();

    if(userData != NULL && colliderUserData != NULL){
        obj      = static_cast<_2DObject*>(userData);
        collider = static_cast<_2DObject*>(colliderUserData);

        obj     ->onPostSolve(collider, impulse);
        collider->onPostSolve(obj     , impulse);
    }
}
