//
//  event.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/4/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "event.h"
#include "event_queue.h"
#include "../character/character.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"

// Temp for player movement!
#include "../character/character_store.h"

static void perform_impl(event_t* e);

static event_t* construct_impl(character_t* c, event_action_t action) {
    
    if(c == NULL) {
        logger.w("Event contruct called with NULL character, returning NULL");
        return NULL;
    }
    
    event_t* e = calloc(1, sizeof(event_t));
    e->controller = c;
    e->action = action;
    e->event_count = EVENT_TIME + c->turn_count;
    e->perform = perform_impl;
    return e;
}

static void destruct_impl(event_t* e) {
    free(e);
}

static void perform_impl(event_t* e) {
    // Perform action!
    // Add new event to queue if needed
    character_t* c = e->controller;
    if(c->type == PC) {
        temp_handle_player_move();
    } else {
        temp_handle_npc_turn(c);
    }
}

event_namespace const eventAPI = {
    construct_impl,
    destruct_impl
};
