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

#ifndef ARSMOUSEPICKOBJ_H_INCLUDED
#define ARSMOUSEPICKOBJ_H_INCLUDED

//--------------------------LOCAL INCLUDES------------------------
#include "2DObject.h"
//--------------------------GLOBAL INCLUDES-----------------------

//--------------------------MACROS--------------------------------

//--------------------------DIRECTIVES----------------------------



class ArsMousePickObj : public _2DObject
{
    void update(float time)
    {
        //TODO::putimpl
    }

    void draw  (float time)
    {
        //TODO::putimpl
    }

    /** Basic b2d collisition detection interfaces **/
    void onCollisionEnter(_2DObject* collider) {};
    void onCollisionEnd  (_2DObject* collider) {};
    void onPreSolve      (_2DObject* collider, const b2Manifold      * oldManifold ) {};
    void onPostSolve     (_2DObject* collider, const b2ContactImpulse* impulse     ) {};
};

#endif // ARSMOUSEPICKOBJ_H_INCLUDED
