#include <iostream>

#include <laya/laya.hpp>

void init_manual() {
    laya::create(laya::subsystem::video);

    // Your application code here...
    std::cout << "Laya is running manually..." << std::endl;

    laya::destroy();
}

// This example uses RAII to manage the laya context automatically.
// No need to call create() or destroy().
void init_automatic() {
    laya::context ctx(laya::subsystem::video);

    // Your application code here...
    std::cout << "Laya is running automatically..." << std::endl;
}

int main() {
    try {
        init_manual();
        init_automatic();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
