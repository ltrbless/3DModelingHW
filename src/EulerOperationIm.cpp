#include"BrepStruct.h"
#include"Debug.h"
#include <vector>
#include<fstream>

#define _CRT_SECURE_NO_WARNINGS

int Loop::loopNum = 0;
std::vector<Loop*> Loop::loopVec;

int Vertex::vertexNum = 0;
std::vector<Vertex*> Vertex::vertexVec;

std::vector<Face*> Face::faceVec;

std::vector<Solid*> Solid::solidVec;

std::vector<HalfEdge*> HalfEdge::halfEdgeVec;

std::vector<Edge *> Edge::edgeVec;

Solid* EulerOp::mvfs(Point point) {

	// 构建点、面、环、体
	Vertex* vertex = new Vertex(point);
	Face* face = new Face();
	Loop* loop = new Loop();
	Solid* solid = new Solid();

	solid->sFace = face;
	face->fLoop = loop;
	face->fSolid = solid;
	loop->lHalfEdge = nullptr;
	loop->lFace = face;

	std::cout << "Use mvfs create point & dace & soild. \n";
	INFO_log("Use mvfs create point & dace & soild.");

	return solid;
}


HalfEdge* EulerOp::mev(Vertex *v1, Point p2, Loop* lp)
{
	Vertex *v2 = new Vertex(p2);

	HalfEdge *he1 = new HalfEdge();
	HalfEdge *he2 = new HalfEdge();
	
	Edge *edge = new Edge();

	Solid *solid = lp->lFace->fSolid;
	
	// 建立半边对应关系
	he1->edge = he2->edge = edge;
	he1->hLoop = he2->hLoop = lp;
	edge->halfEdge1 = he1;
	edge->halfEdge2 = he2;
	he1->neighbor = he2;
	he2->neighbor = he1;


	// 建立半边的数据
	he1->startV = v1;
	he1->endV = v2;
	he2->startV = v2;
	he2->endV = v1;

	if (lp->lHalfEdge == nullptr)
	{
		lp->lHalfEdge = he1;
		he1->nxtHEdge = he2; he1->preHEdge = he2;
		he2->nxtHEdge = he1; he2->preHEdge = he1;
	}
	else
	{
		HalfEdge* he = lp->lHalfEdge;
		for (he = lp->lHalfEdge; he->endV != v1; he = he->nxtHEdge);
		he->nxtHEdge->preHEdge = he2;
		he2->nxtHEdge = he->nxtHEdge;
		he2->preHEdge = he1;
		he1->nxtHEdge = he2;
		he->nxtHEdge = he1;
		he1->preHEdge = he;
	}


	// add  edge  to solid
	Edge *curEdgeInSolid = solid->sEdge;
	if (curEdgeInSolid == nullptr)
	{
		solid->sEdge = edge;
	}
	else
	{
		while (curEdgeInSolid->nxtEdge != nullptr) curEdgeInSolid = curEdgeInSolid->nxtEdge;
		curEdgeInSolid->nxtEdge = edge;
		edge->preEdge = curEdgeInSolid;
	}
	solid->edgeNum++;

	std::cout << "Use mev create v2 connect to " << v1->vertexId << " in loop " << lp->loopId << "\n";
	INFO_log("Use mev create v%d connect to v%d in loop %d", Vertex::vertexVec.size() - 1, v1->vertexId, lp->loopId);

	return he1;
}





Loop* EulerOp::mef(Vertex *v1, Vertex *v2, Loop* lp)
{
	if (lp == nullptr || !JudgeV1InLoop(v1, lp) || !JudgeV1InLoop(v2, lp)) {
		lp = FindLoopExist(v1, v2);
		if (lp == nullptr)
		{
			std::cout << "v1 is not in loop which you set.\n";
			return nullptr;
		}
	}

	Solid *solid = lp->lFace->fSolid;

	HalfEdge *he1 = new HalfEdge();
	HalfEdge *he2 = new HalfEdge();
	Edge *edge = new  Edge();
	
	he1->edge = he2->edge = edge;
	edge->halfEdge1 = he1;
	edge->halfEdge2 = he2;

	he1->startV = v1;
	he1->endV = v2;
	he2->startV = v2;
	he2->endV = v1;

	he1->neighbor = he2;
	he2->neighbor = he1;

	// find loop of end note is v1
	HalfEdge *het1, *het2;
	for (het1 = lp->lHalfEdge; het1->endV != v1; het1 = het1->nxtHEdge);
	for (het2 = het1; het2->endV != v2; het2 = het2->nxtHEdge);

	DEBUG("he->endV this v id is %d", het1->endV->vertexId);

	het1->nxtHEdge->preHEdge = he2;
	he2->nxtHEdge = het1->nxtHEdge;

	het1->nxtHEdge = he1;
	he1->preHEdge = het1;

	DEBUG("he->endV this v id is %d", het2->endV->vertexId);

	het2->nxtHEdge->preHEdge = he1;
	he1->nxtHEdge = het2->nxtHEdge;

	het2->nxtHEdge = he2;
	he2->preHEdge = het2;


	// create new loop and face.
	Loop* newLoop = new Loop();
	Face* newFace = new Face();

	newLoop->lFace = newFace;
	newLoop->lHalfEdge = he2; // half edge inner loop
	he2->hLoop = newLoop;
	for (HalfEdge* tmp = he2->nxtHEdge; tmp != he2; tmp = tmp->nxtHEdge) // update all halfedge link to new loop
	{
		tmp->hLoop = newLoop;
	}

	lp->lHalfEdge = he1; // half edge outter loop
	he1->hLoop = lp;
	for (HalfEdge* tmp = he1->nxtHEdge; tmp != he1; tmp = tmp->nxtHEdge)
	{
		tmp->hLoop = lp;
	}
	

	newFace->fLoop = newLoop;
	newFace->fSolid = solid;
	
	// add face to solid
	Face *tmpFace;
	for (tmpFace = solid->sFace; tmpFace->nxtFace != nullptr; tmpFace = tmpFace->nxtFace);
	tmpFace->nxtFace = newFace;
	newFace->preFace = tmpFace;

	//add edge to  solid
	Edge *curEdgeInSolid = solid->sEdge;
	if (curEdgeInSolid == nullptr)
	{
		solid->sEdge = edge;
	}
	else
	{
		while (curEdgeInSolid->nxtEdge != nullptr) curEdgeInSolid = curEdgeInSolid->nxtEdge;
		curEdgeInSolid->nxtEdge = edge;
		edge->preEdge = curEdgeInSolid;
	}
	solid->edgeNum++;

	std::cout << "Use mef operation to create a new face by connect node " << v1->vertexId << " & " << v2->vertexId << '\n';
	INFO_log("Use mef operation to create a new face by connect node %d & %d", v1->vertexId, v2->vertexId);

	return newLoop;
}


HalfEdge* EulerOp::FindHEdgeStartV(Vertex* v, Loop* lp)
{
	HalfEdge* he;
	for (he = lp->lHalfEdge; he->startV != v; he = he->nxtHEdge);
	return he;
}

HalfEdge* EulerOp::FindHEdgeEndV(Vertex* v, Loop* lp)
{
	HalfEdge* he;
	for (he = lp->lHalfEdge; he->endV != v; he = he->nxtHEdge);
	return he;
}


