#version 460 core

in vec3 WorldPos;

uniform vec3 u_lightWorldPos;

// depth to the pixel to shade from the light source
out float o_lightToPixelDistance;

void main() {
    vec3 lightToVertex = WorldPos - u_lightWorldPos;
    o_lightToPixelDistance = length(lightToVertex);
}
