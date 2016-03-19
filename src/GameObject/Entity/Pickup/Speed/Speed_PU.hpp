//
//  Speed_PU.hpp
//  dungeon_crawler
//
//  Created by Matthew Clark on 19/03/2016.
//
//

#ifndef Speed_PU_hpp
#define Speed_PU_hpp

#include <stdio.h>
#include "../Pickup.hpp"

class Pickup_Speed: public Pickup{
    
public:
    Pickup_Speed(ofVec2f _pos, float _val);
    
    void collisionResponse(GameObject* other);
};
#endif /* Speed_PU_hpp */
