#include "game_structures.h"



void update(float32 dt){
    game->currentScreen = GameScreenType_GameLobby;
}



AudioItem * getAudioQueue(){
    AudioItem * queue = NULL;
    
    return queue;
}

RenderItem * getRenderQueue(){
    RenderItem * first = &PUSH(RenderItem);
    RenderItem * current = first;
    first->type = RenderType_Clear;
    
    switch(game->currentScreen){
        case GameScreenType_Void:{
            
        }break;
        case GameScreenType_GameLobby:{
            current = current->next = &PUSH(RenderItem);
            current->type = RenderType_Image;
            current->image.offset.x = current->image.offset.y = 0;
            current->image.size.h = current->image.size.w = 1;
        }break;
    }
    
    current->next = NULL;
    return first;
}

