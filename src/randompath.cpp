#include <string>
#include <random>

std::string randomPath() {
    // Define the characters to choose from (letters and digits)
    const std::string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::string randomString;

    // Create a random device and engine
    std::random_device rd;
    std::mt19937 gen(rd());  // Mersenne Twister random number generator
    std::uniform_int_distribution<> dis(0, charset.size() - 1);  // Uniform distribution

    // Generate a random string of the specified length
    for (int i = 0; i < 5; ++i) {
        randomString += charset[dis(gen)];  // Append a random character
    }

    return randomString;
}