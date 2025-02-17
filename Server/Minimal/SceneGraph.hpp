#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

#include "ObjectData.hpp"

struct SceneGraph {

	PlayerData player1;
	PlayerData player2;
	ObjectData cuttingBoard;
	ObjectData knife;
	ObjectData singleEgg;
	ObjectData standMixer;
	ObjectData barrel;
	ObjectData sugarBowl;
	ObjectData eggCrate;
	ObjectData flourSack;
	ObjectData chocolateSource;
	ObjectData strawberrySource;
	ObjectData crackedEgg;
	ObjectData sugarCube;
	ObjectData flour;
	ObjectData water;
	ObjectData chocolate;
	ObjectData strawberry;
	ObjectData choppedChocolate;
	ObjectData choppedStrawberry;
	ObjectData cookieDough;
	ObjectData cakeDough;
	ObjectData cookie;
	ObjectData cake;
	int currentMenuItem;
	int currentStep;


	MSGPACK_DEFINE_MAP(player1, player2, cuttingBoard, knife, singleEgg, standMixer, barrel, sugarBowl, eggCrate,
		flourSack, chocolateSource, strawberrySource, crackedEgg, sugarCube, flour, water, chocolate, strawberry,choppedChocolate,choppedStrawberry, cookieDough,cakeDough, cookie, cake,
		currentMenuItem, currentStep);

};

#endif //SCENEGRAPH_H
