#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath> // for log10



void calculateAndSaveHistogram(const cv::Mat& image);
void showChannels(const cv::Mat& image);
void applyGaussianFilter(const cv::Mat& image);
void compareImages(const cv::Mat& img1, const cv::Mat& img2);
void quantizeImage(const cv::Mat& image, int levels);


//save and display used across the program
bool saveFlag = false;
bool displayFlag = false;


int main(int argc, char** argv) {
    if (argc < 2 || std::string(argv[1]) == "--help" || std::string(argv[1]) == "--h") {
        std::cerr << "Usage: " << argv[0] << " [options] [image_path]\n";
        std::cerr << "Options:\n";
        std::cerr << "  --histogram                 Calculate and save the histogram of the image\n";
        std::cerr << "  --channels                  Show the RGB channels of the image\n";
        std::cerr << "  --gaussian                  Apply Gaussian filter to the image\n";
        std::cerr << "  --quantize <levels>         Quantize the image to specified levels\n";
        std::cerr << "  --compare <image1>          Compare two images\n";
        std::cerr << "  --all <quantization_levels> Run all operations (histogram, channels, gaussian, quantize)\n";
        std::cerr << "  --display                   Display images\n";
        std::cerr << "  --save                      Saves images\n";
        std::cerr << "  --help                      Show this help message\n";
        return 1;
    }

    // Flags
    bool calcHistogram = false;
    bool showRGBChannels = false;
    bool applyGaussian = false;
    bool quantizeFlag = false;
    bool compareImagesFlag = false;
    bool runAllOperations = false;
    int quantizationLevels = 0;



    cv::Mat image, image1;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--histogram") {
            calcHistogram = true;
        } else if (arg == "--channels" || arg == "--c") {
            showRGBChannels = true;
        } else if (arg == "--gaussian" ||arg == "--g") {
            applyGaussian = true;
        } else if ((arg == "--quantize"|| arg == "--q") && i + 1 < argc) {
            quantizeFlag = true;
            quantizationLevels = std::stoi(argv[++i]);
        } else if ((arg == "--compare"||arg == "--d") && i + 1 < argc) {
            compareImagesFlag = true;
            image1 = cv::imread(argv[++i], cv::IMREAD_COLOR);

        } else if (arg == "--save" || arg == "--s") {
            saveFlag = true;
        } else if (arg == "--display" || arg == "--d") {
            displayFlag = true;
        } else if (arg == "--all" && i + 1 < argc) {
            runAllOperations = true;
            quantizationLevels = std::stoi(argv[++i]);
        } else {
            std::string imagePath = argv[i];

            // Load the image
            image = cv::imread(imagePath, cv::IMREAD_COLOR);
            if (image.empty()) {
                std::cerr << "Could not open or find the image: " << imagePath << "\n";
                return 1;
            }
            else if(saveFlag){
                cv::imwrite("./output/original.ppm", image);
                std::cout << "Original saved at: ./output/original.ppm"<< std::endl;
            }
            if (calcHistogram) {
                calculateAndSaveHistogram(image);
            }
            if (showRGBChannels) {
                showChannels(image);
            }
            if (applyGaussian) {
                applyGaussianFilter(image);
            }
            if (quantizeFlag) {
                // Convert image to grayscale before quantization
                cv::Mat grayImage;
                cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
                quantizeImage(grayImage, quantizationLevels);
            }

            // Run all operations if --all flag is set
            if (runAllOperations) {
                if(displayFlag){
                    std::cout << "Press any key to see the image channels..." << std::endl;
                    std::cin.get();  // Wait for user input
                    showChannels(image);
                    std::cout << "Press any key to continue and see the grayscale histogram..." << std::endl;
                    std::cin.get();  // Wait for user input
                    calculateAndSaveHistogram(image);
                    std::cout << "Press any key to see gaussian filters..." << std::endl;
                    std::cin.get();  // Wait for user input
                    applyGaussianFilter(image);
                    std::cout << "Press any key to see image quantization" << std::endl;
                    std::cin.get();  // Wait for user input
                    quantizeImage(image, quantizationLevels);
                    std::cout << "End of program" << std::endl;
                }
                else{
                    showChannels(image);
                    calculateAndSaveHistogram(image);
                    applyGaussianFilter(image);
                    quantizeImage(image, quantizationLevels);
                }
            }
        }
    }

    // Compare images if the flag is set
    if (compareImagesFlag) {
        if (image1.empty() || image.empty()) {
            std::cerr << "One or both images could not be loaded for comparison.\n";
            return 1;
        }
        compareImages(image1, image);
    }

    return 0;
}

