//https://en.wikipedia.org/wiki/Mau_mau_(card_game) Prší


enum PlayerActionType{
    PlayerActionType_Invalid,
    PlayerActionType_Draw,
    PlayerActionType_PlayCard,
    PlayerActionType_TakeEffect,
    PlayerActionType_Count
};

struct PlayerAction{
    struct {
        PlayerActionType type;
        uint32 playerIndex;
    };
    union{
        struct{
            
        } draw;
        struct{
            uint32 cardIndex;
        } card;
    };
};


enum CardSuite{
    CardSuite_Invalid,
    CardSuite_Pellet,
    CardSuite_Acorn,
    CardSuite_Leaf,
    CardSuite_Heart,
    CardSuiteCount
};

enum CardFace{
    CardFace_Invalid,
    CardFace_Seven,
    CardFace_Eight,
    CardFace_Nine,
    CardFace_Ten,
    CardFace_Jack,
    CardFace_Queen,
    CardFace_King,
    CardFace_Ace,
    CardFaceCount
};

struct Card{
    CardSuite suite;
    CardFace face;
    char id[3];
};


struct Player{
    Card hand[32];
    uint32 cardCount;
};

enum TableEffect
{
    TableEffect_Invalid,
    TableEffect_None,
    TableEffect_Stop,
    TableEffect_Take2,
    TableEffectCount
};

struct GameTable{
    Card drawStack[32];
    uint32 drawStackCount;
    
    Card playStack[32];
    uint32 playStackCount;
    
    CardSuite currentSuite;
    
    TableEffect currentEffect;
    uint32 take2chain;
    
    
    Player players[2];
    uint32 playerCount;
    uint32 currentPlayer;
};

#include "servercode.cpp"

extern GameTable * gameTable;


inline void render()
{
    char s;
    switch(gameTable->currentSuite)
    {
        case CardSuite_Heart:
        s= 'H';
        break;
        case CardSuite_Leaf:
        s = 'L';
        break;
        case CardSuite_Acorn:
        s = 'A';
        break;
        case CardSuite_Pellet:
        s = 'P';
        break;
    }
    
    char e = 0;
    switch(gameTable->currentEffect)
    {
        case TableEffect_Take2:
        e = '7';
        break;
        case TableEffect_Stop:
        e = 'A';
        break;
        case TableEffect_None:
        e = '0';
        break;
        default:
        INV;
    }
    
    printf("Current player: %d\nCurrent playcard: %s\nCurrent play suite: %c\nCurrent effect: %c\nCurrent '7' chain: %d\nYour hand: ", gameTable->currentPlayer, gameTable->playStack[gameTable->playStackCount-1].id, s, e, gameTable->take2chain);
    
    Player * currentPlayer = &gameTable->players[gameTable->currentPlayer];
    for(uint32 c = 0; c < currentPlayer->cardCount; c++)
    {
        printf("%s ", currentPlayer->hand[c].id);
    }
    
    printf("\nYour play (card or 'd' to draw or 'e' to accept current effect): ");
    
    
    
}

inline void run()
{
    bool validPlay = false;
    while(!validPlay)
    {
        render();
        char choice[3];
        scanf("%3%3s", choice);
        
        Player * currentPlayer = &gameTable->players[gameTable->currentPlayer];
        Card * played = NULL;
        uint32 cardIndex = 0;
        for(; cardIndex < currentPlayer->cardCount; cardIndex++)
        {
            if(!strcmp(currentPlayer->hand[cardIndex].id, choice))
            {
                played = &currentPlayer->hand[cardIndex];
                break;
            }
        }
        PlayerAction action;
        if(played == NULL)
        {
            if(choice[0] == 'd')
            {
                action.type = PlayerActionType_Draw;
            }
            else if(choice[0] == 'e')
            {
                action.type = PlayerActionType_TakeEffect;
            }
            else
            {
                printf("Invalid action\n");
                continue;
            }
        }else{
            action.type = PlayerActionType_PlayCard;
            action.card.cardIndex = cardIndex;
        }
        action.playerIndex = gameTable->currentPlayer;
        
        
        validPlay = handlePlayerAction(&action);
        
        
        
    }
    
}