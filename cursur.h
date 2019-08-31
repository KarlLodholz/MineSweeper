#ifndef CURSUR_H
#define CURSUR_H
#include <iostream>

class Cursur {
public:
    Cursur(const char &icon,const int &width, const int &height);
    void update(const std::string &ctrl);
    char icon;
    int width, height;
    int x,y;
    bool on;
    int cntr;
    int toggle_limit;
};

Cursur::Cursur(const char &icon,const int &width, const int &height) {
    this->icon = icon;
    this->width = width;
    this->height = height;
    x=0;
    y=0;
    on = true;
    cntr = 0;
    toggle_limit = 1000000;
}

void Cursur::update(const std::string &ctrl) {
    if(ctrl=="toggle") {
        on = !on;
        cntr = 0;
    }
    else if(ctrl=="left") {
        cntr = 0;
        on = true;
        if(--x<0)
            x=width-1;
    }
    else if(ctrl=="right") {
        cntr = 0;
        on = true;
        x = (x+1)%-width;
    }
    else if(ctrl=="up") {
        cntr = 0;
        on = true;
        if(--y<0)
            y=height-1;
    }
    else if(ctrl=="down") {
        cntr = 0;
        on = true;
        y = (y+1)%-height;
    }
}

#endif