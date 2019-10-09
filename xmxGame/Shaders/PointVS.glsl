//
//  PointVS.glsl
//  xmxGame
//
//  Created by Mingxian Xiao on 09/10/2019.
//  Copyright © 2019 XMX. All rights reserved.
//

#version 330 core

layout(location = 0) in vec2 v_position;
layout(location = 1) in vec4 v_color;
layout(location = 2) in float v_size;

out vec4 f_color;

uniform mat4 uniProjMat;

void main(void)
{
    f_color = v_color;
    gl_Position = uniProjMat * vec4(v_position, 0.0f, 1.0f);
    gl_PointSize = v_size;
}
