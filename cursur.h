#ifndef CURSUR_H
#define CURSUR_H
#include <iostream>

class Cursur {
public:
    Cursur(const char &icon,const int &width, const int &height) {
        m_icon = icon;
        m_width = width;
        m_height = height;
        m_x=0;
        m_y=0;
        m_on = true;
        m_cntr = 0;
        m_toggle_limit = 1000000;
    }

    void update(const std::string &ctrl) {
        if(ctrl=="toggle") {
            m_on = !m_on;
            m_cntr = 0;
        }
        else if(ctrl=="left") {
            m_cntr = 0;
            m_on = true;
            if(--m_x<0)
                m_x=m_width-1;
        }
        else if(ctrl=="right") {
            m_cntr = 0;
            m_on = true;
            m_x = (m_x+1)%-m_width;
        }
        else if(ctrl=="up") {
            m_cntr = 0;
            m_on = true;
            if(--m_y<0)
                m_y=m_height-1;
        }
        else if(ctrl=="down") {
            m_cntr = 0;
            m_on = true;
            m_y = (m_y+1)%-m_height;
        }
    }

    char m_icon;
    int m_width, m_height;
    int m_x,m_y;
    bool m_on;
    int m_cntr;
    int m_toggle_limit;
};

#endif