Loop* EulerOp::kemr(Vertex *v1, Vertex *v2, Loop *lp)
{
	// Chack loop for correction.
	if (lp == nullptr || !JudgeV1InLoop(v1, lp) || !JudgeV1InLoop(v2, lp)) {
		lp = FindLoopExist(v1, v2);
		if (lp == nullptr)
		{
			std::cout << "v1 is not in loop which you set.\n";
			return nullptr;
		}
	}

	Loop *newLoop = new Loop();
	Face *face = lp->lFace;
	Solid *solid = face->fSolid;

	HalfEdge* delHEdge;
	for (delHEdge = lp->lHalfEdge; delHEdge->startV != v1 || delHEdge->endV != v2; delHEdge = delHEdge->nxtHEdge) {}
	delHEdge->neighbor->nxtHEdge->preHEdge = delHEdge->preHEdge;
	delHEdge->preHEdge->nxtHEdge = delHEdge->neighbor->nxtHEdge;


	delHEdge->nxtHEdge->preHEdge = delHEdge->neighbor->preHEdge;
	delHEdge->neighbor->preHEdge->nxtHEdge = delHEdge->nxtHEdge;

	// Here need to judge the direction of loop for correct oriention.
	lp->lHalfEdge = delHEdge->preHEdge; 
	newLoop->lHalfEdge = delHEdge->nxtHEdge; 
	newLoop->lFace = face;

	if (face->fLoop == nullptr)
	{
		std::cout << "face is empty \n";
		return nullptr;
	}
	Loop *tmpLoop;
	for (tmpLoop = face->fLoop; tmpLoop->nxtLoop != nullptr; tmpLoop = tmpLoop->nxtLoop);
	tmpLoop->nxtLoop = newLoop;
	newLoop->preLoop = tmpLoop;

	Edge *delEdge = delHEdge->edge;
	Edge *tmpEdge = solid->sEdge;
	while (tmpEdge != delEdge)
		tmpEdge = tmpEdge->nxtEdge;
	
	if (tmpEdge->nxtEdge == nullptr)
		tmpEdge->preEdge->nxtEdge = nullptr;
	else if (tmpEdge->preEdge == nullptr) {
		solid->sEdge = tmpEdge->nxtEdge;
		tmpEdge->nxtEdge->preEdge = nullptr;
	}
	else
	{
		tmpEdge->nxtEdge->preEdge = tmpEdge->preEdge;
		tmpEdge->preEdge->nxtEdge = tmpEdge->nxtEdge;
	}
	solid->edgeNum--;

	INFO_log("Use kemr operation to delete a edge v%d--v%d and create a new loop %d", v1->vertexId, v2->vertexId, newLoop->loopId);


	for (int i = 0; i < Edge::edgeVec.size(); i++)
	{
		if (Edge::edgeVec[i] == tmpEdge)
		{
			delete(Edge::edgeVec[i]);
			Edge::edgeVec[i] = nullptr;
			break;
		}
	}
	return newLoop;
}

void EulerOp::kfmrh(Loop *outLp, Loop *lp)
{
	Face *f1 = outLp->lFace;
	Face *f2 = lp->lFace;

	if (f1->fLoop == nullptr)
	{
		std::cout << "face is empty \n";
		return;
	}
	Loop *tmpLoop;
	for (tmpLoop = f1->fLoop; tmpLoop->nxtLoop != nullptr; tmpLoop = tmpLoop->nxtLoop);
	tmpLoop->nxtLoop = lp;
	lp->preLoop = tmpLoop;

	// remove face

	Solid *solid = f1->fSolid;
	Face *delFace = solid->sFace;

	while (delFace != f2) delFace = delFace->nxtFace;

	if (delFace->nxtFace == nullptr)
		delFace->preFace->nxtFace = nullptr;
	else  if (delFace->preFace == nullptr) {
		solid->sFace = delFace->nxtFace;
		delFace->nxtFace->preFace = nullptr;
	}
	else
	{
		delFace->nxtFace->preFace = delFace->preFace;
		delFace->preFace->nxtFace = delFace->nxtFace;
	}

	INFO_log("Use kfmrh operation to delete a face and create a new headle");

	for (int i = 0; i < Face::faceVec.size(); i++)
	{
		if (Face::faceVec[i] == delFace)
		{
			delete(Face::faceVec[i]);
			Face::faceVec[i] = nullptr;
			break;
		}
	}
	return;
}


void EulerOp::sweep(Loop *lp, Eigen::Vector3d& dir, int dis)
{
	Face * face = lp->lFace;
	std::vector< std::vector<int> > vertexId;

	for (Loop * curLp = face->fLoop; curLp != nullptr; curLp = curLp->nxtLoop)
	{
		INFO_log("Cur loop id is %d", curLp->loopId);
		std::vector<int> tmpS;
		HalfEdge *curHe = curLp->lHalfEdge;
		Loop* loop = curHe->neighbor->hLoop;
		curHe = loop->lHalfEdge;
		INFO_log("Init edge is %d %d ", curHe->startV->vertexId, curHe->endV->vertexId);
		INFO_log("Real sweep loop id is %d", loop->loopId);
		Vertex *curV = curHe->startV;
		Vertex *initStartV = curHe->startV;
		Eigen::Vector3d curP = dis * dir;
		HalfEdge *firstHe = this->mev(curV, Point(curV->point.coord[0] + curP[0], curV->point.coord[1] + curP[1], curV->point.coord[2] + curP[2]), loop);
		Vertex *firstV = firstHe->endV;
		tmpS.push_back(firstV->vertexId);

		curHe = curHe->nxtHEdge;
		INFO_log("New edge is %d %d ", curHe->startV->vertexId, curHe->endV->vertexId);
		while (curHe->startV != initStartV)
		{
			curV = curHe->startV;
			HalfEdge *newHe = this->mev(curV, Point(curV->point.coord[0] + curP[0], curV->point.coord[1] + curP[1], curV->point.coord[2] + curP[2]), loop);
			INFO_log("New node pos is %f %f %f.", curV->point.coord[0], curV->point.coord[1], curV->point.coord[2]);
			this->mef(firstV, newHe->endV, loop);
			tmpS.push_back(newHe->endV->vertexId);

			firstV = newHe->endV;

			curHe = curHe->nxtHEdge;
		INFO_log("New edge is %d %d ", curHe->startV->vertexId, curHe->endV->vertexId);
		INFO_log("current sweep loop id is %d", loop->loopId);

			
		}
		this->mef(firstV, firstHe->endV, loop);
		vertexId.push_back(tmpS);
	}
	Loop * outLp;
	std::vector<Loop*> innerLp;

	this->getOutInnerLoop(vertexId, outLp, innerLp);

	for (int i = 0; i < innerLp.size(); i++)
	{
		this->kfmrh(outLp, innerLp[i]);
	}
}

