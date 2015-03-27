/*
-----------------------------------------------------------------------------
Filename:    Viewports.h
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

#ifndef __VIEWPORTS_H_
#define __VIEWPORTS_H_

#include "BaseApplication.h"
#include "Paddle.h"
#include "Ball.h"
#include "Block.h"
#include <unordered_map>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

//---------------------------------------------------------------------------

class Viewports : public BaseApplication,
public Ogre::IntersectionSceneQueryListener
{
public:
	static const Ogre::uint32 SELECTABLE;
	static const Ogre::uint32 INTERSECTABLE;
	static const Ogre::uint32 LEFT_WALL_INTERSECT;
	static const Ogre::uint32 RIGHT_WALL_INTERSECT;
	static const Ogre::uint32 FLOOR_INTERSECT;
	static const Ogre::uint32 ROOF_INTERSECT;
	static const Ogre::uint32 BACK_WALL_INTERSECT;
	static const Ogre::uint32 PADDLE_INTERSECT;
	static const Ogre::uint32 BLOCK_INTERSECT;
	static const Ogre::uint32 NUM_BLOCKS;

	Viewports(void);
	virtual ~Viewports(void);

protected:
	CEGUI::OgreRenderer* mRenderer;
	virtual void createScene			(void);
	virtual void createCamera			(void);
	virtual void createFrameListener	(void);
	virtual void createViewports		(void);
	virtual void endGame				(void);
	virtual void nextLevel				(void);
	virtual void clearMap				(void);
	virtual void setupGUI				(void);
	virtual bool frameRenderingQueued	(const Ogre::FrameEvent& evt);
	virtual bool keyPressed				(const OIS::KeyEvent &arg);
	virtual bool keyReleased			(const OIS::KeyEvent &arg);
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID);
	virtual bool quit(const CEGUI::EventArgs &e);
	virtual bool showHighScores(const CEGUI::EventArgs &e);
	virtual bool closeWindowButton(const CEGUI::EventArgs& e);
	virtual bool mouseEnters(const CEGUI::EventArgs &e);
	virtual bool loadName(const CEGUI::EventArgs &e);

	// IntersectionSceneQuery methods
	virtual bool queryResult(Ogre::MovableObject* first, Ogre::MovableObject* second);
	virtual bool queryResult(Ogre::MovableObject* movable,
			Ogre::SceneQuery::WorldFragment* fragment) {
		return true; // inline method (good for getters/setters)
	}


private:
	Ogre::Real mSpinSpeed;
	Ogre::Vector3 mDirection;
	Ogre::SceneNode *mSelected;
	Ogre::RaySceneQuery *mRaySceneQuery;

	// New stuff
	Ogre::IntersectionSceneQuery *mIntersectionQuery;
	Ogre::Vector3 mOldPosition;
	Paddle *paddle;
	Ball *ball;
	Ogre::Viewport* vp;
	std::unordered_map<std::string, Block*> blockMap;
	std::unordered_map<std::string, Ball*> ballMap;
	bool isPaused;
	int blocksRemaining;
	int gameScore;
	int level;
	bool userNameEntered;
	bool m_overEdit;
	std::string userName;

};

//---------------------------------------------------------------------------

#endif // #ifndef __VIEWPORTS_H_

//---------------------------------------------------------------------------
