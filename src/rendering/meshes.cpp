#include "fae/rendering/meshes.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace fae
{
    auto cube(float size = 1.f) -> mesh
    {
        return mesh{
            .vertices = std::vector<vertex>{
                vertex{ .position = vec3{ -size, -size, -size } },
                vertex{ .position = vec3{ size, -size, -size } },
                vertex{ .position = vec3{ size, size, -size } },
                vertex{ .position = vec3{ -size, size, -size } },
                vertex{ .position = vec3{ -size, -size, size } },
                vertex{ .position = vec3{ size, -size, size } },
                vertex{ .position = vec3{ size, size, size } },
                vertex{ .position = vec3{ -size, size, size } },
            },
        };
    }

    auto mesh::load(std::filesystem::path path) -> std::optional<mesh>
    {
        auto importer = Assimp::Importer{};
        const auto scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
        if (!scene)
        {
            return std::nullopt;
        }

        auto result = mesh{};

        // TODO support multi mesh loading from one file
        auto mesh = scene->mMeshes[0];

        for (std::size_t v = 0; v < mesh->mNumVertices; v++)
        {
            auto vertex = fae::vertex{};
            vertex.position = { mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z };
            if (mesh->HasVertexColors(v))
            {
                auto color = mesh->mColors[v];
                vertex.color = { color->r, color->g, color->b, color->a };
            }
            if (mesh->HasNormals())
            {
                auto normal = mesh->mNormals[v];
                vertex.normal = { normal.x, normal.y, normal.z };
            }
            if (mesh->HasTextureCoords(v))
            {
                auto uv = mesh->mTextureCoords[v][0];
                vertex.uv = { uv.x, uv.y };
            }
            result.vertices.push_back(vertex);
        }

        if (mesh->HasFaces())
        {
            for (std::size_t f = 0; f < mesh->mNumFaces; f++)
            {
                auto face = mesh->mFaces[f];
                for (std::size_t i = 0; i < face.mNumIndices; i++)
                {
                    result.indices.push_back(face.mIndices[i]);
                }
            }
        }

        return result;
    }
}
