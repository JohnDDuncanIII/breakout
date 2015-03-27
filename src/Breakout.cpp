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
#include "HighScores.h"
#include <random>
#include <iostream>
#include <stdlib.h>
#include <thread>

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
const Ogre::uint32 Viewports::NUM_BLOCKS 		= 13;
Viewports app;

//---------------------------------------------------------------------------
Viewports::Viewports(void) {
	mSpinSpeed = 			0;
	mDirection = 			Vector3::ZERO;
	mSelected = 			0;
	mRaySceneQuery = 		0;
	mIntersectionQuery = 	0;
	isPaused = 				false;
	gameScore = 			0;
	level = 1;
	userNameEntered = 		false;
	m_overEdit = 			true;
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
			endGame();
			cout << "GAME OVER GAME OVER GAME OVER";
		}

		// perform ray query
		mRaySceneQuery->setRay(Ray(mCamera->getPosition(),
				mCamera->getDirection()));

		RaySceneQueryResult &qResult = mRaySceneQuery->execute();
		RaySceneQueryResult::iterator it = qResult.begin();

		if(ball->getPosition().z > (paddle->getPosition().z + 5)) {
			ball->reset();
			ball->removeLife();

			CEGUI::Window *guiRoot = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
			guiRoot->getChild("Balls")->setText("Lives Remaining: " + std::to_string(ball->getNumLives()));
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
	if(!isPaused) {
		CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().hide();
		//std::cout << "HEIGHT" << vp->getActualHeight() << std::endl;
		//std::cout << "WIDTH" << vp->getActualWidth() << std::endl;
		Ogre::Real xPct = ((Ogre::Real)arg.state.X.abs)/vp->getActualHeight();
		Ogre::Real yPct = ((Ogre::Real)arg.state.Y.abs)/vp->getActualWidth();

		int newX = 32.0f*xPct - 20.0f;
		int newY = 38.0f*(1-yPct) - 28.0f;

		paddle->setPosition(Ogre::Vector3(newX, newY, 100));
	} else {
		CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().show();
		CEGUI::System &sys = CEGUI::System::getSingleton();
		sys.getDefaultGUIContext().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
	}
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

	for(unsigned int i = 0 ; i<NUM_BLOCKS; i++) {
		Block* b = new Block (mSceneMgr);
		blockMap[b->getName()] = b;
	}

	blocksRemaining = NUM_BLOCKS;

	setupGUI();

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
	if(m_overEdit) {
		CEGUI::System &sys = CEGUI::System::getSingleton();
		sys.getDefaultGUIContext().injectKeyDown((CEGUI::Key::Scan)arg.key);
		sys.getDefaultGUIContext().injectChar((CEGUI::Key::Scan)arg.text);
	} else {
		mDirection = paddle->move(arg);
	}
	if (arg.key == OIS::KC_ESCAPE) {
		mShutDown = true;
	}

	//return BaseApplication::keyPressed(arg);
	return true;
}

bool Viewports::keyReleased(const OIS::KeyEvent& arg) {
	if(m_overEdit){
		CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)arg.key);
	}
	else {
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
			if(userNameEntered)
				isPaused = !isPaused;
			break;
		default:
			break;
		}
	}
	return BaseApplication::keyReleased(arg);
}

/*
 * Query Flags
 */
