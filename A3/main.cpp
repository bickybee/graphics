/* CS 3GC3 - Texturing sample
 * by R. Teather
 */

 /*KEY-COMMANDS/*
 1 - insert cube at origin
 2 - insert sphere
 3 - insert teapot
 T - toggle transformation mode (mode is written at bottom left of screen)
 ASWD (and QE) - use transformations in +/- x, y, z directions
 tab - toggle between objects
 X - delete NOT PROPERLY IMPLEMENTED YET i don't think???
 arrow keys - lame camera control
 also if you try to use a transformation when nothing's there it'll crash

/*SCENE GRAPH:
implement next:
- deleting
- fix materials -> pick the 5 and make em changeable. also pick material for floor
- draw indication of selection
- dealing with ID's
- load/save
*/

#include <gl/glut.h>
#include <stdio.h>
 #include <stdlib.h>
 #include <math.h>
 #include <vector>
#include <string>

#include "scene_graph/structs.h"
#include "scene_graph/sceneObj.cpp"

//camera
float pos[] = {0,1,0};
float camPos[] = {10, 5, 10};
float angleX = 0;
float angleY = 0;

//lighting
float light_pos0[] = {1,1,0,1.0};
float amb0[4] = {0.5,0.5,0.5,1};
float diff0[4] = {1,1,1, 1};
float spec0[4] = {1, 1, 1, 1};

float light_pos1[] = {0,1,1,1.0};
float amb1[4] = {0,0,0,1};
float diff1[4] = {0,1,1, 1};
float spec1[4] = {1, 1, 1, 1};

//materials
float m_amb[] = {0.5,0.5,0.5, 1.0};
float m_dif[] = {0.5,0.5,0.5, 1.0};
float m_spec[] = {0,0,0, 1.0};
float shiny = 27.8*128;
Material m1= Material (m_amb, m_dif, m_spec, shiny);

float emAmb[] = {0.0215,	0.1745,	0.0215, 1.0};
float emDif[] = {0.07568,	0.61424,	0.07568,1};
float emSpec[] = {	0.633,	0.727811,	0.633,1};
float emShiny = 0.6*128;
Material m2 = Material (emAmb, emDif, emSpec, emShiny);

Material curMat = m2;

//node ids
int masterID = 0;
int getID(){
	return masterID++;
}

//sceneGraph
#include "scene_graph/sceneGraph.cpp"
#include "scene_graph/nodeGroup.cpp"

SceneGraph *SG;
int nextChild = 0;
SceneObj * currentObj;
int currentObjIndex = 0;

//transform modes
string t = "translate";
string r = "rotate";
string s = "scale";
string transformMode = t;
int mode = 0;

//scene objects
vector<SceneObj*> *sceneObjs = new vector<SceneObj*>;

void insertObj(ModelType type){
	SG->goToRoot();

	//scale node
	Vector3D temp3;

	//insert group node at root
	NodeGroup *objGroup = new NodeGroup();
	SG->insertChildNodeHere(objGroup);
	SG->goToChild(nextChild++);

	int groupID = SG->getCurrentID(); //identify this object by it's group-node ID
	//insert remaining nodes, child after child

	//reset temp vals to 0 to begin obj at origin, and rotations to be 0
	temp3.x = 0;
	temp3.y = 0;
	temp3.z = 0;

	//translate
	NodeTransform *trNode = new NodeTransform(Translate, temp3);
	SG->insertChildNodeHere(trNode);
	SG->goToChild(0);

	//rotate
	NodeTransform *rotNode = new NodeTransform(Rotate, temp3);
	SG->insertChildNodeHere(rotNode);
	SG->goToChild(0);

	temp3.x = 1;
	temp3.y = 1;
	temp3.z = 1;

	//scale
	NodeTransform *scNode = new NodeTransform(Scale, temp3);
	SG->insertChildNodeHere(scNode);
	SG->goToChild(0);

	//material
	NodeMaterial *matNode = new NodeMaterial(curMat);
	SG->insertChildNodeHere(matNode);
	SG->goToChild(0);

	//model node
	NodeModel *modelNode = new NodeModel(type);
	SG->insertChildNodeHere(modelNode);

	//update globals
	SceneObj* newObj = new SceneObj(groupID, trNode, scNode, rotNode, matNode, modelNode);
	sceneObjs->push_back(newObj);
	currentObj = newObj;
	currentObjIndex = sceneObjs->size()-1;
}

