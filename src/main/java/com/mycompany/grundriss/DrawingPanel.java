package com.mycompany.grundriss;

import java.awt.Graphics;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.ArrayList;
import javax.swing.JPanel;

public class DrawingPanel extends JPanel {
        private ArrayList<Shape> shapes = new ArrayList<>();
        private int startX, startY, currentX, currentY;

        
        
        @Override
        protected void paintComponent(Graphics g) {
            super.paintComponent(g);

            for (Shape shape : shapes) {
                shape.draw(g);
            }

            Mode m = Grundriss.getCurrentMode();
            
            if (m != null) switch (m) {
                case ADD_ROOM -> g.drawRect(startX, startY, currentX - startX, currentY - startY);
                case ADD_WALL -> g.drawLine(startX, startY, currentX, currentY);
                case ADD_SENSOR -> g.drawString("S", startX, startY);
            }
        }

        public DrawingPanel() {
            addMouseListener(new MouseAdapter() {
                @Override
                public void mousePressed(MouseEvent e) {
                    //sets start and current to current mouse position
                    startX = e.getX();
                    startY = e.getY();
                    currentX = startX;
                    currentY = startY;

                    Mode m = Grundriss.getCurrentMode(); //ADD_ROOM, ADD_WALL, ADD_SENSOR
                    if (m != Mode.NONE) {
                        Shape newShape = null;

                        if (m != null) switch (m) {
                            case ADD_ROOM -> newShape = new Rectangle(startX, startY, 0, 0);
                            case ADD_WALL -> newShape = new Line(startX, startY, startX, startY);
                            case ADD_SENSOR -> newShape = new Sensor(startX, startY);
                        }

                        if (newShape != null) shapes.add(newShape);
                    }
                    
                    repaint();
                }


                @Override
                public void mouseReleased(MouseEvent e) {
                    startX = 0;
                    startY = 0;
                    currentX = 0;
                    currentY = 0;
                }
            });

            addMouseMotionListener(new MouseAdapter() {
                @Override
                public void mouseDragged(MouseEvent e) {
                    currentX = e.getX();
                    currentY = e.getY();

                    if (!shapes.isEmpty()) {
                        Shape currentShape = shapes.get(shapes.size() - 1);
                        System.out.print("\ncurrentX= " + currentX + ", currentY= " + currentY + "\n");
                        currentShape.resize(currentX, currentY);
                    }

                    repaint();
                }
            });
        }

        public void clearShapes(){
            shapes.clear();
            repaint();
        }

        public void undoLastShape() {
            if (!shapes.isEmpty()) {
                shapes.remove(shapes.size() - 1);
                repaint();
            }
        }
    }

