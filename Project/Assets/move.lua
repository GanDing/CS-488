-- A simple scene with some miscellaneous geometry.

--mat1 = gr.material({0.0, 0.0, 0.0}, {0.5, 0.7, 0.5}, {1.0, 0.0, 0.0}, 25, 0.0, 5.0)

mat1 = gr.material({0.0, 1.0, 1.0}, {0.5, 0.7, 0.5}, {1.0, 0.0, 0.0}, 25, 0.0, 5.0)
mat2 = gr.material({1.0, 1.0, 0.0}, {0.5, 0.7, 0.5}, {1.0, 0.0, 0.0}, 25, 0.0, 1.0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, {1.0, 0.0, 0.0}, 25, 0.0, 1.0)
mat4 = gr.material({1.0, 1.0, 1.0}, {0.5, 0.4, 0.8}, {1.0, 0.0, 0.0}, 25, 0.0, 1.0)
text = gr.texture("texture/box.png", {0, 0, 0}, {1.0, 0.0, 0.0}, 0, 0.0, 1.0)
earth = gr.texture("texture/earthmap.png", {0, 0, 0}, {1.0, 0.0, 0.0}, 0, 0.0, 1)

scene_root = gr.node('root')

s3 = gr.nh_box('s3', {0, -1200, -500}, 1200)
s3:translate(-400, -300, -500)
scene_root:add_child(s3)
s3:set_material(mat4)

b1 = gr.nh_box('b1', {0, 0, 0}, 1)
b1:scale(100, 150, 100)
b1:translate(-200, -300, -100)
scene_root:add_child(b1)
b1:set_material(text)
b1:move(1, {0, 0.5, 0}, 0.5)

s4 = gr.nh_sphere('s4', {-100, 25, -300}, 100)
s4:translate(200, -200, 0)
scene_root:add_child(s4)
s4:set_material(earth)

white_light = gr.light({100.0, 150.0, 200.0}, {0.9, 0.9, 0.9}, {1, 1, 1})

gr.render(scene_root, 'motionBlur.png', 512, 512,
    {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
    {0.3, 0.3, 0.3}, {white_light})
