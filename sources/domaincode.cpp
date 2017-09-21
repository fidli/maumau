//https://en.wikipedia.org/wiki/Mau_mau_(card_game) Prší

#include "util_sort.cpp"

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

extern GameTable * gameTable;

inline void initGame(){
    gameTable->drawStackCount = 0;
    gameTable->playerCount = 2;
    gameTable->currentPlayer = 0;
    gameTable->playStackCount = 1;
    
    for(int32 s = ((int32)CardSuite_Invalid)+1; s < CardSuiteCount; s++)
    {
        char stamp;
        switch(s)
        {
            case CardSuite_Heart:
            stamp = 'H';
            break;
            case CardSuite_Leaf:
            stamp = 'L';
            break;
            case CardSuite_Acorn:
            stamp = 'A';
            break;
            case CardSuite_Pellet:
            stamp = 'P';
            break;
        }
        for(int32 f = ((int32)CardFace_Invalid)+1; f < CardFaceCount; f++)
        {
            Card * target = &gameTable->drawStack[gameTable->drawStackCount];
            target->suite = (CardSuite)s;
            target->face = (CardFace)f;
            target->id[0] = stamp;
            switch(f)
            {
                case CardFace_Ace:
                target->id[1] = 'A';
                break;
                case CardFace_King:
                target->id[1] = 'K';
                break;
                case CardFace_Queen:
                target->id[1] = 'Q';
                break;
                case CardFace_Jack:
                target->id[1] = 'J';
                break;
                case CardFace_Ten:
                target->id[1] = '1';
                break;
                case CardFace_Nine:
                target->id[1] = '9';
                break;
                case CardFace_Eight:
                target->id[1] = '8';
                break;
                case CardFace_Seven:
                target->id[1] = '7';
                break;
            }
            target->id[2] = '\0';
            
            
            gameTable->drawStackCount++;
        }
        
    }
    shuffle((byte*)gameTable->drawStack, sizeof(Card), gameTable->drawStackCount);
    for(uint32 p = 0; p < gameTable->playerCount; p++)
    {
        Player * currentPlayer = &gameTable->players[p];
        currentPlayer->cardCount = 4;
        for(uint32 c = 0; c < currentPlayer->cardCount; c++)
        {
            gameTable->drawStackCount--;
            currentPlayer->hand[c] = gameTable->drawStack[gameTable->drawStackCount];
        }
    }
    gameTable->drawStackCount--;
    gameTable->playStack[0] = gameTable->drawStack[gameTable->drawStackCount];
    gameTable->currentSuite = gameTable->playStack[0].suite;
    gameTable->currentEffect = TableEffect_None;
    gameTable->take2chain = 0;
    
}

inline void reshuffleDiscardedCards()
{
    for(uint32 i = 0; i < gameTable->playStackCount-1; i++){
        gameTable->drawStack[i] = gameTable->playStack[i];
    }
    gameTable->drawStackCount = gameTable->playStackCount - 1;
    gameTable->playStack[0] = gameTable->playStack[gameTable->playStackCount-1];
    gameTable->playStackCount = 1;
    shuffle((byte*)gameTable->drawStack, sizeof(Card), gameTable->drawStackCount);
}

