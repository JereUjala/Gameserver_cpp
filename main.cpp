#include <cstdlib>
#include <netinet/in.h>
#include <ostream>
#include <sstream>
#include <string>
#include <iostream>
#include <unistd.h>
#include <thread>
#include "header/Header.hpp"
#include "header/terminaltui.hpp"
#include <assert.h>
using namespace GameServer;

 /*  What this server needs to do:
 *   get clients messages tell and
 *   the other players where the client is
 */

 int main(int argc, char* argv[]) {

            uint *port       = (uint*)malloc(sizeof(uint)),
                 *maxPlayers = (uint*)malloc(sizeof(uint));
                 *port       = 1512;
                 *maxPlayers = 16;

            Server::Tui::Size Size{Server::Tui::StartTui()};
            Server::Tui::Size *size{&Size};
            Server::Tui Console(3, size->width, size->height-3, 0, true);

        if (argc < 2) {
            std::cerr << "No port provided, please give a port as an argument\n"
                      << "Using default port: '1512'\n";
        } else {
                *port = htons(atoi(argv[1]));
        }
        Server* server{Server::MakeServer(*port, *maxPlayers, size->height-4, size->width,0,0,true)};

            // make thread for separate io
             std::thread thread (&Server::Tui::Input, Console, server);
            //detach to not crash the program
                    thread.detach();

            // start the server
                server->StartServer();
            // we can get these from server

            Clients::ClientHandler *cl{static_cast<Clients::ClientHandler*>(
                                            server)};
                // a hack to show a string in assert
            assert(cl != nullptr && "Client is null, this shouldn't have happened.");

            free(port);
            free(maxPlayers);
            cl->ClientHandling();

    return 0;
 };
