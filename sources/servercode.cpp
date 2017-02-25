#include "util_thread.h"
#include "util_time.h"
#include "util_rng.cpp"
#include "util_net.h"
#include "util_io.cpp"

struct Client{
    NetSocket connection;
    bool active;
};

struct Server{
    NetSocket localConnection;
    Client clients[10];
    uint8 count;
};

Mutex printMutex;

bool serverRunning = true;

static void print(const char * msg){
    lock(&printMutex);
    printf(msg);
    unlock(&printMutex);
}

void cli(){
    
}

void slave(Server * domain){
    
}

void run(){
    
    Server * server = (Server * ) mem.persistent;
    
    
    NetSocketSettings serverSettings;
    serverSettings.blocking = true;
    
    ASSERT(initSocket(&server->localConnection, "127.0.0.1", "12564", &serverSettings));
    
    printf("Server started.");
    
    ASSERT(tcpListen(&server->localConnection, 10));
    
    ASSERT(createMutex(&printMutex));
    
    Thread workerThread;
    
    
    
    ASSERT(createThread(&workerThread, (void (*)(void *)) slave, (void *) server));
    
    Thread cliThread;
    
    ASSERT(createThread(&cliThread, (void (*)(void *)) cli, (void *) NULL));
    
    while(serverRunning){
        NetSocket client;
        NetSocketSettings clientSettings;
        clientSettings.blocking = false;
        if(!tcpAccept(&server->localConnection, &client, &clientSettings)){
            continue;
        }
        bool found = false;
        for(int i = 0; i < ARRAYSIZE(server->clients); i++){
            if(server->clients[i].active == false){
                server->clients[i].connection = client;
                server->clients[i].active = true;
                found = true;
                break;
            }
        }
        if(!found){
            print("Denied client, because server is full.\r\n");
            closeSocket(&server->localConnection);
        }else{
            print("Accepted client.\r\n");
        }
    }
    
    
    joinThread(&workerThread);
    joinThread(&cliThread);
    
    ASSERT(destroyMutex(&printMutex));
    
    for(int i = 0; i < ARRAYSIZE(server->clients); i++){
        if(server->clients[i].active == true){
            closeSocket(&server->clients[i].connection);
        }
    }
    
    
    
    
}