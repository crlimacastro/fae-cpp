#include "fae/editor.hpp"

#include <string>

#include "fae/application.hpp"
#include "fae/ui.hpp"
#include "fae/logging.hpp"

namespace fae
{
    auto editor_plugin::init(application& app) const noexcept -> void
    {
        app
            .insert_resource(editor{
                .selected_entity = entt::null,
            })
            .add_system<ui_render_step>(ui_render_editor)
            .add_system<editor_render_step>(ui_render_entity_scene)
            .add_system<editor_render_step>(ui_render_entity_inspector);
    }

    auto ui_render_editor(const ui_render_step& step) noexcept -> void
    {
        fae::ui::DockSpaceOverViewport(0U, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
        step.scheduler.invoke(editor_render_step{
            .resources = step.resources,
            .assets = step.assets,
            .scheduler = step.scheduler,
            .ecs_world = step.ecs_world,
        });
    }

    auto ui_render_entity_scene(const editor_render_step& step) noexcept -> void
    {
        fae::ui::Begin("Scene");
        for (auto& entity : step.ecs_world.entities())
        {
            auto name = std::to_string(static_cast<std::uint32_t>(entity.id));
            entity.use_component<fae::name>([&](fae::name& name_component)
                { name = name_component.value; });
            if (name == "")
            {
                name = "##";
            }
            if (fae::ui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen))
            {
                if (fae::ui::IsItemClicked())
                {
                    step.resources.use_resource<editor>([&](editor& editor)
                        { editor.selected_entity = entity.id; });
                }
            }
        }
        fae::ui::End();
    }

    auto ui_render_entity_inspector(const editor_render_step& step) noexcept -> void
    {
        step.resources.use_resource<editor>([&](editor& editor)
            {
                    fae::ui::Begin("Inspector");
                    static entt::entity prev_selected_entity = entt::null;
                if (editor.selected_entity != entt::null)
                {
                    auto entity = fae::entity{
                        .id = editor.selected_entity,
                        .registry = &step.ecs_world.registry,
                    };
                    auto final_name = std::to_string(static_cast<std::uint32_t>(entity.id));
                    entity.use_component<fae::name>([&](fae::name& name)
                        {
                            final_name = name.value;
                        });
                    if (fae::ui::InputText("##", &final_name))
                    {
                        if (prev_selected_entity == editor.selected_entity)
                        {
                        entity.set_component<fae::name>(fae::name{ .value = final_name });
                        }
                    }
                    prev_selected_entity = editor.selected_entity;
                    
                }
                fae::ui::End(); });
    }
}
