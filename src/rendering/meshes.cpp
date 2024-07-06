#include "fae/rendering/mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace fae
{
    auto meshes::cube(float size) -> mesh
    {
        auto half_size = size / 2;
        return mesh{
            .vertices = std::vector<vertex>{
                // clang-format off
				{ { -half_size, half_size, -half_size }, 		{ 1.f, 1.f, 1.f, 1.f },		{ 1.f, 0.f, 0.f },		{ 0.f, 1.f } }, // 0 left up back
				{ { -half_size, -half_size, -half_size },		{ 1.f, 1.f, 1.f, 1.f },		{ 1.f, 0.f, 0.f },		{ 0.f, 0.f } }, // 1 left down back
				{ { half_size, -half_size, -half_size },		{ 1.f, 1.f, 1.f, 1.f },		{ 1.f, 0.f, 0.f },		{ 1.f, 0.f } }, // 2 right down back
				{ { half_size, half_size, -half_size },		    { 1.f, 1.f, 1.f, 1.f },		{ 1.f, 0.f, 0.f },		{ 1.f, 1.f } }, // 3 right up back
				{ { -half_size, half_size, half_size }, 		{ 1.f, 1.f, 1.f, 1.f },		{ 1.f, 0.f, 0.f },		{ 0.f, 1.f } }, // 4 left up front
				{ { -half_size, -half_size, half_size },		{ 1.f, 1.f, 1.f, 1.f },		{ 1.f, 0.f, 0.f },		{ 0.f, 0.f } }, // 5 left down front
				{ { half_size, -half_size, half_size },		    { 1.f, 1.f, 1.f, 1.f },		{ 1.f, 0.f, 0.f },		{ 1.f, 0.f } }, // 6 right down front
				{ { half_size, half_size, half_size },	        { 1.f, 1.f, 1.f, 1.f },		{ 1.f, 0.f, 0.f },		{ 1.f, 1.f } }, // 7 right up front
                // clang-format on
            },
            .indices = std::vector<std::size_t>{
                // clang-format off
                0, 1, 2,
				0, 2, 3,
				3, 2, 6,
				3, 6, 7,
				7, 6, 5,
				7, 5, 4,
				4, 5, 1,
				4, 1, 0,
				0, 3, 7,
				0, 7, 4,
				1, 5, 6,
				1, 6, 2,
                // clang-format on
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