void calculateAndSaveHistogram(const cv::Mat& image) {
    auto start = std::chrono::high_resolution_clock::now();

    cv::Mat grayscaleImage;

    cv::cvtColor(image, grayscaleImage, cv::COLOR_BGR2GRAY);

    std::vector<int> grayHist(256, 0);

    for (int i = 0; i < grayscaleImage.rows; ++i) {
        for (int j = 0; j < grayscaleImage.cols; ++j) {
            int intensity = grayscaleImage.at<uchar>(i, j);
            grayHist[intensity]++;
        }
    }

    // Draw the histogram image
    int histImageWidth = 512;
    int histImageHeight = 400;
    int binWidth = std::round((double)histImageWidth / 256);

    cv::Mat histImage(histImageHeight + 50, histImageWidth + 50, CV_8UC3, cv::Scalar(255, 255, 255));

    // Normalize histogram to fit in the image
    int maxIntensity = *std::max_element(grayHist.begin(), grayHist.end());
    for (int i = 0; i < 256; i++) {
        grayHist[i] = ((double)grayHist[i] / maxIntensity) * histImageHeight;
    }

    // Draw the histogram bars
    for (int i = 1; i < 256; i++) {
        cv::line(histImage,
                 cv::Point(binWidth * (i - 1) + 25, histImageHeight - grayHist[i - 1] + 25),
                 cv::Point(binWidth * i + 25, histImageHeight - grayHist[i] + 25),
                 cv::Scalar(0, 0, 0), 2);
    }

    // Draw the x and y axes
    cv::line(histImage, cv::Point(25, 25), cv::Point(25, histImageHeight + 25), cv::Scalar(0, 0, 0), 2); // y-axis
    cv::line(histImage, cv::Point(25, histImageHeight + 25), cv::Point(histImageWidth + 25, histImageHeight + 25), cv::Scalar(0, 0, 0), 2); // x-axis

    // Add x-axis and y-axis labels (e.g., 0, 64, 128, 192, 255 for x, and frequency values for y)
    for (int i = 0; i < 256; i += 64) {
        std::string label = std::to_string(i);
        cv::putText(histImage, label, cv::Point(binWidth * i + 20, histImageHeight + 45), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 0, 0), 1);
    }

    // Add some labels for y-axis (e.g., at 0%, 25%, 50%, 75%, 100%)
    for (int i = 0; i <= 5; ++i) {
        int yLabelValue = (maxIntensity * i) / 5;
        std::string yLabel = std::to_string(yLabelValue);
        cv::putText(histImage, yLabel, cv::Point(5, histImageHeight - (i * histImageHeight / 5) + 25), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 0, 0), 1);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Task timer: " << duration.count() << " seconds" << std::endl;

    if(saveFlag){
        // Save the histogram as an image
        cv::imwrite("./output/hist.ppm", histImage);
        std::cout << "Histogram saved at: ./output/hist.ppm"<< std::endl;
    }
    if(displayFlag){
        cv::imshow("Grayscale Histogram", histImage);

        // Keep OpenCV GUI alive
        while (cv::getWindowProperty("Grayscale Histogram", cv::WND_PROP_VISIBLE) >= 1) {
            cv::waitKey(1);  // Keep refreshing the OpenCV window
        }
    }
}

void showChannels(const cv::Mat& image) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<cv::Mat> bgrChannels;
    cv::split(image, bgrChannels);

    cv::Mat grayscaleImage;
    cv::cvtColor(image, grayscaleImage, cv::COLOR_BGR2GRAY);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Task timer: " << duration.count() << " seconds" << std::endl;

    if(displayFlag){
        cv::imshow("Grayscale Image", grayscaleImage);
        cv::imshow("Blue Channel", bgrChannels[0]);
        cv::imshow("Green Channel", bgrChannels[1]);
        cv::imshow("Red Channel", bgrChannels[2]);
        cv::imshow("Original Image", image);


        while (cv::getWindowProperty("Grayscale Image", cv::WND_PROP_VISIBLE) >= 1 ||
            cv::getWindowProperty("Blue Channel", cv::WND_PROP_VISIBLE) >= 1 ||
            cv::getWindowProperty("Green Channel", cv::WND_PROP_VISIBLE) >= 1 ||
            cv::getWindowProperty("Red Channel", cv::WND_PROP_VISIBLE) >= 1 ||
            cv::getWindowProperty("Original Image", cv::WND_PROP_VISIBLE) >= 1) {
            cv::waitKey(1);  // Keep refreshing the OpenCV windows
        }
    }

    if (saveFlag) {
        // Convert single-channel images to 3-channel images for saving
        cv::Mat blueChannel, greenChannel, redChannel;

        cv::merge(std::vector<cv::Mat>{bgrChannels[0], cv::Mat::zeros(bgrChannels[0].size(), CV_8U), cv::Mat::zeros(bgrChannels[0].size(), CV_8U)}, blueChannel);
        cv::merge(std::vector<cv::Mat>{cv::Mat::zeros(bgrChannels[0].size(), CV_8U), bgrChannels[1], cv::Mat::zeros(bgrChannels[0].size(), CV_8U)}, greenChannel);
        cv::merge(std::vector<cv::Mat>{cv::Mat::zeros(bgrChannels[0].size(), CV_8U), cv::Mat::zeros(bgrChannels[0].size(), CV_8U), bgrChannels[2]}, redChannel);

        cv::imwrite("./output/blue_channel.ppm", blueChannel);
        cv::imwrite("./output/green_channel.ppm", greenChannel);
        cv::imwrite("./output/red_channel.ppm", redChannel);
        std::cout << "Channel images saved as blue_channel.ppm, green_channel.ppm, red_channel.ppm." << std::endl;
    }
}

