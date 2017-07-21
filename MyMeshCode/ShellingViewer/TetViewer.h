#pragma once
#include <GL\glut.h>
#include "../../MeshLib/core/TetMesh/titerators.h"
#include "../../MeshLib/core/viewer/Arcball.h"                           /*  Arc Ball  Interface         */

namespace MeshLib
{
	namespace TMeshLib
	{
		template< typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		class CTetViewer {
			typedef CTMesh<TV, V, HE, TE, E, HF, F, T> CViewerTMesh;
			typedef HalfFaceVertexIterator<TV, V, HE, TE, E, HF, F, T> HalfFaceVertexIterator;

		public:
			CTetViewer(CViewerTMesh *pTMesh) : HalfFaces(pTMesh->half_faces()) {
				m_pTMesh = pTMesh;
				
			}

			void simpleViewer();
			CViewerTMesh *m_pTMesh;
			std::list<CHalfFace *> &HalfFaces;

		private:
			void init_GL() {
				/* glut stuff */ 
				int argc = 1;
				char * name = "Demo";
				char ** argv = &name;
				glutInit(&argc, argv);                /* Initialize GLUT */
				glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
				glutInitWindowSize(800, 600);
				glutCreateWindow("Mesh Viewer");	  /* Create window with given title */
				glViewport(0, 0, 800, 600);

				void(CTetViewer::*func_display)(void) = display;
				glutDisplayFunc(func_display);             /* Set-up callback functions */

				void(*func_reshape)(int , int) = &reshape;
				glutReshapeFunc(func_reshape);

				void(*func_mouseClick)(int, int, int , int) = &mouseClick;
				glutMouseFunc(func_mouseClick);

				void(*func_mouseMove)(int, int) = &mouseMove;
				glutMotionFunc(func_mouseMove);

				void(*func_keyBoard)(unsigned char, int, int) = &keyBoard;
				glutKeyboardFunc(func_keyBoard);

				setupGLstate();

				glutMainLoop();
			}
			void display();
			void reshape(int w, int h);
			void mouseMove(int x, int y);
			void mouseClick(int button, int state, int x, int y);
			void keyBoard(unsigned char key, int x, int y);
			void draw_half_edges();
			void setupGLstate();

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
				gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
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

		};
	
		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CTetViewer<TV, V, HE, TE, E, HF, F, T>::simpleViewer()
		{
			init_GL();
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CTetViewer<TV, V, HE, TE, E, HF, F, T>::display()
		{
			/* clear frame buffer */
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			setupLight();
			/* transform from the eye coordinate system to the world system */
			setupEye();
			glPushMatrix();
			/* transform from the world to the ojbect coordinate system */
			setupObject();

			/* draw sharp edges */
			f();

			glPopMatrix();
			glutSwapBuffers();
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CTetViewer<TV, V, HE, TE, E, HF, F, T>::reshape(int w, int h)
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

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CTetViewer<TV, V, HE, TE, E, HF, F, T>::mouseMove(int x, int y)
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

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CTetViewer<TV, V, HE, TE, E, HF, F, T>::mouseClick(int button, int state, int x, int y)
		{
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

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CTetViewer<TV, V, HE, TE, E, HF, F, T>::keyBoard(unsigned char key, int x, int y)
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
			case '?':
				help();
				break;
			case 27:
				exit(0);
				break;
			}
			glutPostRedisplay();

		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CTetViewer<TV, V, HE, TE, E, HF, F, T>::draw_half_edges()
		{
			CViewerTMesh *mesh = m_pTMesh;
			//glBindTexture(GL_TEXTURE_2D, texName);
			glBegin(GL_TRIANGLES);
			for (auto hfIter = HalfFaces.begin(); hfIter != HalfFaces.end(); hfIter++)
			{

				HF * pHF = *hfIter;
				F * pF = mesh->HalfFaceFace(pHF);
				T * pT = mesh->HalfFaceTet(pHF);

				glColor3f(0.8, 0.8, 0.8);

				for (HalfFaceVertexIterator fvIter(mesh, pHF); !fvIter.end(); ++fvIter)
				{
					V * v = *fvIter;
					CPoint pt = v->position();
					CPoint n(0.0, 0.0, 0.0);

					HF * pHE = pHF->half_edge();
					CPoint  p[3];
					for (int k = 0; k < 3; k++)
					{
						p[k] = pHE->target()->position();
						pHE = pHE->next();
					}
					CPoint fn = (p[1] - p[0]) ^ (p[2] - p[0]);
					n = fn / fn.norm();

					glNormal3d(n[0], n[1], n[2]);
					glVertex3d(pt[0], pt[1], pt[2]);
				}
			}
			glEnd();
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CTetViewer<TV, V, HE, TE, E, HF, F, T>::setupGLstate()
		{
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

	}
}