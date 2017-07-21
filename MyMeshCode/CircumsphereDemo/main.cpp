#include <iostream>
#include <Eigen\Dense>
#include <MeshLib\core\Mesh\MeshHeaders.h>
#include <MeshLib\core\Geometry\Circumsphere.h>
using namespace MeshLib;
using namespace TMeshLib;
int main() {
	/*CPoint A(1, 0, 0);
	CPoint B(0, 1, 0);
	CPoint C(0, -1, 0);
	CPoint D(0, 0, 1);*/
	/*CPoint A(1, 3, 4);
	CPoint B(1, 1, 0);
	CPoint C(5, -1, 4);
	CPoint D(1, 3, 1);*/
	CPoint A(10, 0, 10);
	CPoint B(0, 1, 10);
	CPoint C(0, -1, 10);
	CPoint D(0, 0, 11);

	CTetCircumSphere tc(A, B, C, D);
	std::cout << "The center is: " << tc.getCenter() << std::endl;
	std::cout << "The radius is: " << tc.getRaduis() << std::endl;
	getchar();
}