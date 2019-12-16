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
#include <chrono>
#include <ctime>
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
    int m_time = 0;
    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    std::chrono::duration<int> time_span;
    Field(const int &width, const int &height);
    ~Field();
    bool mine(const int &pos);
    bool flag(const int &pos);
    void print(Cursur &c);
    //array of mines and nums later

private:
    int width,height;
    static const char ZERO = ' ';
    static const char UNMINED = '-';
    static const char BORDER = '#';
    static const char MINE = 'x';
    static const char FLAG = '#';
    int num_mines;
    std::vector<int> flags;
    bool safeMoves();
};

Field::Field(const int &width, const int &height) {
    this->width = width;
    this->height = height;
    update = true;
    f = new Position[width*height]();
    //the positions must be initalized now because dynamically allocated arrays of objects dont use the overwritten constructor
    for(int i = 0; i < width*height; i++) {
        f[i].num = '!';
        f[i].flagged = false;
        f[i].mined = false;
    }
    
    num_mines = width*height/10;
}

Field::~Field() {
    delete[] f;
}

void Field::print(Cursur &c) {
    //i dont use endl because that would result in the the input have to be printed at different incriments,
    //so instead I flush everything out at the end of the function
    clr();
    std::cout<<"Flags:"<< (num_mines- (int)flags.size());  //the static cast of arr.size() is necissary because size returns an unsigned int
    std::string str = "";
    int len = 1;
    int x = (num_mines- (int)flags.size());
    while (x /= 10)
        len++;
    int size = width*2+3-6-len-5;
    for(int i=0;i<size; i++)
        str += " ";
    std::cout<<str<<"Time:"<<(f[0].num == '!' ? 0 : m_time)<<"\n";
    
    //Board 
    for(int i=0;i<2*width+3;i++)
        std::cout<<BORDER;
    std::cout<<"\n";
    for(int i = 0; i < height*width; i++) {
        if(i%width == 0) {
            std::cout << BORDER;
        }
        std::cout<<" "<<(c.pos == i && c.on ? c.icon : (f[i].flagged ? FLAG : (f[i].mined ? f[i].num : UNMINED)));
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

bool Field::flag(const int &pos) {
    f[pos].flagged = !f[pos].flagged;
    if(f[pos].flagged)
        flags.push_back(pos);
    else {
        //sort through flags and take away the flag thats no supposed to be there
        for(int i=0; i<flags.size()-1; i++) {
            if(flags[i] == pos) {
                flags[i] == flags[flags.size()-1];
                break;
            }
        }
        flags.pop_back();
        flags.shrink_to_fit();
    }
    bool won = false;
    if(flags.size() == num_mines) {
        won = true;
        for(int i=0; i<flags.size(); i++) {
            if(f[flags[i]].num != MINE)
                won = false;
        }
    }
    return won;
}

bool Field::mine(const int &pos) {
    if(!f[pos].mined && !f[pos].flagged) {
        f[pos].mined = true;
        if(f[pos].num == MINE) {
            if(!safeMoves()) {
                std::cout<<"unlucky"<<std::endl;
            }
            return true;
        }
        else if(f[pos].num == ZERO) {
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
        else if(f[pos].num == '!') { //OCCURS FOR THE FIRST EXCAVATION
            int cntr = 0;
            bool up = false, down = false, left = false, right = false ,valid;
            std::vector<int> a;

            //FINDS AREA AROUND THE FIRST EXCAVATION POS
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
            
            //SETS MINES RANDOMLY AROUND THE FIELD
            int p;
            while(cntr < num_mines) {
                up = down = left = right = false;
                valid = true;
                p = rand()%(width*height);
                for(int i=0;i<a.size();i++)
                    if(p == a[i])
                        valid = false;
                if(valid) {
                    f[p].num = MINE;
                    a.push_back(p);
                    cntr++;
                }
            }
            
            //UPDATES THE NUM FOR EACH POSITION.  THIS IS WHEN THE NUMBERS ARE ASSIGNED
            for(int i = 0; i < width*height; i++) {
                if(f[i].num != MINE) {
                    up = down = left = right = false;
                    f[i].num = '0';
                    if((i/width)-1 >= 0) {          //up
                        up = true;
                        if(f[i-width].num == MINE)
                            f[i].num = (char)(f[i].num+1);
                    }
                    if((i/width)+1 < height) {      //down
                        down = true;
                        if(f[i+width].num == MINE)
                            f[i].num = (char)(f[i].num+1);
                    }
                    if((i%width)-1 >= 0) {          //left
                        left = true;
                        if(f[i-1].num == MINE)
                            f[i].num = (char)(f[i].num+1);
                    }
                    if((i%width)+1 < width) {       //right
                        right = true;
                        if(f[i+1].num == MINE)
                            f[i].num = (char)(f[i].num+1);
                    }
                    if(left && up) {
                        if(f[i - width - 1].num == MINE)
                            f[i].num = (char)(f[i].num+1);
                    }
                    if(right && up) {
                        if(f[i - width + 1].num == MINE)
                            f[i].num = (char)(f[i].num+1);
                    }
                    if(left && down) {
                        if(f[i + width - 1].num == MINE)
                            f[i].num = (char)(f[i].num+1);
                    }
                    if(right && down) {
                        if(f[i + width + 1].num == MINE)
                            f[i].num = (char)(f[i].num+1);
                    }
                    if(f[i].num == '0')
                        f[i].num = ZERO;
                }
            }

            f[pos].mined = false;
            t1 = std::chrono::high_resolution_clock::now();
            mine(pos); //calls mine again because everything has been initialized
        }
    }
    return false;
}

bool Field::safeMoves() {

    // create copy of field
    // find where flags should be & place flags
    // with copied field look for positions that should be guessed, return true if one is found

    //places flags on copy of field
    for(int i = 0; i < width*height; i++) {
        if(f[i].mined && f[i].num != ZERO) {
            //basic look around
            //1-2 pattern
            
        }
    }

    //looks for space places to mine
    for(int i = 0; i < width*height; i++) {
        if(f[i].mined && f[i].num != ZERO) {
            //basic look around
            //1-1 pattern
            //the 1-1 pattern only really applies to numbers 1-3 (3 shouldnt really happen but it is possible)
            if(f[i].num <= '1' && f[i].num >= '3') {
                //if surrounding 
            }
        }
    }
    
    // //calculates the number of safe moves
    // bool left,right,up,down;
    // int cntr;
    // for(int i = 0; i < width*height; i++) {
    //     if(f[i].mined && f[i].num != ZERO) {
    //         cntr = 0;
    //         up = down = left = right = false;
    //         if((i/width)-1 >= 0) {         //up
    //             up = true;
    //             if(!f[i-width].mined)
    //                 cntr++;
    //         }
    //         if((i/width)+1 < height) {     //down
    //             down = true;
    //             if(!f[i+width].mined)
    //                 cntr++;
    //         }
    //         if((i%width)-1 >= 0) {         //left
    //             left = true;
    //             if(!f[i-1].mined)
    //                 cntr++;
    //         }
    //         if((i%width)+1 < width) {      //right
    //             right = true;
    //             if(!f[i+1].mined)
    //                 cntr++;
    //         }
    //         if(up && left) {
    //             if(!f[i-width-1].mined)
    //                 cntr++;
    //         }
    //         if(up && right) {
    //             if(!f[i-width+1].mined)
    //                 cntr++;
    //         }
    //         if(down && left) {
    //             if(!f[i+width-1].mined)
    //                 cntr++;
    //         }
    //         if(down && right) {
    //             if(!f[i+width+1].mined)
    //                 cntr++;
    //         }
    //         if(cntr == (int)f[i].num-48)  // you have to subtact 48 because '0' - 48 == 0 because ascii table
    //             return true;
    //     }
    // } 
    std::cout.flush();
    return false;
}
#endif