/*
-----------------------------------------------------------------------------
Filename:    Viewports.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/
Tutorial Framework (for Ogre 1.9)
http://www.ogre3d.org/wiki/
-----------------------------------------------------------------------------
 */

#include "Breakout.h"
#include "Paddle.h"
#include "Block.h"
#include <random>
#include <iostream>
#include <stdlib.h>

using namespace Ogre;

// flag values [should be powers of two]
const Ogre::uint32 Viewports::SELECTABLE = 1;
const Ogre::uint32 Viewports::INTERSECTABLE = 2;
const Ogre::uint32 Viewports::LEFT_WALL_INTERSECT = 4;
const Ogre::uint32 Viewports::RIGHT_WALL_INTERSECT = 8;
const Ogre::uint32 Viewports::FLOOR_INTERSECT = 16;
const Ogre::uint32 Viewports::ROOF_INTERSECT = 32;
const Ogre::uint32 Viewports::BACK_WALL_INTERSECT = 64;
const Ogre::uint32 Viewports::PADDLE_INTERSECT = 128;
const Ogre::uint32 Viewports::BLOCK_INTERSECT = 256;
const Ogre::uint32 Viewports::NUM_BLOCKS 		= 15;
Viewports app;

//---------------------------------------------------------------------------
Viewports::Viewports(void) {
	mSpinSpeed = 			0;
	mDirection = 			Vector3::ZERO;
	mSelected = 			0;
	mRaySceneQuery = 		0;
	mIntersectionQuery = 	0;
	isPaused = 				false;
}
//---------------------------------------------------------------------------
Viewports::~Viewports(void) {
	if(mRaySceneQuery) {
		mSceneMgr->destroyQuery(mRaySceneQuery);
	}
	if(mIntersectionQuery) { // ! = null
		mSceneMgr->destroyQuery(mIntersectionQuery);
	}
}

bool Viewports::frameRenderingQueued(const Ogre::FrameEvent& evt) {
	bool result = BaseApplication::frameRenderingQueued(evt);
	if(!isPaused) {


		if(ball->getNumLives() == 0) {
			//endGame();
		}

		// perform ray query
		mRaySceneQuery->setRay(Ray(mCamera->getPosition(),
				mCamera->getDirection()));

		RaySceneQueryResult &qResult = mRaySceneQuery->execute();
		RaySceneQueryResult::iterator it = qResult.begin();

		if(ball->getPosition().z > (paddle->getPosition().z + 5)) {
			ball->reset();
			//ball.removeLife();
		}

		// make sure there is something and it is an entity
		if(it != qResult.end() && it->movable) {
			if(mSelected) {
				mSelected->showBoundingBox(false);
			}

			mSelected = it->movable->getParentSceneNode();
			mSelected->showBoundingBox(true);
		}

		// start am intersection query
		mIntersectionQuery->execute(this);



		//Ogre::Node *ogre = mSceneMgr->getSceneNode("HeadNode");
		//ogre->yaw(Ogre::Degree(mSpinSpeed * evt.timeSinceLastFrame));

		if(mSelected) {
			// something is selected
			// save the previous position
			mOldPosition = mSelected->getPosition();
			mSelected->setPosition(mOldPosition + mDirection * evt.timeSinceLastFrame);

			// mSelected->translate(mDirection * evt.timeSinceLastEvent);
			mSelected->yaw(Ogre::Degree(mSpinSpeed * evt.timeSinceLastFrame));
		}

		ball->setOldBallPosition(ball->getPosition());
		ball->setPosition(ball->getmOldBallPosition() + ball->getBallDir() * ball->getBallSpeed() * evt.timeSinceLastFrame);

		return result;
	}

	return result;
}

bool Viewports::mouseMoved(const OIS::MouseEvent &arg)
{
	//std::cout << "HEIGHT" << vp->getActualHeight() << std::endl;
	//std::cout << "WIDTH" << vp->getActualWidth() << std::endl;
	Ogre::Real xPct = ((Ogre::Real)arg.state.X.abs)/vp->getActualHeight();
	Ogre::Real yPct = ((Ogre::Real)arg.state.Y.abs)/vp->getActualWidth();

	int newX = 32.0f*xPct - 20.0f;
	int newY = 38.0f*(1-yPct) - 28.0f;

	paddle->setPosition(Ogre::Vector3(newX, newY, 100));
	return true;
}

