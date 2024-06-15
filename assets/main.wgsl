struct t_uniforms
{
	model : mat4x4f,
	view : mat4x4f,
	projection : mat4x4f,
	tint: vec4f,
	time: f32,
	padding0: f32,
	padding1: f32,
	padding2: f32,
};
@group(0) @binding(0) var<uniform> uniforms : t_uniforms;

struct vertex_input {
	@builtin(vertex_index) vertex_index: u32,
	@builtin(instance_index) instance_index: u32,
	@location(0) position: vec4f,
	@location(1) color: vec4f,
	@location(2) normal: vec4f,
	@location(3) uv: vec2f,
};

struct vertex_output {
	@builtin(position) position: vec4f,
	@location(0) color: vec4f,
	@location(1) uv: vec2f,
};

@vertex
fn vs_main(in: vertex_input) -> vertex_output {
	var model_view_projection_matrix = uniforms.projection * uniforms.view * uniforms.model;
	var out: vertex_output;
	out.position = model_view_projection_matrix * in.position;
	out.color = uniforms.tint * in.color;
	out.uv = in.uv;
    return out;
}

@fragment
fn fs_main(in: vertex_output) -> @location(0) vec4f {
	return in.color;
}