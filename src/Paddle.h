/*
 * Paddle.h
 *
 *  Created on: Feb 23, 2015
 *      Author: duncjo01
 */

#ifndef PADDLE_H_
#define PADDLE_H_

#include "BaseApplication.h"

class Paddle {


public:
	static const Ogre::uint32 SELECTABLE;
	static const Ogre::uint32 INTERSECTABLE;
	static const Ogre::uint32 PADDLE_INTERSECT;

	Paddle(Ogre::SceneManager* mSceneMgr);
	virtual ~Paddle();
	virtual Ogre::Vector3 move(const OIS::KeyEvent& arg);
	virtual void setPosition(Ogre::Vector3 v);
	virtual Ogre::Vector3 getPosition();


private:
	// used to create the entity and the scene node
	Ogre::SceneManager* mSceneMgr;
	// Holds the entity with the given mesh
	Ogre::Entity* mEntity;
	// The node that will be attached
	Ogre::SceneNode* mSceneNode;
};

#endif /* PADDLE_H_ */
