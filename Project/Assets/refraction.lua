-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.0, 0.0, 0.0}, {0.5, 0.7, 0.5}, {0.2, 0.0, 0.8}, 25, 0.0, 1.5)
mat2 = gr.material({0.9, 0.9, 0.9}, {0.5, 0.7, 0.5}, {1.0, 0.0, 0.0}, 25, 0.0, 1.0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, {1.0, 0.0, 0.0}, 25, 0.0, 1.0)
mat4 = gr.material({1.0, 1.0, 1.0}, {0.5, 0.4, 0.8}, {1.0, 0.0, 0.0}, 25, 0.0, 1.0)

scene_root = gr.node('root')

s2 = gr.nh_sphere('s2', {0, -150, 0}, 150)
scene_root:add_child(s2)
s2:set_material(mat1)

s3 = gr.nh_box('s3', {0, -1200, -500}, 1200)
s3:translate(-400, -300, -500)
scene_root:add_child(s3)
s3:set_material(mat4)

white_light = gr.light({100.0, 150.0, 200.0}, {0.9, 0.9, 0.9}, {1, 1, 1})

gr.render(scene_root, 'refraction.png', 512, 512,
    {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
    {0.3, 0.3, 0.3}, {white_light})
