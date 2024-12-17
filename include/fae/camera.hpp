#pragma once

#include "fae/entity.hpp"

namespace fae
{
    struct camera
    {
        float fov = 45.f;
        float near_plane = 0.1f;
        float far_plane = 1000.f;
    };

    struct active_camera
    {
        fae::entity camera_entity;
    };
}
