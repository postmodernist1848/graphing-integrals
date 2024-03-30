#include <cmath>
#include <cstdio>
#include <raylib.h>
#include "functions.hpp"
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_CUSTOM_ICONS
#include "ricons.h"
#include "raygui.h"

#define xstr(s) #s
#define str(s) xstr(s)

#define MIN_SCALE_X 0.5f
#define MIN_SCALE_Y 0.5f
#define MAX_SCALE_X 400.0f
#define MAX_SCALE_Y 400.0f

int interface_width = 250;
int screen_width = 1680 - interface_width;
int screen_height = 1020;

void draw_dash_dotted_line(float start_x, float start_y, float end_x, float end_y, float dash_len, Color color) {
    const float len_x = end_x - start_x;
    const float len_y = end_y - start_y;
    const float step = dash_len / std::sqrt(std::pow(len_x, 2) + std::pow(len_y, 2));
    const float step_x = step * len_x;
    const float step_y = step * len_y;
    float cur_x = start_x,
          cur_y = start_y;

    // compare independently of direction
    while (len_x * (cur_x - end_x) < 0 || len_y * (cur_y - end_y) < 0) {
        float next_x = cur_x + step_x;
        if (len_x * (next_x - end_x) > 0) {
            next_x = end_x;
        }
        float next_y = cur_y + step_y;
        if (len_y * (next_y - end_y) > 0) {
            next_y = end_y;
        }
        DrawLine(cur_x, cur_y, next_x, next_y, color);
        cur_x += 2 * step_x;
        cur_y += 2 * step_y;
    }
}

void display (Function *f, Vector2 offset, Vector2 scale) {
    f->plot(GRAY, offset, scale);
    f->draw_description(30, 30, 30, BLACK);
}

// 1)  f(x) = x^2     [1,   2]
// 6)  f(x) = x^3     [0,   1]
// 8)  f(x) = e^(-x)  [0,   1]
// 9)  f(x) = x^2     [-3,  0]
// 27) f(x) = sin(2x) [0, pi/2]

void reset(Vector2 &scale) {
    scale.x = 200.0f;
    scale.y = 200.0f;
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width + interface_width, screen_height, "Integral");
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0xf5f5f5ff); 
    SetTargetFPS(60);

    Vector2 cam_offset = {500.0f, -50.0f};
    Vector2 scale;
    reset(scale);
    Square s;
    Cube c;
    Exponent e;
    Sin2 s2;
    Function::METHOD integration_method = Function::LEFT;

    Function *current_function = &s;

    float l = 0.0f, r = 2.0f;

    while (!WindowShouldClose()) {

        if (IsWindowResized()) {
            screen_height = GetScreenHeight();
            screen_width = GetScreenWidth() - interface_width;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && GetMouseX() < screen_width) {
            Vector2 d = GetMouseDelta();
            cam_offset.x += d.x;
            cam_offset.y += d.y;
        }

        Vector2 mw = GetMouseWheelMoveV();
        if (IsKeyDown(KEY_LEFT_CONTROL)) {

            if (mw.y > 0) {
                if (IsKeyDown(KEY_LEFT_SHIFT)) scale.y *= 1.1f;
                else scale.x *= 1.1f;
            } else if (mw.y < 0) {
                if (IsKeyDown(KEY_LEFT_SHIFT)) scale.y *= 0.9f;
                else scale.x *= 0.9f;
            }

        } else {
            cam_offset.x += mw.x * 10.0f;
            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                cam_offset.x -= mw.y * 10.0f;
            } else {
                cam_offset.y += mw.y * 10.0f;
            }
        }


        if (IsKeyPressed(KEY_ONE)) integration_method = Function::LEFT;
        if (IsKeyPressed(KEY_TWO)) integration_method = Function::RIGHT;
        if (IsKeyPressed(KEY_THREE)) integration_method = Function::MIDDLE;
        if (IsKeyPressed(KEY_FOUR)) integration_method = Function::RANDOM;

        current_function->integrate(l, r, 15, integration_method);

        BeginDrawing();
            ClearBackground(RAYWHITE);

            display(current_function, cam_offset, scale);

            // GUI
            DrawRectangle(screen_width, 0, interface_width, screen_height, LIGHTGRAY);
            //
            const Rectangle scale_x_slider {(float)(screen_width + interface_width / 10), 60, (float)interface_width * 8 / 10, 40};
            DrawText("Scale x", screen_width + interface_width / 10, 40, 20, GRAY);
            GuiSliderBar(scale_x_slider, str(MIN_SCALE_Y), str(MAX_SCALE_X), &scale.x, MIN_SCALE_X, MAX_SCALE_X);

            const Rectangle scale_y_slider {(float)(screen_width + interface_width / 10), 160, (float)interface_width * 8 / 10, 40};
            DrawText("Scale y", screen_width + interface_width / 10, 140, 20, GRAY);
            GuiSliderBar(scale_y_slider, str(MIN_SCALE_Y), str(MAX_SCALE_Y), &scale.y, MIN_SCALE_Y, MAX_SCALE_Y);

            const Rectangle integration_left_bound {(float)(screen_width + interface_width / 10), 250, (float)interface_width * 8 / 10, 40};
            DrawText("Left bound", screen_width + interface_width / 10, 230, 20, GRAY);
            GuiSliderBar(integration_left_bound, str(-10.0), str(10.0), &l, -10.0f, 10.0f);

            const Rectangle integration_right_bound {(float)(screen_width + interface_width / 10), 310, (float)interface_width * 8 / 10, 40};
            DrawText("Right bound", screen_width + interface_width / 10, 290, 20, GRAY);
            GuiSliderBar(integration_right_bound, str(-10.0), str(10.0), &r, -10.0f, 10.0f);

            const Rectangle reset_button {screen_width + (float)interface_width / 5, (float)screen_height - 100, (float)interface_width * 3 / 5, 80};
            if (GuiButton(reset_button, "Reset")) {
                reset(scale);
            }
            
            const float bt_size = (float)interface_width / 4;
            const float l_x = screen_width + bt_size;
            const float r_x = screen_width + bt_size * 2;

            const float t_y = screen_height - 150 - bt_size * 2;
            const float b_y = t_y + bt_size;

            const Rectangle sf_button {l_x, t_y, bt_size, bt_size};
            if (GuiButton(sf_button, GuiIconText(ICON_WAVE_SINUS, NULL))) {
                current_function = &s2;
                l = 0.0;
                r = M_PI / 2.0;
            }
            const Rectangle qp_button {r_x, t_y, bt_size, bt_size};
            if (GuiButton(qp_button, GuiIconText(ICON_QUADRATIC, NULL))) current_function = &s;
            const Rectangle pp_button {l_x, b_y, bt_size, bt_size};
            if (GuiButton(pp_button, GuiIconText(ICON_POWER, NULL))) current_function = &c;
            const Rectangle ep_button {r_x, b_y, bt_size, bt_size};
            if (GuiButton(ep_button, GuiIconText(ICON_EXPONENTIAL, NULL))) current_function = &e;

        EndDrawing();
    }

    return 0;
}
