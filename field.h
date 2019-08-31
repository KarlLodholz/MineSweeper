#ifndef FIELD_H
#define FIELD_H
#include "cursur.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <stropts.h>
#include <unistd.h>


int _kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

void clr() {
    //outputs space the size of the terminal, in a sense clearing the screen
    std::cout<<"\033[2J\033[1;1H";
    std::cout.flush();
    return;
}

class Field {
public:
    char **m_f;
    bool m_update;
    Field(const int &width, const int &height) {
        m_width = width;
        m_height = height;
        m_update = true;
        m_f = new char*[width];
        for(int i=0;i<width;i++)
            m_f[i] = new char[height];
        for(int x=0;x<width;x++)
            for(int y=0;y<height;y++)
                m_f[x][y]=m_unmined;
    }
    void print(Cursur &c);
    //array of mines and stuffs later
private:
    const int m_refresh_rate = 1500;
    int m_width,m_height;
    char m_unmined = ' ';
    char m_border = '#';
};

void Field::print(Cursur &c) {
    clr();
    for(int i=0;i<m_width+2;i++)
        std::cout<<m_border;
    std::cout<<"\n";
    for(int y=0;y<m_height;y++) {
        std::cout<<m_border;
        if(c.m_y==y) //this check reduced number of checks done overall
            for(int x=0;x<m_width;x++) {
                std::cout<< (c.m_x==x && c.m_on ? c.m_icon : m_f[x][y]);
            }
        else {
            for(int x=0;x<m_width;x++) {
                std::cout<<m_f[x][y];
            }
        }
        std::cout<<m_border<<"\n";
    }
    for(int i=0;i<m_width+2;i++)
        std::cout<<m_border;
    std::cout.flush();
    m_update = false;
    return;
}

#endif