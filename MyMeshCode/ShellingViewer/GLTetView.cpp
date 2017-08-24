#pragma once
#include <iostream>
#include <memory>
#include <MeshLib/core/TetMesh/BaseTMesh.h>
#include <MeshLib/core/TetMesh/vertex.h>
#include <MeshLib/core/TetMesh/tvertex.h>
#include <MeshLib/core/TetMesh/edge.h>
#include <MeshLib/core/TetMesh/tedge.h>
#include <MeshLib/core/TetMesh/face.h>
#include <MeshLib/core/TetMesh/halfface.h>
#include <MeshLib/core/TetMesh/halfedge.h>
		 
#include <MeshLib/core/TetMesh/tet.h>
		 
#include <MeshLib/core/TetMesh/titerators.h>
#include <MeshLib/core/viewer/Arcball.h>                         /*  Arc Ball  Interface         */
#include <MeshLib/algorithm/Shelling/TetSheller.h>

#include <GL\glut.h>
#include <GL\freeglut_ext.h>

#define FACE_COLOR 0.8,0.8,0.8
#define ZOOM_LEVEL 1.0 
#define SOLID_MODE 1
#define WIRE_MODE 2
using std::cout;
using std::endl;
using namespace MeshLib;
using namespace MeshLib::TMeshLib;

typedef CTVertex CTVertexGL;
typedef CVertex CVertexGL;
typedef CHalfEdge CHalfEdgeGL;
typedef CTEdge CTEdgeGL;
typedef CEdge CEdgeGL;
typedef CHalfFace CHalfFaceGL;
typedef CFace CFaceGL;
typedef CTetShelling CTetGL;

typedef CTMesh<CTVertexGL, CVertexGL, CHalfEdgeGL, CTEdgeGL, CEdgeGL, CHalfFaceGL, CFaceGL, CTetGL> CTMeshGL;
typedef HalfFaceVertexIterator<CTVertexGL, CVertexGL, CHalfEdgeGL, CTEdgeGL, CEdgeGL, CHalfFaceGL, CFaceGL, CTetGL> HfVIterator;
typedef TetHalfFaceIterator<CTVertexGL, CVertexGL, CHalfEdgeGL, CTEdgeGL, CEdgeGL, CHalfFaceGL, CFaceGL, CTetGL> THfIterator;

/* window width and height */
int win_width, win_height;
int gButton;
int startx, starty;
int shadeFlag = 0;

/* rotation quaternion and translation vector for the object */
CQrot       ObjRot(0, 0, 1, 0);
CPoint      ObjTrans(0, 0, 0);
CPoint		TetCenter;
extern CTMeshGL* pMesh;
extern std::shared_ptr<std::list<CTetShelling*>> shellingList;
std::list < CTetShelling *> renderList;
bool drawCircumSphere = false;
int circumSphereMod = SOLID_MODE;

extern int argcG;
extern char** argvG;
struct Sphere {
	CPoint center;
	double radius;
};
extern struct Sphere sphere;

/* arcball object */
CArcball arcball;

/*! setup the object, transform from the world to the object coordinate system */
void setupObject(void)
{
	double rot[16];

	glTranslated(ObjTrans[0], ObjTrans[1], ObjTrans[2]);
	ObjRot.convert(rot);
	glMultMatrixd((GLdouble *)rot);
}

/*! the eye is always fixed at world z = +5 */
void setupEye(void) {
	glLoadIdentity();
	gluLookAt(0, 0, 3.5, 0, 0, 0, 0, 1, 0);
}

/*! setup light */
void setupLight()
{
	GLfloat lightOnePosition[4] = { 0, 0, 1, 0 };
	glLightfv(GL_LIGHT1, GL_POSITION, lightOnePosition);
}

/*! draw axis */
void draw_axis()
{
	glLineWidth(2.0);
	//x axis
	glColor3f(1.0, 0.0, 0.0);	//red
	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(1, 0, 0);
	glEnd();

	//y axis
	glColor3f(0.0, 1.0, 0);		//green
	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 1, 0);
	glEnd();

	//z axis
	glColor3f(0.0, 0.0, 1.0);	//blue
	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 1);
	glEnd();

	glLineWidth(1.0);
}

