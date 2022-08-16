#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <sys/select.h>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <vector>
#include <chrono>

#include "../header/Header.hpp"

using namespace GameServer;

float SERVER_TICKRATE = 120.0f;

void Clients::ClientHandler::ClientHandling() {
    int newConn{0};
    player = new Clients::ClientHandler::Player[maxPlayers];
    ServerProperties::Tick* tick{new ServerProperties::Tick};

  while(true) {
           tick->NextTick();
          poll(listeningSocket, nfds, -1);
           while(tick->DeltaTime() >= 1/SERVER_TICKRATE) {
                        tick->Reset();
                for (uint i = 0; i < nfds; ++i) {
                  if(listeningSocket[i].revents == 0)
                        continue;

                  if(listeningSocket[i].fd == listeningSocket[0].fd) {
                          newConn = accept(listeningSocket[0].fd, nullptr,
                                                                  nullptr);
                       if (newConn == -1) {
                          if (errno != EWOULDBLOCK) {
                                std::ostringstream ss;
                                ss << "Accepting failed " << newConn;

                            tui->SendToTui(ss.str());
                          }
                         break;
                       }
                        std::ostringstream ss;
                            ss << "New incoming connection: " << newConn;
                        tui->SendToTui(ss.str());
                            listeningSocket[nfds].fd = newConn;
                            listeningSocket[nfds].events = POLLIN;
                        Clients::ClientHandler::LoginHandler(
                               listeningSocket[nfds].fd, nfds);

                    } else {
                        /* std::ostringstream ss; */
                        /*         ss << "going to Siphon() nfds: " << nfds << " newconn " */
                        /*            << newConn << "from "  << listeningSocket[i].fd ; */
                        /*         tui->SendToTui(ss.str()); */
                            Siphon(&listeningSocket[i].fd);
                      }
                   }
                    tick->NextTick();
                }
            }
}

    void Clients::ClientHandler::Siphon(const int* fds) {
            byte buf[4096]{""};
        enum State : byte {
        Forwards   =0x70,
        Backwards  =0xE0,
        Left       =0xC0,
        Right      =0x80,
        Shoot      =0x01,
        msg        =0x71,
                //for debug
        Charachter =0x61,
        };
        ReceivedFromClient(fds, buf);
        byte compare = buf[0];

    std::ostringstream ss1;
        ss1 << "msg: " << buf;
   tui->SendToTui(ss1.str());

        std::ostringstream ss;
        switch(compare) {
                ss << *fds;
                case State::Backwards:
                        ss << " backwards first byte" << buf[0];
                        tui->SendToTui("Backwards");
                        std::cout << *fds << " is moving backwards\n";
                        break;
                case State::Forwards:
                        ss << " forwards.";
                        tui->SendToTui(ss.str());
                        break;
                case State::Left:
                        ss << " Left";
                        tui->SendToTui(ss.str());
                        /* std::cout << *fds << " is going left\n"; */
                        break;
                case State::Right:
                        ss << " Right";
                        tui->SendToTui(ss.str());
                        /* std::cout << *fds << " is going right\n"; */
                        break;
                case State::Shoot:
                        ss << " Shoot";
                        tui->SendToTui(ss.str());
                        /* std::cout << *fds << " is shooting.\n"; */
                        break;
                case State::Charachter:
                        ss << " Charachter";
                        tui->SendToTui(ss.str());
                        break;
                break;
        }
        SendToAllClients(buf);
                  /* clear the buffer*/
                memset(buf, 0, sizeof(buf));
    }

        void Clients::ClientHandler::SendToAllClients(const byte* const msg) {
                for(uint i = nfds-1; i >= 1; --i) {
                    /* if(listeningSocket[i].fd != listeningSocket[0].fd) */
                           SendToClient(&listeningSocket[i].fd, msg);
                }
        }

       bool Clients::ClientHandler::ReceivedFromClient(const int* const fds,
                                                       byte* msg) {
             /* what we get from connected clients*/
            int bytesGot = recv(*fds, msg, sizeof(msg), 0);

            if(bytesGot == -1) {
                std::ostringstream ss;
                ss << "There was a problem talking to client: " << *fds;
                        tui->SendToTui(ss.str());
                /* close(*fds); */
                for(int i = 0; i >= nfds; i++)
                   if(listeningSocket[i].fd == *fds)
                        listeningSocket[i].fd = -1;
                        nfds--;
                return false;
            }

            if(bytesGot == 0) {
                std::ostringstream ss;
                ss << "Client: " << *fds << " disconnected.";
                tui->SendToTui(ss.str());
                        RemovePlayer(*fds);

              return true;
            }
           return true;
        }

        void Clients::ClientHandler::SendToClient(const int* const clientSocket,
                                                  byte const msg[]){
               std::ostringstream ss;
                ss << "Sending to: " << *clientSocket << " | "
                   << msg;

                tui->SendToTui(ss.str());
                   // send to client and see if it worked
                if(send(*clientSocket, msg, sizeof(msg), 0) == -1) {
                   std::ostringstream ss;
                    ss << "Sending data to " <<
                          *clientSocket << " failed.";
                    tui->SendToTui(ss.str());
                        RemovePlayer(*clientSocket);
                }
        }

void Clients::ClientHandler::SendToClient(const int* const clientSocket,
                                          enum AskFromClient clientDo) {
               std::ostringstream ss;
                ss << "Sending to: " << *clientSocket << " | "
                   << clientDo;
                tui->SendToTui(ss.str());

                   // send to client and see if it worked
                if(send(*clientSocket, &clientDo, sizeof(byte), 0) == -1) {
                   std::ostringstream ss;
                    ss << "Sending data to " <<
                          *clientSocket << " failed.";
                    tui->SendToTui(ss.str());
                        RemovePlayer(*clientSocket);
                }
}

void Clients::ClientHandler::RemovePlayer(int rmFds) {

                if(listeningSocket[maxPlayers-1].fd == -1) {
                        close(maxPlayers-1);
                        listeningSocket[maxPlayers-1].fd = 0;
                } else {
                        for(int i = 1; i <= maxPlayers;i++) {
                           if(listeningSocket[i].fd == rmFds) {
                              close(rmFds);
                             for(int j=i; j < maxPlayers-1; j++) {
                               listeningSocket[j].fd = listeningSocket[j+1].fd;
                               listeningSocket[j+1].fd = 0;
                             }
                           }
                        }
                  }
           nfds--;
}
