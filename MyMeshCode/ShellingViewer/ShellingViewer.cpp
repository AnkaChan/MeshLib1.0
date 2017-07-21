// TetOrientation.cpp : 定义控制台应用程序的入口点。
//

#include "../../MeshLib/core/TetMesh/BaseTMeshNew.h"
#include "../../MeshLib/core/TetMesh/vertex.h"
#include "../../MeshLib/core/TetMesh/tvertex.h"
#include "../../MeshLib/core/TetMesh/edge.h"
#include "../../MeshLib/core/TetMesh/tedge.h"
#include "../../MeshLib/core/TetMesh/face.h"
#include "../../MeshLib/core/TetMesh/halfface.h"
#include "../../MeshLib/core/TetMesh/halfedge.h"

#include "../../MeshLib/core/TetMesh/tet.h"
		  
#include "../../MeshLib/core/TetMesh/titerators.h"
#include "../../MeshLib/algorithm/Shelling/TetSheller.h"
#include "GLTetView.h"


//using namespace MeshLib;
using namespace MeshLib::TMeshLib;
typedef CTMesh<CTVertex, CVertex, CHalfEdge, CTEdge, CEdge, CHalfFace, CFace, CTetShelling> MyTMesh;
typedef CTetSheller<CTVertex, CVertex, CHalfEdge, CTEdge, CEdge, CHalfFace, CFace, CTetShelling> CTSheller;

std::shared_ptr<MyTMesh> pMesh(new MyTMesh);
std::shared_ptr<std::list<CTetShelling *>> shellingList;
MyTMesh & mesh = *pMesh;
int argcG;
char ** argvG;

int main(int argc, char ** argv)
{
	argcG = argc;
	argvG = argv;

	if (argc < 2) {
		std::cout << "Please give a input directory." << std::endl;
		return 0;
	}
	mesh._load_t(argv[1]);
	std::cout << "Load done.\n";
	CTSheller sheller(pMesh);
	CTetShelling * p_startTet = pMesh->idTet(10000);
	std::list<CTetShelling *> beginList;
	beginList.push_back(p_startTet);
	sheller.shellingBreadthFirstGreedy(beginList);

	shellingList = sheller.getShellingOrder();
	init_openGL();
	//mesh._write_t("D:\\Data\\tet\\FastOutDemo.t");
	//std::cout << "Save done.\n";
	getchar();
    return 0;
}