/*! draw half faces */
void draw_half_faces()
{
	std::list<CHalfFace *> &HalfFaces = pMesh->half_faces();
	TetCenter = shellingList->front()->vertex(0)->position();

	//glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_TRIANGLES);

	for (auto renderTetIter = renderList.begin(); renderTetIter != renderList.end(); ++renderTetIter)
	{
		CTetGL *pT = *renderTetIter;
		for (THfIterator THfIter(pMesh, pT); !THfIter.end(); ++THfIter)
		{
			glColor3f(FACE_COLOR);
			CHalfFace *pHF = *THfIter;
			if (pHF->dual() != NULL) {
				CTetGL * pTetNeighbor = (CTetGL *)pHF->dual()->tet();
				if (pTetNeighbor->visible) {
					continue;
				}
			}
			for (HfVIterator fvIter(pMesh, pHF); !fvIter.end(); ++fvIter)
			{
				CVertexGL * v = *fvIter;
				CPoint pt = v->position();
				CPoint n(0.0, 0.0, 0.0);

				CHalfEdgeGL * pHE = pHF->half_edge();
				CPoint  p[3];
				for (int k = 0; k < 3; k++)
				{
					p[k] = pHE->target()->position();
					pHE = pHE->next();
				}
				CPoint fn = (p[1] - p[0]) ^ (p[2] - p[0]);
				n = fn / fn.norm();

				glNormal3d(n[0], n[1], n[2]);
				pt = ZOOM_LEVEL * (pt - TetCenter);
				glVertex3d(pt[0], pt[1], pt[2]);
			}
		}
	}
	glEnd();
}

void draw_circumsphereFunc() {
	CPoint newCenter = (sphere.center - TetCenter) * ZOOM_LEVEL;
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(newCenter[0], newCenter[1], newCenter[2]);
	if (circumSphereMod == SOLID_MODE)
		glutSolidSphere(sphere.radius * ZOOM_LEVEL, 40, 40);
	else if (circumSphereMod == WIRE_MODE)
		glutWireSphere(sphere.radius * ZOOM_LEVEL, 40, 40);
	glEnd();
}

/*! display call back function
*/
void display()
{
	/* clear frame buffer */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setupLight();
	/* transform from the eye coordinate system to the world system */
	setupEye();
	glPushMatrix();
	/* transform from the world to the ojbect coordinate system */
	setupObject();

	draw_half_faces();
	draw_axis();
	if (drawCircumSphere)
		draw_circumsphereFunc();

	glPopMatrix();
	glutSwapBuffers();
}

/*! Called when a "resize" event is received by the window. */
void reshape(int w, int h)
{
	float ar;
	//std::cout << "w:" << w << "\th:" << h << std::endl;
	win_width = w;
	win_height = h;

	ar = (float)(w) / h;
	glViewport(0, 0, w, h);               /* Set Viewport */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// magic imageing commands
	gluPerspective(40.0, /* field of view in degrees */
		ar, /* aspect ratio */
		1.0, /* Z near */
		100.0 /* Z far */);

	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

/*! helper function to remind the user about commands, hot keys */
void help()
{
	printf("w  -  Wireframe Display\n");
	printf("f  -  Flat Shading \n");
	printf("s  -  Smooth Shading\n");
	printf("d  -  Show solid circumshpre of first tet. \n");
	printf("a  -  Show wire circumshpre of first tet. \n");
	printf("?  -  Help Information\n");
	printf("esc - quit\n");
}

/*! Keyboard call back function */
void keyBoard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'f':
		//Flat Shading
		glPolygonMode(GL_FRONT, GL_FILL);
		shadeFlag = 0;
		break;
	case 's':
		//Smooth Shading
		glPolygonMode(GL_FRONT, GL_FILL);
		shadeFlag = 1;
		break;
	case 'w':
		//Wireframe mode
		glPolygonMode(GL_FRONT, GL_LINE);
		break;
	case 'd':
		drawCircumSphere = !drawCircumSphere;
		break;
	case 'q':
		circumSphereMod = SOLID_MODE;
		break;
	case 'a':
		circumSphereMod = WIRE_MODE;
		break;
	case '?':
		help();
		break;
	case 27:
		exit(0);
		break;
	
	}
	glutPostRedisplay();
}

void specailKey(int key, int x, int y)
{
	static auto shellingIter = shellingList->begin();
	CTetGL * pTet;
	switch (key)
	{

	case GLUT_KEY_RIGHT:
		if (shellingIter != shellingList->end())
		{
			pTet = *shellingIter;
			pTet->visible = true;
			renderList.push_back(pTet);
			++shellingIter;
		}
		break;
	case GLUT_KEY_LEFT:
		pTet = *shellingIter;
		pTet->visible = false;
		if (shellingIter != shellingList->begin())
			renderList.pop_back();
			--shellingIter;
		break;
	}
}

/*! setup GL states */
void setupGLstate() {
	GLfloat lightOneColor[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat globalAmb[] = { .1, .1, .1, 1 };
	GLfloat lightOnePosition[] = { .0, 0.0, 1.0, 1.0 };

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.35, 0.53, 0.70, 0);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightOneColor);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightOneColor);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glLightfv(GL_LIGHT1, GL_POSITION, lightOnePosition);
}

