//
//  VertexShader.glsl
//  xmxGame
//
//  Created by XMX on 2019/09/16.
//  Copyright © 2019 XMX. All rights reserved.
//

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
}