//---------------------------------------------------------------------------
void Viewports::createScene(void) {
	// Create your scene here :)
	// Set the scene's ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

	// Create a Light and set its position
	//Ogre::Light* light = mSceneMgr->createLight("MainLight");
	//light->setPosition(20.0f, 80.0f, 50.0f);

	Ogre::Light* light = mSceneMgr->createLight("MainLight");
	//light->setPosition(-10.0f, -10.0f, -99.0f);
	//light->setPosition(-10.0f, -10.0f, 100.0f);
	SceneNode * lightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("lightNode");
	//lightNode->setPosition(Ogre::Vector3(0,-12,100));

	lightNode->attachObject(light);
	light->setPosition(0.0f, 0.0f, 100.0f);
	lightNode->setPosition(0.0f, 0.0f, 100.0f);
	lightNode->yaw(Ogre::Degree(86));
	lightNode->showBoundingBox(true);

	//light->setPosition(Ogre::Vector3(0,-10,0));
	//light->setDirection(Vector3::NEGATIVE_UNIT_Z);

	ball = new Ball(mSceneMgr, 5);

	Entity *ceiling = mSceneMgr->createEntity("cube.mesh");
	ceiling->setMaterialName("Examples/CloudySky");
	ceiling->setQueryFlags(INTERSECTABLE | ROOF_INTERSECT);
	SceneNode *ceilingNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ceilingNode->attachObject(ceiling);

	ceilingNode->setPosition(0.0f, 15.0f, 0.0f);
	ceilingNode->scale(.5, 2, 0);
	ceilingNode->pitch(Ogre::Degree(90.0f));


	Entity *wfloor = mSceneMgr->createEntity("cube.mesh");
	wfloor->setMaterialName("Examples/WaterStream");
	wfloor->setQueryFlags(INTERSECTABLE | FLOOR_INTERSECT);
	SceneNode *wfloorNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	wfloorNode->attachObject(wfloor);

	wfloorNode->setPosition(0.0f, -34.9f, 0.0f);
	wfloorNode->scale(.5, 2, 0);
	wfloorNode->pitch(Ogre::Degree(90.0f));

	Entity *floor = mSceneMgr->createEntity("cube.mesh");
	floor->setMaterialName("Examples/EnvMappedRustySteel");
	floor->setQueryFlags(0);
	SceneNode *floorNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	floorNode->attachObject(floor);

	floorNode->setPosition(0.0f, -35.0f, 0.0f);
	floorNode->scale(.5, 2, 0);
	floorNode->pitch(Ogre::Degree(90.0f));


	Entity *leftWall = mSceneMgr->createEntity("cube.mesh");
	leftWall->setMaterialName("Examples/GrassFloor");
	leftWall->setQueryFlags(INTERSECTABLE | LEFT_WALL_INTERSECT);
	SceneNode *leftWallNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	leftWallNode->attachObject(leftWall);

	leftWallNode->setPosition(-25.0f, -10.0f, 0.0f);
	leftWallNode->scale(.5, 2, 0);
	leftWallNode->pitch(Ogre::Degree(90.0f));
	leftWallNode->yaw(Ogre::Degree(90.0f));


	Entity *rightWall = mSceneMgr->createEntity("cube.mesh");
	rightWall->setMaterialName("Examples/GrassFloor");
	rightWall->setQueryFlags(INTERSECTABLE | RIGHT_WALL_INTERSECT);
	SceneNode *rightWallNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	rightWallNode->attachObject(rightWall);

	rightWallNode->setPosition(25.0f, -10.0f, 0.0f);
	rightWallNode->scale(.5, 2, 0);
	rightWallNode->pitch(Ogre::Degree(90.0f));
	rightWallNode->yaw(Ogre::Degree(90.0f));


	Entity *backWall = mSceneMgr->createEntity("cube.mesh");
	backWall->setMaterialName("CPresser/SemiClearRed");
	backWall->getMesh()->getSubMesh(0)->setMaterialName("CPresser/PurpleTile");
	backWall->setQueryFlags(INTERSECTABLE | BACK_WALL_INTERSECT);
	SceneNode *backWallNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	backWallNode->attachObject(backWall);

	backWallNode->setPosition(0.0f, -10.0f, -100.0f);
	backWallNode->scale(.5, .5, 0);

	paddle = new Paddle(mSceneMgr);

	//std::unordered_map<std::string, Block*> blockMap;

	for(int i = 0 ; i<NUM_BLOCKS; i++) {
		Block* b = new Block (mSceneMgr);
		blockMap[b->getName()] = b;
		blocksRemaining = i;
	}

	blocksRemaining = NUM_BLOCKS;



	// select the first item;
	// mSelected = headNode;
}


void Viewports::createCamera(void) {
	BaseApplication::createCamera();

	Ogre::Camera *camera2 = mSceneMgr->createCamera("TopCamera");
	camera2->setPosition(0,500,0);
	camera2->pitch(Ogre::Degree(-90));
}

