#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
#include <string>

int main() {
    // Use a raw string literal for the file path to avoid escaping issues
    std::string filePath = R"(C:\Users\jmtia\Code projects\IC\IC-projeto\Parte2-audio\datasets\sample01.wav)";
    
    // Load the WAV file
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filePath)) {
        std::cerr << "Failed to load WAV file: " << filePath << std::endl;
        return 1;
    }

    // Get the audio samples
    const sf::Int16* samples = buffer.getSamples();
    std::size_t sampleCount = buffer.getSampleCount();

    // Store the samples in a vector
    std::vector<sf::Int16> sampleVector(samples, samples + sampleCount);

    // Print some information
    std::cout << "Sample count: " << sampleVector.size() << std::endl;
    std::cout << "First 10 samples:" << std::endl;
    for (int i = 0; i < 10 && i < sampleVector.size(); ++i) {
        std::cout << sampleVector[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}