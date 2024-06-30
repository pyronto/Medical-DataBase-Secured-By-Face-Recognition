#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>


int main() {
    // Provide the absolute path to the Haar Cascade XML file
    std::string face_cascade_name = "/Users/harshitnimesh/Desktop/opencv/haarcascade_frontalface_default.xml";
    cv::CascadeClassifier face_cascade;

    if (!face_cascade.load(face_cascade_name)) {
        std::cerr << "Error loading face cascade\n";
        return -1;
    }

    // Open the default camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video stream\n";
        return -1;
    }

    std::ifstream infile("face_detection_data.txt");
    if (!infile.is_open()) {
        std::cerr << "Error opening face detection data file\n";
        return -1;
    }

    std::string line;
    bool match_found = false;

    // Read face detection data from file
    while (std::getline(infile, line)) {
        // Parse the line to extract face coordinates
        int frame_number, face_number, x, y, width, height;
        if (std::sscanf(line.c_str(), "Frame %d - Face %d: x=%d, y=%d, width=%d, height=%d",
                        &frame_number, &face_number, &x, &y, &width, &height) == 6) {

            // Read the current frame from camera
            cv::Mat frame;
            cap >> frame;

            if (frame.empty())
                break;

            // Convert the image to gray scale
            cv::Mat gray;
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
            cv::equalizeHist(gray, gray);

            // Detect faces
            std::vector<cv::Rect> faces;
            face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

            // Check for matches with saved face data
            for (size_t i = 0; i < faces.size(); i++) {
                // Compare current face detection with saved data
                if (std::abs(faces[i].x - x) < 20 && std::abs(faces[i].y - y) < 20 &&
                    std::abs(faces[i].width - width) < 20 && std::abs(faces[i].height - height) < 20) {
                    std::cout << "Match found!\n";
                    match_found = true;
                    break;
                }
            }

            // Display the resulting frame
            cv::rectangle(frame, cv::Point(x, y), cv::Point(x + width, y + height), cv::Scalar(255, 0, 255), 4);
            cv::imshow("Face Recognition", frame);
            cv::waitKey(30);

            // Break the loop if match is found
            if (match_found)
                break;
        }
    }

    // Close the face detection data file
    infile.close();

    // Display no match found if match not found within 1 minute
    if (!match_found) {
        std::cout << "No match found within 1 minute.\n";
        cv::waitKey(60000);  // Wait for 1 minute before closing
    }

    // Release the video capture object
    cap.release();

    // Close all OpenCV windows
    cv::destroyAllWindows();

    return 0;
}
