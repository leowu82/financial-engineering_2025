#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

float calc_y(const vector<float>& cn, float r, float x) {
    float y = x;
    for (size_t t = 0; t < cn.size(); ++t) {
        y += cn[t] / pow(1 + r, t + 1);
    }
    return y;
}

void print_graph(const vector<float>& cn, float low, float high, float initVal, float interval) {
    // Print y val for custom interval
    bool sign_change = false;
    float y_prev = calc_y(cn, low, initVal);
    for (float i = low; i <= high; i += interval) {
        float y = calc_y(cn, i, initVal);
        if (y_prev * y < 0) sign_change = true;
        cout<< "f(" << i << "): " << y << '\n';
    }

    // If no sign change in y values, no root
    if (!sign_change) {
        cout<< "No sign change in y values -> no root!\n";
    }
}

float bisection_method(const vector<float>& cn, float low, float high, float initVal) {
    float mid = 0.0;
    while (abs(high - low) > 0.0001) {
        mid = (low + high) / 2;
        float mid_y = calc_y(cn, mid, initVal);
    
        if (mid_y == 0) {
            break;
        } else if (calc_y(cn, low, initVal) * mid_y < 0) { // Opposite sign
            high = mid;
        } else if (calc_y(cn, high, initVal) * mid_y < 0) { // Opposite sign
            low = mid;
        } else {
            cout<< "Low/high not optimally set!\n";
            break;
        }
    }
    
    return mid;
}

float newton_method(const vector<float>& cn, float initVal, float x) {
    // Xn+1 = Xn - f(Xn) / f'(Xn)
    float y = calc_y(cn, x, initVal);
    float y_prime = 0.0;
    while (abs(y) > 0.0001) {
        y_prime = 0.0;
        for (int t = 0; t < cn.size(); t++) {
            y_prime -= (t + 1) * cn[t] / pow(1 + x, t + 2);
        }
        x -= y / y_prime;
        y = calc_y(cn, x, initVal);
    }

    return x;
}

int main() {
    // Input
    float initVal = 9702;
    vector<float> cn;
    cn.push_back(-19700);
    cn.push_back(10000);

    // === Bisection Method ===

    // // Print graph
    // print_graph(cn, 0.0, 1.0, initVal, 0.005);

    // Bisection Method
    float root1 = bisection_method(cn, 0.0, 0.03, initVal);
    float root2 = bisection_method(cn, 0.015, 0.03, initVal);

    cout<< "=== Bisection Method ===\n";
    cout<< "IRR_1: " << root1 << '\n';
    cout<< "IRR_2: " << root2 << '\n';

    // === Newton's Method ===
    root1 = newton_method(cn, initVal, 0.0);
    root2 = newton_method(cn, initVal, 0.5);

    cout<< "=== Newton's Method ===\n";
    cout<< "IRR_1: " << root1 << '\n';
    cout<< "IRR_2: " << root2 << '\n';
    
    return 0;
}