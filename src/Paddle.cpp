/*
 * Paddle.cpp
 *
 *  Created on: Feb 23, 2015
 *      Author: duncjo01
 */

#include "Paddle.h"
using namespace Ogre;

const Ogre::uint32 Paddle::SELECTABLE = 1;
const Ogre::uint32 Paddle::INTERSECTABLE = 2;
const Ogre::uint32 Paddle::PADDLE_INTERSECT = 128;

Paddle::Paddle(Ogre::SceneManager* mSceneMgr) {
	this->mSceneMgr = mSceneMgr;
	// TODO Auto-generated constructor stub
	this->mEntity = mSceneMgr->createEntity("cube.mesh");

	// Set transparency
	Real alphaLevel = 0.5f;
	MaterialPtr alphamat = MaterialManager::getSingleton().create("testy",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Pass* pass = alphamat->getTechnique(0)->getPass(0);
	pass->setSceneBlending(SBT_TRANSPARENT_ALPHA);
	pass->setDepthWriteEnabled(false);
	TextureUnitState* t = pass->createTextureUnitState("small_steps.png");
	t->setAlphaOperation(LBX_SOURCE1, LBS_MANUAL, LBS_CURRENT, alphaLevel);

	mEntity->setMaterialName("testy");
	mEntity->setQueryFlags(SELECTABLE | INTERSECTABLE | PADDLE_INTERSECT);
	//mEntity->setMaterialName("Materials/CelShading");


	this->mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mSceneNode->attachObject(mEntity);
	mSceneNode->setPosition(Ogre::Vector3(0,-12,100));
	mSceneNode->scale(.09, .09, 0);

}

Paddle::~Paddle() {

}

void Paddle::setPosition(Ogre::Vector3 v) {
	mSceneNode->setPosition(v);
}

Ogre::Vector3 Paddle::move(const OIS::KeyEvent& arg) {
	int speed = 75;
	Vector3 mDirection;

	switch(arg.key) {
	/*case OIS::KC_COMMA:
			mSpinSpeed = 360;
			break;
		case OIS::KC_PERIOD:
			mSpinSpeed = -360;
			break;
		case OIS::KC_I:
			mDirection = speed * Vector3::NEGATIVE_UNIT_Z;
			break;
		case OIS::KC_K:
			mDirection = speed * Vector3::UNIT_Z;
			break;*/
	case OIS::KC_A:
		mDirection = speed * Vector3::NEGATIVE_UNIT_X;
		break;
	case OIS::KC_D:
		mDirection = speed * Vector3::UNIT_X;
		break;
	case OIS::KC_W:
		mDirection = speed * Vector3::UNIT_Y;
		break;
	case OIS::KC_S:
		mDirection = speed * Vector3::NEGATIVE_UNIT_Y;
		break;
	default:
		break;
	}
	return mDirection;
}

Ogre::Vector3 Paddle::getPosition() {
	return mSceneNode->getPosition();
}