void insertLight(float pos[4], float amb[4], float dif[4], float spec[4], int n){
	SG->goToRoot();

	//scale node
	Vector3D temp3;
	temp3.x = pos[0];
	temp3.y = pos[1];
	temp3.z = pos[2];

	//insert group node at root
	NodeGroup *objGroup = new NodeGroup();
	SG->insertChildNodeHere(objGroup);
	SG->goToChild(nextChild++);

	int groupID = SG->getCurrentID();

	//lighting
	NodeLight *lightNode = new NodeLight(pos, amb, dif, spec, n);
	SG->insertChildNodeHere(lightNode);
	SG->goToChild(0);

	//translate
	NodeTransform *trNode = new NodeTransform(Translate, temp3);
	SG->insertChildNodeHere(trNode);
	SG->goToChild(0);

	//model node
	NodeModel *modelNode = new NodeModel(Lighting);
	SG->insertChildNodeHere(modelNode);

	//update globals
	SceneObj* newObj = new SceneObj(groupID, trNode, lightNode, modelNode);
	sceneObjs->push_back(newObj);
	currentObj = newObj;
}

//callbacks
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27:
			exit (0);
			break;
		case '1': 
			insertObj(Cube);
			break;
		case '2':
			insertObj(Sphere);
			break;
		case '3':
			insertObj(Teapot);
			break;
		case 'a': //-x
			if (sceneObjs->size()!=0){
				if (mode%3==0) currentObj->translate(-0.1, 0, 0);
				if (mode%3==1) currentObj->scale(-0.1, 0,0);
				if (mode%3==2) currentObj->rotate(-5,0,0);
			}
			break;
		 case 'd': //+x
		 if (sceneObjs->size()!=0){
				if (mode%3==0) currentObj->translate(0.1, 0, 0);
				if (mode%3==1) currentObj->scale(0.1,0,0);
				if (mode%3==2) currentObj->rotate(5,0,0);
			}
			break;
		case 'q': //-z
		if (sceneObjs->size()!=0){
			if (mode%3==0) currentObj->translate(0, 0, -0.1);
			if (mode%3==1) currentObj->scale(0, 0,-0.1);
			if (mode%3==2) currentObj->rotate(0,0,-5);}
			break;
		case 'e': //+z
		if (sceneObjs->size()!=0){
			if (mode%3==0) currentObj->translate(0, 0, 0.1);
			if (mode%3==1) currentObj->scale(0, 0,0.1);
			if (mode%3==2) currentObj->rotate(0,0,5);}
			break;
		case 's': //-y
		if (sceneObjs->size()!=0){
			if (mode%3==0) currentObj->translate(0, -0.1, 0);
			if (mode%3==1) currentObj->scale(0, -0.1,0);
			if (mode%3==2) currentObj->rotate(0,-5,0);}
			break;
		case 'w': //+y
		if (sceneObjs->size()!=0){
			if (mode%3==0) currentObj->translate(0, 0.1, 0);
			if (mode%3==1) currentObj->scale(0, 0.1,0);
			if (mode%3==2) currentObj->rotate(0,5,0);}
			break;
		case 't': //transform type toggle
			mode++;
			if (mode%3==0) transformMode = t;
			if (mode%3==1) transformMode = s;
			if (mode%3==2) transformMode = r;
			break;
		case 'x':
		if (sceneObjs->size()!=0){
			//sceneObjs->erase(sceneObjs->begin()+currentObjIndex);
			// also remove from tree
		}
		case 9: // toggle selected object (temporary fix before ray picking is implemented)
			currentObj = sceneObjs->at(currentObjIndex++%sceneObjs->size());
			break;
		default:
			break;
	}
	glutPostRedisplay();
}

