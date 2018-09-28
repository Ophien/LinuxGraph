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

#ifndef ARS_H_INCLUDED
#define ARS_H_INCLUDED

/** COLLISION TAGS **/
#define GUI        1
#define BUTTON     2
#define MOUSE      3
#define GRAPH_NODE 4
#define GRAPH_EDGE 5

/** Button states **/
#define ARS_BTN_NORMAL  1
#define ARS_BTN_OVER    2
#define ARS_BTN_PRESSED 3

/** Image pivoting **/
#define ARS_CENTER       1
#define ARS_BOTTOM_LEFT  2
#define ARS_BOTTOM_RIGHT 3
#define ARS_TOP_LEFT     4
#define ARS_TOP_RIGHT    5

/** Operations **/
#define ARS_INSERT_VERTEX 1
#define ARS_INSERT_EDGE   2
#define ARS_MOVE_VERTEX   3
#define ARS_REMOVE_VERTEX 4
#define ARS_REMOVE_EDGE   5
#define ARS_LABEL         6
#define ARS_LABEL_EDGE    7
#define ARS_MARK_VERTEX   8

#define ARS_READ_FILE     0
#define ARS_SAVE_FILE     1
#define ARS_OP_DONE       2

#define ARS_TOTAL_VERTEX 50

#define ARS_TOTAL_CHARACTERS 500
#define ARS_MAX_EDGE_WEIGHT 99999999
#define ARS_MAX_WEIGHT_SIZE 7

#define CANNOT_LOAD_IMAGE -200
#define CANNOT_LOAD       -201

#define CANNOT_DRAW_TEXTURE -300
#define CANNOT_DRAW         -301

typedef void (*arsSimpleCallback)();

//--------------------------LOCAL INCLUDES------------------------
#include "ArsImage.h"
#include "MathHelper.h"
#include "ArsContactListener.h"
#include "ArsMousePickObj.h"
#include "ArsButton.h"
#include "ArsVertex.h"
#include "Render.h"
#include "Ars.h"

//--------------------------GLOBAL INCLUDES-----------------------
/** Image manipulation **/
#include <IL/il.h>      //DevIL
#include <IL/ilu.h>     //DevIL
#include <IL/ilut.h>    //DevIL

/** OpenGL extension **/
#include <GL/glew.h>    //GL extension glew

/** Stantard library **/
#include <stdlib.h>     //Free, memory acess
#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <stack>
#include <vector>

/** 2Dworld simulation **/
#include <Box2D/Box2D.h>

/** texture font **/
#include <FTGL/ftgl.h>

/** SubWindow system **/
#include <gtk/gtk.h>

//--------------------------MACROS--------------------------------

//--------------------------DIRECTIVES----------------------------

//--------------------------NAMESPACES----------------------------

using namespace std;

/** Pre creation **/
class ArsVertex;

