#pragma once

#include<iostream>
#include<vector>
#include <Eigen/Dense>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <sstream>
#include <set>
#include <algorithm>

class Solid;
class Face;
class Loop;
class Edge;
class HalfEdge;
class Vertex;
class Point;


class Solid
{
public:
	int solidNum;
	int edgeNum;

	Solid* preSolid;
	Solid* nxtSolid;

	Face* sFace;

	Edge* sEdge;

	static std::vector<Solid*> solidVec;

public:
	Solid() : solidNum(0), edgeNum(0), preSolid(nullptr), nxtSolid(nullptr), sFace(nullptr), sEdge(nullptr) {
		Solid::solidVec.push_back(this);
	};

};

class Face
{
public:
	int faceNum;

	Face* preFace;
	Face* nxtFace;

	Solid* fSolid;
	Loop* fLoop; // 面指向的第一个环，外环

	static std::vector<Face*> faceVec;

public:
	Face() : faceNum(0), preFace(nullptr), nxtFace(nullptr), fSolid(nullptr), fLoop(nullptr) {
		Face::faceVec.push_back(this);
	};


};

class Loop
{
public:
	int loopId;

	Loop* preLoop;
	Loop* nxtLoop;

	Face* lFace;
	HalfEdge* lHalfEdge;

	static std::vector<Loop*> loopVec;
	static int loopNum;

public:
	Loop() : loopId(0), preLoop(nullptr), nxtLoop(nullptr), lFace(nullptr), lHalfEdge(nullptr) {
		loopId = Loop::loopNum++;
		Loop::loopVec.push_back(this);
	};
};

class Edge
{
public:
	int edgeNum;

	Edge* preEdge;
	Edge* nxtEdge;

	HalfEdge* halfEdge1;
	HalfEdge* halfEdge2;

	static std::vector<Edge*> edgeVec;

public:
	Edge() : edgeNum(0), preEdge(nullptr), nxtEdge(nullptr), halfEdge1(nullptr), halfEdge2(nullptr){
		Edge::edgeVec.push_back(this);
	};
};

class HalfEdge
{
public:
	HalfEdge* preHEdge;
	HalfEdge* nxtHEdge;

	HalfEdge* neighbor;

	Vertex* startV;
	Vertex* endV;

	Loop* hLoop;

	Edge* edge;

	static std::vector<HalfEdge*> halfEdgeVec;

public:
	HalfEdge() : preHEdge(nullptr), nxtHEdge(nullptr), startV(nullptr), endV(nullptr), hLoop(nullptr), edge(nullptr) {
		HalfEdge::halfEdgeVec.push_back(this);
	};
};

class Point
{
public:
	double coord[3];
public:
	Point(double x, double y, double z)
	{
		coord[0] = x;
		coord[1] = y;
		coord[2] = z;
	};
	Point() { coord[0] = coord[1] = coord[2] = 0; };

	Point(const Point& point) {
		coord[0] = point.coord[0];
		coord[1] = point.coord[1];
		coord[2] = point.coord[2];
	}
};

class Vertex
{
public:

	int vertexId;
	static int vertexNum;
	static std::vector<Vertex*> vertexVec;

	Vertex* preVertex;
	Vertex* nxtVertex;

	Point point;

public:
	Vertex() : preVertex(nullptr), nxtVertex(nullptr) {
		vertexId = vertexNum++;
		vertexVec.push_back(this);
	};
	Vertex(Point point) : preVertex(nullptr), nxtVertex(nullptr), point(point) {
		vertexId = vertexNum++;
		vertexVec.push_back(this);
	};
};

class EulerOp
{
public:
	Solid* solid;

	Solid* mvfs(Point point);

	HalfEdge* mev(Vertex *v1, Point point, Loop* lp);

	Loop* mef(Vertex *v1, Vertex *v2, Loop* lp);

	Loop* kemr(Vertex *v1, Vertex *v2, Loop *lp);

	void kfmrh(Loop *outLp, Loop *lp);

	void sweep(Loop *lp, Eigen::Vector3d& dir, int dis);

public:
	Loop*  FindLoopExist(Vertex* v1, Vertex *v2);
	Loop*  FindLoopOrder(Vertex* v1, Vertex *v2);
	bool JudgeV1InLoop(Vertex  *v1, Loop  *lp);
	HalfEdge* FindHEdgeEndV(Vertex* v, Loop* lp);
	HalfEdge* FindHEdgeStartV(Vertex* v, Loop* lp);
	void exampleEulerOp();
	void exampleEulerOp2();
	
	void PrintAllFace();
	void PrintAllEdge();

	void GetAllEdge(float** edges);
	void GetAllVertex(float** vertex);

	int CreateAllFaceToView(float** faces);


	bool judgeLoopClose(Loop * lpPt);
	bool judgeFaceView(Face *facePt);

	void getOutInnerLoop(std::vector<std::vector<int>>& vertexId, Loop * & outLp, std::vector<Loop*>& innerLp);

	void clearn();
};

