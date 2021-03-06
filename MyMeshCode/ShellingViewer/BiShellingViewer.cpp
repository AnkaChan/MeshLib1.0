// TetOrientation.cpp : 定义控制台应用程序的入口点。
//
#include <algorithm>
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
#include <MeshLib/algorithm/Shelling/TetSheller.h>
#include <MeshLib/core/Geometry/Circumsphere.h>
#include <MeshLib/core/Geometry/Point.h>

#include "GLTetViewReverseShelling.h"


using namespace MeshLib;
using namespace MeshLib::TMeshLib;
<<<<<<< HEAD
typedef TInterface<CTVertex, CVertex, CHalfEdge, CTEdge, CEdge, CHalfFace, CFace, CTetShelling> TIf;
typedef CTMesh<CTVertex, CVertex, CHalfEdge, CTEdge, CEdge, CHalfFace, CFace, CTetShelling> MyTMesh;
typedef CTetSheller<TIf> CTSheller;
=======
typedef CTMesh<CTVertex, CVertex, CHalfEdge, CTEdge, CEdge, CHalfFace, CFaceShelling, CTetShelling> MyTMesh;
typedef CTetSheller<CTVertex, CVertex, CHalfEdge, CTEdge, CEdge, CHalfFace, CFaceShelling, CTetShelling> CTSheller;
>>>>>>> b7d09589bd9f0f56de41d9df6fe6c978613c4b6e

MyTMesh* pMesh(new MyTMesh);
std::shared_ptr<std::list<CTetShelling *>> shellingList;
MyTMesh & mesh = *pMesh;
int argcG;
char ** argvG;
struct Sphere {
	CPoint center;
	double radius;
} sphere;

int main(int argc, char ** argv)
{
	argcG = argc;
	argvG = argv;

	if (argc < 2) {
		std::cout << "Please give a input directory." << std::endl;
		return 0;
	}
	mesh._load_t(argv[1], true);
	std::cout << "Load done.\n";
	CTSheller sheller(pMesh);
<<<<<<< HEAD
	CTetShelling * p_startTet = pMesh->idTet(50);
=======
	//CTetShelling * p_startTet = pMesh->idTet(10000);
>>>>>>> b7d09589bd9f0f56de41d9df6fe6c978613c4b6e
	std::list<CTetShelling *> beginList;

	auto isBoundaryTet = [&sheller](CTetShelling * pTet) {
		if (sheller.numFaceOnSurfaceInShelling(pTet) > 0) {
			return true;
		}
		return false;
	};

	auto pBoundryTetIter = std::find_if<>(pMesh->tets().begin(), pMesh->tets().end(), isBoundaryTet);

	beginList.push_back(*pBoundryTetIter);
	sheller.shellingBreadthFirstGreedy(beginList);

	shellingList = sheller.getShellingOrder();
	CTetShelling * pFirstTet = shellingList->front();

	CPoint v0, v1, v2, v3;
	v0 = pFirstTet->vertex(0)->position();
	v1 = pFirstTet->vertex(1)->position();
	v2 = pFirstTet->vertex(2)->position();
	v3 = pFirstTet->vertex(3)->position();

	CTetCircumSphere circumSphere(v0, v1, v2, v3);
	sphere.center = circumSphere.getCenter();
	sphere.radius = circumSphere.getRaduis();

	init_openGL();
	//mesh._write_t("D:\\Data\\tet\\FastOutDemo.t");
	//std::cout << "Save done.\n";
	getchar();
    return 0;
}

