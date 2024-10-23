#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <gnuplot-iostream.h>
#include <fftw3.h>

// Function to print audio information
void printAudioInfo(const sf::SoundBuffer &buffer)
{
    std::cout << "Audio File Information:" << std::endl;
    std::cout << "Sample Rate: " << buffer.getSampleRate() << " Hz" << std::endl;
    std::cout << "Channel Count: " << buffer.getChannelCount() << std::endl;
    std::cout << "Duration: " << buffer.getDuration().asSeconds() << " seconds" << std::endl;
    std::cout << "Sample Count: " << buffer.getSampleCount() << std::endl;
    std::cout << "Sample Size: " << sizeof(sf::Int16) * 8 << " bits" << std::endl;
}

// Function to plot waveform data
void plotWaveform(const std::vector<double> &data_vector, unsigned int sample_rate, const std::string &channel_name)
{
    const size_t target_max_samples = 10000;

    // Calculate the downsampling factor
    size_t downsample_factor = 1;
    if (data_vector.size() > target_max_samples)
    {
        downsample_factor = std::ceil(static_cast<double>(data_vector.size()) / target_max_samples);
    }

    // Create downsampled data and time vectors
    std::vector<double> downsampled_data;
    std::vector<double> time;
    downsampled_data.reserve(std::min(data_vector.size(), target_max_samples));
    time.reserve(std::min(data_vector.size(), target_max_samples));

    for (size_t i = 0; i < data_vector.size(); i += downsample_factor)
    {
        downsampled_data.push_back(data_vector[i]);
        time.push_back(static_cast<double>(i) / sample_rate);
    }

    // Ensure that the output directory exists
    std::string output_directory = "../outputs/waveforms/";
    std::filesystem::create_directory("../outputs/");
    std::filesystem::create_directory(output_directory);

    // Create a Gnuplot object with persist option
    Gnuplot gp;

    // Save to PNG file
    gp << "set terminal pngcairo size 1200,400\n";
    gp << "set output '" << output_directory << channel_name << ".png'\n";
    gp << "set title '" << channel_name << "'\n";
    gp << "set xlabel 'Time (s)'\n";
    gp << "set ylabel 'Amplitude'\n";
    gp << "plot '-' with lines title '" << channel_name << "'\n";
    gp.send1d(boost::make_tuple(time, downsampled_data));
}

void plotHistogram(const std::vector<double> &data, const std::string &title, int num_bins)
{
    // Find the range of the data
    auto [min_it, max_it] = std::minmax_element(data.begin(), data.end());
    double min_val = *min_it;
    double max_val = *max_it;

    // Create bins
    std::vector<int> bins(num_bins, 0);
    double bin_width = (max_val - min_val) / num_bins;

    // Fill the bins
    for (double value : data)
    {
        int bin = static_cast<int>((value - min_val) / bin_width);
        if (bin == num_bins)
            bin--; // Handle edge case for maximum value
        bins[bin]++;
    }

    // Create x-axis values (bin centers)
    std::vector<double> bin_centers(num_bins);
    for (int i = 0; i < num_bins; i++)
    {
        bin_centers[i] = min_val + (i + 0.5) * bin_width;
    }

    // Ensure that the output directory exists
    std::string output_directory = "../outputs/histograms/";
    std::filesystem::create_directory("../outputs/");
    std::filesystem::create_directory(output_directory);

    // Plot the histogram
    Gnuplot gp;

    // Save to PNG file
    gp << "set terminal pngcairo size 800,600\n";
    gp << "set output '" << output_directory << title << ".png'\n";
    gp << "set title '" << title << "'\n";
    gp << "set xlabel 'Amplitude'\n";
    gp << "set ylabel 'Frequency'\n";
    gp << "set style fill solid 0.5\n";
    gp << "set boxwidth " << bin_width * 0.9 << "\n";
    gp << "set xrange [-0.8:0.8]\n";
    gp << "plot '-' using 1:2 with boxes notitle\n";
    gp.send1d(boost::make_tuple(bin_centers, bins));
}

