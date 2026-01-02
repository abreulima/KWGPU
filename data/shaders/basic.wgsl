struct Uniforms
{
    mvp: mat4x4<f32>,
};

@group(0) @binding(0)
var<uniform> uniforms: Uniforms;

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
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f
{
    let n = normalize(in.normal);
	return vec4f(abs(n), 1.0);
}
