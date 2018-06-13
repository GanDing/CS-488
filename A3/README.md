# CS488 Assignment 2

---

## Compilation

----

Run the default:

    	$ premake4 gmake
    	$ make

combination to comile the code. 
Then run:

		$ ./A3 Asset/puppet.lua

to start rendering. This assignment was build successfully on computer gl34.

## Manual

----

Puppet Model:

root -> torso	-> torsoNeck(Joint) -> neck -> neckHead(Joint) -> head -> eyes/ears

torso: 
	arms(left and right)
	hip

hip:
	legs(left and right)


Arm (same for both left and right):

	shoulder -> shoulder(Joint) -> arm -> elbow -> elbow(Joint) -> forearm -> wrist -> wrist(Joint) -> hand

Leg (same for both left and right):

	hip(Joint)thigh -> knee -> knee(Joint) -> claf -> ankle(Joint) -> foot

* There are total 14 Joint nodes. 
	All nodes are allowed to rotate along x, in addition, head and feet are allow to rotate along y.
	Thus, there are total 17 DOFs.

* Rotation will be recorded after you finishing each move(release mouse button), that is, only one move will be recorded when you pressing one mouse button on Joint rotate.

* Reset all or Reset joint will also clear the current selected parts.

* There is a window to record your current undo/redo times, when times is 0 and you keep undo/redo, nothing will happen.

## Credit

----

* Most of the codes in picking and trackball are modified by provided material.
