#include "fae/rendering/mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace fae
{
    auto meshes::cube(float size) -> mesh
    {
        static auto static_mesh = *mesh::load(FAE_ASSET_DIR / std::filesystem::path("cube.obj"));
        auto mesh = static_mesh;
        for (auto& vertex : mesh.vertices)
        {
            vertex.position *= size / 2;
        }
        return mesh;
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
        std::size_t mesh_idx = 0;
        auto mesh = scene->mMeshes[mesh_idx];

        for (std::size_t v = 0; v < mesh->mNumVertices; v++)
        {
            auto vertex = fae::vertex{};
            vertex.position = { mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z };
            if (mesh->HasVertexColors(mesh_idx))
            {
                auto color = mesh->mColors[v];
                vertex.color = { color->r, color->g, color->b, color->a };
            }
            if (mesh->HasNormals())
            {
                auto normal = mesh->mNormals[v];
                vertex.normal = { normal.x, normal.y, normal.z };
            }
            if (mesh->HasTextureCoords(mesh_idx))
            {
                auto uv = mesh->mTextureCoords[mesh_idx][v];
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
