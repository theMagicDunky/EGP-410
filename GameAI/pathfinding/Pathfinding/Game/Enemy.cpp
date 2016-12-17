#include "Enemy.h"
#include "Animation.h"
#include "GameApp.h"
#include "Sprite.h"
#include "GraphicsBufferManager.h"
#include "GraphicsBuffer.h"
#include "GameMessageManager.h"
#include "RespawnMessage.h"
#include "Grid.h"
#include "AStarPathfinder.h"
#include "GridGraph.h"
#include <math.h>

Enemy::Enemy(float speed, float frameTime, Vector2D pos)
:Unit(speed, frameTime)
,mSpawnPos(pos)
{
	GraphicsBufferManager* pBuffMan = gpGameApp->getGraphicsBufferManager();

	mpAnime->addSprite(new Sprite(pBuffMan->getBuffer(71), 0, 0, pBuffMan->getBuffer(71)->getWidth(), pBuffMan->getBuffer(71)->getHeight()));

	mpPathfinder = new AStarPathfinder(gpGameApp->getGridGraph());	

	mPos = mSpawnPos;


	mDir = Direction::NONE;

	int i = 0;

	while (gpGameApp->getGrid()->getValueAtIndex(i) != 219)
		++i;

	setPos(gpGameApp->getGrid()->getULCornerOfSquare(i));

	findAPath();
}

Enemy::~Enemy()
{
	delete mpPathfinder;
}

void Enemy::update(double deltaTime)
{

	//std::cout << mPos.getY() << std::endl;

	//Unit::update(deltaTime);

	//seek
	if (distanceBetween(mPos, gpGameApp->getPlayerPos()) > 250)
	if (gpGameApp->getPlayerStateId() == 0 && distanceBetween(mPos, gpGameApp->getPlayerPos()) < distanceBetween(mPos, gpGameApp->getGridAtIndex(0)->getULCornerOfSquare(mNodesInPath[mNodesInPath.size() - 2])) && mNodeSeekIndex > 3)
	{
		findAPath(Vector2D(rand() % 32, rand() % 24));
	}
	else if (gpGameApp->getPlayerStateId() == 0 && distanceBetween(mPos, gpGameApp->getPlayerPos()) < distanceBetween(mPos, gpGameApp->getGrid()->getULCornerOfSquare(mNodesInPath[mNodesInPath.size() - 1])))
	{
		mNodesInPath.clear();
		findAPath();
	}

	//getem
	int index = mNodesInPath[mNodeSeekIndex];

	if (gpGameApp->getGridAtIndex(0)->getSquareIndexFromPixelXY(std::ceil(mPos.getX() + 2), std::ceil(mPos.getY() + 2)) == index
		&& gpGameApp->getGridAtIndex(0)->getSquareIndexFromPixelXY(std::ceil(mPos.getX() + 30), std::ceil(mPos.getY()) + 30) == index)
	{
		if (mNodeSeekIndex == (mNodesInPath.size()) - 1)
		{
			mNodeSeekIndex = 0;
			findAPath();
		}
		else
		{
			++mNodeSeekIndex;
		}
	}

	seek(index, deltaTime);
}

void Enemy::draw(GraphicsBuffer& dest)
{
	mpPathfinder->drawVisualization(gpGameApp->getGridAtIndex(0), &dest, mDrawDebugLine);
	Unit::draw(dest);
}

void Enemy::newPathfinder()
{
	delete mpPathfinder;
	mpPathfinder = new AStarPathfinder(gpGameApp->getGridGraphAtIndex(0));
	mNodesInPath.clear();
	//findAPath();
}

