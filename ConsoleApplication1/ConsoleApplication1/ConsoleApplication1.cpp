#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <windows.h> // For Windows API functions

using namespace std;
using namespace cv;

// Function to get the current timestamp as a string
string getCurrentTimestamp() {
    time_t now = time(0);
    tm ltm;
    localtime_s(&ltm, &now); // Use localtime_s for thread safety and security
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &ltm);
    return string(buffer);
}

int main() {
    Mat img;
    VideoCapture cap(0); // Open the default camera

    if (!cap.isOpened()) {
        cerr << "Error: Could not open camera. Please check if the camera is connected and not used by other applications." << endl;
        return -1;
    }

    CascadeClassifier plateCascade;
    if (!plateCascade.load("haarcascade_russian_plate_number.xml")) {
        cerr << "Error: Could not load classifier file. Please ensure the file path is correct." << endl;
        return -1;
    }

    vector<Rect> plates;
    int plateID = 1; // Initialize plate ID

    // Define the directory to save images
    string saveDir = "haarcascade_russian_plate_number";

    // Open the log file for writing
    ofstream logFile("haarcascade_russian_plate_number/haarcascade_russian_plate_number.txt", ios::app); // Append mode
    if (!logFile.is_open()) {
        cerr << "Error: Could not open file.txt for writing." << endl;
        return -1;
    }

    while (true) {
        cap.read(img);
        if (img.empty()) {
            cerr << "Error: Could not capture frame from camera." << endl;
            break;
        }

        plateCascade.detectMultiScale(img, plates, 1.1, 10);

        for (int i = 0; i < plates.size(); i++) {
            Mat imgCrop = img(plates[i]);

            // Create a unique filename for the image
            string timestamp = getCurrentTimestamp();
            string imagePath = saveDir + "\\plate_" + to_string(plateID) + ".png";

            // Save the cropped image of the license plate
            imwrite(imagePath, imgCrop);

            // Log details to file.txt
            logFile << "ID: " << plateID << ", Timestamp: " << timestamp << ", Image Path: " << imagePath << endl;

            // Draw a rectangle around the detected plate
            rectangle(img, plates[i].tl(), plates[i].br(), Scalar(255, 0, 255), 3);

            plateID++;
        }

        imshow("Image", img);
        char key = waitKey(1);

        if (key == 'q') {
            cout << "Exiting the program." << endl;
            break;
        }
    }

    // Close the log file
    logFile.close();

    cap.release();
    destroyAllWindows();
    return 0;
}
