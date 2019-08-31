#ifndef FIELD_H
#define FIELD_H
#include "cursur.h"
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
    char **f;
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
    int mines[];
};

Field::Field(const int &width, const int &height) {
    this->width = width;
    this->height = height;
    update = true;
    f = new char*[width];
    for(int i=0;i<width;i++)
        f[i] = new char[height];
    for(int x=0;x<width;x++)
        for(int y=0;y<height;y++)
            f[x][y]=unmined;
    num_mines = width*height/10;
}

Field::~Field() {
    for(int i=0;i<width;i++)
        delete[] f[i];
    delete[] f;
}

void Field::print(Cursur &c) {
    clr();
    for(int i=0;i<width+2;i++)
        std::cout<<border;
    std::cout<<"\n";
    for(int y=0;y<height;y++) {
        std::cout<<border;
        if(c.y==y) //this check reduced number of checks done overall
            for(int x=0;x<width;x++) {
                std::cout<< (c.x==x && c.on ? c.icon : f[x][y]);
            }
        else {
            for(int x=0;x<width;x++) {
                std::cout<<f[x][y];
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
    if(safe_moves > 0) {

    }
    else if(safe_moves == 0) {

    }
    else { //this is for the first move in which safe moves is -1
        int cntr = 0;
        int pos = width*c.y + c.x;
        bool up,down,left,right,valid;
        std::vector<int> a;
        if(pos/width-1 >= 0) {          //up
            up = true;
            a.push_back(width*(c.y-1) + c.x);
        }
        if(pos/width+1 < height) {      //down
            down = true;
            a.push_back(width*(c.y+1) + c.x);
        }
        if(pos%width-1 >= 0) {          //left
            left = true;
            a.push_back(width*c.y + c.x - 1);
        }
        if(pos%width+1 < width) {       //right
            right = true;
            a.push_back(width*c.y + c.x + 1);
        }
        if(left && up)
            a.push_back(width*(c.y-1) + c.x - 1);
        if(right && up)
            a.push_back(width*(c.y-1) + c.x + 1);
        if(left && down)
            a.push_back(width*(c.y+1) + c.x - 1);
        if(right && down)
            a.push_back(width*(c.y+1) + c.x + 1);
        
        while(cntr < num_mines) {
            up = down = left = right = false;
            valid = true;
            pos = rand()%(width*height);
            for(int i=0;i<a.size();i++)
                if(pos == a[i])
                    valid = false;
            if(valid) {
                mines[cntr++] = pos;
                a.push_back(pos);

            }
        }
        safe_moves = 0;
        f[c.x][c.y] = ' ';
        
        
    }
}
#endif