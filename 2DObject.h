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

#ifndef A2DOBJECT_H_INCLUDED
#define A2DOBJECT_H_INCLUDED

//--------------------------LOCAL INCLUDES------------------------
#include "MathHelper.h"

//--------------------------GLOBAL INCLUDES-----------------------
#include <Box2D/Box2D.h>

//--------------------------MACROS--------------------------------

//--------------------------DIRECTIVES----------------------------

class _2DObject
{
public:
    _2DObject()
    {
        enable = true;
    }

    virtual ~_2DObject() {}

    /** Rigidbody **/
    b2Body* rigidbody;

    /** Collision tag **/
    int     tag;

    /** Enable option **/
    bool enable;

    /*
     * method <update>
     * param  <float>
     * description <basic real time behavior architecture update>
     */
    virtual void update(float time)
    {
        //TODO::putimpl
    }

    /*
     * method <draw>
     * param  <float>
     * description <basic real time behavior architecture update>
     */
    virtual void draw  (float time)
    {
        //TODO::putimpl
    }

    /*
    * method <onCollisionEnter>
    * param  <_2DObject*>
    * description <On collision enter B2d interface>
    */
    virtual void onCollisionEnter(_2DObject* collider)=0;

    /*
    * method <onCollisionEnd>
    * param  <_2DObject*>
    * description <On collision end B2d interface>
    */
    virtual void onCollisionEnd  (_2DObject* collider)=0;

    /*
    * method <onPreSolve>
    * param  <_2DObject* , b2Manifold*>
    * description <On on pre solve B2d interface>
    */
    virtual void onPreSolve      (_2DObject* collider, const b2Manifold      * oldManifold )=0;

    /*
    * method <onPostSolve>
    * param  <_2DObject*, b2ContactImpulse*>
    * description <On post solve B2d interface>
    */
    virtual void onPostSolve     (_2DObject* collider, const b2ContactImpulse* impulse     )=0;
};

#endif // 2DOBJECT_H_INCLUDED
