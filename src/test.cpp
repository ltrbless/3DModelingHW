//#define _TEST_
#ifdef _TEST_

#include"BrepStruct.h"
#include"Debug.h"

int main()
{
	DEBUG("This debug info.");

	DEBUG("This debug data %d %d", 1, 2);

	EulerOp Euler;
	Euler.solid = Euler.mvfs(Point(0, 0, 0));
	Euler.mev(Vertex::vertexVec[0], Point(3, 0, 0), Loop::loopVec[0]);
	Euler.mev(Vertex::vertexVec[1], Point(3, 3, 0), Loop::loopVec[0]);
	Euler.mev(Vertex::vertexVec[2], Point(0, 3, 0), Loop::loopVec[0]);
	Euler.mef(Vertex::vertexVec[3], Vertex::vertexVec[0], Loop::loopVec[0]);

	Euler.mev(Vertex::vertexVec[0], Point(1, 1, 0), Loop::loopVec[1]);
	Euler.mev(Vertex::vertexVec[4], Point(2, 1, 0), Loop::loopVec[1]);
	Euler.mev(Vertex::vertexVec[5], Point(2, 2, 0), Loop::loopVec[1]);
	Euler.mev(Vertex::vertexVec[6], Point(1, 2, 0), Loop::loopVec[1]);

	Euler.mef(Vertex::vertexVec[7], Vertex::vertexVec[4], Loop::loopVec[1]);

	Euler.kemr(Vertex::vertexVec[0], Vertex::vertexVec[4], Loop::loopVec[1]);

	//Loop* lp = Euler.FindLoopOrder(Vertex::vertexVec[0], Vertex::vertexVec[3]);
	//INFO("loop id is %d", lp->loopId); // 0

	Euler.mev(Vertex::vertexVec[0], Point(0, 0, -1), Loop::loopVec[0]);
	Euler.mev(Vertex::vertexVec[1], Point(3, 0, -1), Loop::loopVec[0]);
	Euler.mev(Vertex::vertexVec[2], Point(3, 3, -1), Loop::loopVec[0]);
	Euler.mev(Vertex::vertexVec[3], Point(0, 3, -1), Loop::loopVec[0]);

	Euler.mef(Vertex::vertexVec[8], Vertex::vertexVec[9], Loop::loopVec[0]);
	Euler.mef(Vertex::vertexVec[9], Vertex::vertexVec[10], Loop::loopVec[0]);
	Euler.mef(Vertex::vertexVec[10], Vertex::vertexVec[11], Loop::loopVec[0]);
	Euler.mef(Vertex::vertexVec[11], Vertex::vertexVec[8], Loop::loopVec[0]);

	//Loop* lp = Euler.FindLoopOrder(Vertex::vertexVec[7], Vertex::vertexVec[4]);
	//INFO("loop id is %d", lp->loopId); // 2

	Euler.mev(Vertex::vertexVec[4], Point(1, 1, -1), Loop::loopVec[2]);
	Euler.mev(Vertex::vertexVec[5], Point(2, 1, -1), Loop::loopVec[2]);
	Euler.mev(Vertex::vertexVec[6], Point(2, 2, -1), Loop::loopVec[2]);
	Euler.mev(Vertex::vertexVec[7], Point(1, 2, -1), Loop::loopVec[2]);

	Euler.mef(Vertex::vertexVec[12], Vertex::vertexVec[13], Loop::loopVec[2]);
	Euler.mef(Vertex::vertexVec[13], Vertex::vertexVec[14], Loop::loopVec[8]);
	Euler.mef(Vertex::vertexVec[14], Vertex::vertexVec[15], nullptr);
	Euler.mef(Vertex::vertexVec[15], Vertex::vertexVec[12], nullptr);

	//Loop* lp = Euler.FindLoopOrder(Vertex::vertexVec[9], Vertex::vertexVec[8]);
	//INFO("loop id is %d", lp->loopId); // 0
	//Loop *lp = Euler.FindLoopOrder(Vertex::vertexVec[12], Vertex::vertexVec[13]);
	//INFO("loop id is %d", lp->loopId); // 11

	Euler.kfmrh(Loop::loopVec[0], Loop::loopVec[11]);

	//
	//Euler.PrintAllEdge();

	//Euler.PrintAllFace();
	//
}



#endif // _TEST_
