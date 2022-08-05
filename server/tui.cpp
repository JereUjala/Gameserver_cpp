#include <cstdio>
#include <cstdlib>
#include <ncurses.h>
#include <curses.h>
#include <stdlib.h>
#include "../header/Header.hpp"
#include "coommands.hpp"
#include <vector>

using namespace GameServer;
    std::vector<std::string> cmds;
    Tui::Tui(uint height, uint width, uint startY,
                     uint startX, bool border) {
        std::string commands[12];
        SetWin(newwin(height, width, startY, startX));
            if(border == true)
                box(win, 0, 0);
    }

    void Tui::SetWin(WINDOW* win){
           this->win = win;
    };

     struct Tui::Size& Tui::StartTui() {
        Server::Tui::Size* size = new Server::Tui::Size;
        initscr();
        cbreak();
        getmaxyx(stdscr, size->height, size->width);
        return *size;
    }

    void Tui::Input(class Server* server) {
                bool enter = false;
                std::string str;
            do {
                box(win, 0, 0);
                wmove(win, 1, 1);
                wrefresh(win);
                    str = "";
                    do {
                         command = wgetch(win);
                           if(command == 10) // decimal representation of newline
                                    break;
                            if(command == 127) {
                                if(str.length() <= 0)
                                    break;
                                else
                                    DeleteChar(str);
                            } else
                                str += command;

                        wrefresh(win);
                    } while(enter != true);
                       if (str == "q") {
                                endwin();
                                exit(-1);
                            }
                        server->SendToTui(str);
                    wclear(win);
            } while(true);
    }

void Tui::DeleteChar(std::string& str) {
                wclear(win);
                str.erase(str.size()-1);
                mvwprintw(win, 1, 1, "%s", str.c_str());
                box(win, 0, 0);
            }

    void Tui::SendToTui(std::string str) {
                Size size;
                cmds.push_back(str);
        getmaxyx(stdscr, size.height, size.width);
            if(cmds.size() > size.height-10) {
                cmds.resize(1);
                cmds[0] = "";
                    wclear(win);
            }
                    mvwprintw(win, 2+cmds.size(), 3,
                          "%s", cmds[cmds.size()-1].c_str());
        wrefresh(win);
    }
