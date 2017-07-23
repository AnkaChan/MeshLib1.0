#pragma once
#include <memory>
#include <vector>
#include <list>
#include <random>
#include <iostream>

#include "../../core/TetMesh/BaseTMeshNew.h"
#include "../../core/TetMesh/vertex.h"
#include "../../core/TetMesh/tvertex.h"
#include "../../core/TetMesh/edge.h"
#include "../../core/TetMesh/tedge.h"
#include "../../core/TetMesh/face.h"
#include "../../core/TetMesh/halfface.h"
#include "../../core/TetMesh/halfedge.h"
#include "../../core/TetMesh/tet.h"
#include "../../core/TetMesh/titerators.h"

using std::cout;
using std::endl;

namespace MeshLib{
	namespace TMeshLib{
		class CTetShelling : public CTet
		{
		public:
			bool inCandidateList = false;
			bool inShellingOrder = false;
			bool visible = false;
		};

		template< typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		class CTetSheller {
		public:
			typedef CTMesh<TV, V, HE, TE, E, HF, F, T> CShellerTMesh;
			typedef HalfFaceVertexIterator<TV, V, HE, TE, E, HF, F, T> HalfFaceVertexIterator;
			typedef TetHalfFaceIterator<TV, V, HE, TE, E, HF, F, T> TetHalfFaceIterator;
			typedef TMeshTetIterator< TV, V, HE, TE, E, HF, F, T> TMeshTetIterator;
			typedef TVertexInHalfEdgeIterator< TV, V, HE, TE, E, HF, F, T> TVertexInHalfEdgeIterator;
			typedef VertexTVertexIterator< TV, V, HE, TE, E, HF, F, T> VertexTVertexIterator;
			typedef TetEdgeIterator< TV, V, HE, TE, E, HF, F, T> TetEdgeIterator;
			typedef HalfFaceHalfEdgeIterator< TV, V, HE, TE, E, HF, F, T> HalfFaceHalfEdgeIterator;
			typedef EdgeTEdgeIterator< TV, V, HE, TE, E, HF, F, T> EdgeTEdgeIterator;
			bool shellingDone = false;

			CTetSheller(std::shared_ptr<CShellerTMesh> pMesh) : m_pShellingOrder(new std::list<T *>) {
				m_pMesh = pMesh;
			}

			std::shared_ptr<std::list<T *>> getShellingOrder() { return m_pShellingOrder; };

			void shellingBreadthFirstRandom(std::list<T*> & sourceList);
			void shellingBreadthFirstBackTrace(std::list<T*> & sourceList);
			void shellingBreadthFirstGreedy(std::list<T*> & sourceList);
			void biShellingBreadthFirstGreedy(std::list<T*> & sourceList);
			bool shellingBackTrace();
			int numFaceOnSurfaceInShelling(T* pTet);

		private:
			std::shared_ptr<CShellerTMesh> m_pMesh;
			std::shared_ptr<std::list<T *>> m_pShellingOrder;
			std::list<T *> candidateList;

			void makeCandidateList(std::list<T*> &_candidateList);
			T* randomChooseShelling(std::list<T*> &tList);
			T* greedilyChooseShelling(std::list<T*> &tList);
			T* greedilyChooseBishelling(std::list<T*> &tList);
			T* greedilyChooseShellingBackTrace(std::list<T*> &tList);

			bool isShelling(T* newSimplex);
			bool isReverseShelling(T* newSimplex);
			bool isBishelling(T* newSimplex);

			bool isBoundaryVertexInShelling(V* pV);
			bool isBoundaryEdgeInShelling(E* pE);
			bool edgeInFace(E *pE, F *pF0);
			void getFacesOnSurfaceInShelling(std::vector<F*> &pFacesOnSurface, T * pTet);
			E * oppositeEdgeOfTwoFaces(T* newSimplex, F* pF0, F* pF1);

			bool backTraceShelling(T* nextSimplex);
			void putInCandidateList( T* candidateSimplex);
			void putInShellingList(T* nextSimplex);
			void removeFromCandidateList(typename std::list<T*>::iterator FLItr);
			void removeFromShellingList(typename std::list<T*>::iterator FLItr);
		};
		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CTetSheller<TV, V, HE, TE, E, HF, F, T>::shellingBreadthFirstRandom(std::list<T*>& sourceList)
		{
			cout << "Shelling Breadth First random begins." << endl;

			for (auto sourceIter = sourceList.begin(); sourceIter != sourceList.end(); ++sourceIter) {
				T* beginsource = *sourceIter;
				putInCandidateList(beginsource);
			}
			
			while (!candidateList.empty()) {

				T* pSimplex = randomChooseShelling(candidateList);;
				putInShellingList(pSimplex);

				//for (MeshVertexIterator viter(this); !viter.end(); ++viter)
				for (TetHalfFaceIterator THFItr(m_pMesh.get(), pSimplex); !THFItr.end(); ++THFItr) {
					HF* pHF = *THFItr;
					HF* pDualHF = (HF*)pHF->dual();
					if (pDualHF != NULL) {
						T* pNextSimplex = (T*)pDualHF->tet();
						if (!pNextSimplex->inCandidateList && !pNextSimplex->inShellingOrder) {
							putInCandidateList(pNextSimplex);
						}
					}
				}
				if (m_pShellingOrder->size() % 100 == 0) {
					cout << m_pShellingOrder->size() << " simplices in shelling order." << endl;
				}
			}
			shellingDone = true;
			cout << "Congratulations, the shelling procedure succeeded." << endl;
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CTetSheller<TV, V, HE, TE, E, HF, F, T>::shellingBreadthFirstGreedy(std::list<T*>& sourceList)
		{
			cout << "Shelling Breadth First greedy begins." << endl;

			for (auto sourceIter = sourceList.begin(); sourceIter != sourceList.end(); ++sourceIter) {
				T* beginsource = *sourceIter;
				putInCandidateList(beginsource);
			}

			while (!candidateList.empty()) {

				T* pSimplex = greedilyChooseShelling(candidateList);;
				putInShellingList(pSimplex);

				//for (MeshVertexIterator viter(this); !viter.end(); ++viter)
				for (TetHalfFaceIterator THFItr(m_pMesh.get(), pSimplex); !THFItr.end(); ++THFItr) {
					HF* pHF = *THFItr;
					HF* pDualHF = (HF*)pHF->dual();
					if (pDualHF != NULL) {
						T* pNextSimplex = (T*)pDualHF->tet();
						if (!pNextSimplex->inCandidateList && !pNextSimplex->inShellingOrder) {
							putInCandidateList(pNextSimplex);
						}
					}
				}
				if (m_pShellingOrder->size() % 100 == 0) {
					cout << m_pShellingOrder->size() << " simplices in shelling order." << endl;
				}
			}
			shellingDone = true;
			cout << "Congratulations, the shelling procedure succeeded." << endl;
		}
		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CTetSheller<TV, V, HE, TE, E, HF, F, T>::biShellingBreadthFirstGreedy(std::list<T*>& sourceList)
		{
			cout << "Bishelling Breadth First greedy begins. First tet must be on the boundary or the algorithm will terminate." << endl;

			for (auto sourceIter = sourceList.begin(); sourceIter != sourceList.end(); ++sourceIter) {
				T* beginsource = *sourceIter;
				putInCandidateList(beginsource);
			}

			while (!candidateList.empty()) {

				T* pSimplex = greedilyChooseBishelling(candidateList);;
				putInShellingList(pSimplex);

				//for (MeshVertexIterator viter(this); !viter.end(); ++viter)
				for (TetHalfFaceIterator THFItr(m_pMesh.get(), pSimplex); !THFItr.end(); ++THFItr) {
					HF* pHF = *THFItr;
					HF* pDualHF = (HF*)pHF->dual();
					if (pDualHF != NULL) {
						T* pNextSimplex = (T*)pDualHF->tet();
						if (!pNextSimplex->inCandidateList && !pNextSimplex->inShellingOrder) {
							putInCandidateList(pNextSimplex);
						}
					}
				}
				if (m_pShellingOrder->size() % 100 == 0) {
					cout << m_pShellingOrder->size() << " simplices in shelling order." << endl;
				}
			}
			shellingDone = true;
			cout << "Congratulations, the shelling procedure succeeded." << endl;
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		T * CTetSheller<TV, V, HE, TE, E, HF, F, T>::randomChooseShelling(std::list<T*>& tList)
		{
			static std::default_random_engine generator(time(NULL));
			std::uniform_int_distribution<int> randInt(0, tList.size() - 1);
			std::list<T*>::iterator FLItr;
			T* simplex;
			do {
				FLItr = tList.begin();
				int k = randInt(generator);
				for (int i = 0; i < k; ++i) {
					++FLItr;
				}
				simplex = *FLItr;

			} while (!isShelling(simplex));
			removeFromCandidateList(FLItr);
			return simplex;
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		T * CTetSheller<TV, V, HE, TE, E, HF, F, T>::greedilyChooseShelling(std::list<T*>& tList)
		{
			std::list<T*>::iterator FLItr = tList.begin();
			T* simplex;

			for (; FLItr != tList.end(); ++FLItr)
			{ 
				T *simplex = *FLItr;
				if (isShelling(simplex)) {
					removeFromCandidateList(FLItr);
					return simplex;
				}
			}
			if (FLItr == tList.end()) {
				std::cerr << "Shelling Failed.";
				exit(0);
			}
			return NULL;

		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		inline T * CTetSheller<TV, V, HE, TE, E, HF, F, T>::greedilyChooseBishelling(std::list<T*>& tList)
		{
			std::list<T*>::iterator FLItr = tList.begin();
			T* simplex;

			for (; FLItr != tList.end(); ++FLItr)
			{
				simplex = *FLItr;
				if (isBishelling(simplex)) {
					removeFromCandidateList(FLItr);
					return simplex;
				}
			}
			if (FLItr == tList.end()) {
				std::cerr << "Shelling Failed.";
				exit(0);
			}
			return NULL;

			return NULL;
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		bool CTetSheller<TV, V, HE, TE, E, HF, F, T>::isShelling(T * newSimplex)
		{
			bool possibleSingular;
			for (int i = 0; i < 4; i++)
			{
				possibleSingular = true;
				TV* pTV = newSimplex->tvertex(i);
				for (TVertexInHalfEdgeIterator TvHfItr(pMesh.get(), pTV); !TvHfItr.end(); ++TvHfItr) {
					HE* pHe = *TvHfItr;
					if (pHe->half_face()->dual() != NULL)
					{
						T* pAdjacentTet = (T*)pHe->half_face()->dual()->tet();
						if (pAdjacentTet->inShellingOrder) {
							possibleSingular = false;
						}
					}
				}
				if (possibleSingular)
				{
					V* pV = pTV->vert();
					for (VertexTVertexIterator VtVItr(pMesh.get(), pV); !VtVItr.end(); ++VtVItr)
					{
						TV* pTV = *VtVItr;
						T* pTet = (T*)pTV->tet();
						if (pTet->inShellingOrder)
						{
							return false;
						}
					}
				}
			}

			//check if there is 1-dimensional intersection
			for (TetEdgeIterator TEItr(pMesh.get(), newSimplex); !TEItr.end(); ++TEItr) {
				possibleSingular = true;
				E *pE = *TEItr;
				TE *pTE = NULL;
				//find the TEdge corresponding to newSimplex 
				for (auto ETEItr = pE->edges()->begin(); ETEItr != pE->edges()->end(); ++ETEItr) {
					TE *pTEtemp = (TE*)*ETEItr;
					if (pTEtemp->tet()->id() == newSimplex->id()) {
						pTE = pTEtemp;
						break;
					}
				}
				assert(pTE != NULL);
				HF* pHF1 = (HF*)pTE->left()->half_face()->dual();
				HF* pHF2 = (HF*)pTE->right()->half_face()->dual();
				if (pHF1 != NULL) {
					T* pTtemp = (T*)pHF1->tet();
					if (pTtemp->inShellingOrder)
						possibleSingular = false;
				}

				if (pHF2 != NULL) {
					T* pTtemp = (T*)pHF2->tet();
					if (pTtemp->inShellingOrder)
						possibleSingular = false;
				}
				if (possibleSingular) {
					for (auto ETEItr = pE->edges()->begin(); ETEItr != pE->edges()->end(); ++ETEItr) {
						TE *pTEtemp = (TE*)*ETEItr;
						T* pAdjacentT = (T*)pTEtemp->tet();
						if (pAdjacentT->inShellingOrder) {
							return false;
						}
					}
				}
			}
			return true;
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		bool CTetSheller<TV, V, HE, TE, E, HF, F, T>::isReverseShelling(T * newSimplex)
		{
			switch (numFaceOnSurfaceInShelling(newSimplex))
			{
			case 0:
				return false;
				break;
			case 1:
				/* In this case, it is a shelling iff:
				*  the opposite vertex of the face on surface is not on the surface. 
				*/
				for (size_t i = 0; i < 4; ++i)
				{
					//V * pV = newSimplex->vertex(i);
					V *pV = (V*)pMesh->TetTVertex(newSimplex, i)->vert();

					if (!isBoundaryVertexInShelling(pV))
						return true;
				}
				return false;
				break;
			case 2:
				/* In this case, it is a shelling iff:
				*  the opposite edge of the two faces on surface is not on the surface
				*/
			{
				std::vector<F*> pFacesOnSurface;
				getFacesOnSurfaceInShelling(pFacesOnSurface, newSimplex);
				F * pF0 = pFacesOnSurface[0];
				F * pF1 = pFacesOnSurface[1];
				E * pE = oppositeEdgeOfTwoFaces(newSimplex, pF0, pF1);
				if (isBoundaryEdgeInShelling(pE))
					return false;
				else
					return true;
				break;
			}
			default:
				return true;
				break;
			}
			return true;
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		inline bool CTetSheller<TV, V, HE, TE, E, HF, F, T>::isBishelling(T * newSimplex)
		{
			return isShelling(newSimplex) && isReverseShelling(newSimplex);
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		inline int CTetSheller<TV, V, HE, TE, E, HF, F, T>::numFaceOnSurfaceInShelling(T * pTet)
		{
			int num = 0;
			for (TetHalfFaceIterator THfIter(pMesh.get(), pTet); !THfIter.end(); ++THfIter) {
				HF * pHF = *THfIter;
				HF * pDualHF = pMesh->HalfFaceDual(pHF);
				
				if (pDualHF == NULL) {
					++num;
				}
				else
				{
					T* pNeiTet = pMesh->HalfFaceTet(pDualHF);
					if (pNeiTet->inShellingOrder)
						++num;
				}
			}

			return num;
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		inline bool CTetSheller<TV, V, HE, TE, E, HF, F, T>::edgeInFace(E * pE, F * pF)
		{
			HF *pHF = (HF *) pF->left() != NULL ? pF->left() : pF->right();

			for (HalfFaceHalfEdgeIterator HFHEITer(pMesh.get(), pHF); !HFHEITer.end(); ++HFHEITer) {
				HE *pHE = *HFHEITer;
				E *pEOnFace = (E *)pHE->tedge()->edge();
				if ((pE->vertex1()->id() == pEOnFace->vertex1()->id()) && (pE->vertex2()->id() == pEOnFace->vertex2()->id()))
					return true;
			}
			return false;
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		inline bool CTetSheller<TV, V, HE, TE, E, HF, F, T>::isBoundaryVertexInShelling(V * pV)
		{
			if (pV->boundary())
				return true;
			else {
				for (VertexTVertexIterator VTVIter(pMesh.get(), pV); !VTVIter.end(); ++VTVIter) {
					TV * pTV = *VTVIter;
					T * pT = pMesh->TVertexTet(pTV);
					if (pT->inShellingOrder) {
						return true;
					}
				}
			}
			return false;
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		inline bool CTetSheller<TV, V, HE, TE, E, HF, F, T>::isBoundaryEdgeInShelling(E * pE)
		{
			if (pE->boundary())
				return true;
			for (EdgeTEdgeIterator ETIter(pMesh.get(), pE); !ETIter.end(); ++ETIter) {
				TE *pTE = *ETIter;
				T* pT = pMesh->TEdgeTet(pTE);
				if (pT->inShellingOrder)
					return true;
			}
			return false;
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		inline E * CTetSheller<TV, V, HE, TE, E, HF, F, T>::oppositeEdgeOfTwoFaces(T * newSimplex, F * pF0, F * pF1)
		{
			E * pE;
			for (TetEdgeIterator TEIter(pMesh.get(), newSimplex); !TEIter.end(); ++TEIter) {
				pE = *TEIter;
				if (!edgeInFace(pE, pF0) && !edgeInFace(pE, pF1)) {
					return pE;
				}
			}
			//Should never gets here.
			assert(true);
			return pE;
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		inline void CTetSheller<TV, V, HE, TE, E, HF, F, T>::getFacesOnSurfaceInShelling(std::vector<F*> & pFacesOnSurface, T * pTet)
		{
			for (TetHalfFaceIterator THfIter(pMesh.get(), pTet); !THfIter.end(); ++THfIter) {
				HF * pHF = *THfIter;
				HF * pDualHF = pMesh->HalfFaceDual(pHF);

				if (pDualHF == NULL) {
					pFacesOnSurface.push_back(pMesh->HalfFaceFace(pHF));
				}
				else
				{
					T* pNeiTet = pMesh->HalfFaceTet(pDualHF);
					if (pNeiTet->inShellingOrder)
						pFacesOnSurface.push_back(pMesh->HalfFaceFace(pHF));
				}
			}
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		inline bool CTetSheller<TV, V, HE, TE, E, HF, F, T>::backTraceShelling(T * nextSimplex)
		{
			return false;
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CTetSheller<TV, V, HE, TE, E, HF, F, T>::putInCandidateList(T * candidateSimplex)
		{
			candidateList.push_back(candidateSimplex);
			candidateSimplex->inCandidateList = true;
		}
		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CTetSheller<TV, V, HE, TE, E, HF, F, T>::putInShellingList(T * nextSimplex)
		{
			m_pShellingOrder->push_back(nextSimplex);
			nextSimplex->inShellingOrder = true;
		}
		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CTetSheller<TV, V, HE, TE, E, HF, F, T>::removeFromCandidateList(typename std::list<T*>::iterator FLItr)
		{
			(*FLItr)->inCandidateList = false;
			candidateList.erase(FLItr);
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CTetSheller<TV, V, HE, TE, E, HF, F, T>::removeFromShellingList(typename std::list<T*>::iterator FLItr)
		{
			(*FLItr)->inShellingOrder = false;
			m_pShellingOrder->erase(FLItr);
		}

	}
}