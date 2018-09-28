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

#ifndef ARSCONTACTLISTENER_H_INCLUDED
#define ARSCONTACTLISTENER_H_INCLUDED

//--------------------------LOCAL INCLUDES------------------------

//--------------------------GLOBAL INCLUDES-----------------------
#include <Box2D/Box2D.h>

//--------------------------MACROS--------------------------------

//--------------------------DIRECTIVES----------------------------


class ArsContactListener : public b2ContactListener
{
public:
    /*
     * method <BeginContact>
     * param  <b2Contact*>
     * description <Used by b2d as contact detector callback>
     */
    void BeginContact( b2Contact * contact );

    /*
    * method <EndContact>
    * param  <b2Contact *>
    * description <Used by b2d as contact end detector callback>
    */
    void EndContact  ( b2Contact * contact );

    /*
    * method <PreSolve>
    * param  <b2Contact*, const b2Manifold*>
    * description <Used by b2d as pre solving callback>
    */
    void PreSolve    ( b2Contact * contact, const b2Manifold      * oldManifold );

    /*
    * method <PostSolve>
    * param  <b2Contact*, const b2ContactImpulse* >
    * description <Used by b2d as post solve callback>
    */
    void PostSolve   ( b2Contact * contact, const b2ContactImpulse* impulse     );
};

#endif // ARSCONTACTLISTENER_H_INCLUDED
