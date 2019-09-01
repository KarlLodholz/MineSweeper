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
    void mine(const int &pos);
    void print(Cursur &c);
    //array of mines and stuffs later

private:
    const int refresh_rate = 1500;
    int width,height;
    static const char UNMINED = '-';
    static const char BORDER = '#';
    static const char MINE = 'x';
    int safe_moves = -1;
    int num_mines;
};

Field::Field(const int &width, const int &height) {
    this->width = width;
    this->height = height;
    update = true;
    f = new Position[width*height]();
    //the positions must be initalized now because dynamically allocated arrays of objects dont use the overwritten constructor
    for(int i = 0; i < width*height; i++) {
        f[i].stuff = '!';
        f[i].mined = false;
    }
    num_mines = width*height/10;
}

Field::~Field() {
    delete[] f;
}

void Field::print(Cursur &c) {
    clr();
    for(int i=0;i<2*width+3;i++)
        std::cout<<BORDER;
    std::cout<<"\n";
    for(int i = 0; i < height*width; i++) {
        if(i%width == 0) {
            std::cout << BORDER;
        }
        std::cout<<' '<<(c.pos == i && c.on ? c.icon : (f[i].mined ? f[i].stuff : UNMINED));
        if(i%width == width-1) {
            std::cout << " " << BORDER << "\n";
        }
    }   
    for(int i=0;i<2*width+3;i++)
        std::cout<<BORDER;
    std::cout.flush();
    update = false;
    return;
}

void Field::mine(const int &pos) {
    if(!f[pos].mined) {
        f[pos].mined = true;
        if(f[pos].stuff == MINE) {
            //you lose probably
        }
        else if(f[pos].stuff == ' ') {
            bool up = false, down = false, left = false, right = false;
            if((pos/width)-1 >= 0) {          //up
                up = true;
                mine(pos-width);
            }
            if((pos/width)+1 < height) {      //down
                down = true;
                mine(pos+width);
            }
            if((pos%width)-1 >= 0) {          //left
                left = true;
                mine(pos-1);
            }
            if((pos%width)+1 < width) {       //right
                right = true;
                mine(pos+1);
            }
            if(left && up)
                mine(pos - width - 1);
            if(right && up)
                mine(pos - width + 1);
            if(left && down)
                mine(pos + width - 1);
            if(right && down)
                mine(pos + width + 1);
        }
        else if(f[pos].stuff == '!') {
            int cntr = 0;
            bool up = false, down = false, left = false, right = false ,valid;
            std::vector<int> a;
            if(pos/width - 1 >= 0) {          //up
                up = true;
                a.push_back(pos-width);
            }
            if(pos/width + 1 < height) {      //down
                down = true;
                a.push_back(pos+width);
            }
            if((pos%width) - 1 >= 0) {        //left
                left = true;
                a.push_back(pos-1);
            }
            if((pos%width) + 1 < width) {     //right
                right = true;
                a.push_back(pos+1);
            }
            if(left && up)
                a.push_back(pos - width - 1);
            if(right && up)
                a.push_back(pos - width + 1);
            if(left && down)
                a.push_back(pos + width - 1);
            if(right && down)
                a.push_back(pos + width + 1);
            
            int p;

            while(cntr < num_mines) {
                up = down = left = right = false;
                valid = true;
                p = rand()%(width*height);
                for(int i=0;i<a.size();i++)
                    if(p == a[i])
                        valid = false;
                if(valid) {
                    f[p].stuff = 'x';
                    a.push_back(pos);
                    cntr++;
                }
            }
            
            for(int i = 0; i < width*height; i++) {
                if(f[i].stuff != MINE) {
                    f[i].stuff = '0';
                    if(i/width-1 >= 0) {          //up
                        up = true;
                        if(f[i-width].stuff == MINE)
                            f[i].stuff = (char)(f[i].stuff+1);
                    }
                    if(i/width+1 < height) {      //down
                        down = true;
                        if(f[i+width].stuff == MINE)
                            f[i].stuff = (char)(f[i].stuff+1);
                    }
                    if(i%width-1 >= 0) {          //left
                        left = true;
                        if(f[i-1].stuff == MINE)
                            f[i].stuff = (char)(f[i].stuff+1);
                    }
                    if(i%width+1 < width) {       //right
                        right = true;
                        if(f[i+1].stuff == MINE)
                            f[i].stuff = (char)(f[i].stuff+1);
                    }
                    if(left && up) {
                        if(f[i - width - 1].stuff == MINE)
                            f[i].stuff = (char)(f[i].stuff+1);
                    }
                    if(right && up) {
                        if(f[i - width + 1].stuff == MINE)
                            f[i].stuff = (char)(f[i].stuff+1);
                    }
                    if(left && down) {
                        if(f[i + width - 1].stuff == MINE)
                            f[i].stuff = (char)(f[i].stuff+1);
                    }
                    if(right && down) {
                        if(f[i + width + 1].stuff == MINE)
                            f[i].stuff = (char)(f[i].stuff+1);
                    }
                    if(f[i].stuff == '0')
                        f[i].stuff = ' ';
                }
            }
            f[pos].mined = false;
            mine(pos); //calls mine again because everything has been initialized
        }
    }
}
#endif