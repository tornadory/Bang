#include "PP.vert"

in vec3 B_In_PositionObject; // Plane positions

void Main()
{
    gl_Position = B_Matrices.PVM * vec4(B_In_PositionObject, 1);
}