void applyGaussianFilter(const cv::Mat& image) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> kernelSizes = {3, 5, 7, 9};

    for (int kernelSize : kernelSizes) {
        cv::Mat blurredImage;
        cv::GaussianBlur(image, blurredImage, cv::Size(kernelSize, kernelSize), 0);
        std::string windowName = "Gaussian Blur (Kernel Size: " + std::to_string(kernelSize) + ")";

        if (displayFlag) {
            cv::imshow(windowName, blurredImage);
        }

        if (saveFlag) {
            std::string savePath = "./output/gaussian_blur_kernel_" + std::to_string(kernelSize) + ".ppm";
            cv::imwrite(savePath, blurredImage);
            std::cout << "Saved " << savePath << std::endl;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Task timer: " << duration.count() << " seconds" << std::endl;

    if (displayFlag) {
        cv::imshow("Original Image", image);

        // Keep OpenCV GUI alive until all windows are closed
        while (cv::getWindowProperty("Gaussian Blur (Kernel Size: 3)", cv::WND_PROP_VISIBLE) >= 1 ||
               cv::getWindowProperty("Gaussian Blur (Kernel Size: 5)", cv::WND_PROP_VISIBLE) >= 1 ||
               cv::getWindowProperty("Gaussian Blur (Kernel Size: 7)", cv::WND_PROP_VISIBLE) >= 1 ||
               cv::getWindowProperty("Gaussian Blur (Kernel Size: 9)", cv::WND_PROP_VISIBLE) >= 1 ||
               cv::getWindowProperty("Original Image", cv::WND_PROP_VISIBLE) >= 1) {
            cv::waitKey(1);  // Keep refreshing the OpenCV windows
        }
    }
}

void compareImages(const cv::Mat& img1, const cv::Mat& img2) {
    auto start = std::chrono::high_resolution_clock::now();

    cv::Mat resizedImg1;
    cv::resize(img1, resizedImg1, img2.size());


    cv::Mat diff;
    cv::absdiff(resizedImg1, img2, diff);

    double mse = cv::norm(resizedImg1 - img2, cv::NORM_L2) / (img2.total() * img2.channels());
    std::cout << "Mean Squared Error (MSE): " << mse << std::endl;

    double psnr = 10 * log10((255 * 255) / mse);
    std::cout << "Peak Signal-to-Noise Ratio (PSNR): " << psnr << " dB" << std::endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Task timer: " << duration.count() << " seconds" << std::endl;

    if(displayFlag){
        cv::imshow("Difference Image", diff);
        while (cv::getWindowProperty("Difference Image", cv::WND_PROP_VISIBLE) >= 1)
        {
            cv::waitKey(1);
        }
    }

    if(saveFlag)
    {
        cv::imwrite("./output/difference_image.ppm", diff);
        std::cout << "Quantized image saved as difference_image.ppm." << std::endl;
    }
}


// Function to quantize a grayscale image
void quantizeImage(const cv::Mat& image, int levels) {
    auto start = std::chrono::high_resolution_clock::now();
    if (levels < 2) {
        std::cerr << "Quantization levels must be at least 2.\n";
        return;
    }

    cv::Mat quantizedImage = image.clone();


    int step = 256 / levels;
    for (int i = 0; i < quantizedImage.rows; ++i) {
        for (int j = 0; j < quantizedImage.cols; ++j) {
            uchar pixel = quantizedImage.at<uchar>(i, j);
            int quantizedPixel = (pixel / step) * step + step / 2; // Center the quantized value
            quantizedPixel = std::min(quantizedPixel, 255); // Ensure it's within range
            quantizedImage.at<uchar>(i, j) = static_cast<uchar>(quantizedPixel);
        }
    }

    double mse = cv::norm(image - quantizedImage, cv::NORM_L2) / (image.total());
    std::cout << "Mean Squared Error (MSE): " << mse << std::endl;
    double psnr = 10 * log10((255 * 255) / mse);
    std::cout << "Peak Signal-to-Noise Ratio (PSNR): " << psnr << " dB" << std::endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Task timer: " << duration.count() << " seconds" << std::endl;

    if (displayFlag) {
        cv::imshow("Original Grayscale Image", image);
        cv::imshow("Quantized Image", quantizedImage);

        // Keep OpenCV GUI alive until all windows are closed
        while (cv::getWindowProperty("Original Grayscale Image", cv::WND_PROP_VISIBLE) >= 1 ||
               cv::getWindowProperty("Quantized Image", cv::WND_PROP_VISIBLE) >= 1) {
            cv::waitKey(1);  // Keep refreshing the OpenCV windows
        }
    }

    if (saveFlag) {
        cv::imwrite("./output/quantized_image.ppm", quantizedImage);
        std::cout << "Quantized image saved as quantized_image.ppm." << std::endl;
    }
}