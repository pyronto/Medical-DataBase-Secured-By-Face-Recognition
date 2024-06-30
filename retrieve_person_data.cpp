#include <opencv2/opencv.hpp>
#include <iostream>
#include <hiredis/hiredis.h>
#include <fstream>
#include <chrono> // For timing
#include <sstream>
#include <vector>

using namespace std;

// Function to match faces using ORB descriptors or other techniques

std::string removeQuotes(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c != '"') {
            result += c;
        }
    }
    return result;
}
bool matchFaces(const cv::Mat& face1, const cv::Mat& face2) {
    cv::Ptr<cv::ORB> orb = cv::ORB::create();
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat descriptors1, descriptors2;
    
    orb->detectAndCompute(face1, cv::Mat(), keypoints1, descriptors1);
    orb->detectAndCompute(face2, cv::Mat(), keypoints2, descriptors2);

    cv::BFMatcher matcher(cv::NORM_HAMMING);
    std::vector<cv::DMatch> matches;
    matcher.match(descriptors1, descriptors2, matches);

    // Simple matching criteria: at least 10 matches
    return matches.size() > 10;
}

// Function to retrieve person data from Redis
std::string retrievePersonData(redisContext* c, const std::string& key) {
    redisReply* reply = (redisReply*)redisCommand(c, "GET %s", key.c_str());
    if (reply == NULL || reply->type != REDIS_REPLY_STRING) {
        std::cerr << "Error: Key '" << key << "' not found in Redis" << std::endl;
        return "";
    }
    std::string personData = reply->str;
    freeReplyObject(reply);
    return personData;
}

int main() {
    // Connect to Redis server running on localhost
    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        if (c) {
            std::cerr << "Error: " << c->errstr << std::endl;
            redisFree(c);
        } else {
            std::cerr << "Failed to connect to Redis server" << std::endl;
        }
        return -1;
    }

    // Prompt for unique ID (key)
    std::string uniqueID;
    std::cout << "Enter unique ID (key) to retrieve person data: ";
    std::cin >> uniqueID;

    // Retrieve person data associated with the unique ID
    std::string storedPersonData = retrievePersonData(c, uniqueID);
    if (storedPersonData.empty()) {
        std::cerr << "No data found for unique ID: " << uniqueID << std::endl;
        redisFree(c);
        return -1;
    }

    // Extract individual fields from stored data (assuming comma-separated format)
    istringstream ss(storedPersonData);
    string name, ageStr, country, state, city, phone, recentCheckUp, recentMedication;
    string surgery, onGoingMedication, currentHealthIssue, allergicMedicines, emergencyContact;
    std::getline(ss, name, '\n');
    std::getline(ss, ageStr, '\n');
    std::getline(ss, country, '\n');
    std::getline(ss, state, '\n');
    std::getline(ss, city, '\n');
    std::getline(ss, phone, '\n');
    std::getline(ss, recentCheckUp, '\n');
    std::getline(ss, recentMedication, '\n');
    std::getline(ss, surgery, '\n');
    std::getline(ss, onGoingMedication, '\n');
    std::getline(ss, currentHealthIssue, '\n');
    std::getline(ss, allergicMedicines, '\n');
    std::getline(ss, emergencyContact, '\n');

    name = removeQuotes(name);
    ageStr = removeQuotes(ageStr);
    country = removeQuotes(country);
    state = removeQuotes(state);
    city = removeQuotes(city);
    phone = removeQuotes(phone);
    recentCheckUp = removeQuotes(recentCheckUp);
    recentMedication = removeQuotes(recentMedication);
    surgery = removeQuotes(surgery);
    onGoingMedication = removeQuotes(onGoingMedication);
    currentHealthIssue = removeQuotes(currentHealthIssue);
    allergicMedicines = removeQuotes(allergicMedicines);
    emergencyContact = removeQuotes(emergencyContact);

    // Open the default camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video stream\n";
        return -1;
    }

    // Initialize variables for face recognition
    cv::CascadeClassifier face_cascade;
    if (!face_cascade.load("/Users/harshitnimesh/Desktop/opencv/haarcascade_frontalface_default.xml")) {
        std::cerr << "Error loading face cascade\n";
        return -1;
    }

    // Capture start time
    auto startTime = std::chrono::steady_clock::now();
    constexpr int captureDurationSeconds = 10; // Capture duration in seconds

    bool matchFound = false;

    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - startTime).count() < captureDurationSeconds && !matchFound) {
        cv::Mat frame;
        cap >> frame;  // Capture frame-by-frame

        if (frame.empty())
            break;  // If no frame is captured, break the loop

        // Convert the frame to grayscale
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray, gray);

        // Detect faces
        std::vector<cv::Rect> faces;
        face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

        // Process each detected face
        for (const auto& face : faces) {
            cv::Mat faceROI = gray(face);

            if (matchFaces(faceROI, gray)) {
                cout << "Match found!\n";
                cout << name << endl;
                cout << ageStr << endl;
                cout << country << endl;
                cout << state << endl;
                cout << city << endl;
                cout << phone << endl;
                cout << recentCheckUp << endl;
                cout << recentMedication << endl;
                cout << surgery << endl;
                cout << onGoingMedication << endl;
                cout << currentHealthIssue << endl;
                cout << allergicMedicines << endl;
                cout << emergencyContact << endl;
                matchFound = true;
                break; // Exit face detection loop
            }
        }

        // Display the frame with rectangles around detected faces
        for (const auto& face : faces) {
            cv::rectangle(frame, face, cv::Scalar(255, 0, 255), 4);
        }
        cv::imshow("Face Recognition", frame);

        // Exit on 'q' key press
        if (cv::waitKey(30) == 'q') {
            break;
        }
    }

    // If no match found after capturing for 10 seconds
    if (!matchFound) {
        std::cout << "No match found for the provided face data." << std::endl;
    }

    // Release resources
    cap.release();
    cv::destroyAllWindows();
    redisFree(c);

    return 0;
}