void EulerOp::getOutInnerLoop(std::vector<std::vector<int>>& vecId, Loop *& outLp, std::vector<Loop*>& innerLp)
{
	int loc = 0;
	int area = 0;

	std::vector<int> verVec;
	for(auto&t : vecId)
		for (auto&t2 : t)
		{
			verVec.push_back(t2);
			if (verVec.size() == 3) goto STOP;
		}

STOP: {};

	Eigen::Vector3d v1 = Eigen::Vector3d(Vertex::vertexVec[verVec[0]]->point.coord[0], Vertex::vertexVec[verVec[0]]->point.coord[1], Vertex::vertexVec[verVec[0]]->point.coord[2]);
	Eigen::Vector3d v2 = Eigen::Vector3d(Vertex::vertexVec[verVec[1]]->point.coord[0], Vertex::vertexVec[verVec[1]]->point.coord[1], Vertex::vertexVec[verVec[1]]->point.coord[2]);
	Eigen::Vector3d v3 = Eigen::Vector3d(Vertex::vertexVec[verVec[2]]->point.coord[0], Vertex::vertexVec[verVec[2]]->point.coord[1], Vertex::vertexVec[verVec[2]]->point.coord[2]);

	Eigen::Vector3d v12 = v2 - v1;
	Eigen::Vector3d v13 = v3 - v1;

	Eigen::Vector3d orient = v12.cross(v13);

	int axis = 0;
	if (orient.dot(Eigen::Vector3d(1, 0, 0)) != 0) axis = 0;
	else if (orient.dot(Eigen::Vector3d(0, 1, 0)) != 0) axis = 1;
	else if (orient.dot(Eigen::Vector3d(0, 0, 1)) != 0) axis = 2;

	int x = (1 + axis) % 3;
	int y = (2 + axis) % 3;

	for (int i = 0; i < vecId.size(); i++)
	{
		int point_num = vecId[i].size();
		double tmpArea = 0;
		for (int j = 0; j < point_num; j++)
		{
			tmpArea += Vertex::vertexVec[vecId[i][j]]->point.coord[x] * Vertex::vertexVec[vecId[i][(j + 1) % point_num]]->point.coord[y] \
				- Vertex::vertexVec[vecId[i][j]]->point.coord[y] * Vertex::vertexVec[vecId[i][(j + 1) % point_num]]->point.coord[x];
		}

		tmpArea = fabs(tmpArea);

		if (area < tmpArea)
		{
			loc = i;
			area = tmpArea;
		}
	}
	std::vector<std::set<int>> vecIdS;
	for (int i = 0; i < vecId.size(); i++)
	{
		std::set<int> tmp;
		for (int j = 0; j < vecId[i].size(); j++)
		{
			tmp.insert(vecId[i][j]);
		}
		vecIdS.push_back(tmp);
	}
	for (Face * face = this->solid->sFace; face != nullptr; face = face->nxtFace)
	{
		if (face->fLoop->nxtLoop != nullptr) continue;

		HalfEdge *he = face->fLoop->lHalfEdge;
		std::set<int> tmps;
		int sv = he->startV->vertexId;
		tmps.insert(sv);

		for (he = he->nxtHEdge; he->startV->vertexId != sv; he = he->nxtHEdge)
		{
			tmps.insert(he->startV->vertexId);
		}

		for (int i = 0; i < vecIdS.size(); i++){
	
			if (tmps == vecIdS[i])
			{
				if (i == loc)
				{
					outLp = face->fLoop;
				}
				else
				{
					innerLp.push_back(face->fLoop);
				}
			}
		}

	}
	
}

bool EulerOp::judgeLoopClose(Loop * lpPt)
{
	HalfEdge *he = lpPt->lHalfEdge;
	int v1 = he->startV->vertexId;
	int v2 = he->endV->vertexId;

	int k = 0;
	for (he = he->nxtHEdge;k < 100 ;k ++, he = he->nxtHEdge)
	{
		if (he->startV->vertexId == v1 && he->endV->vertexId == v2)
		{
			return 1;
		}
		if (he->startV->vertexId == v2 && he->endV->vertexId == v1)
		{
			return 0;
		}
	}
	ERROR("Loop is not close");
	return 0;
}

bool EulerOp::judgeFaceView(Face *facePt)
{
	for (Loop *lpPt = facePt->fLoop; lpPt != nullptr; lpPt = lpPt->nxtLoop)
	{
		if (judgeLoopClose(lpPt)) continue;
		return 0;
	}
	return 1;
}


void getHole(std::vector<std::vector<int>>& vecId, std::vector<double>& hole, int x, int y)
{
	int loc = 0;
	int area = 0;

	for (int i = 0; i < vecId.size(); i++)
	{
		int point_num = vecId[i].size();
		double tmpArea = 0;
		for (int j = 0; j < point_num; j++)
		{
			tmpArea += Vertex::vertexVec[vecId[i][j]]->point.coord[x] * Vertex::vertexVec[vecId[i][(j + 1) % point_num]]->point.coord[y] \
				- Vertex::vertexVec[vecId[i][j]]->point.coord[y] * Vertex::vertexVec[vecId[i][(j + 1) % point_num]]->point.coord[x];
		}

		tmpArea = fabs(tmpArea);

		if (area < tmpArea)
		{
			loc = i;
			area = tmpArea;
		}
	}

	for (int i = 0; i < vecId.size(); i++)
	{
		if (i == loc) continue;
		double tmpx = 0, tmpy = 0;
		for (int j = 0; j < vecId[i].size(); j++)
		{
			tmpx += Vertex::vertexVec[vecId[i][j]]->point.coord[x];
			tmpy += Vertex::vertexVec[vecId[i][j]]->point.coord[y];
		}
		tmpx /= (double)(vecId[i].size());
		tmpy /= (double)(vecId[i].size());
		hole.push_back(tmpx);
		hole.push_back(tmpy);
	}
}

