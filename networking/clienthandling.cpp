#include <cstdlib>
#include <cstring>
#include <iostream>
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

                         /* if a new fd is our listening connection accept */
                  if(listeningSocket[i].fd == listeningSocket[0].fd) {
                        /* SendToTui("client socket is readable"); */
                          newConn = accept(listeningSocket[0].fd, nullptr,
                                                                  nullptr);
                       if (newConn == -1) {
                          if (errno != EWOULDBLOCK)
                            SendToTui("Accepting failed");
                        break;
                       }
                        std::ostringstream ss;
                            ss << "New incoming connection: " << newConn;
                        SendToTui(ss.str());
                            listeningSocket[nfds].fd = newConn;
                            listeningSocket[nfds].events = POLLIN;
                        Clients::ClientHandler::LoginHandler(
                               listeningSocket[nfds].fd, nfds-1);

                      nfds++;
                    }
                    Siphon(&listeningSocket[i].fd);
                }
                    tick->NextTick();
                }
            }
}

    void Clients::ClientHandler::Siphon(const int* fds) {
            byte buf[4096]{""};
        enum State : byte {
        // hex for easy value changing
        Forwards   =0x70,  //
        Backwards  =0xE0,  //
        Left       =0xC0,  //
        Right      =0x80,  //
        Shoot      =0x01,  //
        msg        =0x71,
                //for debug
        Charachter =0x61,  //a
        };

        ReceivedFromClient(fds, buf);
        byte compare = buf[0];

    std::ostringstream ss1;
        ss1 << "msg: " << buf;
   SendToTui(ss1.str());
                /* memset(buf, 0, sizeof(buf)); */

        std::ostringstream ss;
        switch(compare) {
                ss << *fds;
                case State::Backwards:
                        ss << " backwards first byte" << buf[0];
                        SendToTui("Backwards");
                        std::cout << *fds << " is moving backwards\n";
                        break;
                case State::Forwards:
                        ss << " forwards.";
                        SendToTui(ss.str());
                        /* std::cout << *fds << " is going backwards\n"; */
                        /* buf[0] = State::Forwards; // what do */
                        /* buf[1] = static_cast<uint>(*fds); // who */
                        /* buf[2] = 100; // how much */
                        break;
                case State::Left:
                        ss << " Left";
                        SendToTui(ss.str());
                        /* std::cout << *fds << " is going left\n"; */
                        break;
                case State::Right:
                        ss << " Right";
                        SendToTui(ss.str());
                        /* std::cout << *fds << " is going right\n"; */
                        break;
                case State::Shoot:
                        ss << " Shoot";
                        SendToTui(ss.str());
                        /* std::cout << *fds << " is shooting.\n"; */
                        break;
                case State::Charachter:
                        ss << " Charachter";
                        SendToTui(ss.str());
                        break;
                break;
        }
        SendToAllClients(buf);
                /* for(uint i = nfds; i >= 1; --i) { */
                /*     /1* if(listeningSocket[i].fd != *fds) continue; *1/ */
                /*     if(listeningSocket[i].fd != listeningSocket[0].fd) { */
                /*     /1*     continue; *1/ */

                /*            SendToClient(&listeningSocket[i].fd, buf); */
                /* } */
        /* } */
                /* SendToAllClients(buf); */
                  /* clear the buffer*/
                /* memset(buf, 0, sizeof(buf)); */
    }

/* void Clients::ClientHandler:: */
        void Clients::ClientHandler::SendToAllClients(const byte* const msg) {
             // send message to everybody that is not a listening socket
                for(uint i = nfds; i > 0; --i) {
                    /* if(listeningSocket[i].fd != *fds) continue; */
                    if(listeningSocket[i].fd == listeningSocket[0].fd)
                        continue;

                           SendToClient(&listeningSocket[i].fd, msg);
                }

        }

       bool Clients::ClientHandler::ReceivedFromClient(const int* const fds,
                                                       byte* msg) {
             /* what we get from connected clients*/
            int bytesGot = recv(*fds, msg, sizeof(msg), 0);

            if(bytesGot == -1) {
                std::ostringstream ss;
                ss << "There was a problem talking to client " << *fds;
                        SendToTui(ss.str());
                return false;
            }

            if(bytesGot == 0) {
                std::ostringstream ss;
                ss << "Client: " << *fds << " disconnected.";
                SendToTui(ss.str());
                /* close(*fds); */
                return true;
            }
           return true;
        }

        void Clients::ClientHandler::SendToClient(const int* const clientSocket,
                                                  byte const msg[]){
               std::ostringstream ss;
                ss << "Sending to: " << *clientSocket << " | "
                   << msg;

                SendToTui(ss.str());
                   // send to client and see if it worked *sizeof(byte)
                if(send(*clientSocket, msg, sizeof(msg), 0) == -1) {
                   std::ostringstream ss;
                    ss << "Sending data to " <<
                          *clientSocket << " failed.";
                    SendToTui(ss.str());
                }
        }

void Clients::ClientHandler::SendToClient(const int* const clientSocket,
                                                 const enum AskFromClient clientDo){
               std::ostringstream ss;
                ss << "Sending to: " << *clientSocket << " | "
                   << clientDo;
                SendToTui(ss.str());

                   // send to client and see if it worked
                if(send(*clientSocket, &clientDo, sizeof(byte), 0) == -1) {
                   std::ostringstream ss;
                    ss << "Sending data to " <<
                          *clientSocket << " failed.";
                    SendToTui(ss.str());
                }
        }
