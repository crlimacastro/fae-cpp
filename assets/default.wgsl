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
	@location(0) position: vec3f,
	@location(1) color: vec4f,
	@location(2) normal: vec3f,
	@location(3) uv: vec2f,
};

struct vertex_output {
	@builtin(position) position: vec4f,
	@location(0) color: vec4f,
	@location(1) normal: vec3f,
	@location(2) uv: vec2f,
};

@vertex
fn vs_main(in: vertex_input) -> vertex_output {
	let mvp = uniforms.projection * uniforms.view * uniforms.model;
	var out: vertex_output;
	out.position = mvp * vec4f(in.position, 1.0);
	out.color = in.color;
	out.normal = in.normal;
	out.uv = in.uv;
    return out;
}

@group(0) @binding(1) var texture : texture_2d<f32>;
@group(0) @binding(2) var texture_sampler: sampler;

@fragment
fn fs_main(in: vertex_output) -> @location(0) vec4f {
	let lightColor1 = vec3f(1.0, 0.9, 0.6);
	let lightColor2 = vec3f(0.6, 0.9, 1.0);
	let lightDirection1 = vec3f(0.5, -0.9, 0.1);
	let lightDirection2 = vec3f(0.2, 0.4, 0.3);
	let normal = in.normal;
	let shading1 = max(0.0, dot(lightDirection1, normal));
	let shading2 = max(0.0, dot(lightDirection2, normal));
	let shading = shading1 * lightColor1 + shading2 * lightColor2;
	
	let texel_coords = vec2i(in.uv * vec2f(textureDimensions(texture)));
	// let texture_color = textureLoad(texture, texel_coords, 0);
	let texture_color = textureSample(texture, texture_sampler, in.uv);

	let color = uniforms.tint * texture_color * in.color;
	// let color = vec4f(shading, 1.0) * uniforms.tint * in.color;
	// let color = vec4f(shading, 1.0) * uniforms.tint * texture_color * in.color;
	let gamma_corrected_color = pow(color, vec4f(2.2));

	return gamma_corrected_color;
}