namespace Ars
{
/** Main program scene control variables **/
extern std::stack<ArsVertex*> _availableVertex;
extern ArsVertex*             _sceneVertex[ARS_TOTAL_VERTEX];
extern int                    _vertexIdGenerator;

extern std::vector<_2DObject*> _preScene;
extern std::vector<_2DObject*> _scene   ;

/** Box2D world control variables **/
extern b2World       * _world;
extern float32         _timeStep;
extern int32           _velocityIterations;
extern int32           _positionIterations;

/** Main program control variables, handle operations **/
extern int _currentOp;
extern ArsVertex* _opVertex;

extern bool       _canCreateEdge;
extern bool       _drawArrow;
extern bool       _insertion;
extern bool       _removed;
extern bool       _labelB;
extern bool       _usePhysics;
extern bool       _separateEdges;
extern bool       _disableLabelsB;
extern bool       _disableEdgeWeight;

extern ArsVertex* _out;
extern ArsVertex* _in;

/** Debug, TODO::remove later **/
extern  ArsImage      * _testImage;
extern  ArsImage      * _fileButton;
extern  ArsImage      * _opButton;
extern  ArsImage      * _plusbtn;
extern  ArsImage      * _minusbtn;
extern  ArsImage      * _background;
extern  ArsImage      * _graphNode;
extern  ArsImage      * _insBtn;
extern  ArsImage      * _insertVerIco;
extern  ArsImage      * _moveVerIco;
extern  ArsImage      * _insertEdgeIco;
extern  ArsImage      * _rmver;
extern  ArsImage      * _rmedge;
extern  ArsImage      * _complete;
extern  ArsImage      * _digraph;
extern  ArsImage      * _label;
extern  ArsImage      * _newImg;
extern  ArsImage      * _saveIco;
extern  ArsImage      * _loadIco;
extern  ArsImage      * _physics;
extern  ArsImage      * _disablePhysics;
extern  ArsImage      * _separateEdge;
extern  ArsImage      * _disableSeparateEdge;
extern  ArsImage      * _disableLabels;
extern  ArsImage      * _disableLabelsD;
extern  ArsImage      * _labelEdge;
extern  ArsImage      * _drawWeights;
extern  ArsImage      * _markVertex;
extern  ArsImage      * _completeZero;
extern  ArsImage      * _disableWeightX;

/** Main UI components **/
extern  ArsButton     * _newGraph;
extern  ArsButton     * _save;
extern  ArsButton     * _loadFile;
extern  ArsButton     * _insVertBtn;
extern  ArsButton     * _moveVerBtn;
extern  ArsButton     * _rmEdgeBtn;
extern  ArsButton     * _insertEdgeBtn;
extern  ArsButton     * _rmVertexBtn;
extern  ArsButton     * _labelBtn;
extern  ArsButton     * _completeBtn;
extern  ArsButton     * _digraphBtn;
extern  ArsButton     * _usePhysicsBtn;
extern  ArsButton     * _separateEdgeBtn;
extern  ArsButton     * _disableLabelsBtn;
extern  ArsButton     * _labelEdgeBtn;
extern  ArsButton     * _drawWeightsBtn;
extern  ArsButton     * _markVertexBtn;
extern  ArsButton     * _completeZeroBtn;

/** Ars simple Gui holder **/
extern  vector<ArsButton*> _buttons;

/** Open/Close file GTK+ **/
extern  GtkWidget       * _windowWidget;
extern  GtkWidget       * _diagWidget;
extern  GtkDialog       * _diag;

/** Keyboard handler **/

/** Button state variables **/
extern  char     _lastSaved[];
extern  char     _pressedC;
extern  int      _charPressed        [ARS_TOTAL_CHARACTERS];
extern  int      _charPressedPrevious[ARS_TOTAL_CHARACTERS];

/** Mouse handler ***/

/** Button state variables **/
extern  int      _mouseButtonState        [];
extern  int      _mousePreviousButtonState[];

extern  Vector2f _mousePosition2D_window;      // mouse window position
extern  Vector2f _mousePosition2D_world;      // mouse world position
extern  ArsMousePickObj*  _mouseClickObject; // use for mouse collision with things

/** Control helper **/
extern  string _contentPath; //used for file loading from default directory
extern  void* _win;          //used for procedures that uses a glfw window reference
extern  float rotAngle;      //used to produce motion into execution quads at right bottom corner of the screen
extern  FTFont* _font;       //used for font drawing
extern  FTFont* _fontSimple; //used for warnings drawing
extern  int     _marks;      //Identify marks
extern  float   _deltaTime;
extern  b2MouseJoint* _mouseJointE;
extern  int _windowWidth;
extern  int _windowHeight;
/*
 * method <genVertexId>
 * param  <>
 * description <Generate ID to a vertex inserted into the scene>
 */
extern int genVertexId();

/*******************************************************************************************************************/
/** File manager related functions**/
/*
 * method <saveInFile>
 * param  <const char*>
 * description <Save current scenegraph into file, create label>
 */
extern  void saveInFile(const char* fileName);

/*
 * method <loadGraphFromFile>
 * param  <const char*>
 * description <Load graph from file>
 */
extern  void loadGraphFromFile(const char* fileName);

/*
 * method <directCreateGraph>
 * param  <>
 * description <Delete previous graph in scene and create a new one>
 */
extern  void directCreateGraph();

/*
 * method <_fileExists>
 * param  <const std::string>
 * description <Verify if file already exists>
 */
extern  bool _fileExists (const std::string& name);

/*
 * method <_get_filename_ext>
 * param  <const char*>
 * description <Return a file extension string without file name>
 */
extern  const char *_get_filename_ext(const char *filename);

/*
 * method <_get_filename>
 * param  <const char*>
 * description <Return file name string without file extension>
 */
extern  const char *_get_filename(const char* mystr);

/*******************************************************************************************************************/
/** GTK Window manager **/

/*
 * method <createSaveWindow>
 * param  <>
 * description <Create a save file GTK+ window>
 */
extern  int createSaveWindow    ();

/*
 * method <createLoadWindow>
 * param  <>
 * description <Create a load file GTK+ window>
 */
extern  int createLoadWindow    ();

/*
 * method <acceptGraph>
 * param  <>
 * description <Accept operation of creating a new graph, used in GTK+ Dialog when creating a new graph>
 */
extern  void acceptGraph        ();

/*
 * method <rejectGraph>
 * param  <>
 * description <Reject operation of creating a new graph, used in GTK+ Dialog when creating a new graph>
 */
extern  void rejectGraph        ();

/*
 * method <loadInFile>
 * param  <const char*>
 * description <Called by GTK+ Dialog when loading a new graph, helps with some operations>
 */
extern  void loadInFile         (const char* filename);

/*
 * method <message>
 * param  <const char*, void(*), void(*), void(*)>
 * description <Creates a new GTK+ dialog with 3 op buttons: YES, NO and CANCEL>
 */
extern  void message            (const char *message, void (*accept)() , void (*no)(), void (*reject)() );


/*******************************************************************************************************************/
/** ARS UI manager **/

/*
 * method <onClickNew>
 * param  <>
 * description <Use by ARSUI graph creation dialog callbacks to create a new graph>
 */
extern  void onClickNew         ();

/*
 * method <onClickSave>
 * param  <>
 * description <Used by ARSUI graph saving dialog callbacks to save the current scene graph>
 */
extern  void onClickSave        ();

/*
 * method <onClickLoad>
 * param  <>
 * description <Used by ARSUI graph loading dialog callbacks to load a new graph into the scene>
 */
extern  void onClickLoad        ();

/*
 * method <onClickSeparateEdge>
 * param  <>
 * description <Used by ARSUI to activate or deactivate edge contrast>
 */
extern  void onClickSeparateEdge();

/*
 * method <onClickPhysics>
 * param  <>
 * description <Used by ARSUI to activate or deactivate in program physics>
 */
extern  void onClickPhysics     ();

/*
 * method <onClickInsertVertex>
 * param  <>
 * description <Used by ARSUI to insert a new vertex into the current scene>
 */
extern  void onClickInsertVertex();

/*
 * method <onClickInsertEdge>
 * param  <>
 * description <Used by ARSUI to insert a new edge into the current graph>
 */
extern  void onClickInsertEdge  ();

/*
 * method <onClickMoveVertex>
 * param  <>
 * description <Used by ARSUI to move the current vertex>
 */
extern  void onClickMoveVertex  ();

/*
 * method <onClickRmVertex>
 * param  <>
 * description <Used by ARSUI to remove vertex operation>
 */
extern  void onClickRmVertex    ();

/*
 * method <onClickRmEdge>
 * param  <>
 * description <Used by ARSUI to remove edge operation>
 */
extern  void onClickRmEdge      ();

/*
 * method <onClickLabel>
 * param  <>
 * description <Used by ARSUI to insert a label into vertex>
 */
extern  void onClickLabel       ();

/*
 * method <onClickComplete>
 * param  <>
 * description <Used by ARSUI to create a complete graph with the current scene vertex configuration with random weights>
 */
extern  void onClickComplete    ();

/*
 * method <onClickCompleteZero>
 * param  <>
 * description <Used by ARSUI to create a complete graph with the current scene vertex configuration and no weights>
 */
extern void onClickCompleteZero ();

/*
 * method <onClickDag>
 * param  <>
 * description <Used by ARSUI to turn on or off the graph edge direction>
 */
extern  void onClickDag         ();

/*
 * method <onClickDisableLabels>
 * param  <>
 * description <Used by ARSUI to turn on or off the graph labels>
 */
extern void onClickDisableLabels();

/*
 * method <onClickLabelEdge>
 * param  <>
 * description <Used by ARSUI to label an edge>
 */
extern void onClickLabelEdge();

/*
 * method <onClickMarkVertex>
 * param  <>
 * description <Used to mark a vertex for coloring>
 */
extern void onClickMarkVertex();

/*
 * method <onClickDrawWeight>
 * param  <>
 * description <Used to disable weights>
 */
extern void onClickDrawWeight();

/*******************************************************************************************************************/
/** GLFW Related managers **/

/*
 * method <mouseClick>
 * param  <>
 * description <Used by GLFW as mouse click callback>
 */
extern  void mouseClick       ();

/*
 * method <mouseMoveExternal>
 * param  <>
 * description <Used by GLFW as mouse move callback>
 */
extern  void mouseMoveExternal();

/*******************************************************************************************************************/
/** Main program functions draw/update related **/

/*
 * method <updateAllObjects>
 * param  <>
 * description <Update all simple objetcs>
 */
extern  void updateAllObjects();

/*
 * method <drawAllObjects>
 * param  <>
 * description <Draw all simple objetcs>
 */
extern  void drawAllObjects  ();

/*
 * method <drawBackground>
 * param  <>
 * description <Draw some background>
 */
extern  void drawBackground();

/*
 * method <drawMainGui>
 * param  <>
 * description <Draw all ARSUI>
 */
extern  void drawMainGui();

/*
 * method <drawExecutingQuads>
 * param  <>
 * description <Draw execution quads into right bottom corner of the screen, it helps to keep an eye into execution flow>
 */
extern  void drawExecutingQuads();

/*
 * method <setGlDrawFlags>
 * param  <>
 * description <Set some flags for drawing>
 */
extern  void setGlDrawFlags();

/*******************************************************************************************************************/
/** Main program functions load/configura related **/

/*
 * method <loadAllLibraries>
 * param  <>
 * description <Executa all libraries init methods>
 */
extern  void loadAllLibraries();

/*
 * method <configureDefaultFont>
 * param  <>
 * description <Configura default font used by the program>
 */
extern  void configureDefaultFont();

/*
 * method <loadTextures>
 * param  <>
 * description <Load all textures>
 */
extern  void loadTextures();

/*
 * method <configura2DWorld>
 * param  <>
 * description <Configura and load all b2D library >
 */
extern  void configure2DWorld();

/*
 * method <createWorldLimits>
 * param  <>
 * description <Create and manipulates all world limits>
 */
extern  void createWorldLimits();

/*
 * method <configureGuiButtons>
 * param  <>
 * description <Configure all ARSUI elements>
 */
extern  void configureGuiButtons();

/*
 * method <loadAllGraphBuffers>
 * param  <>
 * description <Load all scenegraphs, pre buffer the vertexs>
 */
extern  void loadAllGraphBuffers();

/*
 * method <loadSecondaryGuiSystem>
 * param  <>
 * description <Load all GTK+ system, and helpers>
 */
extern  void loadSecondaryGuiSystem();

/*******************************************************************************************************************/
/** Main basic **/

/*
 * method <load>
 * param  <>
 * description <General loader called by main>
 */
extern void load();

/*
 * method <loop>
 * param  <>
 * description <General loop called by main>
 */
extern void loop();

/*
 * method <update>
 * param  <>
 * description <General update function >
 */
extern void update();

/*
 * method <draw>
 * param  <>
 * description <General Draw method>
 */
extern void draw();

/*******************************************************************************************************************/
/** Basic draw helpers **/

/*
 * method <drawSphere>
 * param  <float, Vector3f,float,Color,bool>
 * description <Draw a sphere at specified position, scale and color>
 */
extern int drawSphere(float radius, Vector3f position, float scale, Color color, bool wired);

/*
 * method <drawImage>
 * param  <int,int,int,Color,Vector2f,int,float>
 * description <Draw an texture, with custom width height, color, position, pivot position and scale>
 */
extern int drawImage(int texId, int width, int height, Color color, Vector2f position, int pivot, float scale, int layer = 0);

/*
 * method <drawLine>
 * param  <Vector3f, Vector3f, Color, float>
 * description <Draw a simple line between two points, with a especified color and tickness>
 */
extern int drawLine(Vector3f out, Vector3f in, Color color, float width);

/*
 * method <drawArrow>
 * param  <b2Vec2, b2Vec2, Color>
 * description <Draw an arrow from out to in and with specified color>
 */
extern int drawArrow(b2Vec2 out, b2Vec2 in, Color color);

/*
 * method <drawRectangle>
 * param  <int,int,Color,Vector2f, float>
 * description <Draw an rectangle with some width height, color, at a specified position and scale>
 */
extern int drawRectangle(int width, int height, Color color, Vector2f position, float scale);

/*
 * method <drawTriangle>
 * param  <int width, height, Color, Vector2f, float>
 * description <Draw a triangle with some width height, color, specified position and scale>
 */
extern int drawTriangle(int width, int height, Color color, Vector2f position, float scale);

/*
 * method <draw2DText>
 * param  <void*, Vector2f, Color, const char*>
 * description <Draw some 2D text at specified position>
 */
extern int draw2DText(void* font, Vector2f position, Color color, const char* text);

/*
 * method <startHud>
 * param  <>
 * description <Used by hud drawing>
 */
extern int startHud();

/*
 * method <endHud>
 * param  <>
 * description <Used to end hud drawing>
 */
extern int endHud();

/*
* method <VerifySignal>
* param  <const int>
* description <Used to treat error::DREPECATED>
*/
extern void VerifySignal(const int code);

/*
* method <createMousePickingObject>
* param  <b2World&>
* description <Create a mouse picking object to detect collision with b2d>
*/
extern void createMousePickingObject (b2World& world);

/*
* method <destroyMossePickingObject>
* param  <b2World&>
* description <Used to destrou mouse picking object>
*/
extern void destroyMossePickingObject(b2World& world);
}
#endif // ARS_H_INCLUDED