void Enemy::seek(int index, double time)
{
	Vector2D dest = gpGameApp->getGridAtIndex(0)->getULCornerOfSquare(index);
	Vector2D pos = dest - mPos;

	pos.normalize();

	//moveAndCheckCollision(mPos + (pos * time * mSpeed));

	int buffer = 10;

	if ((pos.getX() <= dest.getX() - buffer || pos.getX() >= dest.getX() + buffer)
		&& (pos.getY() <= dest.getY() - buffer || pos.getY() >= dest.getY() + buffer))
	{
		pos = dest;
	}

	Vector2D playerPos = gpGameApp->getPlayerPos();

	//colliding with the player
	if ((abs(mPos.getX() - playerPos.getX()) * 2 < (32 + 32)) &&
		(abs(mPos.getY() - playerPos.getY()) * 2 < (32 + 32)))
	{
		
		//player is candy man
		if (gpGameApp->getPlayerStateId() == 1)
		{
			respawn();
			return;
		}

		if (!gpGameApp->isPlayerInvincible())
		{
			GameMessage* pMessage = new RespawnMessage();
			gpGameApp->getMessageManager()->addMessage(pMessage, 0);
		}
	}
}

void Enemy::flee(double time)
{
	/*Vector2D playerPos = gpGameApp->getPlayerPos();
	Vector2D ULCorner = Vector2D(32, 32);
	Vector2D URCorner = Vector2D(32, 750);
	Vector2D BLCorner = Vector2D(1000, 32);
	Vector2D BRCorner = Vector2D(1000, 750);

	float distToUL = distanceBetween(ULCorner, playerPos);
	float distToBL = distanceBetween(BLCorner, playerPos);
	float distToUR = distanceBetween(URCorner, playerPos);
	float distToBR = distanceBetween(BRCorner, playerPos);

	float largest = distToUL;
	Vector2D largestPos = ULCorner;

	if (distToBL > largest)
	{
		largest = distToBL;
		largestPos = BLCorner;
	}
	if (distToUR > largest)
	{
		largest = distToUR;
		largestPos = URCorner;
	}
	if (distToBR > largest)
	{
		largest = distToBR;
		largestPos = BRCorner;
	}

	mNodesInPath.clear();

	findAPath(largestPos);
	//Vector2D pos = mPos - gpGameApp->getPlayerPos();
	//pos.normalize();
	//moveAndCheckCollision(mPos + (pos * time * mSpeed));*/
}

void Enemy::findAPath(Vector2D pos)
{
	GridGraph* pGridGraph = gpGameApp->getGridGraphAtIndex(0);
	Grid* pGrid = gpGameApp->getGridAtIndex(0);
	int fromIndex = pGrid->getSquareIndexFromPixelXY(mPos.getX() + 16, mPos.getY() + 16);
	int toIndex = pGrid->getSquareIndexFromPixelXY(pos.getX(), pos.getY());

	int i;

	if (mNodesInPath.size() == 0)
	{
		i = fromIndex;
	}
	else
	{
		i = mNodesInPath[mNodesInPath.size() - 1];
	}

	Node* pFromNode = pGridGraph->getNode(fromIndex);
	Node* pToNode = pGridGraph->getNode(toIndex);
	if (pFromNode != NULL && pToNode != NULL)
	{
		mpPathfinder->findPath(pToNode, pFromNode);
		mNodesInPath = mpPathfinder->getNodesInPath();
	}

	//if(mPos.getX() < gpGameApp->getPlayerPos().getX() && mPos.getY() < gpGameApp->getPlayerPos().getY())
		//mNodesInPath.erase(mNodesInPath.begin(), mNodesInPath.begin() + 2);

	mNodeSeekIndex = 0;
}

void Enemy::findAPath()
{

	Vector2D pos = Vector2D(gpGameApp->getPlayerPos().getX() + 16, gpGameApp->getPlayerPos().getY() + 16);

	if (gpGameApp->getGridAtIndex(0)->getSquareIndexFromPixelXY(mPos.getX() + 16, mPos.getY() + 16) != gpGameApp->getGridAtIndex(0)->getSquareIndexFromPixelXY(gpGameApp->getPlayerPos().getX() + 16, gpGameApp->getPlayerPos().getY() + 16))
		findAPath(pos);

}

float Enemy::distanceBetween(Vector2D one, Vector2D two)
{
	return std::abs(two.getX() - one.getX()) + std::abs(two.getY() - one.getY());
}