#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <gnuplot-iostream.h>

// Function to print audio information
void printAudioInfo(const sf::SoundBuffer& buffer) {
    std::cout << "Audio File Information:" << std::endl;
    std::cout << "Sample Rate: " << buffer.getSampleRate() << " Hz" << std::endl;
    std::cout << "Channel Count: " << buffer.getChannelCount() << std::endl;
    std::cout << "Duration: " << buffer.getDuration().asSeconds() << " seconds" << std::endl;
    std::cout << "Sample Count: " << buffer.getSampleCount() << std::endl;
    std::cout << "Sample Size: " << sizeof(sf::Int16) * 8 << " bits" << std::endl;
}

// Function to plot waveform data
void plotWaveform(const std::vector<double>& data_vector, unsigned int sample_rate, const std::string& channel_name, bool display = true) {
    const size_t target_max_samples = 10000;
    
    // Calculate the downsampling factor
    size_t downsample_factor = 1;
    if (data_vector.size() > target_max_samples) {
        downsample_factor = std::ceil(static_cast<double>(data_vector.size()) / target_max_samples);
    }
    
    // Create downsampled data and time vectors
    std::vector<double> downsampled_data;
    std::vector<double> time;
    downsampled_data.reserve(std::min(data_vector.size(), target_max_samples));
    time.reserve(std::min(data_vector.size(), target_max_samples));
    
    for (size_t i = 0; i < data_vector.size(); i += downsample_factor) {
        downsampled_data.push_back(data_vector[i]);
        time.push_back(static_cast<double>(i) / sample_rate);
    }
    
    // Ensure that the "../plots" directory exists
    std::filesystem::create_directory("../plots");
    
    // Create a Gnuplot object with persist option
    Gnuplot gp;
    
    // Save to PNG file
    gp << "set terminal pngcairo size 1200,400\n";
    gp << "set output '../plots/" << channel_name << "_waveform.png'\n";
    gp << "set title '" << channel_name << " Waveform (downsampled)'\n";
    gp << "set xlabel 'Time (s)'\n";
    gp << "set ylabel 'Amplitude'\n";
    gp << "plot '-' with lines title '" << channel_name << "'\n";
    gp.send1d(boost::make_tuple(time, downsampled_data));
    
    // Display the plot if display is true
    if (display) {
        gp << "set terminal wxt size 1200,400\n";
        gp << "set title '" << channel_name << " Waveform (downsampled)'\n";
        gp << "set xlabel 'Time (s)'\n";
        gp << "set ylabel 'Amplitude'\n";
        gp << "plot '-' with lines title '" << channel_name << "'\n";
        gp.send1d(boost::make_tuple(time, downsampled_data));
        gp << "set terminal wxt\n";  // Reset terminal to interactive mode
    }
}

void plotHistogram(const std::vector<double>& data, const std::string& title, int num_bins, bool display = true) {
    // Ensure that the "../plots" directory exists
    std::filesystem::create_directory("../plots");

    // Find the range of the data
    auto [min_it, max_it] = std::minmax_element(data.begin(), data.end());
    double min_val = *min_it;
    double max_val = *max_it;

    // Create bins
    std::vector<int> bins(num_bins, 0);
    double bin_width = (max_val - min_val) / num_bins;

    // Fill the bins
    for (double value : data) {
        int bin = static_cast<int>((value - min_val) / bin_width);
        if (bin == num_bins) bin--;  // Handle edge case for maximum value
        bins[bin]++;
    }

    // Create x-axis values (bin centers)
    std::vector<double> bin_centers(num_bins);
    for (int i = 0; i < num_bins; i++) {
        bin_centers[i] = min_val + (i + 0.5) * bin_width;
    }

    // Plot the histogram
    Gnuplot gp;

    // Save to PNG file
    gp << "set terminal pngcairo size 800,600\n";
    gp << "set output '../plots/" << title << ".png'\n";
    gp << "set title '" << title << "'\n";
    gp << "set xlabel 'Amplitude'\n";
    gp << "set ylabel 'Frequency'\n";
    gp << "set style fill solid 0.5\n";
    gp << "set boxwidth " << bin_width * 0.9 << "\n";
    gp << "set xrange [-1:1]\n";
    gp << "plot '-' using 1:2 with boxes notitle\n";
    gp.send1d(boost::make_tuple(bin_centers, bins));

    // Display the plot if display is true
    if (display) {
        gp << "set terminal wxt size 800,600\n";
        gp << "set title '" << title << "'\n";
        gp << "set xlabel 'Amplitude'\n";
        gp << "set ylabel 'Frequency'\n";
        gp << "set style fill solid 0.5\n";
        gp << "set boxwidth " << bin_width * 0.9 << "\n";
        gp << "set xrange [-1:1]\n";
        gp << "plot '-' using 1:2 with boxes notitle\n";
        gp.send1d(boost::make_tuple(bin_centers, bins));
    }
}

int main() {
    std::string filePath = R"(C:\Users\jmtia\Code projects\IC\IC-projeto\Parte2-audio\datasets\sample01.wav)";

    // Load the WAV file
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filePath)) {
        std::cerr << "Failed to load WAV file: " << filePath << std::endl;
        return 1;
    }

    // Read audio samples into a vector and get basic info
    const sf::Int16* samples = buffer.getSamples();
    std::size_t sampleCount = buffer.getSampleCount();
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

    // Plot the left and right channel waveforms
    plotWaveform(leftChannel, buffer.getSampleRate(), "Left Channel", false);
    plotWaveform(rightChannel, buffer.getSampleRate(), "Right Channel", false);

    // Calculate MID and SIDE channels
    std::vector<double> midChannel(leftChannel.size());
    std::vector<double> sideChannel(leftChannel.size());
    for (size_t i = 0; i < leftChannel.size(); ++i) {
        midChannel[i] = (leftChannel[i] + rightChannel[i]) / 2.0;
        sideChannel[i] = (leftChannel[i] - rightChannel[i]) / 2.0;
    }

    // Plot histograms
    int num_bins = 64;  // You can adjust this value
    bool display_histograms = true;  // Set to false if you don't want to display the plots
    plotHistogram(leftChannel, "Left Channel Histogram", num_bins, display_histograms);
    plotHistogram(rightChannel, "Right Channel Histogram", num_bins, display_histograms);
    plotHistogram(midChannel, "MID Channel Histogram", num_bins, display_histograms);
    plotHistogram(sideChannel, "SIDE Channel Histogram", num_bins, display_histograms);

    return 0;
}