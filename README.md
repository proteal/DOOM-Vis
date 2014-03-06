DOOM-Vis
========================================

DOOM port and BSP visualization
(Windows 8 desktop)

by Daniel Fetter (2013-14)
Twitter: @DanielJFetter

========================================
INTRODUCTION 
========================================

This game/app is primarily intended as an educational tool.  The motivation behind it was to learn about DOOM's software renderer and algorithms.  After porting the game (from the LinuxDoom source release) to work with Win 8, I augmented it with a side-by-side visualization of some internal rendering data.  The BSP structure and traversal can be observed concurrently with the game.

The green and yellow lines are BSP planes.  Yellow means only the closer side/child was traversed; the far child was skipped due to its bounding box being out of view.

========================================
RUNNING
========================================

Put a DOOM WAD in the Data folder.  DOOMU.WAD (Ultimate DOOM) and DOOM2.WAD were tested to work.
Build with VS 2013 (I developed in VS 2013 Express), and launch from solution directory. 

========================================
CONTROLS 
========================================

Menu:
  Mouse up/down - menu up/down
  Mouse1 - menu confirm
  Mouse2 - menu back

Game:
  ESC - toggle menu
  Mouse left/right - turn left/right
  W,A,S,D - move
  Mouse1 - fire
  space - use
  tab - map
  
Visualization:
  F1 - toggle control of camera offset with mouse and W,A,S,D
  F2 - reset camera offset to match game
  F3 - toggle running BSP traversal animation
  
========================================
THANKS 
========================================

id Software for the game and source release
Chocolate Doom for some porting reference
WildMIDI for a nice MIDI conversion library
