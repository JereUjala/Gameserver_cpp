#include <cstdlib>
#include <netinet/in.h>
#include <ostream>
#include <sstream>
#include <string>
#include <iostream>
#include <unistd.h>
#include <thread>
#include "header/Header.hpp"
#include <assert.h>
using namespace GameServer;

 int main(int argc, char* argv[]) {

            uint *port       = (uint*)malloc(sizeof(uint)),
                 *maxPlayers = (uint*)malloc(sizeof(uint));
                 *port       = 1512;
                 *maxPlayers = 16;

            Tui::Size Size{Tui::StartTui()};
            Tui::Size *size{&Size};
            Tui Console(3, size->width, size->height-3, 0, true);

        if (argc < 2) {
            std::cerr << "No port provided, please give a port as an argument\n"
                      << "Using default port: '1512'\n";
        } else {
            *port = htons(atoi(argv[1]));
        }

        Server* server{Server::MakeServer(*port, *maxPlayers)};

                server->tui = new Tui(size->height-4, size->width,
                                     0,0, false);

            // make thread for separate io
             std::thread thread (&Tui::Input, Console, server);
            //detach to not crash the program
                    thread.detach();

            // start the server
                server->StartServer();

            Clients::ClientHandler *cl{static_cast<Clients::ClientHandler*>(
                                            server)};
                // a hack to show a string in assert
            assert(cl != nullptr && "Client is null, this shouldn't have happened.");

            free(port);
            free(maxPlayers);
            cl->ClientHandling();

    return 0;
 };
