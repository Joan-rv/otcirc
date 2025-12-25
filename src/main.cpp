#include <iostream>
#include <memory>

#include <raylib.h>
#include <raymath.h>

#define SIZE 800

class Circle {
  public:
    Circle() : left(nullptr), right(nullptr), degree(1) {}
    void balance();
    void subdivide_at(Vector2 position, float radius, Vector2 at);
    void draw(Vector2 position, float radius) const;

  private:
    void compute_degree_();
    std::unique_ptr<Circle> left, right;
    int degree;
};

void Circle::compute_degree_() {
    if (!left || !right)
        degree = 1;
    else
        degree = left->degree + right->degree;
}

void Circle::balance() {
    if (!left || !right)
        return;
    left->balance();
    right->balance();
    if (left->degree - right->degree > 1) {
        auto &largest =
            left->left->degree > left->right->degree ? left->left : left->right;
        auto &smallest = !right->left || !right->right ||
                                 right->left->degree >= right->right->degree
                             ? right->right
                             : right->left;
        if (smallest && smallest->degree + 1 == largest->degree) {
            swap(largest->left, smallest->left);
            swap(largest->right, smallest->right);
        } else {
            swap(largest->left, right->left);
            swap(largest->right, right->right);
        }
        if (smallest)
            smallest->compute_degree_();
        largest->compute_degree_();
        left->compute_degree_();
        right->compute_degree_();
    } else if (right->degree - left->degree > 1) {
        auto &largest = right->left->degree > right->right->degree
                            ? right->left
                            : right->right;
        auto &smallest = !left->left || !left->right ||
                                 left->left->degree >= left->right->degree
                             ? left->right
                             : left->left;
        if (smallest && smallest->degree + 1 == largest->degree) {
            swap(largest->left, smallest->left);
            swap(largest->right, smallest->right);
        } else {
            swap(largest->left, left->left);
            swap(largest->right, left->right);
        }
        if (smallest)
            smallest->compute_degree_();
        largest->compute_degree_();
        left->compute_degree_();
        right->compute_degree_();
    }
}

void Circle::subdivide_at(Vector2 position, float radius, Vector2 at) {
    auto left_pos = Vector2{position.x - radius / 2, position.y};
    auto right_pos = Vector2{position.x + radius / 2, position.y};
    if (left && Vector2DistanceSqr(left_pos, at) < radius / 2 * radius / 2) {
        left->subdivide_at(left_pos, radius / 2, at);
        degree = left->degree + right->degree;
    } else if (right &&
               Vector2DistanceSqr(right_pos, at) < radius / 2 * radius / 2) {
        right->subdivide_at(right_pos, radius / 2, at);
        degree = left->degree + right->degree;
    } else {
        if (left || right) {
            std::cerr << "Cannot subdivide already divided circle\n";
        } else {
            left = std::make_unique<Circle>();
            right = std::make_unique<Circle>();
            degree = 2;
        }
    }
}

void Circle::draw(Vector2 position, float radius) const {
    DrawCircleLinesV(position, radius, BLACK);
    if (!left || !right) {
        DrawTextEx(GetFontDefault(), "1", position,
                   radius > 32 ? 16 : radius / 2, 1.0f, BLACK);
        return;
    }
    auto left_pos = Vector2{position.x - radius / 2, position.y};
    left->draw(left_pos, radius / 2);
    auto right_pos = Vector2{position.x + radius / 2, position.y};
    right->draw(right_pos, radius / 2);
    auto text_pos = Vector2{position.x, position.y + radius / 2};
    DrawTextEx(GetFontDefault(), std::to_string(degree).c_str(), text_pos,
               radius > 32 ? 16 : radius / 2, 1.0f, BLACK);
}

int main() {
    InitWindow(SIZE, SIZE, "otcirc");
    std::unique_ptr<Circle> circle = nullptr;
    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            auto mouse = GetMousePosition();
            if (!circle)
                circle = std::make_unique<Circle>();
            else
                circle->subdivide_at(Vector2{SIZE / 2.0f, SIZE / 2.0f},
                                     SIZE / 2.0f - 50, mouse);
        }
        if (IsKeyPressed(KEY_SPACE)) {
            if (circle)
                circle->balance();
        }
        ClearBackground(WHITE);
        BeginDrawing();
        if (circle)
            circle->draw(Vector2{SIZE / 2.0f, SIZE / 2.0f}, SIZE / 2.0f - 50);
        EndDrawing();
    }
}
