#include <sstream>
#include <string>

#include "../header/Header.hpp"
using namespace GameServer;

char id = 0;

void Clients::ClientHandler::LoginHandler(int const fds,int playerNum) {
                    byte buf[10]{""};

                bool gotMsg{false};
                // get clients name
                while(!gotMsg)
                gotMsg = ReceivedFromClient(&fds, buf);
        std::ostringstream ss;
        ss << "Got client name as: " << buf <<".";
       tui->SendToTui(ss.str());
                // write player name to struct
                for(int i = 0; i< 10; ++i)
                  player[playerNum].name += buf[i];

                player[playerNum].name.shrink_to_fit();

                // send tickrate to client
                        byte serverTick[2]{SendToClient::SendTickRate,120};
                std::ostringstream ss2;
                ss2 << "Sending tickrate " << serverTick <<
               " to " << player[playerNum].name;
               tui->SendToTui(ss2.str());
                SendToClient(&fds, serverTick);

                        // send the newly connected client their id
                        byte clientId[2]{SendToClient::SendId,static_cast<byte>
                                                                       (id++)};
                        std::ostringstream ss3;
                        ss3 << "Sending id " << clientId << " to  player "<<
                                                                   fds;
                        tui->SendToTui(ss3.str());
                        SendToClient(&fds, clientId);

                if(id > 1) {
                        std::ostringstream ss;
                        ss << "Telling about other players to client " << fds;
                // tell other clients a new client joined
                byte* aNewPlayer{new byte[]{SendToClient::SendAnewPlayer,
                                static_cast<byte>(playerNum)}};
                SendToAllClients(aNewPlayer);

                // tell newly connected client that there are other clients connected
                        byte existingPlayers[2];
                        existingPlayers[0] = SendToClient::SendExistingPlayer;

                for(uint i = nfds-1; i > nfds; i--) {
                        existingPlayers[1] = player[i].id;
                        SendToClient(&fds,existingPlayers);
                }
        }

}
