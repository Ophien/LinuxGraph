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
#include "ArsImage.h"
#include "MathHelper.h"
#include "ArsContactListener.h"
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
#include <GLFW/glfw3.h>

/** Stantard library **/
#include <stdlib.h>     //Free, memory acess
#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <stack>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

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

namespace Ars
{
/** Physics world setup, used by box2D **/
b2World       * _world;
float32         _timeStep;
int32           _velocityIterations;
int32           _positionIterations;

/** Main program control variables, handle operations **/
int _currentOp  = ARS_INSERT_VERTEX;

bool _canCreateEdge       = true;
bool _insertion           = false;
bool _removed             = false;
bool _drawArrow           = false;
bool _labelB              = false;
bool _usePhysics          = true;
bool _separateEdges       = true;
bool _disableLabelsB      = true;
bool _disableEdgeWeight   = false;

ArsVertex* _opVertex      = 0x0;
ArsVertex* _out           = 0x0;
ArsVertex* _in            = 0x0;

/** Main program scene control variables **/
std::stack<ArsVertex*> _availableVertex;
ArsVertex*             _sceneVertex[ARS_TOTAL_VERTEX];
int                    _vertexIdGenerator = ARS_TOTAL_VERTEX-1;

std::vector<_2DObject*> _preScene;
std::vector<_2DObject*> _scene;


/**--------------------- Debug, TODO::remove later, transform into texture manager ------------------------**/
ArsImage      * _testImage;
ArsImage      * _fileButton;
ArsImage      * _opButton;
ArsImage      * _plusbtn;
ArsImage      * _minusbtn;
ArsImage      * _background;
ArsImage      * _graphNode;
ArsImage      * _insBtn;
ArsImage      * _insertVerIco;
ArsImage      * _moveVerIco;
ArsImage      * _insertEdgeIco;
ArsImage      * _rmver;
ArsImage      * _rmedge;
ArsImage      * _complete;
ArsImage      * _digraph;
ArsImage      * _label;
ArsImage      * _newImg;
ArsImage      * _saveIco;
ArsImage      * _loadIco;
ArsImage      * _physics;
ArsImage      * _disablePhysics;
ArsImage      * _separateEdge;
ArsImage      * _disableSeparateEdge;
ArsImage      * _disableLabels;
ArsImage      * _disableLabelsD;
ArsImage      * _labelEdge;
ArsImage      * _drawWeights;
ArsImage      * _markVertex;
ArsImage      * _completeZero;
ArsImage      * _disableWeightX;

/** Main UI components **/
ArsButton     * _newGraph;
ArsButton     * _save;
ArsButton     * _loadFile;
ArsButton     * _insVertBtn;
ArsButton     * _moveVerBtn;
ArsButton     * _rmEdgeBtn;
ArsButton     * _insertEdgeBtn;
ArsButton     * _rmVertexBtn;
ArsButton     * _labelBtn;
ArsButton     * _completeBtn;
ArsButton     * _digraphBtn;
ArsButton     * _usePhysicsBtn;
ArsButton     * _separateEdgeBtn;
ArsButton     * _disableLabelsBtn;
ArsButton     * _labelEdgeBtn;
ArsButton     * _drawWeightsBtn;
ArsButton     * _markVertexBtn;
ArsButton     * _completeZeroBtn;
/**--------------------- Debug, TODO::remove later, transform into texture manager ------------------------**/

/** Ars simple Gui holder **/
vector<ArsButton*> _buttons;

/** Open/Close file GTK+ **/
GtkWidget       * _windowWidget = NULL;
GtkWidget       * _diagWidget;
GtkDialog       * _diag;

/** Keyboard handler **/

/** Button state variables **/
char     _lastSaved[1024];
char     _pressedC;
int      _charPressed        [ARS_TOTAL_CHARACTERS];
int      _charPressedPrevious[ARS_TOTAL_CHARACTERS];

/** Mouse handler ***/

/** Button state variables **/
int      _mouseButtonState        [3];
int      _mousePreviousButtonState[3];

Vector2f _mousePosition2D_window(0,0);      // mouse window position
Vector2f _mousePosition2D_world (0,0);      // mouse world position
ArsMousePickObj*  _mouseClickObject = NULL; // use for mouse collision with things

/** Control helper **/
string _contentPath; //used for file loading from default directory
void* _win;          //used for procedures that uses a glfw window reference
float rotAngle;      //used to produce motion into execution quads at right bottom corner of the screen
FTFont* _font;       //used for font drawing
FTFont* _fontSimple;
int _marks = 0;
float _deltaTime = 0.0f;
b2MouseJoint* _mouseJointE;

int _windowWidth;
int _windowHeight;

//----------------------------------------------------------------------------------------------
int genVertexId()
{
    int toRet = _vertexIdGenerator;
    _vertexIdGenerator--;
    return toRet;
}
//----------------------------------------------------------------------------------------------
ArsVertex* buildVertex(ArsImage* texture, b2World* world, Vector2f position, Color normal, Color over, Color selected, float scale)
{
    b2BodyDef bodyDef;
    b2FixtureDef fixture;
    b2CircleShape shape;

    bodyDef.position.Set(position.x,position.y);
    bodyDef.type          = b2_kinematicBody;
    bodyDef.fixedRotation = true;

    shape.m_radius   = 25.0f;
    fixture.shape    = &shape;
    fixture.density  = 1.0f;
    fixture.friction = 0.3f;

    b2Body* body = world->CreateBody(&bodyDef);
    body->CreateFixture(&fixture);

    ArsVertex* obj = new ArsVertex(texture, body, normal, selected, over, scale);

    obj->rigidbody->SetUserData((void*)obj);

    obj->tag = GRAPH_NODE;

    return obj;
}
//----------------------------------------------------------------------------------------------
ArsButton* buildButton(ArsImage* insBtn, b2World* world, Vector2f position, float scale)
{
    ArsButton* insVertBtn;

    b2BodyDef def;
    def.type        = b2_kinematicBody;
    def.position.Set(position.x, position.y);
    b2Body* btnBody = world->CreateBody(&def);

    /** Create fixture and shape configuration **/
    b2PolygonShape btnShape;
    btnShape.SetAsBox(insBtn->getWidth() * scale / 2, insBtn->getHeight() * scale / 2);

    /** Configure fixture **/
    b2FixtureDef fix;
    fix.shape    = &btnShape;
    fix.isSensor = true;

    /** Attach fixture to the body **/
    btnBody->CreateFixture(&fix);

    insVertBtn = new ArsButton(insBtn,
                               btnBody,
                               Color(1.0f,1.0f,1.0f,1.0f),
                               Color(0.8f,0.8f,0.8f,1.0f),
                               Color(0.5f,0.5f,0.5f,1.0f),
                               scale);

    btnBody->SetSleepingAllowed(false);

    /** Set data into rigidbody **/
    btnBody->SetUserData((void*)insVertBtn);

    return insVertBtn;
}
//----------------------------------------------------------------------------------------------
bool fileExists (const std::string& name)
{
    if (FILE *file = fopen(name.c_str(), "r"))
    {
        fclose(file);
        return true;
    }
    else
    {
        return false;
    }
}
//----------------------------------------------------------------------------------------------
const char *get_filename_ext(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}
//----------------------------------------------------------------------------------------------
const char *get_filename(const char* mystr)
{
    char *retstr;
    char *lastdot;
    if (mystr == NULL)
        return NULL;
    if ((retstr = (char*)malloc (strlen (mystr) + 1)) == NULL)
        return NULL;
    strcpy (retstr, mystr);
    lastdot = strrchr (retstr, '.');
    if (lastdot != NULL)
        *lastdot = '\0';
    return retstr;
}
//----------------------------------------------------------------------------------------------
/** File Operations **/
void saveInFile(const char* fileName)
{
    /** Configura arquivo padrao de trabalho **/
    strcpy(_lastSaved, fileName);

    /** Create File for labels **/
    FILE *fp;

    char arsgFile [1024];
    strcpy(arsgFile , fileName);

    /** Grant extension to file **/
    if(strcmp("arsg", get_filename_ext(fileName)) != 0)
    {
        //Concat file extension
        strcat(arsgFile, ".arsg");
    }

    /** Openfile and save **/
    fp = fopen(arsgFile, "w");

    /** Id simplification **/
    int realId[ARS_TOTAL_VERTEX];

    /** Vertex count and directed graph flag **/
    int flag = 0;
    if(_drawArrow)
        flag = 1;

    int vertexCount = ARS_TOTAL_VERTEX - _availableVertex.size();

    /** Write the header of the file **/
    fprintf(fp, "%d %d\n", vertexCount, flag);

    /** Check ID`s and create a ordered vertex ID buffer **/
    int IDr = 0;
    for(int i = 0; i < ARS_TOTAL_VERTEX; i++)
    {
        if(_sceneVertex[i] != NULL)
        {
            /** Update IDs to use later **/
            realId[IDr] = i;
            IDr++;
        }
    }

    /** Now save the graph with new IDs **/
    /** If non directed **/
    if(flag == 0)
    {
        for(int i = 0; i < vertexCount; i++)
        {
            for(int j = i+1; j < vertexCount; j++)
            {
                if(_sceneVertex[realId[i]]->hasEdgeTo(realId[j], _drawArrow))
                {
                    int weight = _sceneVertex[realId[i]]->getEdgeWeight(realId[j], _drawArrow);

                    if(j + 1 == vertexCount)
                        fprintf(fp, "%d", weight);
                    else
                        fprintf(fp, "%d ", weight);
                }
                else
                {
                    if(j + 1 == vertexCount)
                        fprintf(fp, "%d", -1);
                    else
                        fprintf(fp, "%d ", -1);
                }
            }//ENDFOR
            if(i + 1 != vertexCount)
                fprintf(fp, "\n");
        }//ENDFOR
    }//END IF
    else /** Directed graph **/
    {
        for(int i = 0; i < vertexCount; i++)
        {
            for(int j = 0; j < vertexCount; j++)
            {
                if(_sceneVertex[realId[i]]->hasEdgeTo(realId[j], _drawArrow))
                {
                    int weight = _sceneVertex[realId[i]]->getEdgeWeight(realId[j], _drawArrow);

                    if(j + 1 == vertexCount)
                        fprintf(fp, "%d", weight);
                    else
                        fprintf(fp, "%d ", weight);
                }
                else
                {
                    if(j + 1 == vertexCount)
                        fprintf(fp, "%d", -1);
                    else
                        fprintf(fp, "%d ", -1);
                }
            }//ENDFOR
            fprintf(fp, "\n");
        }
    }//END ELSE

    /** Now write the labels and vertex positions into .args file **/
    for(int i = 0; i < vertexCount; i++)
    {
        float x = _sceneVertex[realId[i]]->rigidbody->GetPosition().x;
        float y = _sceneVertex[realId[i]]->rigidbody->GetPosition().y;

        string label = _sceneVertex[realId[i]]->getLabel();

        fprintf(fp, "%f %f @%s\n", x, y, label.c_str());
    }

    fclose(fp);
}
//----------------------------------------------------------------------------------------------
void loadGraphFromFile(const char* fileName)
{
    /** Try to load the current file with label extension **/
    if(strcmp("arsg", get_filename_ext(fileName)) != 0)
    {
        return;
    }

    /** Readed string **/
    string rd;

    char asrgFile[1024];
    strcpy(asrgFile, get_filename(fileName));
    strcat(asrgFile, ".arsg");

    ifstream fileArsg (asrgFile, ifstream::in);

    /** Get vertex count and reading flag: directed graph or non directed graph **/
    getline(fileArsg, rd);

    istringstream buf(rd);
    istream_iterator<string> beg(buf), end;
    vector<string> tokens(beg, end);

    int vertexCount = atoi(tokens.at(0).c_str());
    int flag        = atoi(tokens.at(1).c_str());

    /** Create all vertex **/
    for(int i = 0; i < vertexCount; i++)
    {
        ArsVertex* vertex = _availableVertex.top();
        _availableVertex.pop();

        /** Habilitar corpo rigido **/
        vertex->rigidbody->SetActive(true);
        vertex->enable = true;

        vertex->rigidbody->SetTransform(b2Vec2(0, 0), 0.0f);
        _sceneVertex[vertex->getId()] = vertex;
    }

    /** Configure control variable to draw correct graph after loading **/
    if(flag != 0)
        _drawArrow = true;
    else
        _drawArrow = false;

    /** Read all vertex**/
    if(flag == 0)
    {
        for(int i = 0; i < vertexCount-1; i++)
        {
            getline(fileArsg, rd);
            stringstream buf(rd);
            istream_iterator<string> beg(buf), end;
            vector<string> tokens(beg, end);

            /** Configure edges **/
            int j = i + 1;
            for(unsigned int h = 0; h < tokens.size(); h++, j++)
            {
                int edge = atoi(tokens.at(h).c_str());
                if(edge >= 0)
                    _sceneVertex[i]->insertEdge(j, edge, _drawArrow);
            }
        }//END FOR
    }
    else
    {
        for(int i = 0; i < vertexCount; i++)
        {
            getline(fileArsg, rd);
            stringstream buf(rd);
            istream_iterator<string> beg(buf), end;
            vector<string> tokens(beg, end);

            for(unsigned int j = 0; j < tokens.size(); j++)
            {
                int edge = atoi(tokens.at(j).c_str());
                if(edge >= 0)
                    _sceneVertex[i]->insertEdge(j, edge, _drawArrow);
            }
        }//END FOR
    }

    /** Process label **/
    for(int i = 0; i < vertexCount; i++)
    {
        getline(fileArsg, rd);
        stringstream buf(rd);
        istream_iterator<string> beg(buf), end;
        vector<string> tokens(beg, end);

        if(tokens.size() == 0)
        {
            _sceneVertex[i]->rigidbody->SetTransform(b2Vec2(rand() % _windowWidth,rand() % _windowHeight),0.0f);
            continue;
        }

        float pos_x = atof(tokens.at(0).c_str());
        float pos_y = atof(tokens.at(1).c_str());

        /** Parser label **/
        string label = "";

        /** Lex parser **/
        int state              = 0;
        const int findDelimin  = 0;
        const int readingLabel = 1;
        for(unsigned int i = 0; i < rd.size(); i++)
        {
            switch(state)
            {
            case findDelimin:
                if(rd.at(i) == '@')
                    state = readingLabel;
                break;
            case readingLabel:
                label += rd.at(i);
                break;
            }
        }

        /** Set config into vertex **/
        _sceneVertex[i]->rigidbody->SetTransform(b2Vec2(pos_x,pos_y),0.0f);
        _sceneVertex[i]->setLabel(label.c_str());
    }

    fileArsg.close();
}
//----------------------------------------------------------------------------------------------
void directCreateGraph()
{
    /** Remove at reversal order to guarantee the execution flow **/
    while(_availableVertex.size() >0)
    {
        ArsVertex* vertex = _availableVertex.top();
        _availableVertex.pop();

        _sceneVertex[vertex->getId()] = vertex;
    }

    for(int i = ARS_TOTAL_VERTEX-1; i >= 0; i--)
    {
        if(_sceneVertex[i] != NULL)
            _sceneVertex[i]->removeVertex();
    }

    if(_diagWidget != NULL)
    {
        gtk_widget_destroy (_diagWidget);
        _diag = NULL;
    }
}
//----------------------------------------------------------------------------------------------
void acceptGraph()
{
    int OP = createSaveWindow();

    if(OP == GTK_RESPONSE_ACCEPT)
    {
        directCreateGraph();
    }
    else
    {
        gtk_widget_destroy (_diagWidget);
        _diag = NULL;
    }
}
//----------------------------------------------------------------------------------------------
void rejectGraph()
{
    gtk_widget_destroy (_diagWidget);
    _diag = NULL;
}
//----------------------------------------------------------------------------------------------
void loadInFile(const char* filename)
{
    /** Configura arquivo padrao de trabalho **/
    strcpy(_lastSaved, filename);

    /** Destroy current graph **/
    directCreateGraph();

    /** Abrir arquivo **/
    loadGraphFromFile(filename);
}
//----------------------------------------------------------------------------------------------
int createLoadWindow()
{
    /** Push mouse show event **/
    glfwSetInputMode((GLFWwindow*)_win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    /** Force event polling **/
    glfwPollEvents();

    /** Get GTWWindow from widget **/
    GtkWindow *parent = gtk_widget_get_tooltip_window(_windowWidget);

    /** Create openFile GTK dialog **/
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    dialog = gtk_file_chooser_dialog_new ("Open File",
                                          parent,
                                          action,
                                          ("_Cancel"),
                                          GTK_RESPONSE_CANCEL,
                                          ("_Open"),
                                          GTK_RESPONSE_ACCEPT,
                                          NULL);

    int res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        filename = gtk_file_chooser_get_filename (chooser);

        /** Load Graph from UVA FILE **/
        loadInFile(filename);

        /** Prevent leak **/
        g_free(filename);
    }

    /** Hidem cursor **/
    glfwSetInputMode((GLFWwindow*)_win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    gtk_widget_destroy (dialog);
    return res;
}
//----------------------------------------------------------------------------------------------
int createSaveWindow()
{
    /** Push mouse show event **/
    glfwSetInputMode((GLFWwindow*)_win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    /** Force event polling **/
    glfwPollEvents();

    /** Get GTWWindow from widget **/
    GtkWindow *parent = gtk_widget_get_tooltip_window(_windowWidget);

    /** Create openFile GTK dialog **/
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;

    dialog = gtk_file_chooser_dialog_new ("Save File",
                                          parent,
                                          action,
                                          ("_Cancel"),
                                          GTK_RESPONSE_CANCEL,
                                          ("_Save"),
                                          GTK_RESPONSE_ACCEPT,
                                          NULL);

    chooser    = GTK_FILE_CHOOSER(dialog);

    gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

    //gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), default_folder_for_saving);
    gtk_file_chooser_set_current_name   (chooser                  , (_lastSaved)    );

    int res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        filename = gtk_file_chooser_get_filename (chooser);

        /** Load Graph from UVA FILE **/
        saveInFile(filename);

        /** Prevent leak **/
        g_free(filename);
    }

    /** Hidem cursor **/
    glfwSetInputMode((GLFWwindow*)_win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    gtk_widget_destroy (dialog);
    return res;
}
//----------------------------------------------------------------------------------------------
void message(const char *message, void (*accept)() , void (*no)(), void (*reject)() )
{
    /** Get GTWWindow from widget **/
    GtkWindow *parent       = gtk_widget_get_tooltip_window(_windowWidget);

    GtkWidget *dialog, *label, *content_area;
    GtkDialogFlags flags;

    flags  = GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons ("Message",
                                          parent,
                                          flags,
                                          ("_YES"),
                                          GTK_RESPONSE_ACCEPT,
                                          ("_NO"),
                                          GTK_RESPONSE_NO,
                                          ("_CANCEL"),
                                          GTK_RESPONSE_CANCEL,
                                          NULL);

    content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    label        = gtk_label_new (message);

    /** Create events fro realtime window**/
    GtkWidget* saveButton = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
    g_signal_connect(G_OBJECT(saveButton), "clicked", G_CALLBACK(accept), dialog);

    GtkWidget* cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
    g_signal_connect(G_OBJECT(cancel), "clicked", G_CALLBACK(reject), dialog);

    GtkWidget* noBtn = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_NO);
    g_signal_connect(G_OBJECT(noBtn), "clicked", G_CALLBACK(no), dialog);

    g_signal_connect(dialog, "delete_event", G_CALLBACK(reject), dialog);

    gtk_container_add   (GTK_CONTAINER (content_area), label);
    gtk_widget_show_all (dialog);

    _diagWidget = dialog;
    _diag       = GTK_DIALOG(dialog);
}
//----------------------------------------------------------------------------------------------
/** UI buttons callbacks **/
void onClickNew()
{
    if(_diag != NULL)
        rejectGraph();

    message("Wanna save before create a new graph?", acceptGraph, directCreateGraph, rejectGraph);
}
//----------------------------------------------------------------------------------------------
void onClickSave()
{
    if(_diag == NULL)
        createSaveWindow();
    else
        gtk_widget_grab_focus(_diagWidget);
}
//----------------------------------------------------------------------------------------------
void onClickLoad()
{
    if(_diag == NULL)
        createLoadWindow();
    else
        gtk_widget_grab_focus(_diagWidget);
}
//----------------------------------------------------------------------------------------------
void onClickSeparateEdge()
{
    _separateEdges = !_separateEdges;

    if(_separateEdges)
    {
        _separateEdgeBtn->setIcon(_separateEdge, 0.1f);
    }
    else
    {
        _separateEdgeBtn->setIcon(_disableSeparateEdge, 0.1f);
    }
}
//----------------------------------------------------------------------------------------------
void onClickPhysics()
{
    _usePhysics = !_usePhysics;

    if(_usePhysics)
    {
        _usePhysicsBtn->setIcon(_physics, 0.1f);
    }
    else
    {
        _usePhysicsBtn->setIcon(_disablePhysics, 0.1f);
    }
}
//----------------------------------------------------------------------------------------------
void onClickInsertVertex()
{
    _currentOp   = ARS_INSERT_VERTEX;
    _in          = NULL;
    _out         = NULL;
    _opVertex    = NULL;
    _insertion   = false;
    _removed     = false;
}
//----------------------------------------------------------------------------------------------
void onClickInsertEdge()
{
    _currentOp   = ARS_INSERT_EDGE;
    _in          = NULL;
    _out         = NULL;
    _opVertex    = NULL;
    _insertion   = false;
    _removed     = false;
}
//----------------------------------------------------------------------------------------------
void onClickMoveVertex()
{
    _currentOp   = ARS_MOVE_VERTEX;
    _in          = NULL;
    _out         = NULL;
    _opVertex    = NULL;
    _insertion   = false;
    _removed     = false;
}
//----------------------------------------------------------------------------------------------
void onClickRmVertex()
{
    _currentOp   = ARS_REMOVE_VERTEX;
    _in          = NULL;
    _out         = NULL;
    _opVertex    = NULL;
    _insertion   = false;
    _removed     = false;
}
//----------------------------------------------------------------------------------------------
void onClickRmEdge()
{
    _currentOp   = ARS_REMOVE_EDGE;
    _in          = NULL;
    _out         = NULL;
    _opVertex    = NULL;
    _insertion   = false;
    _removed     = false;
}
//----------------------------------------------------------------------------------------------
void onClickLabel()
{
    _currentOp   = ARS_LABEL;
    _in          = NULL;
    _out         = NULL;
    _opVertex    = NULL;
    _insertion   = false;
    _removed     = false;
}
//----------------------------------------------------------------------------------------------
void onClickLabelEdge()
{
    _currentOp   = ARS_LABEL_EDGE;
    _in          = NULL;
    _out         = NULL;
    _opVertex    = NULL;
    _insertion   = false;
    _removed     = false;
}
//----------------------------------------------------------------------------------------------
void onClickComplete()
{
    for(int i = 0; i < ARS_TOTAL_VERTEX; i++)
    {
        if(_sceneVertex[i] != NULL && _sceneVertex[i]->enable)
        {
            for(int j = i + 1; j < ARS_TOTAL_VERTEX; j++)
            {
                if(_sceneVertex[j] != NULL && _sceneVertex[j]->enable)
                {
                    //if(_sceneVertex[i]->getEdgeWeight(j, _drawArrow) < 0)
                        _sceneVertex[i]->insertEdge(j, rand() % 1000, _drawArrow);

                    //if(_sceneVertex[j]->getEdgeWeight(i, _drawArrow) < 0)
                        _sceneVertex[j]->insertEdge(i, rand() % 1000, _drawArrow);
                }
            }
        }
    }
}
//----------------------------------------------------------------------------------------------
void onClickCompleteZero ()
{
    for(int i = 0; i < ARS_TOTAL_VERTEX; i++)
    {
        if(_sceneVertex[i] != NULL && _sceneVertex[i]->enable)
        {
            for(int j = i + 1; j < ARS_TOTAL_VERTEX; j++)
            {
                if(_sceneVertex[j] != NULL && _sceneVertex[j]->enable)
                {
                    //if(_sceneVertex[i]->getEdgeWeight(j, _drawArrow) < 0)
                        _sceneVertex[i]->insertEdge(j, 0, _drawArrow);

                    //if(_sceneVertex[j]->getEdgeWeight(i, _drawArrow) < 0)
                        _sceneVertex[j]->insertEdge(i, 0, _drawArrow);
                }
            }
        }
    }
}
//----------------------------------------------------------------------------------------------
void onClickDag()
{
    _drawArrow = !_drawArrow;
}
//----------------------------------------------------------------------------------------------
void onClickDisableLabels()
{
    _disableLabelsB = !_disableLabelsB;

    if(_disableLabelsB)
    {
        _disableLabelsBtn->setIcon(_disableLabels, 0.1f);
    }
    else
    {
        _disableLabelsBtn->setIcon(_disableLabelsD, 0.1f);
    }
}
//----------------------------------------------------------------------------------------------
void onClickMarkVertex()
{
    _currentOp   = ARS_MARK_VERTEX;
    _in          = NULL;
    _out         = NULL;
    _opVertex    = NULL;
    _insertion   = false;
    _removed     = false;
}
//----------------------------------------------------------------------------------------------
void onClickDrawWeight()
{
    _disableEdgeWeight = !_disableEdgeWeight;

    if(_disableEdgeWeight)
    {
         _drawWeightsBtn->setIcon(_disableWeightX, 0.1f);
    }else
    {
        _drawWeightsBtn->setIcon(_drawWeights, 0.1f);
    }
}
//----------------------------------------------------------------------------------------------
void updateAllObjects()
{
    /** Draw all vertex **/
    for(int i = 0; i < ARS_TOTAL_VERTEX; i++)
    {
        if(_sceneVertex[i] != NULL && _sceneVertex[i]->enable)
        {
            _sceneVertex[i]->update(0);
        }
    }
}
//----------------------------------------------------------------------------------------------
void drawAllObjects()
{
    Vector3f outVert(0,0,0);
    Vector3f inVert(0,0,0);

    /** Draw all edges **/
    for(int i = 0; i < ARS_TOTAL_VERTEX; i++)
    {
        if(_sceneVertex[i] != NULL && _sceneVertex[i]->enable)
        {
            _sceneVertex[i]->preDraw(0);
        }
    }//END BIG FOR

    for(int i = 0; i < ARS_TOTAL_VERTEX; i++)if(_sceneVertex[i] != NULL)_sceneVertex[i]->markDrawed(false);

    /** Draw all vertex **/
    for(int i = 0; i < ARS_TOTAL_VERTEX; i++)
    {
        if(_sceneVertex[i] != NULL && _sceneVertex[i]->enable)
        {
            _sceneVertex[i]->draw(0);
        }
    }

    /** Draw all fonts **/
    for(int i = 0; i < ARS_TOTAL_VERTEX; i++)
    {
        if(_sceneVertex[i] != NULL && _sceneVertex[i]->enable)
        {
            _sceneVertex[i]->posDraw(0);
        }//END IF
    }//END FOR

    for(int i = 0; i < ARS_TOTAL_VERTEX; i++)if(_sceneVertex[i] != NULL)_sceneVertex[i]->markDrawed(false);
}
//----------------------------------------------------------------------------------------------
void onGuiEnter()
{
}
//----------------------------------------------------------------------------------------------
void onGuiExit()
{
}
//----------------------------------------------------------------------------------------------
void mouseClick()
{
    if(_mouseButtonState[GLFW_MOUSE_BUTTON_LEFT]         == GLFW_PRESS &&
            _mousePreviousButtonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_PRESS)
    {
        /** Debug print **/
        // printf("APERTEI!\n");

        /** Call all mouse click events of the main program here **/
        switch(_currentOp)
        {
        case ARS_INSERT_VERTEX:
            if(_mousePosition2D_world.y < _windowHeight - 75 &&
                    _mousePosition2D_world.y > 50       &&
                    _mousePosition2D_world.x < _windowWidth - 50&&
                    _mousePosition2D_world.x > 50)
            {
                if(_availableVertex.size() > 0)
                {
                    ArsVertex* vertex = _availableVertex.top();
                    _availableVertex.pop();

                    /** Habilitar corpo rigido **/
                    vertex->rigidbody->SetActive(true);
                    vertex->enable = true;

                    vertex->rigidbody->SetTransform(b2Vec2(_mousePosition2D_world.x, _mousePosition2D_world.y), 0.0f);
                    _sceneVertex[vertex->getId()] = vertex;
                }
            }
            break;
        }
    }

    /** Cancel current operation **/
    if(_mouseButtonState[GLFW_MOUSE_BUTTON_RIGHT]         == GLFW_PRESS &&
            _mousePreviousButtonState[GLFW_MOUSE_BUTTON_RIGHT] != GLFW_PRESS)
    {
        _in          = NULL;
        _out         = NULL;
        _opVertex    = NULL;
        _insertion   = false;
        _removed     = false;
    }

    if(_mouseButtonState[GLFW_MOUSE_BUTTON_LEFT]              == GLFW_RELEASE &&
            _mousePreviousButtonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_RELEASE)
    {
        /** Debug print **/
        // printf("APERTEI!\n");

        /** Call all mouse click events of the main program here **/
    }
}
//----------------------------------------------------------------------------------------------
void mouseMoveExternal()
{
    if(_mouseClickObject != NULL)
    {
        _mouseClickObject->rigidbody->SetTransform(b2Vec2(_mousePosition2D_world.x, _mousePosition2D_world.y), 0.0f);
    }
}
//----------------------------------------------------------------------------------------------
void drawBackground()
{
    drawImage    (_background->getId(), _windowWidth, _windowHeight, Color(1.0f,1.0f,1.0,1), Vector2f(0,0), ARS_BOTTOM_LEFT, 1.0f, -10);
}
//----------------------------------------------------------------------------------------------
void drawMainGui()
{
    drawRectangle(_windowWidth*2 , 64 , Color(0,0,0,1)  , Vector2f(0,_windowHeight-64*0.5), 0.5f);

    drawRectangle(_windowWidth*2 , 2 , Color(1,1,1,1) , Vector2f(0,_windowHeight-64*0.5), 0.5f);


    for(unsigned int i = 0; i < _buttons.size(); i++)
    {
        _buttons[i]->update(0);
        _buttons[i]->draw  (0);
    }

    if(_mousePosition2D_world.y > _windowHeight - 25)
    {
        drawImage(_insertVerIco->getId(), _insertVerIco->getWidth(), _insertVerIco->getHeight(), Color(1,1,1,1), Vector2f(_mousePosition2D_world.x - 50 * 0.1f, _mousePosition2D_world.y + 80 * 0.1f), ARS_TOP_LEFT, 0.1f);
    }
    else
    {
        /** Draw mouse cursor **/
        switch(_currentOp)
        {
        case ARS_INSERT_VERTEX:
            drawImage(_insertVerIco->getId(), _insertVerIco->getWidth(), _insertVerIco->getHeight(), Color(1,1,1,1), Vector2f(_mousePosition2D_world.x - 50 * 0.1f, _mousePosition2D_world.y + 80 * 0.1f), ARS_TOP_LEFT, 0.1f);
            break;
        case ARS_INSERT_EDGE:
            drawImage(_insertEdgeIco->getId(), _insertEdgeIco->getWidth(), _insertEdgeIco->getHeight(), Color(1,1,1,1), Vector2f(_mousePosition2D_world.x, _mousePosition2D_world.y), ARS_CENTER, 0.1f);
            break;
        case ARS_MOVE_VERTEX:
            drawImage(_moveVerIco->getId()  , _moveVerIco->getWidth()  , _moveVerIco->getHeight()  , Color(1,1,1,1), Vector2f(_mousePosition2D_world.x, _mousePosition2D_world.y), ARS_CENTER, 0.1f);
            break;
        case ARS_REMOVE_VERTEX:
            drawImage(_rmver->getId()   , _rmver->getWidth()  , _rmver->getHeight()  , Color(1,1,1,1),Vector2f(_mousePosition2D_world.x - 50 * 0.1f, _mousePosition2D_world.y + 80 * 0.1f), ARS_TOP_LEFT, 0.1f);
            break;
        case ARS_REMOVE_EDGE:
            drawImage(_rmedge->getId()  , _rmedge->getWidth()  , _rmedge->getHeight()  , Color(1,1,1,1), Vector2f(_mousePosition2D_world.x, _mousePosition2D_world.y), ARS_CENTER, 0.1f);
            break;
        case ARS_LABEL:
            drawImage(_label->getId()   , _label->getWidth()  , _label->getHeight()  , Color(1,1,1,1), Vector2f(_mousePosition2D_world.x, _mousePosition2D_world.y), ARS_CENTER, 0.1f);
            break;
        case ARS_LABEL_EDGE:
            drawImage(_labelEdge->getId()   , _labelEdge->getWidth()  , _labelEdge->getHeight()  , Color(1,1,1,1), Vector2f(_mousePosition2D_world.x, _mousePosition2D_world.y), ARS_CENTER, 0.1f);
            break;
        case ARS_MARK_VERTEX:
            drawImage(_markVertex->getId()   , _markVertex->getWidth()  , _markVertex->getHeight()  , Color(1,1,1,1), Vector2f(_mousePosition2D_world.x, _mousePosition2D_world.y), ARS_CENTER, 0.1f);
            break;
        }
    }
}
//----------------------------------------------------------------------------------------------
void drawExecutingQuads()
{
    /* OpenGL matrix stack operation */
    glPushMatrix();

    /* Afine transformations */
    glTranslatef(_windowWidth - 10 - 10, _windowHeight - 60, 0.0f);

    glRotatef   (rotAngle, 0,0,1);
    glScalef    ((GLfloat)10, (GLfloat)10, 1.0f);
    glScalef    (1, 1, 1);
    /* Material color */
    glColor4f   (0.5, 0.5, 0.8, 1);

    /* Texture mapping */
    glBegin(GL_QUADS);
    glVertex3f   (0.0, 0.0, 0.0);
    glVertex3f   (0.0, 1.0, 0.0);
    glVertex3f   (1.0, 1.0, 0.0);
    glVertex3f   (1.0, 0.0, 0.0);
    glEnd();

    /* Draw end  */
    glPopMatrix();

    /* OpenGL matrix stack operation */
    glPushMatrix();

    /* Afine transformations */
    glTranslatef(_windowWidth - 10 - 10, _windowHeight - 60, 0.0f);

    glRotatef   (rotAngle, 0,0,1);
    glTranslatef(- 10, -10, 0.0f);
    glScalef    ((GLfloat)10, (GLfloat)10, 1.0f);
    glScalef    (1, 1, 1);
    /* Material color */
    glColor4f   (0.5, 0.5, 0.8, 1);

    /* Texture mapping */
    glBegin(GL_QUADS);
    glVertex3f   (0.0, 0.0, 0.0);
    glVertex3f   (0.0, 1.0, 0.0);
    glVertex3f   (1.0, 1.0, 0.0);
    glVertex3f   (1.0, 0.0, 0.0);
    glEnd();

    /* Draw end  */
    glPopMatrix();

    rotAngle -= 1.0f;
}
//----------------------------------------------------------------------------------------------
void setGlDrawFlags()
{
    glEnable    (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
//----------------------------------------------------------------------------------------------
void loadAllLibraries()
{
    /* DevIL initialization */
    ilInit ();
    iluInit();
    ilutRenderer(ILUT_OPENGL);
}
//----------------------------------------------------------------------------------------------
void configureDefaultFont()
{
    _font       = new FTPolygonFont(string(_contentPath + "xirod.ttf").c_str());
    _fontSimple = new FTPolygonFont(string(_contentPath + "xirod.ttf").c_str());

    if(_font->Error() || _fontSimple->Error())
    {
        printf("Nao foi possivel carregar a fonte...");
        getchar();
        exit(-1);
    }

    _font->FaceSize(15);
    _fontSimple->FaceSize(10);
    //font.Render("Hello World!");
}
//----------------------------------------------------------------------------------------------
void loadTextures()
{
// Load all debug content in above
    /** TODO:replace with a texture manager **/
    _testImage            = new ArsImage("testImage.jpg"            );
    _fileButton           = new ArsImage("bntFile.png"              );
    _opButton             = new ArsImage("btnOperation.png"         );
    _background           = new ArsImage("bg.png"                   );
    _graphNode            = new ArsImage("GraphNode.png"            );
    _insBtn               = new ArsImage("btn.png"                  );
    _insertVerIco         = new ArsImage("insertVerIco.png"         );
    _moveVerIco           = new ArsImage("moveVerIco.png"           );
    _insertEdgeIco        = new ArsImage("insertEdgeIco.png"        );
    _plusbtn              = new ArsImage("plus.png"                 );
    _minusbtn             = new ArsImage("minus.png"                );
    _rmver                = new ArsImage("rmvertex.png"             );
    _rmedge               = new ArsImage("rmedge.png"               );
    _complete             = new ArsImage("complete.png"             );
    _digraph              = new ArsImage("digraph.png"              );
    _label                = new ArsImage("label.png"                );
    _newImg               = new ArsImage("new.png"                  );
    _saveIco              = new ArsImage("save.png"                 );
    _loadIco              = new ArsImage("load.png"                 );
    _physics              = new ArsImage("physics.png"              );
    _disablePhysics       = new ArsImage("disablephysics.png"       );
    _separateEdge         = new ArsImage("separateEdge.png"         );
    _disableSeparateEdge  = new ArsImage("separateEdgeDisable.png"  );
    _disableLabels        = new ArsImage("showLabels.png"           );
    _disableLabelsD       = new ArsImage("showLabelsB.png"          );
    _labelEdge            = new ArsImage("labeledge.png"            );
    _drawWeights          = new ArsImage("disableWeight.png"        );
    _markVertex           = new ArsImage("mark.png"                 );
    _completeZero         = new ArsImage("completeZero.png"         );
    _disableWeightX       = new ArsImage("disableWeightX.png"       );
}
//----------------------------------------------------------------------------------------------
void configure2DWorld()
{
    /** Box2D world configuration **/
    // Define the gravity vector.
    b2Vec2 gravity(0.0f, 0.0f);

    // Construct a world object, which will hold and simulate the rigid bodies.
    _world = new b2World(gravity);

    // Configure world behavior
    _timeStep           = 1.0f / 60.0f;
    _velocityIterations = 6;
    _positionIterations = 3;

    // Contacts listener config
    ArsContactListener * contactListener = new ArsContactListener();
    _world->SetContactListener(contactListener);

    /** Setup drawer **/
    DebugDraw* drawer = new DebugDraw();

    // Drawer flags
    drawer->SetFlags    ( b2Draw::e_shapeBit );
    _world ->SetDebugDraw( drawer );
}
//----------------------------------------------------------------------------------------------
void createWorldLimits()
{
    //-------------------BOTTOM---------------------
    /** Physics debug **/
    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    bodyDef.position.Set(0.0f, -50.0f);
    b2Body* body = _world->CreateBody(&bodyDef);

    /** Configure corpse shape and fixture **/
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(2000.0f, 50.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape    = &dynamicBox;
    fixtureDef.density  = 1.0f;
    fixtureDef.friction = 0.3f;

    body->CreateFixture(&fixtureDef);

    //-------------------TOP---------------------
    b2BodyDef bodyDefTop;
    bodyDefTop.type = b2_kinematicBody;
    bodyDefTop.position.Set(0.0f, _windowHeight + 25);
    b2Body* bodyTop = _world->CreateBody(&bodyDefTop);

    /** Configure corpse shape and fixture **/
    b2PolygonShape dynamicBoxTop;
    dynamicBoxTop.SetAsBox(2000.0f, 50.0f);

    b2FixtureDef fixtureDefTop;
    fixtureDefTop.shape    = &dynamicBoxTop;
    fixtureDefTop.density  = 1.0f;
    fixtureDefTop.friction = 0.3f;

    bodyTop->CreateFixture(&fixtureDefTop);

    //-------------------LEFT---------------------
    b2BodyDef bodyDefLeft;
    bodyDefLeft.type = b2_kinematicBody;
    bodyDefLeft.position.Set(0.0f-20.0f, 0.0f);
    b2Body* bodyLeft = _world->CreateBody(&bodyDefLeft);

    /** Configure corpse shape and fixture **/
    b2PolygonShape dynamicBoxLeft;
    dynamicBoxLeft.SetAsBox(20.0f, _windowHeight);

    b2FixtureDef fixtureDefLeft;
    fixtureDefLeft.shape    = &dynamicBoxLeft;
    fixtureDefLeft.density  = 1.0f;
    fixtureDefLeft.friction = 0.3f;

    bodyLeft->CreateFixture(&fixtureDefLeft);

    //-------------------RIGHT---------------------
    b2BodyDef bodyDefRight;
    bodyDefRight.type = b2_kinematicBody;
    bodyDefRight.position.Set(_windowWidth+20, 0.0f);
    b2Body* bodyRight = _world->CreateBody(&bodyDefRight);

    /** Configure corpse shape and fixture **/
    b2PolygonShape dynamicBoxRight;
    dynamicBoxRight.SetAsBox(20.0f, _windowHeight);

    b2FixtureDef fixtureDefRight;
    fixtureDefRight.shape    = &dynamicBoxRight;
    fixtureDefRight.density  = 1.0f;
    fixtureDefRight.friction = 0.3f;

    bodyRight->CreateFixture(&fixtureDefRight);
}
//----------------------------------------------------------------------------------------------
void configureGuiButtons()
{
    int  btnSpace = 0;
    int  btnSize  = 256;
    int  marginY  = 15 + 128;
    int  marginX  = 50 + 128;
    float scale   = 0.15f;
    marginY      *= scale;
    marginX      *= scale;
    btnSize      *= scale;
    btnSpace      = btnSize + marginX - 128 * scale;

    /** Configure gui toolbar**/
    _newGraph = buildButton(_insBtn, _world, Vector2f(marginX, _windowHeight - marginY), scale);
    _newGraph->setIcon(_newImg, scale);
    _newGraph->setTip(new ArsButtonTip("New graph", Vector2f(marginX, _windowHeight - marginY-256.0f*scale - 10), _fontSimple, 20, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_newGraph);

    _save = buildButton(_insBtn, _world, Vector2f(btnSpace + marginX, _windowHeight - marginY), scale);
    _save->setIcon(_saveIco, scale);
    _save->setTip(new ArsButtonTip("Save graph", Vector2f(btnSpace +marginX, _windowHeight - marginY-256.0f*scale - 10), _fontSimple, 20, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_save);

    _loadFile = buildButton(_insBtn, _world, Vector2f(btnSpace*2 + marginX, _windowHeight - marginY), scale);
    _loadFile->setIcon(_loadIco, scale);
    _loadFile->setTip(new ArsButtonTip("Load graph", Vector2f(btnSpace*2 + marginX, _windowHeight - marginY-256.0f*scale - 10), _fontSimple, 20, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_loadFile);

    _insVertBtn = buildButton(_insBtn, _world, Vector2f(btnSpace*4 + marginX, _windowHeight - marginY), scale);
    _insVertBtn->setIcon(_insertVerIco, scale);
    _insVertBtn->setTip(new ArsButtonTip("Insert vertex", Vector2f(btnSpace*4 +marginX, _windowHeight - marginY-256.0f*scale - 10), _fontSimple, 20, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_insVertBtn);

    _moveVerBtn = buildButton(_insBtn, _world, Vector2f(btnSpace*5 + marginX, _windowHeight - marginY), scale);
    _moveVerBtn->setIcon(_moveVerIco, scale);
    _moveVerBtn->setTip(new ArsButtonTip("Move vertex", Vector2f(btnSpace*5 + marginX, _windowHeight - marginY-256.0f*scale - 10), _fontSimple, 20, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_moveVerBtn);

    _labelBtn= buildButton(_insBtn, _world, Vector2f(btnSpace*6 + marginX, _windowHeight - marginY), scale);
    _labelBtn->setIcon(_label, scale);
    _labelBtn->setTip(new ArsButtonTip("Label vertex", Vector2f(btnSpace*6 + marginX, _windowHeight - marginY-256.0f*scale - 10), _fontSimple, 20, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_labelBtn);

    //_markVertexBtn = buildButton(_insBtn, _world, Vector2f(btnSpace*7 + marginX, _windowHeight - marginY), scale);
    //_markVertexBtn->setIcon(_markVertex, 0.1f);
    //_buttons.push_back(_markVertexBtn);

    _rmVertexBtn = buildButton(_insBtn, _world, Vector2f(btnSpace*7 + marginX, _windowHeight - marginY), scale);
    _rmVertexBtn->setIcon(_rmver, scale);
    _rmVertexBtn->setTip(new ArsButtonTip("Remove vertex", Vector2f(btnSpace*7 +marginX, _windowHeight - marginY-256.0f*scale - 10), _fontSimple, 10, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_rmVertexBtn);

    _insertEdgeBtn = buildButton(_insBtn, _world, Vector2f(btnSpace*8 + marginX, _windowHeight - marginY), scale);
    _insertEdgeBtn->setIcon(_insertEdgeIco, scale);
    _insertEdgeBtn->setTip(new ArsButtonTip("Insert edge: select OUT and IN vertex to insert", Vector2f(btnSpace*8 + marginX, _windowHeight - marginY-256.0f*scale - 10), _fontSimple, 10, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_insertEdgeBtn);

    _labelEdgeBtn = buildButton(_insBtn, _world, Vector2f(btnSpace*9 + marginX, _windowHeight - marginY), scale);
    _labelEdgeBtn->setIcon(_labelEdge, scale);
    _labelEdgeBtn->setTip(new ArsButtonTip("Label edge: select OUT and IN vertex to label", Vector2f(btnSpace*9 + marginX, _windowHeight - marginY-256.0f*scale - 10), _fontSimple, 10, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_labelEdgeBtn);

    _rmEdgeBtn = buildButton(_insBtn, _world, Vector2f(btnSpace*10 + marginX, _windowHeight - marginY), scale);
    _rmEdgeBtn->setIcon(_rmedge, scale);
    _rmEdgeBtn->setTip(new ArsButtonTip("Remove edge: select OUT and IN vertex to remove", Vector2f(btnSpace*10 +marginX, _windowHeight - marginY-256.0f*scale - 10), _fontSimple, 10, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_rmEdgeBtn);

    _completeBtn= buildButton(_insBtn, _world, Vector2f(btnSpace*11 + marginX, _windowHeight - marginY), scale);
    _completeBtn->setIcon(_complete,scale);
    _completeBtn->setTip(new ArsButtonTip("Generate complete graph with random weights", Vector2f(btnSpace*11 +marginX, _windowHeight - marginY-256.0f*scale - 10), _fontSimple, 10, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_completeBtn);

    _completeZeroBtn= buildButton(_insBtn, _world, Vector2f(btnSpace*12 + marginX, _windowHeight - marginY), scale);
    _completeZeroBtn->setIcon(_completeZero, scale);
    _completeZeroBtn->setTip(new ArsButtonTip("Generate complete graph without weights", Vector2f(btnSpace*12 +marginX, _windowHeight - marginY-256.0f*scale - 10), _fontSimple, 10, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_completeZeroBtn);

    _digraphBtn= buildButton(_insBtn, _world, Vector2f(btnSpace*13 + marginX, _windowHeight - marginY), scale);
    _digraphBtn->setIcon(_digraph, scale);
    _digraphBtn->setTip(new ArsButtonTip("Witch between directed and non directed", Vector2f(btnSpace*13 +marginX, _windowHeight - marginY-256.0f*scale - 10), _fontSimple, 10, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_digraphBtn);

    /*_usePhysicsBtn = buildButton(_insBtn, _world, Vector2f(btnSpace*14 + marginX, _windowHeight - marginY), scale);
    _usePhysicsBtn->setIcon(_physics, 0.1f);
    _usePhysicsBtn->setTip(new ArsButtonTip("Use physics", Vector2f(btnSpace*14 + marginX, _windowHeight - marginY-256.0f*0.1f - 10), _fontSimple, 10, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_usePhysicsBtn);*/

    _separateEdgeBtn = buildButton(_insBtn, _world, Vector2f(btnSpace*14 + marginX, _windowHeight - marginY), scale);
    _separateEdgeBtn->setIcon(_separateEdge, scale);
    _separateEdgeBtn->setTip(new ArsButtonTip("highlight edges", Vector2f(btnSpace*14 +marginX, _windowHeight - marginY-256.0f*scale - 10), _fontSimple, 10, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_separateEdgeBtn);

    _disableLabelsBtn = buildButton(_insBtn, _world, Vector2f(btnSpace*15 + marginX, _windowHeight - marginY), scale);
    _disableLabelsBtn->setIcon(_disableLabels, scale);
    _disableLabelsBtn->setTip(new ArsButtonTip("Disable/Enable vertex labels", Vector2f(btnSpace*15 +marginX, _windowHeight - marginY-256.0f*scale - 10), _fontSimple, 10, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_disableLabelsBtn);

    _drawWeightsBtn = buildButton(_insBtn, _world, Vector2f(btnSpace*16 + marginX, _windowHeight - marginY), scale);
    _drawWeightsBtn->setIcon(_drawWeights, scale);
    _drawWeightsBtn->setTip(new ArsButtonTip("Disable/Enable edges weights", Vector2f(btnSpace*16 + marginX, _windowHeight - marginY-256.0f*scale - 10), _fontSimple, 10, Color(1.0f, 1.0f, 1.0f,1.0f )));
    _buttons.push_back(_drawWeightsBtn);

    /** Configure btn callbacks **/
    _newGraph            ->onClick = &onClickNew;
    _save                ->onClick = &onClickSave;
    _loadFile            ->onClick = &onClickLoad;
    _insVertBtn          ->onClick = &onClickInsertVertex;
    _rmVertexBtn         ->onClick = &onClickRmVertex;
    _insertEdgeBtn       ->onClick = &onClickInsertEdge;
    _rmEdgeBtn           ->onClick = &onClickRmEdge;
    _moveVerBtn          ->onClick = &onClickMoveVertex;
    _labelBtn            ->onClick = &onClickLabel;
    _completeBtn         ->onClick = &onClickComplete;
    _digraphBtn          ->onClick = &onClickDag;
    //_usePhysicsBtn       ->onClick = &onClickPhysics;
    _separateEdgeBtn     ->onClick = &onClickSeparateEdge;
    _disableLabelsBtn    ->onClick = &onClickDisableLabels;
    _labelEdgeBtn        ->onClick = &onClickLabelEdge;
    _drawWeightsBtn      ->onClick = &onClickDrawWeight;
    _completeZeroBtn     ->onClick = &onClickCompleteZero;
    // _markVertexBtn       ->onClick = &onClickMarkVertex;
}
//----------------------------------------------------------------------------------------------
void loadAllGraphBuffers()
{
    for(int i = 0; i < ARS_TOTAL_VERTEX; i++)
    {
        /** Create with factory **/
        ArsVertex* toInsert = buildVertex(
                                  _graphNode,
                                  _world,
                                  _mousePosition2D_world,
                                  Color(0.5f,0.5f,0.5f,1.0f),
                                  Color(0.8f,0.8f,0.8f,1.0f),
                                  Color(1.0f,0.0f,0.0f,1.0f),
                                  0.2f);

        /** Inserto to available stack **/
        _availableVertex.push(toInsert);

        toInsert->rigidbody->SetActive(false);
        toInsert->enable = false;

        _sceneVertex[i] = NULL;
    }
}
//----------------------------------------------------------------------------------------------
void loadSecondaryGuiSystem()
{
    /** Create window widget **/
    _windowWidget = gtk_window_new (GTK_WINDOW_TOPLEVEL);
}
//----------------------------------------------------------------------------------------------
void load()
{
    // Basic loading
    loadAllLibraries();
    loadTextures();
    configureDefaultFont();

    // Configure all control variables
    rotAngle = 0.0f;

    configure2DWorld();

    /** Configure simulation limits **/
    //createWorldLimits();

    /** Configura GUI **/
    configureGuiButtons();

    /** Load secondary GUI **/
    loadSecondaryGuiSystem();

    /** Prepare graph for operations **/
    loadAllGraphBuffers();

    /** Mouse picking **/
    createMousePickingObject(*_world);

    //Configure current program op
    _currentOp = ARS_INSERT_VERTEX;
}
//----------------------------------------------------------------------------------------------
void update()
{
    /** Update physics **/
    _world->Step(_timeStep, _velocityIterations, _positionIterations);

    /** Update all objects **/
    updateAllObjects();
}
//----------------------------------------------------------------------------------------------
void draw()
{
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /** Camera basics TODO:create camera object **/
    glLoadIdentity();

    gluLookAt(0,0,100,0,0,0,0,1,0);

    /** Draw all backgrounds **/
    drawBackground();

    //simulation area
    //drawRectangle(1280 - 80 , 1             , Color(0.4,0.4,0.4,1)  , Vector2f(40,50)       , 1.0f);
    //drawRectangle(1280 - 78 , 1             , Color(0.4,0.4,0.4,1)  , Vector2f(40,_windowHeight - 75) , 1.0f);
    //drawRectangle(1         , _windowHeight - 75 - 50 , Color(0.4,0.4,0.4,1)  , Vector2f(40,50)       , 1.0f);
    //drawRectangle(1         , _windowHeight - 75 - 50 , Color(0.4,0.4,0.4,1)  , Vector2f(1240,50)     , 1.0f);

    drawAllObjects();

    //_world->DrawDebugData();

    /** Debug executing triangle **/
    drawExecutingQuads();

    /** Draw prototype and main gui **/
    setGlDrawFlags();

    drawMainGui();
}
//----------------------------------------------------------------------------------------------
void loop()
{
    update();
    draw();
}
//----------------------------------------------------------------------------------------------
int drawSphere(float radius, Vector3f position, float scale, Color color, bool wired)
{
    glPushMatrix();

    glTranslatef(position.x, position.y, position.z);

    /* Afine transformations */
    glScalef((GLfloat)scale, (GLfloat)scale, (GLfloat)scale);

    glColor4f(color.r, color.g, color.b, color.a);

    GLUquadricObj *sphere;
    sphere = gluNewQuadric();

    if(wired)
    {
        gluQuadricDrawStyle(sphere, GLU_LINE );
        gluSphere(sphere, radius, 20, 20);
    }
    else
    {
        gluQuadricDrawStyle(sphere, GLU_FILL );
        gluSphere(sphere, radius, 20, 20);
    }

    gluDeleteQuadric(sphere);

    glPopMatrix();

    return 0;
}
//----------------------------------------------------------------------------------------------
int drawLine(Vector3f out, Vector3f in, Color color, float width)
{

    glPushMatrix();
    glLineWidth(width);
    glDisable(GL_TEXTURE_2D);
    glBegin  (GL_LINE_STRIP);
    glColor4f(color.r, color.g, color.b, color.a);
    glVertex3f(out.x, out.y, out.z);
    glVertex3f(in.x,in.y, in.z);
    glEnd();
    glLineWidth(1);
    glPopMatrix();

    return 0;
}
//----------------------------------------------------------------------------------------------
int drawImage(int texId, int width, int height, Color color, Vector2f position, int pivot, float scale, int layer)
{
    /* If texture is invalid treat error signal */
    if(texId < 0)
    {
        return CANNOT_DRAW_TEXTURE;
    }

    /* Enable the texture_2d into opengl machine */
    glEnable(GL_TEXTURE_2D);

    /* OpenGL matrix stack operation */
    glPushMatrix();

    switch(pivot)
    {
    case ARS_BOTTOM_LEFT:
        glTranslatef(position.x, position.y, layer);
        break;
    case ARS_BOTTOM_RIGHT:
        glTranslatef(position.x - width * scale, position.y, layer);
        break;
    case ARS_TOP_LEFT:
        glTranslatef(position.x, position.y - height*scale, layer);
        break;
    case ARS_TOP_RIGHT:
        glTranslatef(position.x - width * scale, position.y - height * scale, layer);
        break;
    case ARS_CENTER:
        glTranslatef(position.x - width*scale / 2, position.y - height*scale / 2, layer);
        break;
    }

    /* Afine transformations */
    glScalef((GLfloat)width, (GLfloat)height, 1.0f);
    glScalef(scale, scale, 1.0f);

    /* Texture assignment */
    glBindTexture(GL_TEXTURE_2D, texId);

    /* Material color */
    glColor4f(color.r, color.g, color.b, color.a);

    /* Texture mapping */
    glBegin      (GL_QUADS);
    glTexCoord2f (0.0, 1.0);
    glVertex3f   (0.0, 0.0, 0.0);
    glTexCoord2f (0.0, 0.0);
    glVertex3f   (0.0, 1.0, 0.0);
    glTexCoord2f (1.0, 0.0);
    glVertex3f   (1.0, 1.0, 0.0);
    glTexCoord2f (1.0, 1.0);
    glVertex3f   (1.0, 0.0, 0.0);
    glEnd ();

    /* Draw end  */
    glBindTexture(GL_TEXTURE_2D, 0x0);
    glPopMatrix();

    /* Disable texture to guarantee no future errors */
    glDisable(GL_TEXTURE_2D);

    return 0;
}
//----------------------------------------------------------------------------------------------
int drawArrow(b2Vec2 out, b2Vec2 in, Color color)
{

    b2Vec2 v = in - out;
    v.Normalize();
    b2Vec2 vPerp1(-v.y, v.x);
    b2Vec2 vPerp2(v.y, -v.x);

    b2Vec2 v1 = v + vPerp1;
    b2Vec2 v2 = v + vPerp2;

    v1.Normalize();
    v2.Normalize();

    b2Vec2 newP1 = out + 15 * v + 10 * v1;
    b2Vec2 newP2 = in  - 50 * v + 10 * v1;

    b2Vec2 triA = newP2 + 10 * v;
    b2Vec2 triB = newP2 - 40 * v + 20 * v1;
    b2Vec2 triC = newP2 - 40 * v + 20 * v2;

    glColor4f(color.r, color.g, color.b, color.a);

    glLineWidth(3);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINE_STRIP);
    glVertex3f(newP1.x, newP1.y, 0.0f);
    glVertex3f(newP2.x,newP2.y, 0.0f);
    glEnd();

    glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(triA.x, triA.y, 0.0f);
    glVertex3f(triB.x, triB.y, 0.0f);
    glVertex3f(triC.x, triC.y, 0.0f);
    glEnd();

    return 0;
}
//----------------------------------------------------------------------------------------------
int drawTriangle(int width, int height, Color color, Vector2f position, float scale)
{
    /* OpenGL matrix stack operation */
    glPushMatrix();

    glTranslatef(position.x, position.y, 0.0f);

    /* Afine transformations */
    glScalef((GLfloat)width, (GLfloat)height, 1.0f);
    glScalef(scale, scale, 1.0f);

    /* Material color */
    glColor4f(color.r, color.g, color.b, color.a);

    /* Texture mapping */
    glBegin(GL_TRIANGLES);                  // Start Drawing A Triangle
    glVertex3f( 0.0f, 1.0f, 0.0f);          // First Point Of The Triangle
    glVertex3f(-1.0f,-1.0f, 0.0f);          // Second Point Of The Triangle
    glVertex3f( 1.0f,-1.0f, 0.0f);          // Third Point Of The Triangle
    glEnd();

    /* Draw end  */
    glPopMatrix();

    return 0;
}
//----------------------------------------------------------------------------------------------
int drawRectangle(int width, int height, Color color, Vector2f position, float scale)
{
    /* OpenGL matrix stack operation */
    glPushMatrix();

    glTranslatef(position.x, position.y, 0.0f);

    /* Afine transformations */
    glScalef((GLfloat)width, (GLfloat)height, 1.0f);
    glScalef(scale, scale, 1.0f);

    /* Material color */
    glColor4f(color.r, color.g, color.b, color.a);
    /* Texture mapping */
    glBegin      (GL_QUADS);
    glVertex3f   (0.0, 0.0, 0.0);
    glVertex3f   (0.0, 1.0, 0.0);
    glVertex3f   (1.0, 1.0, 0.0);
    glVertex3f   (1.0, 0.0, 0.0);
    glEnd ();

    /* Draw end  */
    glPopMatrix();

    return 0;
}
//----------------------------------------------------------------------------------------------
int startHud()
{
    int    width      = _windowWidth;
    int  height       = _windowHeight;
    float zNear       = 0.00001f;
    float zFar        = 10000.0f;

    glMatrixMode  (GL_PROJECTION);
    glPushMatrix  ();
    glLoadIdentity();
    glViewport    (0, 0   , width, height);
    glOrtho       (0,width,0     , height, zNear, zFar);
    glMatrixMode  (GL_MODELVIEW);
    glLoadIdentity();

    return 0;
}
//----------------------------------------------------------------------------------------------
int endHud()
{
    glMatrixMode  (GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    return 0;
}
//----------------------------------------------------------------------------------------------
void VerifySignal(const int code)
{
    //TODO::implement
}
//----------------------------------------------------------------------------------------------
void createMousePickingObject (b2World& world)
{
    if(_mouseClickObject == NULL)
        _mouseClickObject = new ArsMousePickObj();

    b2BodyDef      bodyDef   ;
    b2CircleShape  shapeDef  ;
    b2FixtureDef   fixtureDef;

    // Configure body transformation and behavior
    bodyDef.position     = b2Vec2(_mousePosition2D_world.x, _mousePosition2D_world.y);
    bodyDef.type         = b2_dynamicBody;
    bodyDef.gravityScale = 0.0f;

    // Configure shape
    shapeDef.m_radius  = 1.0f;

    // Configure fixture

    // Linka elementos e cria corpo
    fixtureDef.shape    = &shapeDef;
    fixtureDef.isSensor = true;                    // Attach shape
    b2Body* body = world.CreateBody (&bodyDef   ); // Create corpse
    body->CreateFixture             (&fixtureDef); // Attach fixture and shape

    _mouseClickObject->rigidbody = body;

    _mouseClickObject->tag       = MOUSE;

    body->SetSleepingAllowed(false);

    //Set user data do rigidbody
    body->SetUserData((void*)_mouseClickObject);
}
//----------------------------------------------------------------------------------------------
void destroyMossePickingObject(b2World& world)
{
    if(_mouseClickObject!=NULL)
    {
        world.DestroyBody(_mouseClickObject->rigidbody);
        _mouseClickObject->rigidbody = NULL;
        delete _mouseClickObject;
        _mouseClickObject = NULL;
    }
}
//----------------------------------------------------------------------------------------------
}
