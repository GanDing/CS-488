# CS488 Project

---

## Compilation

----

Run the default:

      $ premake4 gmake
      $ make

combination to compile the code.
Then run:

    $ ./A4 Asset/<file.lua>

to start rendering. This assignment was build successfully on computer gl34.

## Objective

All pictures generated are file store in the png folder.

1. Primitives: primitive.png
  primitive.lua for generate the picture

2. CSG: union.png difference.png intersect.png
  intersection.lua to run the file, run other option need to change the last argument in gr.csg to 0(union), 1(intersect), 2(difference)

3. TextureMapping: textureMapping.png
  texture.lua for generate the picture (texture pic need)

4. bumpMapping: bumpMapping.png
  bump.lua for generate the picture (texture pic need)

5. Reflection: reflection.png reflection2.png
  To run this sence, need to change the parameter in A4.cpp "REFLECTION" to be true,
  then run reflection.lua

6. Refraction: refraction.png refraction2.png
  To run this sence, need to change the parameter in A4.cpp "REFRACTION" to be true,
  then run refraction.lua

7. Adative Anti Alising: adativeAntiAlising.png
  To run for this scene, need to change the parameter in A4.cpp "ADATIVE_REGULAR_SAMPLING" to be true

8. Soft Shadow: softShadow.png
  To run for this scene, need to change the parameter in A4.cpp "AREA_LIGHT" to be true

9. Motion Blur: motionBlur.png
  To run for this scene, need to change the parameter in A4.cpp "FINAL_TIME" to be anytime greater than 0(e.g. 5)

10.Final scene: final.png


## Extra Objective:

1.DOF: depthOfField.png depthOfField2.png
  To run for this scene, need to change the parameter in A4.cpp "FIELD" to be ture