/*! mouse click call back function */
void  mouseClick(int button, int state, int x, int y) {
	/* set up an arcball around the Eye's center
	switch y coordinates to right handed system  */

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		gButton = GLUT_LEFT_BUTTON;
		arcball = CArcball(win_width, win_height, x - win_width / 2, win_height - y - win_height / 2);
	}

	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		startx = x;
		starty = y;
		gButton = GLUT_MIDDLE_BUTTON;
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		startx = x;
		starty = y;
		gButton = GLUT_RIGHT_BUTTON;
	}
	return;
}

/*! mouse motion call back function */
void mouseMove(int x, int y)
{
	CPoint trans;
	CQrot       rot;

	/* rotation, call arcball */
	if (gButton == GLUT_LEFT_BUTTON)
	{
		rot = arcball.update(x - win_width / 2, win_height - y - win_height / 2);
		ObjRot = rot * ObjRot;
		glutPostRedisplay();
	}

	/*xy translation */
	if (gButton == GLUT_MIDDLE_BUTTON)
	{
		double scale = 10. / win_height;
		trans = CPoint(scale*(x - startx), scale*(starty - y), 0);
		startx = x;
		starty = y;
		ObjTrans = ObjTrans + trans;
		glutPostRedisplay();
	}

	/* zoom in and out */
	if (gButton == GLUT_RIGHT_BUTTON) {
		double scale = 10. / win_height;
		trans = CPoint(0, 0, scale*(starty - y));
		startx = x;
		starty = y;
		ObjTrans = ObjTrans + trans;
		glutPostRedisplay();
	}

}

/*! Normalize mesh
* \param pMesh the input mesh
*/
//void normalize_mesh(CTMeshGL * pMesh)
//{
//	CPoint s(0, 0, 0);
//	for (CTMeshGL::MeshVertexIterator viter(pMesh); !viter.end(); ++viter)
//	{
//		CMyVertexGL * v = *viter;
//		s = s + v->point();
//	}
//	s = s / pMesh->numVertices();
//
//	for (MyMeshFL::MeshVertexIterator viter(pMesh); !viter.end(); ++viter)
//	{
//		CMyVertexGL * v = *viter;
//		CPoint p = v->point();
//		p = p - s;
//		v->point() = p;
//	}
//
//	double d = 0;
//	for (MyMeshFL::MeshVertexIterator viter(pMesh); !viter.end(); ++viter)
//	{
//		CMyVertexGL * v = *viter;
//		CPoint p = v->point();
//		for (int k = 0; k < 3; k++)
//		{
//			d = (d > fabs(p[k])) ? d : fabs(p[k]);
//		}
//	}
//
//	for (MyMeshFL::MeshVertexIterator viter(pMesh); !viter.end(); ++viter)
//	{
//		CMyVertexGL * v = *viter;
//		CPoint p = v->point();
//		p = p / d;
//		v->point() = p;
//	}
//};

/*! Compute the face normal and vertex normal
* \param pMesh the input mesh
*/
//void compute_normal(MyMeshFL * pMesh)
//{
//	for (MyMeshFL::MeshVertexIterator viter(pMesh); !viter.end(); ++viter)
//	{
//		CMyVertexGL * v = *viter;
//		CPoint n(0, 0, 0);
//		for (MyMeshFL::VertexFaceIterator vfiter(v); !vfiter.end(); ++vfiter)
//		{
//			CMyFaceGL * pF = *vfiter;
//
//			CPoint p[3];
//			CHalfEdge * he = pF->halfedge();
//			for (int k = 0; k < 3; k++)
//			{
//				p[k] = he->target()->point();
//				he = he->he_next();
//			}
//
//			CPoint fn = (p[1] - p[0]) ^ (p[2] - p[0]);
//			pF->normal() = fn / fn.norm();
//			n += fn;
//		}
//
//		n = n / n.norm();
//		v->normal() = n;
//	}
//};

void init_openGL()
{
	/* glut stuff */
	glutInit(&argcG, argvG);                /* Initialize GLUT */
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Mesh Viewer");	  /* Create window with given title */
	glViewport(0, 0, 800, 600);

	glutDisplayFunc(display);             /* Set-up callback functions */
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove);
	glutKeyboardFunc(keyBoard);
	glutSpecialFunc(specailKey);

	setupGLstate();
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();                       /* Start GLUT event-processing loop */
}

void init_openGL_EventPrepair()
{
	/* glut stuff */
	glutInit(&argcG, argvG);                /* Initialize GLUT */
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Mesh Viewer");	  /* Create window with given title */
	glViewport(0, 0, 800, 600);

	glutDisplayFunc(display);             /* Set-up callback functions */
	glutReshapeFunc(reshape);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove);
	glutKeyboardFunc(keyBoard);
	setupGLstate();
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
}