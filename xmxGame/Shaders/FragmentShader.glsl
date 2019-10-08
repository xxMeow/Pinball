//
//  FragmentShader.glsl
//  xmxGame
//
//  Created by XMX on 2019/09/17.
//  Copyright © 2019 XMX. All rights reserved.
//

#version 330 core

in vec3 ourColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(ourColor, 1.0f);
}
