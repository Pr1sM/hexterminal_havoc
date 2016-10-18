//
//  ai.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/16/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef ai_h
#define ai_h

#include "character.h"
#include "../graph/graph.h"
#include "../point/point.h"

extern graph_t* _PLAYER_PATH;

void setup_pc_movement();
void handle_pc_move();

void handle_npc_move(character_t* c);
path_node_t* los_to_pc(point_t* p);

#endif /* ai_h */
