#include <laya/init.hpp>
#include <iostream>

int main() {
    try {
        std::cout << "Initializing laya..." << std::endl;
        laya::init();
        std::cout << "laya initialized successfully!" << std::endl;

        std::cout << "Shutting down laya..." << std::endl;
        laya::quit();
        std::cout << "laya shut down successfully!" << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
