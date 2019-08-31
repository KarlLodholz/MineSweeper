#ifndef CURSUR_H
#define CURSUR_H
#include <iostream>

class Cursur {
public:
    Cursur(const char &icon,const int &width, const int &height);
    void update(const std::string &ctrl);
    char icon;
    int width, height;
    int pos;
    bool on;
    int cntr;
    int toggle_limit;
};

Cursur::Cursur(const char &icon,const int &width, const int &height) {
    this->icon = icon;
    this->width = width;
    this->height = height;
    pos = 0;
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
        if(--pos % width <= width - 1 && (pos + 1) % width == 0)
            pos += width;
    }
    else if(ctrl=="right") {
        cntr = 0;
        on = true;
        if(++pos % width == 0 && (pos - 1) % width == width - 1)
            pos -= width; 
    }
    else if(ctrl=="up") {
        cntr = 0;
        on = true;
        pos -= width;
        if(pos < 0)
            pos += width*height;
    }
    else if(ctrl=="down") {
        cntr = 0;
        on = true;
        pos += width;
        if(pos>=width*height)
            pos -= width*height;
    }
}

#endif