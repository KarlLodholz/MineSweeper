#include "field.h"

int main(int argc, char** argv) {
    srand (time(NULL));
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
        if(f.update)
            f.print(c);
        if(_kbhit()) {
            f.update = true;
            input = std::getchar(); //you might want to clear the buffer at this point
            if(input=="w")
                c.update("up");
            if(input=="s")
                c.update("down");
            if(input=="a")
                c.update("left");
            if(input=="d")
                c.update("right");
            if(input==" ")
                f.mine(c.pos);
            if(input=="f")
                exit = f.flag(c.pos);
            if(input=="q") {
                exit = true;
                std::cout<<"";
                std::cout.flush();
            }
        }
        if(++c.cntr==c.toggle_limit) {
            f.update = true;
            c.update("toggle");
        }
        f.t2 = std::chrono::high_resolution_clock::now();
        f.time_span = std::chrono::duration_cast<std::chrono::duration<int>>(f.t2 - f.t1);
        if(f.m_time != (int)(f.time_span.count())) {
            f.m_time = (int)(f.time_span.count());
            f.update = true;
        }
    } while(!exit);
    printf("\nDone\n");

    return 0;
} 