int EulerOp::CreateAllFaceToView(float** face)
{

	if (this->solid == nullptr || this->solid->sFace->fLoop->lHalfEdge == nullptr)
	{
		*face = nullptr;
		return 0;
	}

	int axis = 0;

	std::vector<float> tmpFace;

	Face *facePt = this->solid->sFace;
	int faceId = 0;
	
	std::vector < std::set<int> > faceNode;
	for (Face *tf = this->solid->sFace; tf != nullptr; tf = tf->nxtFace)
	{
		std::set<int> tmpS;
		for (Loop * tl = tf->fLoop; tl != nullptr; tl = tl->nxtLoop)
		{
			int v1 = tl->lHalfEdge->startV->vertexId;
			tmpS.insert(v1);
			for (HalfEdge *he = tl->lHalfEdge->nxtHEdge;he->startV->vertexId != v1; he = he->nxtHEdge)
			{
				tmpS.insert(he->startV->vertexId);
			}
		}
		faceNode.push_back(tmpS);
	}

	for (int i = 0; i < faceNode.size(); i++)
	{
		if (faceNode[i].size() == 0) continue;
		for (int j = 0; j < faceNode.size(); j++)
		{
			if (i == j) continue;
			if (faceNode[j].size() == 0) continue;
			std::set<int> intersection;
			for (auto & t : faceNode[i])
			{
				if (faceNode[j].count(t))
				{
					intersection.insert(t);
				}
			}
			if (intersection == faceNode[i])
			{
				faceNode[i].clear();
				break;
			}
			if(intersection == faceNode[j])
			{
				faceNode[j].clear();
				continue;
			}
		}
	}

	for (; facePt != nullptr; facePt = facePt->nxtFace, faceId++)
	{
		if (faceNode[faceId].size() < 3) continue;

		if (!judgeFaceView(facePt)) continue; // cannot view because this face is not close.

		std::vector<int> verVec;
		int loopNum = 0;
	
		/****************************/
		/*  (1 + axis) % 3  &&  (2 + axis) % 3  */

		for (Loop *loopPt = facePt->fLoop; loopPt != nullptr; loopPt = loopPt->nxtLoop)
		{
			loopNum++;
		}

		FILE *fp = nullptr;
		if (loopNum)
		{
			if (fopen_s(&fp, "cube_test.poly", "w"))
			{
				ERROR("Open cube.poly is failure.");
				return -1;
			}
		}


		if (loopNum == 1)
		{
			//continue;
			verVec.clear();
			HalfEdge *he = facePt->fLoop->lHalfEdge;
			int sv = he->startV->vertexId;
			verVec.push_back(sv);
			for (he = he->nxtHEdge; he->startV->vertexId != sv; he = he->nxtHEdge)
			{
				verVec.push_back(he->startV->vertexId);
			}

			Eigen::Vector3d v1 = Eigen::Vector3d(Vertex::vertexVec[verVec[0]]->point.coord[0], Vertex::vertexVec[verVec[0]]->point.coord[1], Vertex::vertexVec[verVec[0]]->point.coord[2]);
			Eigen::Vector3d v2 = Eigen::Vector3d(Vertex::vertexVec[verVec[1]]->point.coord[0], Vertex::vertexVec[verVec[1]]->point.coord[1], Vertex::vertexVec[verVec[1]]->point.coord[2]);
			Eigen::Vector3d v3 = Eigen::Vector3d(Vertex::vertexVec[verVec[2]]->point.coord[0], Vertex::vertexVec[verVec[2]]->point.coord[1], Vertex::vertexVec[verVec[2]]->point.coord[2]);

			Eigen::Vector3d v12 = v2 - v1;
			Eigen::Vector3d v13 = v3 - v1;

			Eigen::Vector3d orient = v12.cross(v13);

			if (orient.dot(Eigen::Vector3d(1, 0, 0)) != 0) axis = 0;
			else if (orient.dot(Eigen::Vector3d(0, 1, 0)) != 0) axis = 1;
			else if (orient.dot(Eigen::Vector3d(0, 0, 1)) != 0) axis = 2;


			fprintf(fp, "%d %d %d %d\n", (int)verVec.size(), 2, 0, 0);
			for (int i = 0; i < verVec.size(); i++)
			{
				fprintf(fp, "%d %f %f\n", i + 1, Vertex::vertexVec[verVec[i]]->point.coord[(1 + axis) % 3], Vertex::vertexVec[verVec[i]]->point.coord[(2 + axis) % 3]);
			}
			fprintf(fp, "%d %d\n", (int)verVec.size(), 0);
			for (int i = 0; i < verVec.size(); i++)
			{
				fprintf(fp, "%d %d %d\n", i + 1, i + 1, (i + 1) % (int)(verVec.size()) + 1);
			}
			fprintf(fp, "0\n");
		}
		else
		{
			std::cout << "loop num is " << loopNum << "\n";
			std::vector<std::vector<int>> vecId;

			verVec.clear();
			HalfEdge *he = facePt->fLoop->lHalfEdge;
			int sv = he->startV->vertexId;
			verVec.push_back(sv);
			for (he = he->nxtHEdge; he->startV->vertexId != sv; he = he->nxtHEdge)
			{
				verVec.push_back(he->startV->vertexId);
			}

			Eigen::Vector3d v1 = Eigen::Vector3d(Vertex::vertexVec[verVec[0]]->point.coord[0], Vertex::vertexVec[verVec[0]]->point.coord[1], Vertex::vertexVec[verVec[0]]->point.coord[2]);
			Eigen::Vector3d v2 = Eigen::Vector3d(Vertex::vertexVec[verVec[1]]->point.coord[0], Vertex::vertexVec[verVec[1]]->point.coord[1], Vertex::vertexVec[verVec[1]]->point.coord[2]);
			Eigen::Vector3d v3 = Eigen::Vector3d(Vertex::vertexVec[verVec[2]]->point.coord[0], Vertex::vertexVec[verVec[2]]->point.coord[1], Vertex::vertexVec[verVec[2]]->point.coord[2]);

			Eigen::Vector3d v12 = v2 - v1;
			Eigen::Vector3d v13 = v3 - v1;

			Eigen::Vector3d orient = v12.cross(v13);

			if (orient.dot(Eigen::Vector3d(1, 0, 0)) != 0) axis = 0;
			else if (orient.dot(Eigen::Vector3d(0, 1, 0)) != 0) axis = 1;
			else if (orient.dot(Eigen::Vector3d(0, 0, 1)) != 0) axis = 2;

			int x = (1 + axis) % 3;
			int y = (2 + axis) % 3;
			
			int vertexNum = 0;
			verVec.clear();
			for (Loop *lp = facePt->fLoop; lp != nullptr; lp = lp->nxtLoop)
			{
				std::vector<int> tmpVec;
				int sv = lp->lHalfEdge->startV->vertexId;
				tmpVec.push_back(sv);
				for (HalfEdge *he = lp->lHalfEdge->nxtHEdge; he->startV->vertexId != sv; he = he->nxtHEdge)
				{
					tmpVec.push_back(he->startV->vertexId);
				}
				vecId.push_back(tmpVec);
				vertexNum += tmpVec.size();
			}

			fprintf(fp, "%d %d %d %d\n", vertexNum, 2, 0, 0);
			int k = 0;
			for (int i = 0; i < vecId.size(); i++)
			{
				for (int j = 0; j < vecId[i].size(); j++)
				{
					k++;
					verVec.push_back(vecId[i][j]);
					fprintf(fp, "%d %f %f\n", k, Vertex::vertexVec[vecId[i][j]]->point.coord[x], Vertex::vertexVec[vecId[i][j]]->point.coord[y]);
				}
			}
			fprintf(fp, "%d %d\n", vertexNum, 0);
			
			k = 0;
			int lj = 0;
			for (int i = 0; i < vecId.size(); i++)
			{
				for (int j = 0; j < vecId[i].size(); j++)
				{
					k++;
					fprintf(fp, "%d %d %d\n", k, j + 1 + lj, (j + 1) % (int)(vecId[i].size()) + 1 + lj);
				}
				lj = k;
			}

			std::vector<double> hole;
			getHole(vecId, hole, x, y);
			fprintf(fp, "%d\n", vecId.size() - 1);
			for (int i = 0; i < vecId.size() - 1; i++)
			{
				fprintf(fp, "%d %f %f\n", i + 1, hole[i * 2 + 0], hole[i * 2 + 1]);
			}
		}
		fclose(fp);

		/**************** process **************************/

		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		TCHAR commandLine[] = "./triangle.exe -p cube_test.poly";
		// Start the child process. 
		if (!CreateProcess(NULL,   // No module name (use command line)
			commandLine,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi)           // Pointer to PROCESS_INFORMATION structure
			)
		{
			printf("CreateProcess failed (%d).\n", GetLastError());
			return -1 ;
		}
		// Wait until child process exits.
		WaitForSingleObject(pi.hProcess, INFINITE);
		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		/************************** read face **************************/
		
		std::ifstream file;
		file.open("./cube_test.1.ele");
		if (!file.is_open())
		{
			ERROR("This is not find !");
			return -1;
		}

		std::string line;
		std::stringstream ss;
		std::getline(file, line);
		ss.clear();
		ss.str(line);
		int triNum, _, curV;
		ss >> triNum;


		for (int i = 0; i < triNum; i++)
		{
			std::getline(file, line);
			ss.clear();
			ss.str(line);
			ss >> _;
			for (int j = 0; j < 3; j++)
			{
				ss >> curV;
				for (int k = 0; k < 3; k++)
				{
					tmpFace.push_back(Vertex::vertexVec[verVec[curV - 1]]->point.coord[k]); // 从0开始所以减一
				}
			}
		}

	}

	std::cout << "tmpFace size is" << tmpFace.size() << '\n';

	*face = new float[tmpFace.size()];

	for (int i = 0; i < tmpFace.size(); i++)
	{
		(*face)[i] = tmpFace[i] / 10.0; // 坐标范围 10
	}

	return tmpFace.size() / 3;
}