void deleteObj(int ID){
	int index = -1;
	for (int i=0; i<sceneObjs->size(); i++){
		if (sceneObjs->at(i)->ID==ID){
			index = i;
			break;
		}
	}

}

void special(int key, int x, int y)
{
	/* arrow key presses move the camera */
	switch(key)
	{

		case GLUT_KEY_LEFT:
			if (glutGetModifiers()==GLUT_ACTIVE_CTRL) angleX += 5;
			else camPos[0]-=0.1;
			
			break;

		case GLUT_KEY_RIGHT:
			if (glutGetModifiers()==GLUT_ACTIVE_CTRL) angleX += -5;
			else camPos[0]+=0.1;
			
			break;

		case GLUT_KEY_UP:
			if (glutGetModifiers()==GLUT_ACTIVE_CTRL) angleY += 5;
			else camPos[2] -= 0.1;
			
			break;

		case GLUT_KEY_DOWN:
			if (glutGetModifiers()==GLUT_ACTIVE_CTRL) angleY += -5;
			else camPos[2] += 0.1;
			break;
		
		case GLUT_KEY_HOME:
			camPos[1] += 0.1;
			break;

		case GLUT_KEY_END:
			camPos[1] -= 0.1;
			break;

	}
	glutPostRedisplay();
}


void init(void)
{	GLuint id = 1;

	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);
	glShadeModel(GL_SMOOTH);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 100);
	glEnable(GL_LIGHTING);
	//init our scenegraph
	SG = new SceneGraph();
	insertLight(light_pos0, amb0, diff0, spec0, 0);
	insertLight(light_pos1, amb1, diff1, spec1, 1);

}

void drawText(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 600, 0, 600);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_LIGHTING);

	glColor3f(0.0, 1.0, 0.0);
	glRasterPos2i(10, 10);
	for (int i = 0; i < transformMode.length(); i++){
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15,transformMode[i]);
	}
	glFlush();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
}

void drawXZPlane(float y_intercept, float size){
	glColor3f(0.2,0.2,0.2);
	glLineWidth(1);
	glBegin(GL_QUADS);
		for (int x = 0; x < size-1; x++) { 
      		for (int z = 0; z < size-1; z++) {
		    	//draw quad vertices CCW
		    	//assigning normals as well
		    	//it's a flat x-z plane so the normal is always 1 in the y direction
		       	glNormal3f(0,1,0);
		        glVertex3f(x, y_intercept, z);

		        glNormal3f(0,1,0);
		        glVertex3f(x+1, y_intercept, z);

		        glNormal3f(0,1,0);
		        glVertex3f(x+1, y_intercept, z+1);

		        glNormal3f(0,1,0);
		        glVertex3f(x, y_intercept, z+1);
      }
    }
    glEnd();

}

void drawAxis(float size)
{	glLineWidth(10);
	glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex3f(0,0,0);
		glVertex3f(size,0,0);

		glColor3f(0,1,0);
		glVertex3f(0,0,0);
		glVertex3f(0,size,0);
		
		glColor3f(0,0,1);
		glVertex3f(0,0,0);
		glVertex3f(0,0,size);
	glEnd();
}


/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 100);

	float origin[3] = {0,0,0};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camPos[0], camPos[1], camPos[2], 0,0,0, 0,1,0);
	glRotatef(angleY,1,0,0);
	glRotatef(angleX,0,1,0);
	//glTranslatef(50, 0, 50);

	drawAxis(50);
	//draw the sceneGraph
	glEnable(GL_LIGHTING);
	setMaterial(m1);
	drawXZPlane(0,50);

	glColor3f(0.5,0.5,0.5);
	SG->draw();
	drawText();

	glutSwapBuffers();
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("SimpleSceneGraph");	//creates the window

	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	init();

	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}