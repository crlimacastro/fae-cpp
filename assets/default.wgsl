struct t_global_uniforms {
	camera_world_position: vec3f,
	time: f32,
};

struct t_local_uniforms {
	model: mat4x4f,
	view: mat4x4f,
	projection: mat4x4f,
	tint: vec4f,
};
@group(0) @binding(0) var<uniform> global_uniforms : t_global_uniforms;
@group(0) @binding(1) var<uniform> local_uniforms : t_local_uniforms;

struct vertex_input {
	@builtin(vertex_index) vertex_index: u32,
	@builtin(instance_index) instance_index: u32,
	@location(0) local_position: vec3f,
	@location(1) color: vec4f,
	@location(2) local_normal: vec3f,
	@location(3) uv: vec2f,
};

struct vertex_output {
	@builtin(position) projected_position: vec4f,
	@location(0) world_position: vec3f,
	@location(1) color: vec4f,
	@location(2) world_normal: vec3f,
	@location(3) uv: vec2f,
	@location(4) camera_view_direction: vec3f,
};

@vertex
fn vs_main(in: vertex_input) -> vertex_output {
    let mvp = local_uniforms.projection * local_uniforms.view * local_uniforms.model;
    var out: vertex_output;
    out.projected_position = mvp * vec4f(in.local_position, 1.0);
    out.world_position = (local_uniforms.model * vec4f(in.local_position, 1.0)).xyz;
    out.color = in.color;
    out.world_normal = normalize(local_uniforms.model * vec4(in.local_normal, 0.0)).xyz;
    out.uv = in.uv;
    out.camera_view_direction = normalize(out.world_position - global_uniforms.camera_world_position);
    return out;
}

@group(0) @binding(2) var texture : texture_2d<f32>;
@group(0) @binding(3) var texture_sampler: sampler;


const max_lights: u32 = 512;

struct t_light_info {
	colors: array<vec4f, max_lights>,
	count: u32,
	padding0: u32,
	padding1: u32,
	padding2: u32,
}

struct t_ambient_light_info {
	lights: t_light_info,
}
@group(0) @binding(4) var<uniform> ambient_light_info : t_ambient_light_info;

struct t_directional_light_info {
	directions: array<vec4f, max_lights>,
	lights: t_light_info,
}
@group(0) @binding(5) var<uniform> directional_light_info : t_directional_light_info;

@fragment
fn fs_main(in: vertex_output) -> @location(0) vec4f {
	// let texel_coords = vec2i(in.uv * vec2f(textureDimensions(texture)));
	// let texture_color = textureLoad(texture, texel_coords, 0);
    let texture_color = textureSample(texture, texture_sampler, in.uv);
    let hardness = 1.0;
    let diffuse_scalar = 0.5;
    let specular_scalar = 0.5;

    var color = vec4f(0.0, 0.0, 0.0, 1.0);

    let base_color = local_uniforms.tint * texture_color * in.color;

    for (var i: u32 = 0; i < max_lights; i++) {
        if i >= ambient_light_info.lights.count {
			break;
        }
        let light_color = ambient_light_info.lights.colors[i];
        let scaled_light_color = vec4f(light_color.a * light_color.rgb, 0.0);

        let ambient = scaled_light_color;

        color += scaled_light_color * base_color;
    }

    let V = normalize(in.camera_view_direction);


    for (var i: u32 = 0; i < max_lights; i++) {
        if i >= directional_light_info.lights.count {
			break;
        }
        let light_direction = -normalize(directional_light_info.directions[i].xyz);
        let light_color = directional_light_info.lights.colors[i];
        let scaled_light_color = vec4f(light_color.a * light_color.rgb, 0.0);

        let diffuse = max(0.0, dot(light_direction, normalize(in.world_normal))) * scaled_light_color;

        let reflect_light_direction = reflect(light_direction, in.world_normal);
        let specular = pow(max(0.0, dot(reflect_light_direction, in.camera_view_direction)), hardness);

        color += specular_scalar * specular + diffuse_scalar * diffuse * base_color;
    }

    let gamma_corrected_color = pow(color, vec4f(2.2));

    return gamma_corrected_color;
}