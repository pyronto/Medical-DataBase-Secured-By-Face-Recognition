#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
using namespace std;
int main() {
    // storing the absolute path of the Haar Cascade File in a string
    std::string face_cascade_name = "/Users/harshitnimesh/Desktop/opencv/haarcascade_frontalface_default.xml";
    cv::CascadeClassifier face_cascade; // face_cascade is an object of CascadeClassifier class in OpenCV

    //Checks whether the Haar Cascade File can be loaded in the face_cascade object
    if (!face_cascade.load(face_cascade_name)) {
        std::cerr << "Error loading face cascade\n";
        return -1;
    }
    
    // Open the default camera
    cv::VideoCapture cap(0); // cap is an object of VideoCapture class in OpenCV Library. 0 refers to the default camera.
    //Check whether the default camera opens or not.
    if (!cap.isOpened()) {
        std::cerr << "Error opening video stream\n";
        return -1;
    }

    std::ofstream outfile("face_detection_data.txt");// Here it simple opens the file to write in it if it is not created and then first it created and then opened for writing in it.

    // no of clock ticks since a reference time
    double start_time = cv::getTickCount();
    double duration = 20.0;  // This is the duration for which the user's face would be captured.

    while (true) {
        //Mat is the primary structure for storing and manipulating image data
        cv::Mat frame; // create an object frame of the class Mat in OpenCV
        cap >> frame;  // cap meant for capturing an instance of the frame 

        //Checks whether a valid frame is captured or not.
        if (frame.empty())
            break;  // If no frame is captured, break the loop

        // Convert the image to gray scale
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray, gray);

        // Detect faces
        std::vector<cv::Rect> faces;
        face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

        // Save face detection data
        for (size_t i = 0; i < faces.size(); i++) {
            outfile << "Frame " << cap.get(cv::CAP_PROP_POS_FRAMES) << " - Face " << i + 1 << ": "
                    << "x=" << faces[i].x << ", "
                    << "y=" << faces[i].y << ", "
                    << "width=" << faces[i].width << ", "
                    << "height=" << faces[i].height << std::endl;
            cv::rectangle(frame, faces[i], cv::Scalar(255, 0, 255), 4);
        }

        // Display the resulting frame
        cv::imshow("Face Detection", frame);

        // Check if 20 seconds have passed
        double current_time = (cv::getTickCount() - start_time) / cv::getTickFrequency();
        if (current_time > duration) {
            break;
        }

        // Break the loop on 'q' key press
        if (cv::waitKey(30) >= 0)
            break;
    }

    // Release the video capture object
    cap.release();
    outfile.close();

    // Close all OpenCV windows
    cv::destroyAllWindows();

    return 0;
}
