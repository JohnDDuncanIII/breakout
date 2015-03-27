/*
 * Ball.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: duncjo01
 */

#include "Ball.h"

using namespace Ogre;

const Ogre::uint32 Ball::INTERSECTABLE = 2;

Ball::Ball(Ogre::SceneManager* mSceneMgr, int nLives) {
	this->mSceneMgr = mSceneMgr;
	// TODO Auto-generated constructor stub
	this->mEntity = mSceneMgr->createEntity("sphere.mesh");

	mEntity->setMaterialName("CPresser/Sphere");
	mEntity->setQueryFlags(INTERSECTABLE);
	name = "ballNode";
	this->mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
	mSceneNode->attachObject(mEntity);
	mSceneNode->setPosition(Ogre::Vector3(0.0f,-10.0f,90.0f));
	mSceneNode->scale(.01, .01, .01);

	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_real_distribution<> x(-1.0f, 1.0f);
	std::uniform_real_distribution<> y(0.0f, 1.0f);
	mBallDirection = Vector3(x(gen), y(gen), -1).normalisedCopy();
	//mBallDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;
	mBallSpeed = 60;
	numLives = nLives;
}

Ball::~Ball() {
	// TODO Auto-generated destructor stub
}

Ogre::Vector3 Ball::getPosition() {
	return mSceneNode->getPosition();
}
void Ball::setPosition(Ogre::Vector3 v) {
	mSceneNode->setPosition(v);
}

void Ball::setOldBallPosition(Ogre::Vector3 v) {
	mOldBallPosition = v;
}

Ogre::Vector3 Ball::getBallDir() {
	return mBallDirection;
}
void Ball::setBallDir(Ogre::Vector3 v) {
	mBallDirection = v;
}
Ogre::int32 Ball::getBallSpeed() {
	return mBallSpeed;
}
Ogre::Vector3 Ball::getmOldBallPosition() {
	return mOldBallPosition;
}
int Ball::getNumLives() {
	return numLives;
}
std::string Ball::getName() {
	return name;
}

void Ball::removeLife() {
	numLives--;
}

void Ball::reset() {
	mSceneNode->setPosition(Ogre::Vector3(0.0f,-10.0f,90.0f));
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_real_distribution<> x(-1.0f, 1.0f);
	std::uniform_real_distribution<> y(0.0f, 1.0f);
	mBallDirection = Vector3(x(gen), y(gen), -1).normalisedCopy();
}
