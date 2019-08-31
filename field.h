#ifndef FIELD_H
#define FIELD_H
#include "cursur.h"
#include "position.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <stropts.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

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
    Position *f;
    bool update;
    Field(const int &width, const int &height);
    ~Field();
    void mine(Cursur &c);
    void print(Cursur &c);
    //array of mines and stuffs later

private:
    const int refresh_rate = 1500;
    int width,height;
    char unmined = '-';
    char border = '#';
    int safe_moves = -1;
    int num_mines;
};

Field::Field(const int &width, const int &height) {
    this->width = width;
    this->height = height;
    update = true;
    f = new Position[width*height];
    num_mines = width*height/10;
}

Field::~Field() {
    delete[] f;
}

void Field::print(Cursur &c) {
    clr();
    for(int i=0;i<width+2;i++)
        std::cout<<border;
    std::cout<<"\n";
    for(int y=0;y<height;y++) {
        std::cout<<border;
        if(c.pos/width == y) //this check reduced number of checks done overall
            for(int x=0;x<width;x++) {
                std::cout<< (c.pos%width == x && c.on ? c.icon : (f[y*width+x].mined ? f[y*width+x].stuff : unmined));
            }
        else {
            for(int x=0;x<width;x++) {
                std::cout<<(f[y*width+x].mined ? f[y*width+x].stuff : unmined);
            }
        }
        std::cout<<border<<"\n";
    }
    for(int i=0;i<width+2;i++)
        std::cout<<border;
    std::cout.flush();
    update = false;
    return;
}

void Field::mine(Cursur &c) {
    if(!f[c.pos].mined) {
        f[c.pos].mined = true;
        if(f[c.pos].stuff == 'x') {
            
        }
        else if(f[c.pos].stuff <= '8' && f[c.pos].stuff >='0') {
            
        }
        else {
            int cntr = 0;
            bool up,down,left,right,valid;
            std::vector<int> a;
            if(c.pos/width-1 >= 0) {          //up
                up = true;
                a.push_back(c.pos-width);
            }
            if(c.pos/width+1 < height) {      //down
                down = true;
                a.push_back(c.pos+width);
            }
            if(c.pos%width-1 >= 0) {          //left
                left = true;
                a.push_back(c.pos-1);
            }
            if(c.pos%width+1 < width) {       //right
                right = true;
                a.push_back(c.pos+1);
            }
            if(left && up)
                a.push_back(c.pos - width - 1);
            if(right && up)
                a.push_back(c.pos - width + 1);
            if(left && down)
                a.push_back(c.pos + width - 1);
            if(right && down)
                a.push_back(c.pos + width + 1);
            
            int pos;

            while(cntr < num_mines) {
                up = down = left = right = false;
                valid = true;
                pos = rand()%(width*height);
                for(int i=0;i<a.size();i++)
                    if(pos == a[i])
                        valid = false;
                if(valid) {
                    f[pos].stuff = 'x';
                    a.push_back(pos);
                }
            }
        }
    }
}
#endif