#pragma once

#include "fae/entity.hpp"

namespace fae
{
    struct camera
    {
        float fov = 45.f;
        float aspect = 1920.f / 1080.f;
        float near_plane = 0.1f;
        float far_plane = 1000.f;
    };

    struct primary_camera
    {
        fae::entity camera_entity;

        [[nodiscard]] auto camera() noexcept -> fae::camera&
        {
            return *camera_entity.get_component<fae::camera>();
        }
    };
}