std::vector<sf::Int16> quantizeAudio(const std::vector<sf::Int16> &samples, int bitsToReduce)
{
    std::vector<sf::Int16> quantizedSamples;
    quantizedSamples.reserve(samples.size());

    for (const auto &sample : samples)
    {
        // Shift right to remove least significant bits
        sf::Int16 quantizedSample = sample >> bitsToReduce;
        // Shift left to restore the original scale
        quantizedSample = quantizedSample << bitsToReduce;
        quantizedSamples.push_back(quantizedSample);
    }

    return quantizedSamples;
}

void saveQuantizedWav(const std::vector<sf::Int16> &samples, unsigned int sampleRate, unsigned int channelCount, const std::string &filename)
{
    sf::SoundBuffer buffer;
    buffer.loadFromSamples(samples.data(), samples.size(), channelCount, sampleRate);

    // Ensure that the output directory exists
    std::string output_directory = "../outputs/audio/";
    std::filesystem::create_directory("../outputs/");
    std::filesystem::create_directory(output_directory);

    if (!buffer.saveToFile(output_directory + filename))
    {
        std::cerr << "Failed to save WAV file: " << filename << std::endl;
    }
}

std::pair<double, double> calculateMSEAndSNR(const std::vector<double> &original, const std::vector<double> &processed)
{
    if (original.size() != processed.size())
    {
        throw std::runtime_error("Signal lengths do not match");
    }

    double mse = 0.0;
    double signal_power = 0.0;

    for (size_t i = 0; i < original.size(); ++i)
    {
        double error = original[i] - processed[i];
        mse += error * error;
        signal_power += original[i] * original[i];
    }

    mse /= original.size();
    signal_power /= original.size();

    double snr = 10 * std::log10(signal_power / mse);

    return {mse, snr};
}

std::vector<std::complex<double>> computeFFT(const std::vector<double> &signal)
{
    int n = signal.size();

    // Allocate input and output arrays for FFTW
    fftw_complex *in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * n);
    fftw_complex *out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * n);

    // Create FFTW plan
    fftw_plan plan = fftw_plan_dft_1d(n, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    // Copy input data
    for (int i = 0; i < n; i++)
    {
        in[i][0] = signal[i];
        in[i][1] = 0.0;
    }

    // Execute FFT
    fftw_execute(plan);

    // Copy output data
    std::vector<std::complex<double>> result(n);
    for (int i = 0; i < n; i++)
    {
        result[i] = std::complex<double>(out[i][0], out[i][1]);
    }

    // Clean up
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);

    return result;
}

void plotFrequencySpectrum(const std::vector<std::complex<double>> &fft_result, double sample_rate, const std::string &title)
{
    std::vector<double> magnitudes;
    std::vector<double> frequencies;

    int n = fft_result.size();
    for (int i = 0; i < n / 2; i++)
    {
        double magnitude = std::abs(fft_result[i]);
        double frequency = i * sample_rate / n;

        magnitudes.push_back(magnitude);
        frequencies.push_back(frequency);
    }

    // Ensure that the output directory exists
    std::string output_directory = "../outputs/fft/";
    std::filesystem::create_directory("../outputs/");
    std::filesystem::create_directory(output_directory);

    // Create a Gnuplot object
    Gnuplot gp;

    // Save to png
    gp << "set terminal pngcairo size 800,600\n";
    gp << "set output '" << output_directory << title << ".png'\n";
    gp << "set title '" << title << "'\n";
    gp << "set xlabel 'Frequency (Hz)'\n";
    gp << "set ylabel 'Magnitude'\n";
    gp << "set xrange [20:10000]\n";
    gp << "set yrange [0:*]\n";
    gp << "set logscale x\n";
    gp << "plot '-' with lines title 'Magnitude Spectrum'\n";
    gp.send1d(boost::make_tuple(frequencies, magnitudes));
}

