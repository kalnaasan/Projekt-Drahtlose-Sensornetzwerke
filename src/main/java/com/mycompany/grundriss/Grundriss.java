package com.mycompany.grundriss;

import javax.swing.*;
import java.awt.*;

public class Grundriss extends JFrame {

    private DrawingPanel drawingPanel = new DrawingPanel();
    private static Mode currentMode = Mode.NONE;
    
    //Buttons
    JToggleButton roomButton = new JToggleButton("Add Room");
    JToggleButton wallButton = new JToggleButton("Add Wall");
    JToggleButton sensorButton = new JToggleButton("Add Sensor");
    JButton clearButton = new JButton("Clear");
    JButton undoButton = new JButton("Undo");
    
    public Grundriss() {
        setTitle("Grundriss");
        setSize(800, 600);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLocationRelativeTo(null);    //setGUI in center of the screen

        //when button is clicked; e is the action event
        roomButton.addActionListener(e -> button_clicked(roomButton));
        wallButton.addActionListener(e -> button_clicked(wallButton));
        sensorButton.addActionListener(e -> button_clicked(sensorButton));
        clearButton.addActionListener(e -> drawingPanel.clearShapes());
        undoButton.addActionListener(e -> drawingPanel.undoLastShape());

        //Button Panel 1
        JPanel panel1 = new JPanel();
        panel1.add(roomButton);
        panel1.add(wallButton);
        panel1.add(sensorButton);
        
        //Button Panel 2
        JPanel panel2 = new JPanel();
        panel2.add(clearButton);
        panel2.add(undoButton);
        
        add(panel1, BorderLayout.NORTH);
        add(panel2, BorderLayout.SOUTH);
        add(drawingPanel, BorderLayout.CENTER);
    }

    private void button_clicked(JToggleButton b){
        
        //shows the other buttons as unselected
        if(b!=roomButton)roomButton.setSelected(false);
        if(b!=wallButton)wallButton.setSelected(false);
        if(b!=sensorButton)sensorButton.setSelected(false);
        
        if(b.isSelected()==false) currentMode = Mode.NONE; //when button was unselected
        else if(b==roomButton)currentMode = Mode.ADD_ROOM; 
        else if(b==wallButton)currentMode = Mode.ADD_WALL;
        else if(b==sensorButton)currentMode = Mode.ADD_SENSOR;
    }
    
    public static Mode getCurrentMode() {
        return currentMode;
    }
    
    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            Grundriss grundriss = new Grundriss();
            grundriss.setVisible(true);
        });
    }

    
}







