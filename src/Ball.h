/*
 * Ball.h
 *
 *  Created on: Mar 6, 2015
 *      Author: duncjo01
 */

#ifndef BALL_H_
#define BALL_H_

#include "BaseApplication.h"

class Ball {
public:
	static const Ogre::uint32 INTERSECTABLE;
	Ball(Ogre::SceneManager* sm, int nLives);
	virtual ~Ball();
	virtual Ogre::Vector3 getPosition();
	virtual void reset();
	virtual void setPosition(Ogre::Vector3 v);
	virtual void setOldBallPosition(Ogre::Vector3 v);
	virtual Ogre::Vector3 getBallDir();
	virtual void setBallDir(Ogre::Vector3 v);
	virtual Ogre::int32 getBallSpeed();
	virtual Ogre::Vector3 getmOldBallPosition();
	virtual int getNumLives();
	virtual std::string getName();

private:
// used to create the entity and the scene node
	Ogre::SceneManager* mSceneMgr;
	// Holds the entity with the given mesh
	Ogre::Entity* mEntity;
	// The node that will be attached
	Ogre::SceneNode* mSceneNode;
    Ogre::Vector3 mOldBallPosition;
    Ogre::int32 mBallSpeed;
    Ogre::Vector3 mBallDirection;
    int numLives;
    static int numBalls;
    std::string name;
};
#endif /* BALL_H_ */
