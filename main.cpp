#include "field.h"
#include <iostream>

int main(int argc, char** argv) {
    std::string input;
    int width = 10;
    int height = 10;
    bool exit = false;
    // char **field = new char*[width];
    // for(int i=0;i<width;i++)
    //     field[i] = new char[height];
    // for(int x=0;x<width;x++)
    //     for(int y=0;y<height;y++)
    //         field[x][y]=' ';
    Field f(width,height);
    Cursur c('o',width,height);

    do {
        if(f.m_update)
            f.print(c);
        if(_kbhit()) {
            f.m_update = true;
            input = std::getchar(); //you might want to clear the buffer at this point
            if(input=="w")
                c.update("up");
            if(input=="s")
                c.update("down");
            if(input=="a")
                c.update("left");
            if(input=="d")
                c.update("right");
            if(input=="q") 
                exit = true;
        }
        //usleep(REFRESH_RATE);
        if(++c.m_cntr==c.m_toggle_limit) {
            c.update("toggle");
            f.m_update = true;
        }
    } while(!exit);
    printf("\nDone\n");

    return 0;
} 