bool Viewports::queryResult(Ogre::MovableObject* first,
		Ogre::MovableObject* second) {
	if(blocksRemaining == 0) {
		nextLevel();
	}
	else {

		SceneNode * ballNode = first->getParentSceneNode();
		if( second->getQueryFlags() == (INTERSECTABLE | BLOCK_INTERSECT) &&  first->getQueryFlags() == (INTERSECTABLE) ) {

			SceneNode * blockNode = second->getParentSceneNode();

			// Ensure that the ball doesn't get stuck in a wall
			ballNode->setPosition(ball->getmOldBallPosition());

			Vector3 ballNodePos = ballNode->getPosition();
			Vector3 blockNodePos = blockNode->getPosition();

			if(ballNodePos.z > blockNodePos.z) {
				//blockNodePos = Vector3(blockNodePos.x, blockNodePos.y, blockNodePos.z-8.0f).normalisedCopy();
				blockNodePos = Vector3(blockNodePos.x, blockNodePos.y, blockNodePos.z-5.0f).normalisedCopy();
			}
			else if (ballNodePos.z < blockNodePos.z) {
				//blockNodePos = Vector3(blockNodePos.x, blockNodePos.y, blockNodePos.z+5.0f).normalisedCopy();
				blockNodePos = Vector3(blockNodePos.x, blockNodePos.y, blockNodePos.z+5.0f).normalisedCopy();
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
			CEGUI::Window *guiRoot = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
			second->getParentSceneNode()->showBoundingBox(true);
			if(blockMap[second->getParentSceneNode()->getName()]->removeHP()) {
				blocksRemaining--;
				gameScore+=100;
				guiRoot->getChild("Blocks")->setText("Blocks: " + std::to_string(blocksRemaining));
			}
			gameScore+=25;
			guiRoot->getChild("Score")->setText("Score: " + std::to_string(gameScore));
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

	}
	return true;
}

void Viewports::endGame() {
	HighScores highscore ("HighScores", 10, true);
	string fileName = "HighScores.dat";
	//highscore.print();
	highscore.readFromFile(fileName);
	highscore.addScore(userName, gameScore);
	highscore.writeToFile(fileName);
	gameScore = 0;
	std::thread t(&Viewports::clearMap, this);
	t.join();
	delete ball;
	delete paddle;
	cout << "Do you want to play again?";
	//std::cin.ignore();
}

void Viewports::nextLevel() {
	gameScore += 777;
	level++;
	isPaused = false;
	//blockMap.clear();

	blocksRemaining = NUM_BLOCKS;

	CEGUI::Window *guiRoot = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
	guiRoot->getChild("Level")->setText("Level: " + std::to_string(level));

	std::thread t(&Viewports::clearMap, this);
	t.join();
	/*mSceneMgr->destroySceneNode(mSceneMgr->getSceneNode(ball->getName()));
	delete ball;
	ball = new Ball(mSceneMgr, 5);
	ball->setOldBallPosition(Ogre::Vector3(0.0f,-10.0f,90.0f));
	ball->reset();*/


	for(int i = 0 ; i<NUM_BLOCKS; i++) {
		Block* b = new Block (mSceneMgr);
		blockMap[b->getName()] = b;
	}


	//ball->setOldBallPosition(Ogre::Vector3::NEGATIVE_UNIT_Z);
	//ball->reset();
}

void Viewports::clearMap() {
	blockMap.clear();
}

void Viewports::setupGUI(){

	mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();

	isPaused = true;
	CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

	// Loading scheme
	//CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
	CEGUI::SchemeManager::getSingleton().createFromFile("GlossySerpent.scheme");

	// Set the mouse cursor
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("GlossySerpentCursors/MouseArrow");
	CEGUI::Size<float> s = CEGUI::Size<float>(32,32);
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setExplicitRenderSize(s);

	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();

	/*
	ParticleSystem *ps = mSceneMgr->createParticleSystem("PS", "Breakout/Water");
	ps->setQueryFlags(SELECTABLE);
	//ParticleSystem *ps = mSceneMgr->createParticleSystem("PS", "Examples/Smoke");
	//SceneNode *psNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("PSNode");

	for(int i=0; i< ps->getNumEmitters(); i++) {
		ParticleEmitter *em = ps->getEmitter(i);
		em->setRepeatDelay(2);
	}
	 */


	// Load our file
	CEGUI::Window *guiRoot = wmgr.loadLayoutFromFile("ModelViewer.layout");

	// Set up the main window
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(guiRoot);

	//CEGUI::FontManager::getSingleton().createFreeTypeFont("DejaVuSans-10.font");
	CEGUI::FontManager::getSingleton().createFreeTypeFont( "DejaVuSans-10", 10, true, "DejaVuSans.ttf", "Fonts" );
	//CEGUI::System::getSingleton().setDefaultFont( "DejaVuSans-10" );
	//FontManager::getSingleton().getByName("DejaVuSans-10.font");
	//guiRoot->getChild("Lives")->setFont("DejaVuSans.ttf");
	guiRoot->getChild("Balls")->setText("Balls Remaining: " + std::to_string(ball->getNumLives()));
	guiRoot->getChild("Score")->setText("Score: " + std::to_string(gameScore));
	guiRoot->getChild("Level")->setText("Level: " + std::to_string(level));
	guiRoot->getChild("Blocks")->setText("Blocks: " + std::to_string(blocksRemaining));

	guiRoot->getChild("QuitButton")->subscribeEvent(
			CEGUI::PushButton::EventClicked, // which event to call
			CEGUI::Event::Subscriber(&Viewports::quit, // method to call
					this));

	guiRoot->getChild("HighScoresWindow")->setPosition(CEGUI::UVector2(CEGUI::UDim( 0.25f, 0 ), CEGUI::UDim( 0.25f, 0 )));
	guiRoot->getChild("HighScoresWindow")->hide();

	guiRoot->getChild("HighScoresWindow")->subscribeEvent(
			CEGUI::FrameWindow::EventCloseClicked,
			CEGUI::Event::Subscriber(&Viewports::closeWindowButton, this));

	guiRoot->getChild("HighScoresButton")->subscribeEvent(
			CEGUI::PushButton::EventClicked, // which event to call
			CEGUI::Event::Subscriber(&Viewports::showHighScores, // method to call
					this));

	//m_overEdit = false;

	CEGUI::Editbox *nameBox = static_cast<CEGUI::Editbox*> (guiRoot->getChild(
			std::string("UserNameWindow/NameInputField")));
	nameBox->setText("Enter your Name");

	guiRoot->getChild("UserNameWindow/NameInputField")->subscribeEvent(CEGUI::Editbox::EventMouseEntersArea,
			CEGUI::Event::Subscriber(&Viewports::mouseEnters, this));
	/*guiRoot->getChild("UserNameWindow/NameInputField")->subscribeEvent(CEGUI::Editbox::EventMouseLeavesArea,
			CEGUI::Event::Subscriber(&Viewports::mouseLeaves, this));*/

	guiRoot->getChild("UserNameWindow/SubmitNameButton")->subscribeEvent(
			CEGUI::PushButton::EventClicked, // which event to call
			CEGUI::Event::Subscriber(&Viewports::loadName, // method to call
					this));



	/*
	CEGUI::Listbox* listbox =  static_cast<CEGUI::Listbox*>(guiRoot->getChild("ListBox"));
	listbox->setMultiselectEnabled(false);
	CEGUI::ListboxTextItem* itemListbox = new CEGUI::ListboxTextItem("Value A", 1);
	//itemListbox->setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
	listbox->addItem(itemListbox);
	itemListbox = new CEGUI::ListboxTextItem("Value B", 2);
	//itemListbox->setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
	listbox->addItem(itemListbox);
	itemListbox = new CEGUI::ListboxTextItem("Value C", 3);
	//itemListbox->setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
	listbox->addItem(itemListbox);
	itemListbox = new CEGUI::ListboxTextItem("Value D", 4);
	//itemListbox->setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
	listbox->addItem(itemListbox);
	listbox->setItemSelectState(itemListbox, true);
	listbox->ensureItemIsVisible(itemListbox);
	uint valueListbox = listbox->getFirstSelectedItem()->getID(); // Retrieve the ID of the selected listbox item
	 */


	// Event handling
	// NOTE: PushButton is the CEGUI object
	// 		 Button is the type defined in the XML file
	/*guiRoot->getChild("QuitButton")->subscribeEvent(
    		CEGUI::PushButton::EventClicked, // which event to call
    		CEGUI::Event::Subscriber(&ModelViewer::quit, // method to call
    				this));

    guiRoot->getChild("OpenButton")->subscribeEvent(CEGUI::PushButton::EventClicked,
    		CEGUI::Event::Subscriber(&ModelViewer::openMesh, this));
    guiRoot->getChild("SpinButton")->subscribeEvent(CEGUI::PushButton::EventClicked,
       		CEGUI::Event::Subscriber(&ModelViewer::spinMesh, this));
    guiRoot->getChild("ActionBox")->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,
			CEGUI::Event::Subscriber(&ModelViewer::selectItem, this));

    guiRoot->getChild("MeshBox")->subscribeEvent(CEGUI::Editbox::EventMouseEntersArea,
    		CEGUI::Event::Subscriber(&ModelViewer::mouseEnters, this));
    guiRoot->getChild("MeshBox")->subscribeEvent(CEGUI::Editbox::EventMouseLeavesArea,
    		CEGUI::Event::Subscriber(&ModelViewer::mouseLeaves, this));
    guiRoot->getChild("MeshBox")->subscribeEvent(CEGUI::Editbox::EventTextAccepted,
    		CEGUI::Event::Subscriber(&ModelViewer::openMesh, this));

    guiRoot->getChild("Loop")->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged,
    		CEGUI::Event::Subscriber(&ModelViewer::stateChange, this));
	 */

}

bool Viewports::quit(const CEGUI::EventArgs &e) {
	mShutDown = true;
	return true;
}

bool Viewports::showHighScores(const CEGUI::EventArgs &e) {
	CEGUI::Window *guiRoot = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
	guiRoot->getChild("HighScoresWindow")->show();

	int count = 0;
	CEGUI::Editbox *scoreBox;
	CEGUI::Editbox *nameBox;

	HighScores highscore ("HighScores", 10, false);
	string fileName = "HighScores.dat";
	//highscore.print();
	highscore.readFromFile(fileName);
	std::vector<pair<string, int>> vec = highscore.getVector();

	for (const auto &pos : vec){
		if(pos.first != "" && (pos.second != std::numeric_limits<int>::min()
				|| pos.second != std::numeric_limits<int>::max())) {
			count++;
			scoreBox = static_cast<CEGUI::Editbox*> (guiRoot->getChild(
					std::string("HighScoresWindow/") + "Score" + std::to_string(count) + "Label"));
			nameBox = static_cast<CEGUI::Editbox*> (guiRoot->getChild(
					std::string("HighScoresWindow/") + "Name" + std::to_string(count) + "Label"));
			scoreBox->setText(pos.first);
			nameBox->setText(std::to_string(pos.second));
		}
	}
	return true;
}

bool Viewports::closeWindowButton(const CEGUI::EventArgs& e) {
	static_cast<const CEGUI::WindowEventArgs&>(e).window->hide();
	return true;
}

bool Viewports::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertButton(id));
	if(m_overEdit) {
		CEGUI::Window *guiRoot = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();

		CEGUI::Editbox *nameBox = static_cast<CEGUI::Editbox*> (guiRoot->getChild(
				std::string("UserNameWindow/NameInputField")));
		nameBox->setSelection(0, nameBox->getMaxTextLength());
	}

	return true;
}
//-------------------------------------------------------------------------------------
bool Viewports::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertButton(id));
	return true;
}
CEGUI::MouseButton Viewports::convertButton(OIS::MouseButtonID buttonID) {
	switch (buttonID) {
	case OIS::MB_Left:
		return CEGUI::LeftButton;

	case OIS::MB_Right:
		return CEGUI::RightButton;

	case OIS::MB_Middle:
		return CEGUI::MiddleButton;

	default:
		return CEGUI::LeftButton;
	}
}

bool Viewports::mouseEnters(const CEGUI::EventArgs &e) {
	m_overEdit = true;
	return true;
}

bool Viewports::loadName(const CEGUI::EventArgs &e){
	m_overEdit = false;
	isPaused = false;
	userNameEntered = true;
	CEGUI::Window *guiRoot = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
	CEGUI::FrameWindow *nameWindow = static_cast<CEGUI::FrameWindow*> (guiRoot->getChild(
			std::string("UserNameWindow")));
	nameWindow->hide();

	CEGUI::Editbox *nameField = static_cast<CEGUI::Editbox*> (guiRoot->getChild(
			std::string("UserNameWindow/NameInputField")));

	userName = nameField->getText().c_str();

	return true;
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