int main(int argc, char *argv[])
{
    /***************************************
     *           PARSING ARGUMENTS         *
     ***************************************/

    // Default values
    int sampleNumber = 1;
    int bitsToReduce = 10;
    bool generateWaveform = false;
    bool generateHistograms = false;
    bool generateQuantizedWaveform = false;
    bool generateAudioFile = false;
    bool generateFFT = false;

    // Parse command line arguments
    if (argc > 1)
        sampleNumber = std::stoi(argv[1]);
    if (argc > 2)
        bitsToReduce = std::stoi(argv[2]);

    // Parse command line arguments
    for (int i = 3; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-w")
            generateWaveform = true;
        else if (arg == "-h")
            generateHistograms = true;
        else if (arg == "-q")
            generateQuantizedWaveform = true;
        else if (arg == "-a")
            generateAudioFile = true;
        else if (arg == "-f")
            generateFFT = true;
    }

    // If no specific outputs are requested, generate all
    if (!(generateWaveform || generateHistograms || generateQuantizedWaveform || generateAudioFile || generateFFT))
    {
        generateWaveform = generateHistograms = generateQuantizedWaveform = generateAudioFile = generateFFT = true;
    }

    /***************************************
     *                TASK 1               *
     ***************************************/

    std::string filePath = "../datasets/";
    std::string fileName = "sample";
    if (sampleNumber < 10)
        fileName += "0";
    fileName += std::to_string(sampleNumber) + ".wav";
    filePath += fileName;

    // Load the WAV file
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filePath))
    {
        std::cerr << "Failed to load WAV file: " << filePath << std::endl;
        return 1;
    }

    // Read audio samples into a vector and get basic info
    const sf::Int16 *samples = buffer.getSamples();
    std::size_t sampleCount = buffer.getSampleCount();
    std::vector<sf::Int16> sampleVector(samples, samples + sampleCount);

    // Print audio file information
    printAudioInfo(buffer);

    /***************************************
     *                TASK 2               *
     ***************************************/

    // Split the samples into left and right channels
    std::vector<double> leftChannel;
    std::vector<double> rightChannel;
    for (std::size_t i = 0; i < sampleCount; i += 2)
    {
        leftChannel.push_back(static_cast<double>(sampleVector[i]) / 32768.0);
        rightChannel.push_back(static_cast<double>(sampleVector[i + 1]) / 32768.0);
    }

    // Plot the left and right channel waveforms
    if (generateWaveform)
    {
        plotWaveform(leftChannel, buffer.getSampleRate(), fileName + " - Left Channel");
        plotWaveform(rightChannel, buffer.getSampleRate(), fileName + " - Right Channel");
    }

    /***************************************
     *                TASK 3               *
     ***************************************/

    // Calculate MID and SIDE channels
    std::vector<double> midChannel(leftChannel.size());
    std::vector<double> sideChannel(leftChannel.size());
    for (size_t i = 0; i < leftChannel.size(); ++i)
    {
        midChannel[i] = (leftChannel[i] + rightChannel[i]) / 2.0;
        sideChannel[i] = (leftChannel[i] - rightChannel[i]) / 2.0;
    }

    // Plot histograms
    int num_bins = 64;
    if (generateHistograms)
    {
        plotHistogram(leftChannel, fileName + " - Left Channel Histogram", num_bins);
        plotHistogram(rightChannel, fileName + " - Right Channel Histogram", num_bins);
        plotHistogram(midChannel, fileName + " - MID Channel Histogram", num_bins);
        plotHistogram(sideChannel, fileName + " - SIDE Channel Histogram", num_bins);
    }

    /***************************************
     *                TASK 4               *
     ***************************************/

    // Quantize the audio (reduce by 4 bits)
    std::vector<sf::Int16> quantizedSamples = quantizeAudio(sampleVector, bitsToReduce);
    if (generateAudioFile)
    {
        // Save the quantized audio to a new WAV file
        std::string quantizedFileName = "sample";
        if (sampleNumber < 10)
            quantizedFileName += "0";
        quantizedFileName += std::to_string(sampleNumber) + "_" + std::to_string(16 - bitsToReduce) + "bits.wav";
        saveQuantizedWav(quantizedSamples, buffer.getSampleRate(), buffer.getChannelCount(), quantizedFileName);
    }

    // Split the quantized samples into left and right channels
    std::vector<double> quantizedLeftChannel;
    std::vector<double> quantizedRightChannel;
    for (std::size_t i = 0; i < sampleCount; i += 2)
    {
        quantizedLeftChannel.push_back(static_cast<double>(quantizedSamples[i]) / 32768.0);
        quantizedRightChannel.push_back(static_cast<double>(quantizedSamples[i + 1]) / 32768.0);
    }

    if (generateQuantizedWaveform)
    {
        // Create vectors for the first few samples
        int initialOffset = 44100; // skip the first second (may be silence)
        int samplesToDisplay = 500;
        std::vector<double> leftChannelZoomIn(leftChannel.begin() + initialOffset, leftChannel.begin() + samplesToDisplay + initialOffset);
        std::vector<double> rightChannelZoomIn(rightChannel.begin() + initialOffset, rightChannel.begin() + samplesToDisplay + initialOffset);
        std::vector<double> quantizedLeftChannelZoomIn(quantizedLeftChannel.begin() + initialOffset, quantizedLeftChannel.begin() + samplesToDisplay + initialOffset);
        std::vector<double> quantizedRightChannelZoomIn(quantizedRightChannel.begin() + initialOffset, quantizedRightChannel.begin() + samplesToDisplay + initialOffset);

        // Plot the original and quantized waveforms
        plotWaveform(leftChannelZoomIn, buffer.getSampleRate(), fileName + " - Left Channel (zoom)");
        plotWaveform(rightChannelZoomIn, buffer.getSampleRate(), fileName + " - Right Channel (zoom)");
        plotWaveform(quantizedLeftChannelZoomIn, buffer.getSampleRate(), fileName + " - Left Channel (" + std::to_string(16 - bitsToReduce) + " bits) (zoom)");
        plotWaveform(quantizedRightChannelZoomIn, buffer.getSampleRate(), fileName + " - Right Channel (" + std::to_string(16 - bitsToReduce) + " bits) (zoom)");
    }

    /***************************************
     *                TASK 5               *
     ***************************************/

    // Calculate MSE and SNR for left and right channels
    auto [leftMSE, leftSNR] = calculateMSEAndSNR(leftChannel, quantizedLeftChannel);
    auto [rightMSE, rightSNR] = calculateMSEAndSNR(rightChannel, quantizedRightChannel);

    // Print the results
    std::cout << "\nQuantization Quality Metrics for " << fileName << " (" << std::to_string(16 - bitsToReduce) << " bit):" << std::endl;
    std::cout << "Left Channel:" << std::endl;
    std::cout << "  MSE: " << leftMSE << std::endl;
    std::cout << "  SNR: " << leftSNR << " dB" << std::endl;
    std::cout << "Right Channel:" << std::endl;
    std::cout << "  MSE: " << rightMSE << std::endl;
    std::cout << "  SNR: " << rightSNR << " dB" << std::endl;

    // Calculate average MSE and SNR across both channels
    double avgMSE = (leftMSE + rightMSE) / 2.0;
    double avgSNR = (leftSNR + rightSNR) / 2.0;

    std::cout << "Average across both channels:" << std::endl;
    std::cout << "  MSE: " << avgMSE << std::endl;
    std::cout << "  SNR: " << avgSNR << " dB" << std::endl;

    /***************************************
     *         Extra: FFT comparison       *
     ***************************************/
    if (generateFFT)
    {
        // Compute FFT for the original mid channel
        std::vector<std::complex<double>> fft_mid = computeFFT(midChannel);

        // Plot and save the frequency spectrum of the original mid channel
        plotFrequencySpectrum(fft_mid, buffer.getSampleRate(), fileName + " - Mid Channel FFT (Original)");

        // Compute and plot FFT for the quantized mid channel
        std::vector<double> quantizedMidChannel(midChannel.size());
        for (size_t i = 0; i < midChannel.size(); ++i)
        {
            quantizedMidChannel[i] = (quantizedLeftChannel[i] + quantizedRightChannel[i]) / 2.0;
        }
        std::vector<std::complex<double>> fft_quantized_mid = computeFFT(quantizedMidChannel);
        plotFrequencySpectrum(fft_quantized_mid, buffer.getSampleRate(), fileName + " - Mid Channel FFT (Quantized " + std::to_string(16 - bitsToReduce) + " bits)");
    }

    return 0;
}
