#include<iostream>
#include<functional>
#include<string>
#include<io.h>
#include<vector>
#include "..\core\Mesh\Vertex.h"
#include "..\core\Mesh\Edge.h"
#include "..\core\Mesh\Face.h"
#include "..\core\Mesh\HalfEdge.h"
#include "..\core\Mesh\BaseMesh.h"
#include "..\core\Mesh\iterators.h"
#include "..\core\Mesh\boundary.h"
#include "..\core\Parser\parser.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace MeshLib {
	template<typename V, typename E, typename F, typename H>
	class MeshToolBox {
	public:
		typedef CBoundary<V, E, F, H>					CBoundary;
		typedef CLoop<V, E, F, H>						CLoop;

		typedef MeshVertexIterator<V, E, F, H>			MeshVertexIterator;
		typedef MeshEdgeIterator<V, E, F, H>			MeshEdgeIterator;
		typedef MeshFaceIterator<V, E, F, H>			MeshFaceIterator;
		typedef MeshHalfEdgeIterator<V, E, F, H>		MeshHalfEdgeIterator;

		typedef VertexVertexIterator<V, E, F, H>		VertexVertexIterator;
		typedef VertexEdgeIterator<V, E, F, H>			VertexEdgeIterator;
		typedef VertexFaceIterator<V, E, F, H>			VertexFaceIterator;
		typedef VertexInHalfedgeIterator<V, E, F, H>	VertexInHalfedgeIterator;
		typedef VertexOutHalfedgeIterator<V, E, F, H>	VertexOutHalfedgeIterator;

		typedef FaceVertexIterator<V, E, F, H>			FaceVertexIterator;
		typedef FaceEdgeIterator<V, E, F, H>			FaceEdgeIterator;
		typedef FaceHalfedgeIterator<V, E, F, H>		FaceHalfedgeIterator;
		typedef CBaseMesh<V, E, F, H> CMesh;
		//Vertex
		static void MeshVertexIterFunc(CMesh *pMesh, std::function<void(V*)> VFunc) {
			for (MeshVertexIterator MVIter(pMesh); !MVIter.end(); ++MVIter) {
				VFunc(*MVIter);
			}
		};
		static MeshVertexIterator MeshVertexIterFuncCondition(CMesh *pMesh, std::function<bool(V*)> VFunc) {
			MeshVertexIterator MVIter(pMesh);
			for (; !MVIter.end(); ++MVIter) {
				if ( !VFunc(*MVIter) )
					return MVIter;
			}
			return MVIter;
		};
		//Face
		static void MeshFaceIterFunc(CMesh *pMesh, std::function<void(F*)> FFunc) {
			for (MeshFaceIterator MFITer(pMesh); !MFITer.end(); ++MFITer) {
				FFunc(*MFITer);
			}
		};
		static MeshFaceIterator MeshFaceIterFuncCondition(CMesh *pMesh, std::function<bool(F*)> FFunc) {
			MeshFaceIterator MFIter(pMesh);
			for (; !MFIter.end(); ++MFIter) {
				if (!FFunc(*MFIter))
					return MFIter;
			}
			return MFIter;
		};
		//Edge
		static void MeshEdgeIterFunc(CMesh *pMesh, std::function<void(E*)> EFunc) {
			for (MeshEdgeIterator MEIter(pMesh); !MEIter.end(); ++MEIter) {
				EFunc(*MEIter);
			}
		};
		static MeshEdgeIterator MeshEdgeIterFuncCondition(CMesh *pMesh, std::function<bool(E*)> FFunc) {
			MeshEdgeIterator MEIter(pMesh);
			for (; !MEIter.end(); ++MEIter) {
				if (!FFunc(*MEIter))
					return MEIter;
			}
			return MEIter;
		};
		//basic for_each template
		template<typename IterType, typename IterCenterClass, typename IterClass>
		static void IterEachFunc(IterCenterClass *pObj, std::function<void(IterClass*)> Func) {		
			for (IterType Iter(pObj); !Iter.end(); ++Iter) {
				Func(*Iter);
			}
		}
		template<typename IterType, typename IterCenterClass, typename IterClass>
		static void IterEachFuncWithPMesh(CMesh *pMesh, IterCenterClass *pObj, std::function<void(IterClass*)> Func) {
			for (IterType Iter(pMesh, pObj); !Iter.end(); ++Iter) {
				Func(*Iter);
			}
		}
		template<typename IterType, typename IterCenterClass, typename IterClass>
		static typename IterType IterEachFuncCondition(IterCenterClass *pObj, std::function<bool(IterClass*)> Func) {
			IterType Iter(pObj);
			for (; !Iter.end(); ++Iter) {
				if (!Func(*Iter)) {
					return Iter;
				}
			}
			return Iter;
		}
		template<typename IterType, typename IterCenterClass, typename IterClass>
		static typename IterType IterEachFuncConditionWithPMesh(CMesh *pMesh, IterCenterClass *pObj, std::function<bool(IterClass*)> Func) {
			IterType Iter(pMesh, pObj);
			for (; !Iter.end(); ++Iter) {
				if (!Func(*Iter)) {
					return Iter;
				}
			}
			return Iter;
		}
		//vertex
		static constexpr auto VertexEdgeIterFunc = IterEachFunc<VertexEdgeIterator, V, E>;
		static constexpr auto VertexFaceIterFunc = IterEachFunc<VertexFaceIterator, V, F>;
		static constexpr auto VertexVertexIterFun = IterEachFunc<VertexVertexIterator, V, V>;
		static constexpr auto VertexInHalfEdgeIterFun = IterEachFuncWithPMesh<VertexInHalfedgeIterator, V, H>;
		static constexpr auto VertexOutHalfEdgeIterFun = IterEachFuncWithPMesh<VertexOutHalfedgeIterator, V, H>;

		static constexpr auto VertexEdgeIterFuncCondition = IterEachFuncCondition<VertexEdgeIterator, V, E>;
		static constexpr auto VertexFaceIterFuncCondition = IterEachFuncCondition<VertexFaceIterator, V, F>;
		static constexpr auto VertexVertexIterFunCondition = IterEachFuncCondition<VertexVertexIterator, V, V>;
		static constexpr auto VertexInHalfEdgeIterFunCondition = IterEachFuncConditionWithPMesh<VertexInHalfedgeIterator, V, H>;
		static constexpr auto VertexOutHalfEdgeIterFunCondition = IterEachFuncConditionWithPMesh<VertexOutHalfedgeIterator, V, H>;
		//face
		static constexpr auto FaceVertexIterFunc = IterEachFunc<FaceVertexIterator, F, V>;
		static constexpr auto FaceEdgeIterFunc = IterEachFunc<FaceEdgeIterator, F, E>;
		static constexpr auto FaceHalfEdgeIterFunc = IterEachFunc<FaceHalfedgeIterator, F, H>;

		static constexpr auto FaceVertexIterFuncCondition = IterEachFuncCondition<FaceVertexIterator, F, V>;
		static constexpr auto FaceEdgeIterFuncCondition = IterEachFuncCondition<FaceEdgeIterator, F, E>;
		static constexpr auto FaceHalfEdgeIterFuncCondition = IterEachFuncCondition<FaceHalfedgeIterator, F, H>;

		struct FileParts
		{
			string path;
			string name;
			string ext;
		};

		static FileParts fileparts(string filename)
		{
			int idx0 = filename.rfind("/");
			int idx1 = filename.rfind(".");

			FileParts fp;
			fp.path = filename.substr(0, idx0 + 1);
			fp.name = filename.substr(idx0 + 1, idx1 - idx0 - 1);
			fp.ext = filename.substr(idx1);

			return fp;
		}
		static void getAllFiles(string path, vector<string>& files) {
			//文件句柄
			long long hFile = 0;
			//文件信息
			struct _finddata_t fileinfo;  //很少用的文件信息读取结构
			string p;  //string类很有意思的一个赋值函数:assign()，有很多重载版本
			if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
				do {
					if ((fileinfo.attrib & _A_SUBDIR)) {  //比较文件类型是否是文件夹
						if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
							files.push_back(p.assign(path).append("\\").append(fileinfo.name));
							getFilesall(p.assign(path).append("\\").append(fileinfo.name), files);
						}
					}
					else {
						files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					}
				} while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
				_findclose(hFile);
			}
		}
		static void getJustCurrentDir(string path, vector<string>& files) {
			//文件句柄
			long long hFile = 0;
			//文件信息 
			struct _finddata_t fileinfo;
			string p;
			if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
				do {
					if ((fileinfo.attrib & _A_SUBDIR)) {
						if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
							files.push_back(fileinfo.name);
							//files.push_back(p.assign(path).append("\\").append(fileinfo.name));
						}
					}
				} while (_findnext(hFile, &fileinfo) == 0);
				_findclose(hFile);
			}
		}
		static void getJustCurrentFile(string path, vector<string>& files) {
			//文件句柄
			long long hFile = 0;
			//文件信息
			struct _finddata_t fileinfo;
			string p;
			if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
				do {
					if ((fileinfo.attrib & _A_SUBDIR)) {
						;
					}
					else {
						//files.push_back(fileinfo.name);
						files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					}
				} while (_findnext(hFile, &fileinfo) == 0);
				_findclose(hFile);
			}
		}
		static void getFilesAll(string path, vector<string>& files) {
			//文件句柄
			long long hFile = 0;
			//文件信息
			struct _finddata_t fileinfo;
			string p;
			if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
				do {
					if ((fileinfo.attrib & _A_SUBDIR)) {
						if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
							//files.push_back(p.assign(path).append("\\").append(fileinfo.name));
							getFilesA(p.assign(path).append("\\").append(fileinfo.name), files);
						}
					}
					else {
						files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					}
				} while (_findnext(hFile, &fileinfo) == 0);
				_findclose(hFile);
			}
		}
	};
}