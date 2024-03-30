#pragma once
#include <cstdio>
#include <raylib.h>
#include <cmath>
#include <vector>
#include <random>

extern int screen_width, screen_height;

struct Function {

    enum METHOD { LEFT, RIGHT, MIDDLE, RANDOM };

    virtual double evaluate_at(double x) = 0;
    virtual const char *name() = 0;

    // integrate with bound [l, r] and n partitions
    void integrate(double l, double r, int n, METHOD method) {

        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dist(0, 1);

        if (l == this->l &&
            r == this->r &&
            n == this->n &&
            method == this->method) return;

        this->l = l;
        this->r = r;
        this->n = n;
        this->method = method;

        value = 0;
        integration_ys.resize(n);

        for (int i = 0; i < n; i++) {
            double x; // left (0..n-1)
            switch (method) {
                case LEFT:
                    x = l + (r - l) * i / n;
                    break;
                case RIGHT:
                    x = l + (r - l) * (i + 1) / n;
                    break;
                case MIDDLE:
                    x = l + (r - l) * (i + 0.5) / n;
                    break;
                case RANDOM:
                    x = l + (r - l) * (i + dist(gen)) / n;
                    break;
            }
            integration_ys[i] = evaluate_at(x);
            value += integration_ys[i] * (r - l) / n;
        }
        printf("integral from %f to %f is %f\n", l, r, value);
    }

    void plot(Color color, Vector2 offset, Vector2 scale) {

        double prev_x = l;
        for (int i = 0; i < n; i++) {
            double cur_x = l + (r - l) * (i + 1) / n;
            double screen_x_prev = prev_x * scale.x + offset.x;
            double screen_x_cur = cur_x * scale.x + offset.x;
            double y = integration_ys[i];
            if (y > 0) {
                DrawRectangle(screen_x_prev, screen_height - (y * scale.y) + offset.y, screen_x_cur - screen_x_prev, y * scale.y, LIGHTGRAY);
            } else {
                DrawRectangle(screen_x_prev, screen_height + offset.y, screen_x_cur - screen_x_prev, (-y) * scale.y, LIGHTGRAY);
            }
            prev_x = cur_x;
        }

        // axes
        DrawLine(0 + offset.x, screen_height + offset.y, screen_width, screen_height + offset.y, GRAY);
        DrawLine(0 + offset.x, screen_height + offset.y, 0, screen_height + offset.y, GRAY);

        DrawLine(0 + offset.x, screen_height + offset.y, 0 + offset.x, screen_height, GRAY);
        DrawLine(0 + offset.x, screen_height + offset.y, 0 + offset.x, 0, GRAY);

        for (int y = -9; y <= 9; y++) {
            DrawLine(-10 + offset.x, screen_height + offset.y - y * scale.y, +10 + offset.x, screen_height + offset.y - y * scale.y, GRAY);
            DrawText(TextFormat("%d", y), -45 + offset.x, screen_height + offset.y - y * scale.y - 15, 30, GRAY);
        }

        for (int x = -9; x <= 9; x++) {
            DrawLine(x * scale.x + offset.x, screen_height + offset.y + 10, x * scale.x + offset.x, screen_height + offset.y - 10, GRAY);
            DrawText(TextFormat("%d", x), x * scale.x + offset.x - 15, screen_height + offset.y + 15, 30, GRAY);
        }

        double prev_y = evaluate_at(0);
        for (int x = 1; x < screen_width; ++x) {
            double true_x = (x - offset.x) / scale.x;
            double cur = evaluate_at(true_x) * scale.y - offset.y;
            if (0 < cur && cur < screen_height || 0 < prev_y && prev_y < screen_height) {
                DrawLine(x - 1, screen_height - prev_y, x, screen_height - cur, color);
            }
            prev_y = cur;
        }

        DrawCircle(offset.x, screen_height + offset.y, 5.0f, RED);
    }

    void draw_description(int x, int y, int font_size, Color color) {
        DrawText(name(), x, y, font_size, color);
        if (l != r) {
            DrawText(TextFormat("[%f, %f]", l, r), 7 * font_size, y, font_size, color);
            DrawText(TextFormat("%f", value), 20 * font_size, y, font_size, color);

            switch(method) {
                case LEFT:
                    DrawText("Integration method: left", 30 * font_size, y, font_size, color);
                    break;
                case RIGHT:
                    DrawText("Integration method: right", 30 * font_size, y, font_size, color);
                    break;
                case MIDDLE:
                    DrawText("Integration method: middle", 30 * font_size, y, font_size, color);
                    break;
                case RANDOM:
                    DrawText("Integration method: random", 30 * font_size, y, font_size, color);
                    break;
            }

        }
    }

    double l = 0, r = 0, n = 0, value; // integral
    METHOD method = LEFT;
    std::vector<double> integration_ys;
};

struct Square : Function {
    double evaluate_at(double x) override {
        return std::pow(x, 2);
    }
    const char *name() override {
        return "y = x^2";
    }
};

struct Cube : Function {
    double evaluate_at(double x) override {
        return std::pow(x, 3);
    }
    const char *name() override {
        return "y = x^3";
    }
};

struct Exponent : Function {
    double evaluate_at(double x) override {
        return std::exp(-x);
    }
    const char *name() override {
        return "y = e^(-x)";
    }
};

struct Sin2 : Function {
    double evaluate_at(double x) override {
        return std::sin(2.0*x);
    }
    const char *name() override {
        return "y = sin(2x)";
    }
};
