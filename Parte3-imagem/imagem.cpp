#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

// Function declarations
void calculateHistogram(const cv::Mat& image);
void showChannels(const cv::Mat& image);
void applyGaussianFilter(const cv::Mat& image);
void processDataset(const std::string& datasetPath);
void compareImages(const cv::Mat& img1, const cv::Mat& img2);

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [options] [image_path] or [dataset_path]\n";
        std::cerr << "Options:\n";
        std::cerr << "  --histogram          Calculate the histogram of the image\n";
        std::cerr << "  --channels           Show the RGB channels of the image\n";
        std::cerr << "  --gaussian           Apply Gaussian filter to the image\n";
        std::cerr << "  --compare <image1> <image2>  Compare two images\n";
        std::cerr << "  --dataset <path>     Process a dataset (folder) of images\n";
        return 1;
    }

    // Flags
    bool calcHistogram = false;
    bool showRGBChannels = false;
    bool applyGaussian = false;
    bool compareImagesFlag = false;
    std::string datasetPath;

    cv::Mat image1, image2;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--histogram") {
            calcHistogram = true;
        } else if (arg == "--channels") {
            showRGBChannels = true;
        } else if (arg == "--gaussian") {
            applyGaussian = true;
        } else if (arg == "--compare" && i + 2 < argc) {
            compareImagesFlag = true;
            image1 = cv::imread(argv[++i], cv::IMREAD_COLOR);
            image2 = cv::imread(argv[++i], cv::IMREAD_COLOR);
        } else if (arg == "--dataset" && i + 1 < argc) {
            datasetPath = argv[++i];
        } else {
            std::string imagePath = argv[i];

            // Load the image
            cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
            if (image.empty()) {
                std::cerr << "Could not open or find the image: " << imagePath << "\n";
                return 1;
            }

            if (calcHistogram) {
                calculateHistogram(image);
            }
            if (showRGBChannels) {
                showChannels(image);
            }
            if (applyGaussian) {
                applyGaussianFilter(image);
            }
        }
    }

    // If a dataset path is provided, process the dataset
    if (!datasetPath.empty()) {
        processDataset(datasetPath);
    }

    // Compare images if the flag is set
    if (compareImagesFlag) {
        if (image1.empty() || image2.empty()) {
            std::cerr << "One or both images could not be loaded for comparison.\n";
            return 1;
        }
        compareImages(image1, image2);
    }

    return 0;
}

void calculateHistogram(const cv::Mat& image) {
    cv::Mat grayscaleImage;
    cv::cvtColor(image, grayscaleImage, cv::COLOR_BGR2GRAY);

    std::vector<int> grayHist(256, 0);
    for (int i = 0; i < grayscaleImage.rows; ++i) {
        for (int j = 0; j < grayscaleImage.cols; ++j) {
            int intensity = grayscaleImage.at<uchar>(i, j);
            grayHist[intensity]++;
        }
    }

    std::cout << "\nGrayscale Histogram (Raw Values):\n";
    for (int i = 0; i < 256; ++i) {
        std::cout << "Intensity " << i << ": " << grayHist[i] << "\n";
    }
}

void showChannels(const cv::Mat& image) {
    std::vector<cv::Mat> bgrChannels;
    cv::split(image, bgrChannels);

    cv::Mat grayscaleImage;
    cv::cvtColor(image, grayscaleImage, cv::COLOR_BGR2GRAY);

    cv::imshow("Grayscale Image", grayscaleImage);
    cv::imshow("Blue Channel", bgrChannels[0]);
    cv::imshow("Green Channel", bgrChannels[1]);
    cv::imshow("Red Channel", bgrChannels[2]);
    cv::imshow("Original Image", image);

    cv::waitKey(0);
}

void applyGaussianFilter(const cv::Mat& image) {
    std::vector<int> kernelSizes = {3, 5, 7, 9};
    for (int kernelSize : kernelSizes) {
        cv::Mat blurredImage;
        cv::GaussianBlur(image, blurredImage, cv::Size(kernelSize, kernelSize), 0);
        std::string windowName = "Gaussian Blur (Kernel Size: " + std::to_string(kernelSize) + ")";
        cv::imshow(windowName, blurredImage);
    }
    cv::imshow("Original Image", image);
    cv::waitKey(0);
}

void processDataset(const std::string& datasetPath) {
    for (const auto& entry : fs::directory_iterator(datasetPath)) {
        std::string imagePath = entry.path().string();
        cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
        if (image.empty()) {
            std::cerr << "Could not open or find the image: " << imagePath << "\n";
            continue;
        }
        std::cout << "Processing image: " << imagePath << std::endl;
        calculateHistogram(image);
    }
}

void compareImages(const cv::Mat& img1, const cv::Mat& img2) {
    if (img1.size() != img2.size()) {
        std::cerr << "Error: Images must be of the same size for comparison.\n";
        return;
    }

    cv::Mat diff;
    cv::absdiff(img1, img2, diff);

    double mse = cv::norm(img1 - img2, cv::NORM_L2) / (img1.total() * img1.channels());
    std::cout << "Mean Squared Error (MSE): " << mse << std::endl;

    double psnr = 10 * log10((255 * 255) / mse);
    std::cout << "Peak Signal-to-Noise Ratio (PSNR): " << psnr << " dB" << std::endl;

    cv::imshow("Difference Image", diff);
    cv::waitKey(0);
}
