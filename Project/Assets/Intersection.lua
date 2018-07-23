mat2 = gr.material({1.0, 1.0, 0.0}, {0.5, 0.7, 0.5}, {1.0, 0.0, 0.0}, 25, 0.0, 1.0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, {1.0, 0.0, 0.0}, 25, 0.0, 1.0)
mat4 = gr.material({1.0, 1.0, 1.0}, {0.5, 0.4, 0.8}, {1.0, 0.0, 0.0}, 25, 0.0, 1.0)

scene_root = gr.node('root')

s2 = gr.nh_sphere('s2', {0, -150, 0}, 100)
s2:set_material(mat2)

s4 = gr.nh_sphere('s2', {100, -150, 100}, 100)
s4:set_material(mat3)

csg = gr.CSG('csg', s2, s4, 1)
scene_root:add_child(csg)

s3 = gr.nh_box('s3', {0, -1200, -500}, 1200)
s3:translate(-400, -300, -500)
scene_root:add_child(s3)
s3:set_material(mat4)

white_light = gr.light({100.0, 150.0, 200.0}, {0.9, 0.9, 0.9}, {1, 1, 1})

gr.render(scene_root, 'nonhier.png', 512, 512,
    {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
    {0.3, 0.3, 0.3}, {white_light})
