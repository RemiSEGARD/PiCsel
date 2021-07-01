# PiCsel

## What is PiCsel

PiCsel is an image and animation editor written in C

## Installation

In order to compile PiCsel, you need to download the following dependencies:

- SDL (v1.2+)
- GTK (v3.0+)

## Usage

To execute the program, do the following:

```
./main <args>

Here are the possible arguments:
    <w> <h>: Starts with an image of size w*h
    <filename>: Imports the given file on launch
```

----

### How to use the program:

You can pick any tool on the top left of the screen

- Pen tool:
    - Left click on the canvas to draw a continuous line.
- Eraser tool:
    - Left click on the canvas to erase a part of the drawing
- Fill tool:
    - Left click on an area to fill it with the selected color
- Line tool:
    - Click, drag and release to draw a line
- Rectangle tool:
    - Click, drag and release to draw a rectangle
- Circle tool:
    - Click, drag and release to draw a circle
- Selection tool:
    - Click, drag and release to select an area
    - Click anywhere to deselect or start a new selection
    - Use the arrow keys to move the selected area
    - Ctrl + A to select the whole layer
    - Ctrl + X to cut the selection
    - Ctrl + C to copy the selection
    - Ctrl + V to paste
    - Del to delete the selection

Layers and Frames:

- Press the "+" button of to add a layer or a frame
- Press a button containing a number to go to the n-th layer or frame
- Press the button below a number of a layer to show/hide the layer
- Press the "Play" button to play the animation

Saving, importing and exporting:

- Press the Open button on the menu Bar to open a new file
- Press the Save button to save the current project (add .aseprite to the name of the file)
- Press Export current image to export what you are seeing (backgroud excluded)
- Press Export Spritesheet to export a spritesheet
- Press Export GIF to export the animation as a GIF (add .gif to the name of the file)


