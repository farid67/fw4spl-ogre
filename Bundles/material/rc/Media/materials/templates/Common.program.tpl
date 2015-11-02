//---------------------------------------------------------------------------
// File generated with genMaterials.py
//
// DO NOT EDIT MANUALLY !!!
//
//---------------------------------------------------------------------------


vertex_program Lighting_VP glsl
{
    source Lighting.glsl
    preprocessor_defines NUM_LIGHTS=2
}

//---------------------------------------------------------------------------

fragment_program Lighting_FP glsl
{
    source Lighting.glsl
    preprocessor_defines NUM_LIGHTS=2
}

//---------------------------------------------------------------------------

fragment_program DepthPeelingCommon_FP glsl
{
    source DepthPeelingCommon_FP.glsl
}

//---------------------------------------------------------------------------

fragment_program Negato_FP glsl
{
    source Negato_FP.glsl
}

//-----------------------------------------------------------------------------
// Vertex shader materials
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, params in configsVP %}

//---------------------------------------------------------------------------

vertex_program RenderScene_{{ shading }}_VP_glsl glsl
{
    source RenderScene_VP.glsl
    {% if shadersVP %}attach {{ shadersVP }}{% endif %}

    {% if defines %}preprocessor_defines {{ defines }}{% endif %}

    default_params
    {
        param_named_auto u_worldViewProj worldviewproj_matrix
        param_named_auto u_world world_matrix
        param_named_auto u_normalMatrix inverse_transpose_world_matrix
{% for param in params['renderSceneVP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}



//-----------------------------------------------------------------------------
// Geometry shader materials
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, params in configsGP %}

//---------------------------------------------------------------------------

geometry_program PerPrimitiveAttribute_{{ shading }}_GP_glsl glsl
{
    source PerPrimitiveAttribute.glsl

    {% if defines %}preprocessor_defines {{ defines }}{% endif %}

    default_params
    {
        // Unit state is set to 10, but the real index will be set in SMaterial::setShadingMode() at runtime
        // Ogre packs texture unit indices so we can't use spare indices :'(
        param_named u_colorPrimitiveTexture int 10
        param_named u_colorPrimitiveTextureSize float2 0 0
    }
}
{% endfor %}

//-----------------------------------------------------------------------------
// Common color materials
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, params in configsFP %}

//---------------------------------------------------------------------------

fragment_program MaterialColor_{{ shading }}_FP glsl
{
    source MaterialColor_FP.glsl
    {% if defines %}preprocessor_defines {{ defines }}{% endif %}

}
{% endfor %}

//-----------------------------------------------------------------------------
// Default technique
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, params in configsFP %}

//-----------------------------------------------------------------------------

fragment_program Default_{{ shading }}_FP_glsl glsl
{
    source Default_FP.glsl
    attach MaterialColor_{{ shading }}_FP
    {% if shadersFP %}attach {{ shadersFP }}{% endif %}

    default_params
    {
{% for param in params['defaultFP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}

//-----------------------------------------------------------------------------
// Depth Peeling technique
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, params in configsFP %}

//---------------------------------------------------------------------------

fragment_program DepthPeeling_peel_{{ shading }}_FP_glsl glsl
{
    source DepthPeelingPeel_FP.glsl
    attach DepthPeelingCommon_FP
    attach MaterialColor_{{ shading }}_FP
    {% if shadersFP %}attach {{ shadersFP }}{% endif %}

    default_params
    {
        param_named u_fragData0 int 0
        param_named_auto u_vpWidth viewport_width
        param_named_auto u_vpHeight viewport_height
        param_named_auto u_diffuse surface_diffuse_colour
{% for param in params['depthPeelingFP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}

//-----------------------------------------------------------------------------
// Dual Depth Peeling technique
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, params in configsFP %}

//---------------------------------------------------------------------------

fragment_program DualDepthPeeling_peel_{{ shading }}_FP_glsl glsl
{
    source DualDepthPeelingPeel_FP.glsl
    attach DepthPeelingCommon_FP
    attach MaterialColor_{{ shading }}_FP
    {% if shadersFP %}attach {{ shadersFP }}{% endif %}

    default_params
    {
        param_named u_nearestDepthBuffer int 0
        param_named u_farthestDepthBuffer int 1
        param_named u_forwardColorBuffer int 2
        param_named u_forwardAlphasBuffer int 3
        param_named_auto u_vpWidth viewport_width
        param_named_auto u_vpHeight viewport_height
        param_named_auto u_diffuse surface_diffuse_colour
{% for param in params['dualDepthPeelingFP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}

//-----------------------------------------------------------------------------
// Hybrid Transparency technique
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, params in configsFP %}

//---------------------------------------------------------------------------

fragment_program HybridTransparency_peel_{{ shading }}_FP_glsl glsl
{
    source DepthPeelingPeel_FP.glsl
    attach DepthPeelingCommon_FP
    attach MaterialColor_{{ shading }}_FP
    {% if shadersFP %}attach {{ shadersFP }}{% endif %}

    default_params
    {
        param_named u_fragData0 int 0
        param_named_auto u_vpWidth viewport_width
        param_named_auto u_vpHeight viewport_height
        param_named_auto u_diffuse surface_diffuse_colour
{% for param in params['depthPeelingFP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}


{% for shading, defines, shadersVP, shadersFP, params in configsFP %}

//---------------------------------------------------------------------------

fragment_program HybridTransparency_weight_blend_{{ shading }}_FP_glsl glsl
{
    source WeightedBlended_Weight_Blend_FP.glsl
    attach DepthPeelingCommon_FP
    attach MaterialColor_{{ shading }}_FP
    {% if shadersFP %}attach {{ shadersFP }}{% endif %}

    preprocessor_defines HYBRID=1
    default_params
    {
        param_named u_frontDepthBuffer int 0
        param_named u_occlusionDepthBuffer int 1
        param_named_auto u_vpWidth viewport_width
        param_named_auto u_vpHeight viewport_height
        param_named_auto u_near near_clip_distance
        param_named_auto u_far far_clip_distance
{% for param in params['HT_weight_blendFP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}


//-----------------------------------------------------------------------------
// Weighted Blended technique
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, params in configsFP %}

//---------------------------------------------------------------------------

fragment_program WeightedBlended_weight_blend_{{ shading }}_FP_glsl glsl
{
    source WeightedBlended_Weight_Blend_FP.glsl
    attach MaterialColor_{{ shading }}_FP
    {% if shadersFP %}attach {{ shadersFP }}{% endif %}

    default_params
    {
        param_named u_occlusionDepthBuffer int 0
        param_named_auto u_vpWidth viewport_width
        param_named_auto u_vpHeight viewport_height
        param_named_auto u_near near_clip_distance
        param_named_auto u_far far_clip_distance
{% for param in params['weighted_blendFP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}


//-----------------------------------------------------------------------------
// Cell shading + depth peeling technique
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, params in configsVP %}

//---------------------------------------------------------------------------

vertex_program CelShadingDepthPeelingRenderScene_{{ shading }}_VP_glsl glsl
{
    source RenderScene_VP.glsl
    {% if shadersVP %}attach {{ shadersVP }}{% endif %}

    preprocessor_defines CEL_SHADING=1{% if defines %},{{ defines }}{% endif %}

    default_params
    {
        param_named_auto u_worldViewProj worldviewproj_matrix
        param_named_auto u_world world_matrix
        param_named_auto u_normalMatrix inverse_transpose_world_matrix
{% for param in params['renderSceneVP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}


{% for shading, defines, shadersVP, shadersFP, params in configsFP %}

//-----------------------------------------------------------------------------

fragment_program CelShadingDepthPeeling_peel_{{ shading }}_FP_glsl glsl
{
    source CelShadingDepthPeelingPeel_FP.glsl
    attach DepthPeelingCommon_FP
    attach MaterialColor_{{ shading }}_FP
    {% if shadersFP %}attach {{ shadersFP }}{% endif %}

    default_params
    {
        param_named u_bufferDepth int 0
        param_named_auto u_vpWidth viewport_width
        param_named_auto u_vpHeight viewport_height
        param_named_auto u_diffuse surface_diffuse_colour
{% for param in params['depthPeelingFP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}