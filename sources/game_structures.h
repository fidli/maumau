#ifndef GAME_STRUCTURES_H
#define GAME_STRUCTURES_H

#define TIME_STEP 0.02f

struct Font{
    Image source;
    uint32 gridWidth;
    uint32 gridHeight;
};


enum GameScreenType{
    GameScreenType_Void,
    GameScreenType_GameLobby
};

struct World{
    GameScreenType currentScreen;
    
    
};



World * game;

#endif