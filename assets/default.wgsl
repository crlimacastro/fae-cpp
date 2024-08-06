struct t_uniforms {
	model: mat4x4f,
	view: mat4x4f,
	projection: mat4x4f,
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


const MAX_LIGHTS: u32 = 32;

struct ambient_lighting_info {
	colors: array<vec4f, MAX_LIGHTS>,
}
@group(0) @binding(3) var<uniform> ambient_light_info : ambient_lighting_info;

struct directional_lighting_info {
	directions: array<vec4f, MAX_LIGHTS>,
	colors: array<vec4f, MAX_LIGHTS>,
}
@group(0) @binding(4) var<uniform> directional_light_info : directional_lighting_info;

@fragment
fn fs_main(in: vertex_output) -> @location(0) vec4f {
    var shading = vec4f(0.0, 0.0, 0.0, 1.0);

    for (var i: u32 = 0; i < MAX_LIGHTS; i++) {
        let color = ambient_light_info.colors[i];
		let scaled_color = vec4f(color.a * color.rgb, 0.0);
        shading += scaled_color;
    }

    for (var i: u32 = 0; i < MAX_LIGHTS; i++) {
        let direction = normalize(directional_light_info.directions[i].xyz);
        let color = directional_light_info.colors[i];
		let scaled_color = vec4f(color.a * color.rgb, 0.0);
		let world_normal = (uniforms.model * vec4f(in.normal, 0.0)).xyz;
        shading += max(0.0, dot(direction, world_normal)) * scaled_color;
    }

	// let texel_coords = vec2i(in.uv * vec2f(textureDimensions(texture)));
	// let texture_color = textureLoad(texture, texel_coords, 0);

    let texture_color = textureSample(texture, texture_sampler, in.uv);

    let color = shading * uniforms.tint * texture_color * in.color;
    let gamma_corrected_color = pow(color, vec4f(2.2));

    return gamma_corrected_color;
}