void Viewports::createFrameListener(void) {
	BaseApplication::createFrameListener();

	// init scene query
	mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray(), SELECTABLE);
	mIntersectionQuery = mSceneMgr->createIntersectionQuery(INTERSECTABLE);
}

void Viewports::createViewports(void) {
	// rewrite of BaseApplication's method
	// Create one viewport, entire window

	vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0.5,0.5,0.5));

	vp->setDimensions(0,0,.75, 1.02);
	vp->setOverlaysEnabled(false);

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	Ogre::Camera * camera2 = mSceneMgr->getCamera("TopCamera");
	// 2nd param: z-order, which to draw first
	Ogre::Viewport * vp2 = mWindow->addViewport(camera2, 1);

	// set the viewport's dimensions
	vp2->setDimensions(.75, -.4, .25, 1.8);
	camera2->setAspectRatio(Ogre::Real(vp2->getActualWidth()) /
			Ogre::Real(vp2->getActualHeight()));

	vp2->setOverlaysEnabled(false);
}

bool Viewports::keyPressed(const OIS::KeyEvent& arg) {

	mDirection = paddle->move(arg);

	return BaseApplication::keyPressed(arg);
}

bool Viewports::keyReleased(const OIS::KeyEvent& arg) {

	switch(arg.key) {
	case OIS::KC_COMMA:
	case OIS::KC_PERIOD:
		mSpinSpeed = 0;
		break;

	case OIS::KC_W:
	case OIS::KC_A:
	case OIS::KC_S:
	case OIS::KC_D:
		mDirection = Vector3::ZERO;
		break;
	case OIS::KC_SPACE:
		isPaused = !isPaused;
		break;
	default:
		break;
	}
	return BaseApplication::keyReleased(arg);
}

/*
 * Query Flags
 */
