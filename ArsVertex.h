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

#ifndef ARSVERTEX_H_INCLUDED
#define ARSVERTEX_H_INCLUDED

//--------------------------LOCAL INCLUDES------------------------
#include "2DObject.h"
#include "ArsImage.h"
#include "Ars.h"

//--------------------------GLOBAL INCLUDES-----------------------
#include <Box2D/Box2D.h>
#include <vector>
#include <string>
//--------------------------MACROS--------------------------------

//--------------------------DIRECTIVES----------------------------




using namespace std;

class ArsVertex : public _2DObject
{
public:

    ArsVertex (ArsImage* texture,
               b2Body * body,
               Color normal ,
               Color pressed,
               Color over   ,
               float scale);

    ~ArsVertex();

    /** Basic behavioral architecture methods **/
    arsSimpleCallback onClick;
    arsSimpleCallback onRelease;

    void preDraw(float time);
    void posDraw(float time);
    void draw   (float time);
    void update (float time);

    /** Basic b2d collisition detection interfaces **/
    void onCollisionEnter(_2DObject* collider);
    void onCollisionEnd  (_2DObject* collider);
    void onPreSolve      (_2DObject* collider, const b2Manifold      * oldManifold ) {};
    void onPostSolve     (_2DObject* collider, const b2ContactImpulse* impulse     ) {};

    /*
     * method <setIcon>
     * param  <ArsImage*, float>
     * description <Set vetex icon with some scale>
     */
    void setIcon(ArsImage* icon, float iconScale);

    /*
     * method <insertEdge>
     * param  <int>
     * description <Insert an edge into this vertex>
     */
    void insertEdge      (int in, int weight, bool direct);

    /*
    * method <removeEdge>
    * param  <int>
    * description <Remove an edge from this vertex>
    */
    void removeEdge      (int in, bool direct);

    /*
    * method <getEdgeWeight>
    * param  <int>
    * description <Return edge weight, -1 if doesn`t exists>
    */
    int getEdgeWeight    (int in, bool direct);

    /*
    * method <clearNullEdges>
    * param  <>
    * description <DEPRECATED>
    */
    void clearAllEdges  ();

    /*
    * method <setLabel>
    * param  <char*>
    * description <Set a string into the label of this vetex>
    */
    void setLabel        (const char* label);

    /*
     * method <removeVertex>
     * param  <>
     * description <Remove this vertex from scenegraph>
     */
    void removeVertex    ();

    void setEdgeWeight(int in, int weight, bool direct);

    /*
     * method <getLabel>
     * param  <>
     * description <Return the vertex label>
     */
    string getLabel();

    /*
     * method <hasEdgeTo>
     * param  <int>
     * description <Verify if it has an edge to specified vertex>
     */
    bool hasEdgeTo       (int in, bool direct);

    /*
     * method <getState>
     * param  <>
     * description <Return vertex state: selected, over...>
     */
    int getState();

    /*
     * method <getId>
     * param  <>
     * description <Return vertex ID>
     */
    int  getId();

    /*
     * method <isSelected>
     * param  <>
     * description <Return true if this vertex is selected on scenegraph>
     */
    bool isSelected();

    void setDirectedEdgeWeight(int in, int weight);
    void setNonEdgeWeight     (int in, int weight);

    void mark();

    bool isMarked()
    {
        return marked;
    }

    bool isDrawed(){
        return drawed;
    }

    void markDrawed(bool d){
        drawed = d;
    }

private:
    string labelV;

    /** Marked, to mark the vertex for better visualization **/
    bool marked;

    /** To select the vertex to operate **/
    bool selected;

    bool drawed;

    /** Button texture image **/
    ArsImage* texture;

    /** Button icon **/
    ArsImage* icon;


    /** Colors to use in button states  **/
    Color     normal;
    Color     pressed;
    Color     over;
    Color     overlap;

    /** Draw control **/
    float     iconScale;
    float     scale;
    int       state;

    /** Vertex ID **/
    int ID;

    /** Edges list **/
    int edges[ARS_TOTAL_VERTEX];

    /** Directed graph edges **/
    int edgesDirected[ARS_TOTAL_VERTEX];
};


#endif // ARSVERTEX_H_INCLUDED
