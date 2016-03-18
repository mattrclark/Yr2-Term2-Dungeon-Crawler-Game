//
//  Guy.cpp
//  dungeon_crawler
//
//  Created by Matthew Clark on 08/03/2016.
//
//

#include "Guy.hpp"

Enemy_Guy::Enemy_Guy(ofVec2f _pos, float _maxVel):Enemy(_pos,_maxVel,10){
    gameSprite = Sprite("player", 16, 0, 7, 0, 7, 8, 15);
    movementPattern = {{2,100},{3,100},{0,100},{1,100}};
    addHealth(10);
}