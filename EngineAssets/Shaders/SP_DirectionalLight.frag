#include "SP.frag"
#include "DirectionalLight.glsl"

void Main()
{
    if (B_SampleReceivesLight())
    {
        vec4 diffColor = B_SampleDiffColor();
        vec3 dirLightApport = GetDirectionalLightColorApportation(
                                    B_ComputeWorldPosition(),
                                    B_SampleNormal(),
                                    diffColor.rgb,
                                    B_SampleShininess(),
                                    B_LightForwardWorld,
                                    B_LightIntensity,
                                    B_LightColor.rgb,
                                    B_CameraPositionWorld.xyz);

        B_Out_Color = vec4(B_SampleColor().rgb + dirLightApport, diffColor.a);
    }
}
