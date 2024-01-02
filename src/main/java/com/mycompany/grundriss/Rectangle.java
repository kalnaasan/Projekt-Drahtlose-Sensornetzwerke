package com.mycompany.grundriss;

import java.awt.Graphics;

class Rectangle implements Shape {
    private int x;
    private int y;
    private int width;
    private int height;
    private int x2;
    private int y2;

    public Rectangle(int x, int y, int width, int height) {
        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
    }

    @Override
    public void draw(Graphics g) {
        if(x<x2 && y<y2) g.drawRect(x, y, width, height);
        else if(x<x2 && y>y2) g.drawRect(x, y2, width, height);
        else if(x>x2 && y<y2) g.drawRect(x2, y, width, height);
        else if(x>x2 && y>y2) g.drawRect(x2, y2, width, height);
    }

    @Override
    public void resize(int x2, int y2) {
        this.x2 = x2;
        this.y2 = y2;
        
        width = x2 - x;
        height = y2 - y;
        
        if(x > x2){ width *= (-1);}
        if(y > y2){ height *= (-1);}
    }
}
