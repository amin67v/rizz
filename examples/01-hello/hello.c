#include "sx/allocator.h"
#include "sx/math.h"
#include "sx/string.h"
#include "sx/timer.h"

#include "rizz/app.h"
#include "rizz/asset.h"
#include "rizz/camera.h"
#include "rizz/core.h"
#include "rizz/entry.h"
#include "rizz/graphics.h"
#include "rizz/imgui-extra.h"
#include "rizz/imgui.h"
#include "rizz/input.h"
#include "rizz/plugin.h"
#include "rizz/vfs.h"

RIZZ_STATE static rizz_api_core* the_core;
RIZZ_STATE static rizz_api_gfx* the_gfx;
RIZZ_STATE static rizz_api_app* the_app;
RIZZ_STATE static rizz_api_imgui* the_imgui;
RIZZ_STATE static rizz_api_asset* the_asset;
RIZZ_STATE static rizz_api_imgui_extra* the_imguix;
RIZZ_STATE static rizz_api_camera* the_camera;
RIZZ_STATE static rizz_api_vfs* the_vfs;

RIZZ_STATE static rizz_api_input* the_input;

RIZZ_STATE rizz_gfx_stage g_stage;

RIZZ_STATE rizz_input_device g_pad0;

static bool init()
{
    // register main graphics stage.
    // at least one stage should be registered if you want to draw anything
    g_stage = the_gfx->stage_register("main", (rizz_gfx_stage){ .id = 0 });
    sx_assert(g_stage.id);

    //the_input->create_device(RIZZ_INPUT_DEVICETYPE_MOUSE);
    rizz_input_device keyboard = the_input->create_device(RIZZ_INPUT_DEVICETYPE_KEYBOARD);
    the_input->create_device(RIZZ_INPUT_DEVICETYPE_PAD);
    g_pad0 = the_input->create_device(RIZZ_INPUT_DEVICETYPE_PAD);
    // the_input->map_bool(g_pad0, RIZZ_INPUT_PADBUTTON_A, 666);
    // the_input->map_float(g_pad0, RIZZ_INPUT_PADBUTTON_LEFTSTICKX, 667, 0, 1.0f, NULL, NULL);
    the_input->map_bool(keyboard, RIZZ_INPUT_KBKEY_SPACE, 666);
    return true;
}

static void shutdown() {}

static void update(float dt)
{
    static bool debugger = true;
    if (debugger)
        the_input->show_debugger(&debugger);
}

static void render()
{
    sg_pass_action pass_action = { .colors[0] = { SG_ACTION_CLEAR, { 0.25f, 0.5f, 0.75f, 1.0f } },
                                   .depth = { SG_ACTION_CLEAR, 1.0f } };

    the_gfx->staged.begin(g_stage);
    the_gfx->staged.begin_default_pass(&pass_action, the_app->width(), the_app->height());
    the_gfx->staged.end_pass();
    the_gfx->staged.end();

    // Use imgui UI
    the_imgui->SetNextWindowContentSize(sx_vec2f(100.0f, 50.0f));
    if (the_imgui->Begin("Hello", NULL, 0)) {
        the_imgui->LabelText("Fps", "%.3f", the_core->fps());

        // bool dev_avail = the_input->device_avail(g_pad0);
        // the_imgui->Checkbox("avail?", &dev_avail);
        bool button = the_input->get_bool(666);
        the_imgui->Checkbox("A", &button);
        // float analogx = the_input->get_float(667);
        // the_imgui->SliderFloat("AnalogueX", &analogx, -1.0f, 1.0f, "%.2f", 1.0f);
    }
    the_imgui->End();
}

rizz_plugin_decl_main(hello, plugin, e)
{
    switch (e) {
    case RIZZ_PLUGIN_EVENT_STEP:
        update((float)sx_tm_sec(the_core->delta_tick()));
        render();
        break;

    case RIZZ_PLUGIN_EVENT_INIT:
        // runs only once for application. Retreive needed APIs
        the_core = plugin->api->get_api(RIZZ_API_CORE, 0);
        the_gfx = plugin->api->get_api(RIZZ_API_GFX, 0);
        the_app = plugin->api->get_api(RIZZ_API_APP, 0);
        the_imgui = plugin->api->get_api_byname("imgui", 0);
        the_input = plugin->api->get_api_byname("input", 0);

        init();
        break;

    case RIZZ_PLUGIN_EVENT_LOAD:
        break;

    case RIZZ_PLUGIN_EVENT_UNLOAD:
        break;

    case RIZZ_PLUGIN_EVENT_SHUTDOWN:
        shutdown();
        break;
    }

    return 0;
}

rizz_plugin_decl_event_handler(hello, e)
{
    switch (e->type) {
    case RIZZ_APP_EVENTTYPE_SUSPENDED:
        break;
    case RIZZ_APP_EVENTTYPE_RESTORED:
        break;
    case RIZZ_APP_EVENTTYPE_MOUSE_DOWN:
        break;
    case RIZZ_APP_EVENTTYPE_MOUSE_UP:
        break;
    case RIZZ_APP_EVENTTYPE_MOUSE_MOVE:
        break;
    default:
        break;
    }
}

rizz_game_decl_config(conf)
{
    conf->app_name = "hello";
    conf->app_version = 1000;
    conf->app_title = "01 - Hello";
    conf->window_width = 800;
    conf->window_height = 600;
    conf->core_flags |= RIZZ_CORE_FLAG_VERBOSE;
    conf->multisample_count = 4;
    conf->swap_interval = 2;
    conf->plugins[0] = "imgui";
    conf->plugins[1] = "input";
}