bool Viewports::queryResult(Ogre::MovableObject* first,
		Ogre::MovableObject* second) {
	SceneNode * ballNode = first->getParentSceneNode();
	if( second->getQueryFlags() == (INTERSECTABLE | BLOCK_INTERSECT) &&  first->getQueryFlags() == (INTERSECTABLE) ) {

		SceneNode * blockNode = second->getParentSceneNode();

		// Ensure that the ball doesn't get stuck in a wall
		ballNode->setPosition(ball->getmOldBallPosition());

		Vector3 ballNodePos = ballNode->getPosition();
		Vector3 blockNodePos = blockNode->getPosition();

		if(ballNodePos.z > blockNodePos.z) {
			//blockNodePos = Vector3(blockNodePos.x, blockNodePos.y, blockNodePos.z-8.0f).normalisedCopy();
			blockNodePos = Vector3(blockNodePos.x, blockNodePos.y, blockNodePos.z-30.0f).normalisedCopy();
		}
		else if (ballNodePos.z < blockNodePos.z) {
			//blockNodePos = Vector3(blockNodePos.x, blockNodePos.y, blockNodePos.z+5.0f).normalisedCopy();
			blockNodePos = Vector3(blockNodePos.x, blockNodePos.y, blockNodePos.z+30.0f).normalisedCopy();
		}

		//ball->setBallDir((blockNodePos-ballNodePos).normalisedCopy());
		// position of block, direction of ball
		ball->setBallDir((blockNodePos - ball->getBallDir()).normalisedCopy());
		//ball->setBallDir((ball->getBallDir() - blockNodePos).normalisedCopy());

		/*
		if(ballNodePos.x < blockNodePos.x) {
			ball->setBallDir (Vector3(-ball->getBallDir().x, ball->getBallDir().y, ball->getBallDir().z));
		}
		if(ballNodePos.x > blockNodePos.x) {
			ball->setBallDir (Vector3(-ball->getBallDir().x, ball->getBallDir().y, ball->getBallDir().z));
		}
		if(ballNodePos.y < blockNodePos.y) {
			ball->setBallDir(Vector3(ball->getBallDir().x, -ball->getBallDir().y, ball->getBallDir().z));
		}
		if(ballNodePos.y > blockNodePos.y) {
			ball->setBallDir(Vector3(ball->getBallDir().x, -ball->getBallDir().y, ball->getBallDir().z));
		}
		if(ballNodePos.z > blockNodePos.z) {
			ball->setBallDir (Vector3(ball->getBallDir().x, ball->getBallDir().y, -ball->getBallDir().z));
		}
		if(ballNodePos.z < blockNodePos.z) {
			ball->setBallDir (Vector3(ball->getBallDir().x, ball->getBallDir().y, -ball->getBallDir().z));
		}
		 */

		//Entity* mEntity = static_cast<Entity*>(second);




		/*
		if(ballNodePos.x < blockNodePos.x) {
			std::cout << "X LESS THAN firstX was " << ballNodePos.x << " secondX was " << blockNodePos.x;
			ball->setBallDir (Vector3(-ball->getBallDir().x, ball->getBallDir().y, ball->getBallDir().z));
		}
		if(ballNodePos.x > blockNodePos.x) {
			std::cout << "X GREATER THAN firstX was " << ballNodePos.x << " secondX was " << blockNodePos.x;
			ball->setBallDir (Vector3(-ball->getBallDir().x, ball->getBallDir().y, -ball->getBallDir().z));
		}
		if(ballNodePos.y < blockNodePos.y) {
			ball->setBallDir(Vector3(ball->getBallDir().x, -ball->getBallDir().y, ball->getBallDir().z));
		}
		if(ballNodePos.y > blockNodePos.y) {
			ball->setBallDir(Vector3(ball->getBallDir().x, -ball->getBallDir().y, ball->getBallDir().z));
		}
		if(ballNodePos.z > blockNodePos.z) {
			ball->setBallDir (Vector3(ball->getBallDir().x, ball->getBallDir().y, -ball->getBallDir().z));
		}
		if(ballNodePos.z < blockNodePos.z) {
			ball->setBallDir (Vector3(ball->getBallDir().x, ball->getBallDir().y, -ball->getBallDir().z));
		}
		 */

		/*if(blockMap[second->getParentSceneNode()->getName()]->getMaterialName() == "SemiClearBlue") {
			Ball * nBall = new Ball (mSceneMgr, 2);
		}*/
		second->getParentSceneNode()->showBoundingBox(true);
		if(blockMap[second->getParentSceneNode()->getName()]->removeHP()) {
			blocksRemaining--;
		}

		/*if(blockMap[second->getParentSceneNode()->getName()]->getHitPoints() == 0) {
			blockMap.erase(blockMap[second->getParentSceneNode()->getName()]);
		}*/


		//mSceneMgr->destroySceneNode(second->getParentSceneNode());
	}


	if ((second->getQueryFlags() == (INTERSECTABLE | BACK_WALL_INTERSECT) && first->getQueryFlags() == (INTERSECTABLE)))
	{
		ballNode->setPosition(ball->getmOldBallPosition());
		ball->setBallDir(Vector3(ball->getBallDir().x, ball->getBallDir().y, -ball->getBallDir().z).normalisedCopy());
	}
	else if (second->getQueryFlags() == (SELECTABLE | INTERSECTABLE | PADDLE_INTERSECT) && first->getQueryFlags() == (INTERSECTABLE)) {
		ballNode->setPosition(ball->getmOldBallPosition());
		Vector3 ballPos = second->getParentSceneNode()->getPosition();
		Vector3 paddlePos = first->getParentSceneNode()->getPosition();
		paddlePos = Vector3(paddlePos.x, paddlePos.y, paddlePos.z-10.0f);

		ball->setBallDir((paddlePos-ballPos).normalisedCopy());
	}
	else if ( (second->getQueryFlags() == (INTERSECTABLE | RIGHT_WALL_INTERSECT) && first->getQueryFlags() == (INTERSECTABLE))
			|| (second->getQueryFlags() == (INTERSECTABLE | LEFT_WALL_INTERSECT) && first->getQueryFlags() == (INTERSECTABLE))) {
		ballNode->setPosition(ball->getmOldBallPosition());
		ball->setBallDir (Vector3(-ball->getBallDir().x, ball->getBallDir().y, ball->getBallDir().z).normalisedCopy());
	}
	else if ((second->getQueryFlags() == (INTERSECTABLE | FLOOR_INTERSECT) && first->getQueryFlags() == (INTERSECTABLE))
			||  (second->getQueryFlags() == (INTERSECTABLE | ROOF_INTERSECT) && first->getQueryFlags() == (INTERSECTABLE))) {
		ballNode->setPosition(ball->getmOldBallPosition());
		ball->setBallDir(Vector3(ball->getBallDir().x, -ball->getBallDir().y, ball->getBallDir().z).normalisedCopy());
	}


	if (mSelected && // is there something selected?
			(first->getParentSceneNode() == mSelected || // is the first thing selected?
					second->getParentSceneNode() == mSelected) // is the second thing selected?
	) {
		mDirection = Vector3::ZERO;
		// reset previous position
		mSelected->setPosition(mOldPosition);
	}
	return true;
}

void Viewports::endGame() {

}
//---------------------------------------------------------------------------

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char *argv[])
#endif
{
	// Create application object
	Viewports app;

	try {
		app.go();
	} catch(Ogre::Exception& e)  {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		std::cerr << "An exception has occurred: " <<
				e.getFullDescription().c_str() << std::endl;
#endif
	}

	return 0;
}

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------