inline bool run()
{
    bool play = true;
    bool validPlay = false;
    while(!validPlay)
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
        char choice[3];
        
        
        
        printf("\nYour play (card or 'd' to draw or 'e' to accept current effect): ");
        scanf("%3%3s", choice);
        
        
        
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
                continue;
            }
        }else{
            action.type = PlayerActionType_PlayCard;
            action.card.cardIndex = cardIndex;
        }
        action.playerIndex = gameTable->currentPlayer;
        
        
        
        switch(action.type){
            case PlayerActionType_TakeEffect:
            {
                switch(gameTable->currentEffect)
                {
                    case TableEffect_Stop:
                    validPlay = true;
                    break;
                    case TableEffect_Take2:
                    {
                        ASSERT(gameTable->take2chain > 0);
                        uint32 toTake = gameTable->take2chain * 2;
                        ASSERT(gameTable->drawStackCount > 0);
                        
                        do
                        {
                            toTake--;
                            gameTable->drawStackCount--;
                            currentPlayer->hand[currentPlayer->cardCount] = gameTable->drawStack[gameTable->drawStackCount];
                            currentPlayer->cardCount++;
                        }
                        while(toTake != 0 && gameTable->drawStackCount != 0);
                        //need to reshuffle and keep taking
                        if(toTake != 0)
                        {
                            reshuffleDiscardedCards();
                            do
                            {
                                toTake--;
                                gameTable->drawStackCount--;
                                currentPlayer->hand[currentPlayer->cardCount] = gameTable->drawStack[gameTable->drawStackCount];
                                currentPlayer->cardCount++;
                            }
                            while(toTake != 0 && gameTable->drawStackCount != 0);
                            
                            if(toTake != 0)
                            {
                                printf("Cannot draw any more. You have avoided full effect\n");
                            }
                            
                        }
                        
                        gameTable->take2chain = 0;
                        validPlay = true;
                    }
                    break;
                    case TableEffect_None:
                    {
                        printf("There is no effect\n");
                    }
                    break;
                    default:
                    INV;
                    break;
                }
                gameTable->currentEffect = TableEffect_None;
            }
            break;
            case PlayerActionType_Draw:
            {
                if(gameTable->currentEffect == TableEffect_None)
                {
                    if(gameTable->drawStackCount > 0)
                    {
                        gameTable->drawStackCount--;
                        currentPlayer->hand[currentPlayer->cardCount] = gameTable->drawStack[gameTable->drawStackCount];
                        currentPlayer->cardCount++;
                        validPlay = true;
                    }
                    else
                    {
                        printf("Cannot draw any more\n");
                        continue;
                    }
                }
                else
                {
                    printf("Cannot draw under effects\n");
                    continue;
                }
            }
            break;
            case PlayerActionType_PlayCard:
            {
                ASSERT(gameTable->playStackCount > 0);
                Card * topCard = &gameTable->playStack[gameTable->playStackCount-1];
                bool queen = topCard->suite != gameTable->currentSuite && played->suite == gameTable->currentSuite && gameTable->currentEffect == TableEffect_None;
                bool ace = gameTable->currentEffect == TableEffect_Stop && played->face == CardFace_Ace;
                bool seven = 
                    gameTable->currentEffect == TableEffect_Take2 &&
                    played->face == CardFace_Seven;
                bool regular = (played->face == topCard->face ||
                                played->suite == gameTable->currentSuite) && gameTable->currentEffect == TableEffect_None;
                
                if(queen || ace || seven || regular)
                {
                    gameTable->currentSuite = played->suite;
                    if(played->face == CardFace_Queen){
                        char choice = 0;
                        while(true)
                        {
                            printf("Select new suite (L, H, A, P): ");
                            scanf("%c", &choice);
                            if(choice == 'L')
                            {
                                gameTable->currentSuite = CardSuite_Leaf;
                                break;
                            }
                            else if(choice == 'H')
                            {
                                gameTable->currentSuite = CardSuite_Heart;
                                break;
                            }
                            else if(choice == 'A')
                            {
                                gameTable->currentSuite = CardSuite_Acorn;
                                break;
                            }
                            else if(choice == 'P')
                            {
                                gameTable->currentSuite = CardSuite_Pellet;
                                break;
                            }
                            printf("\nInvalid choice\n");
                        }
                    }
                    else if(played->face == CardFace_Ace)
                    {
                        gameTable->currentEffect = TableEffect_Stop;
                    }
                    else if(played->face == CardFace_Seven)
                    {
                        gameTable->currentEffect = TableEffect_Take2;
                        gameTable->take2chain++;
                    }
                    
                    
                    gameTable->playStack[gameTable->playStackCount] =currentPlayer->hand[action.card.cardIndex];
                    gameTable->playStackCount++;
                    currentPlayer->cardCount--;
                    currentPlayer->hand[action.card.cardIndex] = currentPlayer->hand[currentPlayer->cardCount];
                    validPlay = true;
                }
                else
                {
                    printf("Cannot play this card\n");
                    continue;
                }
            }
            break;
            default:
            INV;
            break;
        }
        
        if(gameTable->drawStackCount == 0){
            reshuffleDiscardedCards();
        }
        
        
    }
    
    
    
    gameTable->currentPlayer = (gameTable->currentPlayer + 1) % gameTable->playerCount;
    for(uint32 p = 0; p < gameTable->playerCount; p++)
    {
        play &= gameTable->players[p].cardCount > 0;
    }
    return play;
    
}