void EulerOp::PrintAllFace() {

	Face *facePt = this->solid->sFace;
	int faceId = 0;
	for (; facePt != nullptr; facePt = facePt->nxtFace, faceId++)
	{
		INFO_log("The %dth face commbine is :", faceId);
		for (Loop *loopPt = facePt->fLoop; loopPt != nullptr; loopPt = loopPt->nxtLoop)
		{
			INFO_log("Loop Id is %d", loopPt->loopId);
			std::vector<int> vertexLst;
			int sv1 = loopPt->lHalfEdge->startV->vertexId;
			int sv2 = loopPt->lHalfEdge->endV->vertexId;
			vertexLst.push_back(sv1);
			vertexLst.push_back(sv2);
			HalfEdge* he = loopPt->lHalfEdge->nxtHEdge;
			int f = 1;
			for (int k = 0; k  < 1000; he = he->nxtHEdge, k++)
			{
				vertexLst.push_back(he->startV->vertexId);
				vertexLst.push_back(he->endV->vertexId);
				if (k > 900){ERROR("When print all face , die loop");exit(0);}
				if (he->startV->vertexId == sv1 && he->endV->vertexId == sv2)
				{
					f = 1;
					break;
				}
				if (he->startV->vertexId == sv2 && he->endV->vertexId == sv1)
				{
					f = 0;
					break;
				}
			}
			if (1)
			{
				for (int i = 0; i < vertexLst.size(); i += 2)
				{
					INFO_log("%d %d", vertexLst[i], vertexLst[i + 1]);
				}
			}
		}
	}
}

void EulerOp::PrintAllEdge()
{
	Edge *edgePt = this->solid->sEdge;
	for (; edgePt != nullptr; edgePt = edgePt->nxtEdge)
	{
		INFO("Edge : v1 & v2 is : %d , %d", edgePt->halfEdge1->startV->vertexId, edgePt->halfEdge1->endV->vertexId);
	}
}

void EulerOp::GetAllEdge(float ** edges)
{
	if (this->solid == nullptr)
	{
		*edges = nullptr;
		return;
	}
	*edges = new float[6 * this->solid->edgeNum];

	Edge *edgePt = this->solid->sEdge;
	for (int i = 0; edgePt != nullptr; edgePt = edgePt->nxtEdge, i += 6)
	{
		(*edges)[i + 0] = edgePt->halfEdge1->startV->point.coord[0];
		(*edges)[i + 1] = edgePt->halfEdge1->startV->point.coord[1];
		(*edges)[i + 2] = edgePt->halfEdge1->startV->point.coord[2];
		(*edges)[i + 3] = edgePt->halfEdge1->endV->point.coord[0];
		(*edges)[i + 4] = edgePt->halfEdge1->endV->point.coord[1];
		(*edges)[i + 5] = edgePt->halfEdge1->endV->point.coord[2];
	}
	for (int i = 0; i < this->solid->edgeNum * 6 ; i++)
	{
		(*edges)[i] = (*edges)[i] / 10.0f;
	}

}

void EulerOp::GetAllVertex(float ** vertexs)
{
	if (Vertex::vertexNum == 0)
	{
		*vertexs = nullptr;
		return;
	}
	*vertexs = new float[Vertex::vertexNum * 3];
	for (int i = 0; i < Vertex::vertexVec.size(); i++)
	{
		(*vertexs)[i * 3 + 0] = Vertex::vertexVec[i]->point.coord[0];
		(*vertexs)[i * 3 + 1] = Vertex::vertexVec[i]->point.coord[1];
		(*vertexs)[i * 3 + 2] = Vertex::vertexVec[i]->point.coord[2];
	}
	for (int i = 0; i < Vertex::vertexNum * 3; i++)
	{
		(*vertexs)[i] = (*vertexs)[i] / 10.0f;
	}
}

