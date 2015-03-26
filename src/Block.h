/*
 * Block.h
 *
 *  Created on: Mar 15, 2015
 *      Author: duncjo01
 */

#ifndef BLOCK_H_
#define BLOCK_H_

#include "BaseApplication.h"

class Block {
public:
	static const Ogre::uint32 SELECTABLE;
	static const Ogre::uint32 INTERSECTABLE;
	static const Ogre::uint32 BLOCK_INTERSECT;

	Block(Ogre::SceneManager* mSceneMgr);
	virtual ~Block ();

	virtual bool removeHP ();
	virtual std::string getName();
	virtual Ogre::String getMaterialName();
	virtual int getHitPoints();

private:
	// used to create the entity and the scene node
	Ogre::SceneManager* mSceneMgr;
	// Holds the entity with the given mesh
	Ogre::Entity* mEntity;
	// The node that will be attached
	Ogre::SceneNode* mSceneNode;
	static int numBlocks;
	int hitPoints;
	std:: string name;
	Ogre::String materialName;

};

#endif /* BLOCK_H_ */
