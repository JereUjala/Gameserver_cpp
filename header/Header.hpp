#pragma once  /* to only include this file once*/
#include <iostream>
#include <chrono>

namespace GameServer {
  using byte = unsigned char;

    class Tui {
      private:
        /* struct WINDOW; */
          typedef struct  _win_st WINDOW;
          uint command;
          void DeleteChar(std::string& str);
      public:
          struct Size {
           uint height;
           uint width;
          };
           WINDOW* win;
          const WINDOW* GetWindow();
          void SetWin(WINDOW* win);
          void Input(class Server* server);
         /* Make a window*/
          Tui(uint height, uint width, uint startY, uint startX, bool border);
          static struct Tui::Size& StartTui();
         /* A string we want to send to cli*/
          void SendToTui(const std::string str);

    };

    class Server : public Tui {
      public:
          void StartServer();
          static Server* MakeServer(uint port, uint maxPlayers, uint height,
                                    uint width, uint startY=0, uint startX=0,
                                    bool border=false);
          auto getMaxPlayers() -> int;
      private:
          Server(uint port, uint maxPlayers, uint height, uint width, uint
                 startY=0, uint startX=0, bool border=false);
           static Server* sServer;
           typedef struct pollfd pollfd;
           uint port;
      public:

        class ServerProperties {
         public:
            /* Convert std::string to a c string*/
             /* static char* strTocstr(std::string str); */
              class Tick {
                private:
                   std::chrono::system_clock::time_point mStartTime;
                   std::chrono::duration<float> mDeltatime;
                    float mTimeScale;
                   ~Tick();
                public:
                    Tick();
                    void Reset();
                    void Release();
                    void TimeScale(float time = 1.0f);
                    void NextTick();
                    float DeltaTime();
                    float TimeScale();
              };
        };
      protected:
          uint maxPlayers;
          pollfd* listeningSocket;
          uint nfds{1};
    };

 namespace Clients {

     class ClientHandler : public Server {
        struct Player;
          public:
               void ClientHandling();
               Player* player;
      private:

                enum AskFromClient : byte {
                     AskName            = 0x6E,
                };
                enum SendToClient : byte {
                     SendTickRate       = 0x74,
                     SendExistingPlayer = 0x70,
                     SendAnewPlayer     = 0x4C,
                     SendId             = 0x96,
                };

         void Siphon(const int*const fds);
         void MovePlayer(const int* const fds, const int* const moveAmount);
         void LoginHandler(const int fds, const int playerNum);
         void SendToClient(const int*const fds, byte const* const msg);
         void SendToClient(const int*const fds, const enum AskFromClient);
         void SendToAllClients(const byte* const msg);
         bool ReceivedFromClient(const int* fds, byte* msg);
          struct Player {
               int         kills;
               uint        id;
               uint        deaths;
               bool        leading;
               std::string name;
          };
     };
 }
}
