struct Uniforms
{
    mvp: mat4x4<f32>,
};

@group(0) @binding(0)
var<uniform> uniforms: Uniforms;

@group(0) @binding(1)
var texture: texture_2d<f32>;

@group(0) @binding(2)
var texture_sampler: sampler;

struct VertexInput
{
	@location(0) position: vec3f,
	@location(1) text_coord: vec2f,
	@location(2) normal: vec3f,
};

struct VertexOutput
{
	@builtin(position) position: vec4f,
	@location(0) text_coord: vec2f,
	@location(1) normal: vec3f,
};

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;

	out.position = uniforms.mvp * vec4f(in.position, 1.0);
	out.normal = in.normal;
	out.text_coord = in.text_coord;
    //out.normal = (viewMatrix * modelMatrix * vec4f(input.normal, 0.0)).xyz;


	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f
{
    let n = normalize(in.normal);
    let color = textureSample(texture, texture_sampler, in.text_coord).rgb;

	return vec4f(color, 1.0);
}
