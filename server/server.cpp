#include <curses.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/poll.h>
#include "../header/Header.hpp"
#include <string>
#include <sstream>
#include <iostream>

using namespace GameServer;

Server::ServerProperties::Tick::Tick() {
        Reset();
        mTimeScale = 1.0f;
        mDeltatime = std::chrono::duration<float>(0.0f);
}
Server::ServerProperties::Tick::~Tick() {};

void Server::ServerProperties::Tick::Reset() {
        mStartTime = std::chrono::system_clock::now();

}

float Server::ServerProperties::Tick::DeltaTime() {
        return mDeltatime.count();
}

void Server::ServerProperties::Tick::Release() {
}

void Server::ServerProperties::Tick::TimeScale(float t) {
        mTimeScale = t;
}

float Server::ServerProperties::Tick::TimeScale() {
        return mTimeScale;
}

void Server::ServerProperties::Tick::NextTick() {
        mDeltatime = std::chrono::system_clock::now() - mStartTime;
}


// to make singelton work
Server* Server::sServer = nullptr;
Server* Server::MakeServer(uint port, uint maxPlayers, uint height, uint width,
                            uint startY, uint startX, bool border) {

        if(sServer == nullptr)
        sServer = new Server(port, maxPlayers, height, width,
                                    startY, startX, border);

return sServer;
};


    Server::Server::Server(uint port, uint maxPlayers, uint height, uint width,
                           uint startY, uint startX, bool border) :
                           Tui(height, width, startY, startX, border) {

                    this->port = port;
                    this->maxPlayers = maxPlayers;
                    SetWin(newwin(height,width, startY, startX));

                    /* if(border == true) */
                        /* box(this->win,0,0); */
    }

  void Server::StartServer() {
        constexpr uint on{1};

          int listener{socket(AF_INET, SOCK_STREAM, 0)};
        if(ioctl(listener, FIONBIO, (char)*&on) < 0)
            SendToTui("listener\n");

        sockaddr_in hint;
            hint.sin_family = AF_INET;
            hint.sin_port = port; /* port to bind to*/
        inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    std::ostringstream ss;

        if (bind(listener, (struct sockaddr*)&hint, sizeof(hint)) < 0) {
             std::cerr << "Cannot bind to ip / port: " << port;
                /* ss << "Cannot bind to ip/port: " << port; */
                /* SendToTui(ss.str()); */
             exit(-1);
        } else
             std::cout << "Binding was succesful\n";

        if(listen(listener, SOMAXCONN) < 0) {
                std::cerr << "cannot listen.\n";
            exit(-1);
        }

        if (listener < 0) {
                std::cerr << "socket() failed.";
          exit(-1);
        } else
           std::cout << "Socket was created successfully.\n"
                     << "Server started successfully.\n";

          std::cout << "Allocating memory.\n";
                pollfd* Socket{new pollfd[1+maxPlayers]};
                Socket[0].fd    = listener;
                Socket[0].events  = POLLIN;
                listeningSocket = Socket;

                /* listeningSocket[0].fd = listener; */
                /* listeningSocket->events = POLLIN; */

                std::cout << "Bound to a socket" << std::endl;
 }
