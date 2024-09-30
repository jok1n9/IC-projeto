#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <iomanip>
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

void printAudioInfo(const sf::SoundBuffer& buffer) {
    std::cout << "Audio File Information:" << std::endl;
    std::cout << "Sample Rate: " << buffer.getSampleRate() << " Hz" << std::endl;
    std::cout << "Channel Count: " << buffer.getChannelCount() << std::endl;
    std::cout << "Duration: " << buffer.getDuration().asSeconds() << " seconds" << std::endl;
    std::cout << "Sample Count: " << buffer.getSampleCount() << std::endl;
    std::cout << "Sample Size: " << sizeof(sf::Int16) * 8 << " bits" << std::endl;
}

int main() {
    // Use a raw string literal for the file path to avoid escaping issues
    std::string filePath = R"(C:\Users\jmtia\Code projects\IC\IC-projeto\Parte2-audio\datasets\sample01.wav)";
    
    // Load the WAV file
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filePath)) {
        std::cerr << "Failed to load WAV file: " << filePath << std::endl;
        return 1;
    }

    // T1: Reading and loading audio files
    // Get the audio samples
    const sf::Int16* samples = buffer.getSamples();
    std::size_t sampleCount = buffer.getSampleCount();

    // Store the samples in a vector
    std::vector<sf::Int16> sampleVector(samples, samples + sampleCount);

    // Print audio file information
    printAudioInfo(buffer);

    // Split the samples into left and right channels
    std::vector<double> leftChannel;
    std::vector<double> rightChannel;
    
    for (std::size_t i = 0; i < sampleCount; i += 2) {
        leftChannel.push_back(static_cast<double>(sampleVector[i]) / 32768.0);
        rightChannel.push_back(static_cast<double>(sampleVector[i + 1]) / 32768.0);
    }

    // Create a time vector for x-axis
    std::vector<double> time(leftChannel.size());
    for (std::size_t i = 0; i < time.size(); ++i) {
        time[i] = static_cast<double>(i) / buffer.getSampleRate();
    }

    // Plot left channel waveform
    plt::figure_size(1200, 400);
    plt::named_plot("Left Channel", time, leftChannel);
    plt::title("Left Channel Waveform");
    plt::xlabel("Time (s)");
    plt::ylabel("Amplitude");
    plt::legend();
    plt::save("left_channel_waveform.png");
    plt::clf();

    // Plot right channel waveform
    plt::figure_size(1200, 400);
    plt::named_plot("Right Channel", time, rightChannel);
    plt::title("Right Channel Waveform");
    plt::xlabel("Time (s)");
    plt::ylabel("Amplitude");
    plt::legend();
    plt::save("right_channel_waveform.png");

    return 0;
}