void EulerOp::exampleEulerOp()
{
	this->solid = this->mvfs(Point(0, 0, 0));
	this->mev(Vertex::vertexVec[0], Point(3, 0, 0), Loop::loopVec[0]);
	this->mev(Vertex::vertexVec[1], Point(3, 3, 0), Loop::loopVec[0]);
	this->mev(Vertex::vertexVec[2], Point(0, 3, 0), Loop::loopVec[0]);
	this->mef(Vertex::vertexVec[3], Vertex::vertexVec[0], Loop::loopVec[0]);

	this->mev(Vertex::vertexVec[0], Point(1, 1, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[4], Point(2, 1, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[5], Point(2, 2, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[6], Point(1, 2, 0), Loop::loopVec[1]);
	
	this->mef(Vertex::vertexVec[7], Vertex::vertexVec[4], Loop::loopVec[1]);
	this->kemr(Vertex::vertexVec[0], Vertex::vertexVec[4], Loop::loopVec[1]);

	// Big side
	this->mev(Vertex::vertexVec[0], Point(0, 0, 3), Loop::loopVec[0]);
	this->mev(Vertex::vertexVec[1], Point(3, 0, 3), Loop::loopVec[0]);
	this->mev(Vertex::vertexVec[2], Point(3, 3, 3), Loop::loopVec[0]);
	this->mev(Vertex::vertexVec[3], Point(0, 3, 3), Loop::loopVec[0]);

	this->mef(Vertex::vertexVec[8], Vertex::vertexVec[9], Loop::loopVec[0]);
	this->mef(Vertex::vertexVec[9], Vertex::vertexVec[10], Loop::loopVec[0]);
	this->mef(Vertex::vertexVec[10], Vertex::vertexVec[11], Loop::loopVec[0]);
	this->mef(Vertex::vertexVec[11], Vertex::vertexVec[8], Loop::loopVec[0]);

	//inner side
	this->mev(Vertex::vertexVec[4], Point(1, 1, 3), Loop::loopVec[2]);
	this->mev(Vertex::vertexVec[5], Point(2, 1, 3), Loop::loopVec[2]);
	this->mev(Vertex::vertexVec[6], Point(2, 2, 3), Loop::loopVec[2]);
	this->mev(Vertex::vertexVec[7], Point(1, 2, 3), Loop::loopVec[2]);

	this->mef(Vertex::vertexVec[12], Vertex::vertexVec[13], Loop::loopVec[2]);
	this->mef(Vertex::vertexVec[13], Vertex::vertexVec[14], Loop::loopVec[2]);
	this->mef(Vertex::vertexVec[14], Vertex::vertexVec[15], Loop::loopVec[2]);
	this->mef(Vertex::vertexVec[15], Vertex::vertexVec[12], Loop::loopVec[2]);

	this->kfmrh(Loop::loopVec[0], Loop::loopVec[11]);

	// second hool
	
	this->mev(Vertex::vertexVec[4], Point(1, 1, 1), Loop::loopVec[2]); // vertex id 16
	this->mev(Vertex::vertexVec[16], Point(2, 1, 1), Loop::loopVec[2]); // 17
	this->mev(Vertex::vertexVec[17], Point(2, 1, 2), Loop::loopVec[2]); // 18
	this->mev(Vertex::vertexVec[18], Point(1, 1, 2), Loop::loopVec[2]); // 19
	this->mef(Vertex::vertexVec[19], Vertex::vertexVec[16], Loop::loopVec[2]);
	this->kemr(Vertex::vertexVec[4], Vertex::vertexVec[16], Loop::loopVec[2]);


	this->mev(Vertex::vertexVec[16], Point(1, 0, 1), Loop::loopVec[12]);
	this->mev(Vertex::vertexVec[17], Point(2, 0, 1), Loop::loopVec[12]);
	this->mev(Vertex::vertexVec[18], Point(2, 0, 2), Loop::loopVec[12]);
	this->mev(Vertex::vertexVec[19], Point(1, 0, 2), Loop::loopVec[12]);

	this->mef(Vertex::vertexVec[20], Vertex::vertexVec[21], Loop::loopVec[12]);
	this->mef(Vertex::vertexVec[21], Vertex::vertexVec[22], Loop::loopVec[12]);
	this->mef(Vertex::vertexVec[22], Vertex::vertexVec[23], Loop::loopVec[12]);
	this->mef(Vertex::vertexVec[23], Vertex::vertexVec[20], Loop::loopVec[12]);
	this->kfmrh(Loop::loopVec[4], Loop::loopVec[17]);


	// three hool

	this->mev(Vertex::vertexVec[5], Point(2, 1, 1), Loop::loopVec[8]); // vertex id 16
	this->mev(Vertex::vertexVec[24], Point(2, 2, 1), Loop::loopVec[8]); // 17
	this->mev(Vertex::vertexVec[25], Point(2, 2, 2), Loop::loopVec[8]); // 18
	this->mev(Vertex::vertexVec[26], Point(2, 1, 2), Loop::loopVec[8]); // 19
	this->mef(Vertex::vertexVec[27], Vertex::vertexVec[24], Loop::loopVec[8]);
	this->kemr(Vertex::vertexVec[5], Vertex::vertexVec[24], Loop::loopVec[8]);


	this->mev(Vertex::vertexVec[24], Point(3, 1, 1), Loop::loopVec[18]);
	this->mev(Vertex::vertexVec[25], Point(3, 2, 1), Loop::loopVec[18]);
	this->mev(Vertex::vertexVec[26], Point(3, 2, 2), Loop::loopVec[18]);
	this->mev(Vertex::vertexVec[27], Point(3, 1, 2), Loop::loopVec[18]);

	this->mef(Vertex::vertexVec[28], Vertex::vertexVec[29], Loop::loopVec[18]);
	this->mef(Vertex::vertexVec[29], Vertex::vertexVec[30], Loop::loopVec[18]);
	this->mef(Vertex::vertexVec[30], Vertex::vertexVec[31], Loop::loopVec[18]);
	this->mef(Vertex::vertexVec[31], Vertex::vertexVec[28], Loop::loopVec[18]);
	this->kfmrh(Loop::loopVec[5], Loop::loopVec[23]);

	// third hool

	this->mev(Vertex::vertexVec[6], Point(2, 2, 1), Loop::loopVec[9]); // vertex id 16
	this->mev(Vertex::vertexVec[32], Point(1, 2, 1), Loop::loopVec[9]); // 17
	this->mev(Vertex::vertexVec[33], Point(1, 2, 2), Loop::loopVec[9]); // 18
	this->mev(Vertex::vertexVec[34], Point(2, 2, 2), Loop::loopVec[9]); // 19
	this->mef(Vertex::vertexVec[35], Vertex::vertexVec[32], Loop::loopVec[9]);
	this->kemr(Vertex::vertexVec[6], Vertex::vertexVec[32], Loop::loopVec[9]);

	this->mev(Vertex::vertexVec[32], Point(2, 3, 1), Loop::loopVec[24]);
	this->mev(Vertex::vertexVec[33], Point(1, 3, 1), Loop::loopVec[24]);
	this->mev(Vertex::vertexVec[34], Point(1, 3, 2), Loop::loopVec[24]);
	this->mev(Vertex::vertexVec[35], Point(2, 3, 2), Loop::loopVec[24]);

	this->mef(Vertex::vertexVec[36], Vertex::vertexVec[37], Loop::loopVec[24]);
	this->mef(Vertex::vertexVec[37], Vertex::vertexVec[38], Loop::loopVec[24]);
	this->mef(Vertex::vertexVec[38], Vertex::vertexVec[39], Loop::loopVec[24]);
	this->mef(Vertex::vertexVec[39], Vertex::vertexVec[36], Loop::loopVec[24]);
	this->kfmrh(Loop::loopVec[6], Loop::loopVec[29]);

	// five

	this->mev(Vertex::vertexVec[7], Point(1, 2, 1), Loop::loopVec[10]); // vertex id 16
	this->mev(Vertex::vertexVec[40], Point(1, 1, 1), Loop::loopVec[10]); // 17
	this->mev(Vertex::vertexVec[41], Point(1, 1, 2), Loop::loopVec[10]); // 18
	this->mev(Vertex::vertexVec[42], Point(1, 2, 2), Loop::loopVec[10]); // 19
	this->mef(Vertex::vertexVec[43], Vertex::vertexVec[40], Loop::loopVec[10]);
	this->kemr(Vertex::vertexVec[7], Vertex::vertexVec[40], Loop::loopVec[10]);

	this->mev(Vertex::vertexVec[40], Point(0, 2, 1), Loop::loopVec[30]);
	this->mev(Vertex::vertexVec[41], Point(0, 1, 1), Loop::loopVec[30]);
	this->mev(Vertex::vertexVec[42], Point(0, 1, 2), Loop::loopVec[30]);
	this->mev(Vertex::vertexVec[43], Point(0, 2, 2), Loop::loopVec[30]);

	this->mef(Vertex::vertexVec[44], Vertex::vertexVec[45], Loop::loopVec[30]);
	this->mef(Vertex::vertexVec[45], Vertex::vertexVec[46], Loop::loopVec[30]);
	this->mef(Vertex::vertexVec[46], Vertex::vertexVec[47], Loop::loopVec[30]);
	this->mef(Vertex::vertexVec[47], Vertex::vertexVec[44], Loop::loopVec[30]);
	this->kfmrh(Loop::loopVec[7], Loop::loopVec[35]);


	//







	//this->mev(Vertex::vertexVec[0], Point(1, 1, 0), Loop::loopVec[1]);
	//this->mev(Vertex::vertexVec[4], Point(2, 1, 0), Loop::loopVec[1]);
	//this->mev(Vertex::vertexVec[5], Point(2, 2, 0), Loop::loopVec[1]);
	//this->mev(Vertex::vertexVec[6], Point(1, 2, 0), Loop::loopVec[1]);

	//this->mef(Vertex::vertexVec[7], Vertex::vertexVec[4], Loop::loopVec[1]);

	//this->kemr(Vertex::vertexVec[0], Vertex::vertexVec[4], Loop::loopVec[1]);

	//this->mev(Vertex::vertexVec[0], Point(0, 0, -3), Loop::loopVec[0]);
	//this->mev(Vertex::vertexVec[1], Point(3, 0, -3), Loop::loopVec[0]);
	//this->mev(Vertex::vertexVec[2], Point(3, 3, -3), Loop::loopVec[0]);
	//this->mev(Vertex::vertexVec[3], Point(0, 3, -3), Loop::loopVec[0]);

	//this->mef(Vertex::vertexVec[8], Vertex::vertexVec[9], Loop::loopVec[0]);
	//this->mef(Vertex::vertexVec[9], Vertex::vertexVec[10], Loop::loopVec[0]);
	//this->mef(Vertex::vertexVec[10], Vertex::vertexVec[11], Loop::loopVec[0]);
	//this->mef(Vertex::vertexVec[11], Vertex::vertexVec[8], Loop::loopVec[0]);

	//Loop* lp = Euler.FindLoopOrder(Vertex::vertexVec[7], Vertex::vertexVec[4]);
	//INFO("loop id is %d", lp->loopId); // 2

	//this->mev(Vertex::vertexVec[4], Point(1, 1, -1), Loop::loopVec[2]);
	//this->mev(Vertex::vertexVec[5], Point(2, 1, -1), Loop::loopVec[2]);
	//this->mev(Vertex::vertexVec[6], Point(2, 2, -1), Loop::loopVec[2]);
	//this->mev(Vertex::vertexVec[7], Point(1, 2, -1), Loop::loopVec[2]);

	//this->mef(Vertex::vertexVec[12], Vertex::vertexVec[13], Loop::loopVec[2]);
	//this->mef(Vertex::vertexVec[13], Vertex::vertexVec[14], Loop::loopVec[8]);
	//this->mef(Vertex::vertexVec[14], Vertex::vertexVec[15], nullptr);
	//this->mef(Vertex::vertexVec[15], Vertex::vertexVec[12], nullptr);

	//Loop* lp = Euler.FindLoopOrder(Vertex::vertexVec[9], Vertex::vertexVec[8]);
	//INFO("loop id is %d", lp->loopId); // 0
	//Loop *lp = Euler.FindLoopOrder(Vertex::vertexVec[12], Vertex::vertexVec[13]);
	//INFO("loop id is %d", lp->loopId); // 11

	//this->kfmrh(Loop::loopVec[0], Loop::loopVec[11]);
}
void EulerOp::exampleEulerOp2()
{
	/**********Debug**********/

	this->solid = this->mvfs(Point(0, 0, 0));
	this->mev(Vertex::vertexVec[0], Point(0, 5, 0), Loop::loopVec[0]);
	this->mev(Vertex::vertexVec[1], Point(5, 5, 0), Loop::loopVec[0]);
	this->mev(Vertex::vertexVec[2], Point(5, 0, 0), Loop::loopVec[0]);
	this->mef(Vertex::vertexVec[3], Vertex::vertexVec[0], Loop::loopVec[0]);

	this->mev(Vertex::vertexVec[0], Point(1, 1, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[4], Point(1, 2, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[5], Point(2, 2, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[6], Point(2, 1, 0), Loop::loopVec[1]);

	this->mef(Vertex::vertexVec[7], Vertex::vertexVec[4], Loop::loopVec[1]);

	this->kemr(Vertex::vertexVec[0], Vertex::vertexVec[4], Loop::loopVec[0]);

	this->mev(Vertex::vertexVec[0], Point(3, 3, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[8], Point(4, 3, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[9], Point(4, 4, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[10], Point(3, 4, 0), Loop::loopVec[1]);

	this->mef(Vertex::vertexVec[11], Vertex::vertexVec[8], Loop::loopVec[1]);

	this->kemr(Vertex::vertexVec[0], Vertex::vertexVec[8], Loop::loopVec[1]);

/***************************************************************************************/

	this->mev(Vertex::vertexVec[0], Point(1.5, 3, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[12], Point(2, 3.5, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[13], Point(1.5, 4, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[14], Point(1, 3.5, 0), Loop::loopVec[1]);

	this->mef(Vertex::vertexVec[15], Vertex::vertexVec[12], Loop::loopVec[1]);

	this->kemr(Vertex::vertexVec[0], Vertex::vertexVec[12], Loop::loopVec[1]);

/***************************************************************************************/

	this->mev(Vertex::vertexVec[0], Point(3.25, 1, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[16], Point(3.75, 1, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[17], Point(4, 1.25, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[18], Point(4, 1.75, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[19], Point(3.75, 2, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[20], Point(3.25, 2, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[21], Point(3, 1.75, 0), Loop::loopVec[1]);
	this->mev(Vertex::vertexVec[22], Point(3, 1.25, 0), Loop::loopVec[1]);

	this->mef(Vertex::vertexVec[23], Vertex::vertexVec[16], Loop::loopVec[1]);

	this->kemr(Vertex::vertexVec[0], Vertex::vertexVec[16], Loop::loopVec[1]);

	Eigen::Vector3d vec = Eigen::Vector3d(0, 0, -1);
	this->sweep(Loop::loopVec[1],vec, 1);
		

	/***********Demo***********/
//
//	this->solid = this->mvfs(Point(0, 0, 0));
//	this->mev(Vertex::vertexVec[0], Point(5, 0, 0), Loop::loopVec[0]);
//	this->mev(Vertex::vertexVec[1], Point(5, 5, 0), Loop::loopVec[0]);
//	this->mev(Vertex::vertexVec[2], Point(0, 5, 0), Loop::loopVec[0]);
//	this->mef(Vertex::vertexVec[3], Vertex::vertexVec[0], Loop::loopVec[0]);
//
//	this->mev(Vertex::vertexVec[0], Point(1, 1, 0), Loop::loopVec[1]);
//	this->mev(Vertex::vertexVec[4], Point(2, 1, 0), Loop::loopVec[1]);
//	this->mev(Vertex::vertexVec[5], Point(2, 2, 0), Loop::loopVec[1]);
//	this->mev(Vertex::vertexVec[6], Point(1, 2, 0), Loop::loopVec[1]);
//
//	this->mef(Vertex::vertexVec[7], Vertex::vertexVec[4], Loop::loopVec[1]);
//
//	this->kemr(Vertex::vertexVec[0], Vertex::vertexVec[4], Loop::loopVec[1]);
//
///***************/
//
//	this->mev(Vertex::vertexVec[0], Point(3, 3, 0), Loop::loopVec[1]);
//	this->mev(Vertex::vertexVec[8], Point(4, 3, 0), Loop::loopVec[1]);
//	this->mev(Vertex::vertexVec[9], Point(4, 4, 0), Loop::loopVec[1]);
//	this->mev(Vertex::vertexVec[10], Point(3, 4, 0), Loop::loopVec[1]);
//
//	this->mef(Vertex::vertexVec[11], Vertex::vertexVec[8], Loop::loopVec[1]);
//
//	this->kemr(Vertex::vertexVec[0], Vertex::vertexVec[8], Loop::loopVec[1]);
//
//	Eigen::Vector3d vec = Eigen::Vector3d(0, 0, -1);
//	this->sweep(Loop::loopVec[1],vec, 5);

	/***********Demo***********/

	//Loop* lp = Euler.FindLoopOrder(Vertex::vertexVec[0], Vertex::vertexVec[3]);
	//INFO("loop id is %d", lp->loopId); // 0

	//this->mev(Vertex::vertexVec[0], Point(0, 0, -1), Loop::loopVec[0]);
	//this->mev(Vertex::vertexVec[1], Point(3, 0, -1), Loop::loopVec[0]);
	//this->mev(Vertex::vertexVec[2], Point(3, 3, -1), Loop::loopVec[0]);
	//this->mev(Vertex::vertexVec[3], Point(0, 3, -1), Loop::loopVec[0]);

	//this->mef(Vertex::vertexVec[8], Vertex::vertexVec[9], Loop::loopVec[0]);
	//this->mef(Vertex::vertexVec[9], Vertex::vertexVec[10], Loop::loopVec[0]);
	//this->mef(Vertex::vertexVec[10], Vertex::vertexVec[11], Loop::loopVec[0]);
	//this->mef(Vertex::vertexVec[11], Vertex::vertexVec[8], Loop::loopVec[0]);

	////Loop* lp = Euler.FindLoopOrder(Vertex::vertexVec[7], Vertex::vertexVec[4]);
	////INFO("loop id is %d", lp->loopId); // 2

	//this->mev(Vertex::vertexVec[4], Point(1, 1, -1), Loop::loopVec[2]);
	//this->mev(Vertex::vertexVec[5], Point(2, 1, -1), Loop::loopVec[2]);
	//this->mev(Vertex::vertexVec[6], Point(2, 2, -1), Loop::loopVec[2]);
	//this->mev(Vertex::vertexVec[7], Point(1, 2, -1), Loop::loopVec[2]);

	//this->mef(Vertex::vertexVec[12], Vertex::vertexVec[13], Loop::loopVec[2]);
	//this->mef(Vertex::vertexVec[13], Vertex::vertexVec[14], Loop::loopVec[8]);
	//this->mef(Vertex::vertexVec[14], Vertex::vertexVec[15], nullptr);
	//this->mef(Vertex::vertexVec[15], Vertex::vertexVec[12], nullptr);

	////Loop* lp = Euler.FindLoopOrder(Vertex::vertexVec[9], Vertex::vertexVec[8]);
	////INFO("loop id is %d", lp->loopId); // 0
	////Loop *lp = Euler.FindLoopOrder(Vertex::vertexVec[12], Vertex::vertexVec[13]);
	////INFO("loop id is %d", lp->loopId); // 11

	//this->kfmrh(Loop::loopVec[0], Loop::loopVec[11]);
}


Loop* EulerOp::FindLoopOrder(Vertex *v1, Vertex *v2)
{
	int f = 0;
	for (auto& tmpLp : Loop::loopVec)
	{
		f = 0;
		HalfEdge *Head = tmpLp->lHalfEdge;
		HalfEdge *tmpH = tmpLp->lHalfEdge;
		//DEBUG("tmpLp is %d", tmpLp->loopId);
		while (tmpH->startV != v1 || tmpH->endV != v2)
		{
			tmpH = tmpH->nxtHEdge;
			//DEBUG("tmpH start is %d tmpH end is %d", tmpH->startV->vertexId, tmpH->endV->vertexId);
			if (tmpH == Head)
			{
				f = 1;
				break;
			}
		}
		if (f == 1) continue; // not find
		else return tmpLp;
	}
	std::cout << "[Order] Not find loop where (v1--v2) is.\n";

	return nullptr;
}

Loop* EulerOp::FindLoopExist(Vertex *v1, Vertex *v2)
{
	int f1 = 0;
	int f2 = 0;
	int f = 0;
	for (auto& tmpLp : Loop::loopVec)
	{
		HalfEdge *Head = tmpLp->lHalfEdge;
		HalfEdge *tmpH = tmpLp->lHalfEdge;
		f1 = f2 = f = 0;
		while (f1 == 0 || f2 == 0)
		{
			if (tmpH->startV == v1) f1 = 1;
			if (tmpH->startV == v2) f2 = 1;
			tmpH = tmpH->nxtHEdge;
			if (tmpH == Head)
			{
				f = 1;
				break;
			}
		}
		if (f == 1) continue; // not find
		else return tmpLp;
	}
	std::cout << "[Exist] Not find loop where (v1--v2) is.\n";
	return nullptr;
}

bool  EulerOp::JudgeV1InLoop(Vertex  *v1, Loop *lp)
{
	int f = 1;
	HalfEdge *Head = lp->lHalfEdge;
	HalfEdge *tmpH = lp->lHalfEdge;
	while (tmpH->endV != v1)
	{
		tmpH = tmpH->nxtHEdge;
		if (tmpH == Head)
		{
			f = 0;
			break;
		}
	}
	if (f)  return 1;
	return 0;
}
void EulerOp::clearn()
{

	for (int i = 0; i < Face::faceVec.size(); i++)
	{
		if (Face::faceVec[i] == nullptr) continue;
		
		delete(Face::faceVec[i]);
		Face::faceVec[i] = nullptr;
	}
	Face::faceVec.clear();

	for (int i = 0; i < Solid::solidVec.size(); i++)
	{
		delete(Solid::solidVec[i]);
		Solid::solidVec[i] = nullptr;
	}
	Solid::solidVec.clear();
	//globalVar::Euler.solid = nullptr;
	


	for (int i = 0; i < Loop::loopVec.size(); i++)
	{
		delete(Loop::loopVec[i]);
		Loop::loopVec[i] = nullptr;
	}
	Loop::loopVec.clear();
	Loop::loopNum = 0;

	for (int i = 0; i < Edge::edgeVec.size(); i++)
	{
		if (Edge::edgeVec[i] == nullptr) continue;

		delete(Edge::edgeVec[i]);
		Edge::edgeVec[i] = nullptr;
	}
	Edge::edgeVec.clear();

	for (int i = 0; i < HalfEdge::halfEdgeVec.size(); i++)
	{
		delete(HalfEdge::halfEdgeVec[i]);
		HalfEdge::halfEdgeVec[i] = nullptr;
	}
	HalfEdge::halfEdgeVec.clear();

	for (int i = 0; i < Vertex::vertexVec.size(); i++)
	{
		delete(Vertex::vertexVec[i]);
		Vertex::vertexVec[i] = nullptr;
	}
	Vertex::vertexVec.clear();
	Vertex::vertexNum = 0;
}



