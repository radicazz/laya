#include <iostream>

#include <laya/laya.hpp>

int main() {
    try {
        std::cout << "Initializing laya..." << std::endl;
        laya::create(laya::subsystem::video);
        std::cout << "laya initialized successfully!" << std::endl;

        std::cout << "Shutting down laya..." << std::endl;
        laya::destroy();
        std::cout << "laya shut down successfully!" << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
