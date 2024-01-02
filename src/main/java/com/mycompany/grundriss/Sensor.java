package com.mycompany.grundriss;

import java.awt.Graphics;

class Sensor implements Shape {
    private int x;
    private int y;

    public Sensor(int x, int y) {
        this.x = x;
        this.y = y;
    }

    @Override
    public void draw(Graphics g) {
        g.drawString("S", x, y);
    }

    @Override
    public void resize(int x2, int y2) {
        
    }
    
}
