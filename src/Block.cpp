/*
 * Block.cpp
 *
 *  Created on: Mar 15, 2015
 *      Author: duncjo01
 */

#include "Block.h"

using namespace Ogre;

const Ogre::uint32 Block::SELECTABLE = 1;
const Ogre::uint32 Block::INTERSECTABLE = 2;
const Ogre::uint32 Block::BLOCK_INTERSECT = 256;
int Block::numBlocks = 0;

Block::Block(Ogre::SceneManager* mSceneMgr) {
	this->mSceneMgr = mSceneMgr;
	// TODO Auto-generated constructor stub
	this->mEntity = mSceneMgr->createEntity("cube.mesh");
	mEntity->setQueryFlags(INTERSECTABLE | BLOCK_INTERSECT);

	name = "blockNode" + numBlocks++;
	//std::cout << "BLOCK NAME: " +  name;
	this->mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
	mSceneNode->attachObject(mEntity);
	mSceneNode->setPosition(Ogre::Vector3(0.0f,-10.0f,-50.0f));
	mSceneNode->scale(.08, .08, .08);


	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_real_distribution<> x(-23.0f, 23.0f);
	std::uniform_real_distribution<> y(-33.0f, 13.0f);
	std::uniform_real_distribution<> z(-100.0f, 80.0f);
	std::uniform_int_distribution <> materialType(1, 6);

	int materialTypeGen = materialType(gen);
	switch(materialTypeGen) {
	case 1:
		materialName = "SemiClearRed";
		mEntity->setMaterialName("CPresser/" + materialName);
		break;
	case 2:
		materialName = "SemiClearGreen";
		mEntity->setMaterialName("CPresser/" + materialName);
		break;
	case 3:
		materialName = "SemiClearBlue";
		mEntity->setMaterialName("CPresser/" + materialName);
		break;
	case 4:
		materialName = "SemiClearYellow";
		mEntity->setMaterialName("CPresser/" + materialName);
		break;
	case 5:
		materialName = "SemiClearPurple";
		mEntity->setMaterialName("CPresser/" + materialName);
		break;
	case 6:
		materialName = "SemiClearCyan";
		mEntity->setMaterialName("CPresser/" + materialName);
		break;
	}






	hitPoints = 2;

	//mSceneNode->setPosition(Ogre::Vector3(0.0f,-10.0f,90.0f));
	mSceneNode->setPosition(Ogre::Vector3(x(gen), y(gen), z(gen)));


}

Block::~Block() {
	// TODO Auto-generated destructor stub
}

bool Block::removeHP () {
	mSceneNode->scale(1.2, 1.2, 1.2);
	hitPoints--;
	if(hitPoints == 0) {
		mSceneMgr->destroySceneNode(mSceneNode);
		return true;
	}

	return false;

	/*// Set transparency
	Real alphaLevel = 0.5f;
	MaterialPtr alphamat = MaterialManager::getSingleton().create("moreTrans",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Pass* pass = alphamat->getTechnique(0)->getPass(0);
	pass->setSceneBlending(SBT_TRANSPARENT_ALPHA);
	pass->setDepthWriteEnabled(false);

	TextureUnitState* t = pass->createTextureUnitState();
	t->setAlphaOperation(LBX_SOURCE1, LBS_MANUAL, LBS_CURRENT, alphaLevel);
	t->setColourOperationEx(LBX_SOURCE1,LBS_MANUAL,LBS_CURRENT);
	mEntity->setMaterialName("moreTrans");*/
}

std::string Block::getName() {
	return name;
}

Ogre::String Block::getMaterialName() {
	return materialName;
}

int Block::getHitPoints() {
	return hitPoints;
}
