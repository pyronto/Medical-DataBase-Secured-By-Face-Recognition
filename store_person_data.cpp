#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <hiredis/hiredis.h>
#include <opencv2/opencv.hpp>
#include <uuid/uuid.h>

using namespace std;

// Function to store person's data in Redis
void storePersonData(const std::string& key, const std::string& value) {
    redisContext *c;
    redisReply *reply;

    // Connect to Redis server running on localhost
    c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        if (c) {
            std::cerr << "Error: " << c->errstr << std::endl;
            redisFree(c);
        } else {
            std::cerr << "Failed to connect to Redis server" << std::endl;
        }
        return;
    }

    // Store the data under the specified key
    reply = (redisReply *)redisCommand(c, "SET %s \"%s\"", key.c_str(), value.c_str());
    if (reply == NULL) {
        std::cerr << "Error setting value in Redis" << std::endl;
        redisFree(c);
        return;
    }

    freeReplyObject(reply);
    redisFree(c);
}

std::string generateUUID() {
    uuid_t uuid;
    uuid_generate(uuid);
    char uuidStr[37];
    uuid_unparse(uuid, uuidStr);
    return std::string(uuidStr);
}

int main() {
    // Details to be taken as input from the user
    string name, country, state, city, phone, recentCheckUp, recentMedication;
    string surgery, onGoingMedication, currentHealthIssue, allergicMedicines, emergencyContact, faceData;
    int age;
    
    // Input details from user
    cout << "Enter name: ";
    std::getline(std::cin, name);

    cout << "Enter age: ";
    cin >> age;
    cin.ignore(); // Ignore newline left in buffer

    cout << "Enter country: ";
    std::getline(std::cin, country);

    cout << "Enter state: ";
    std::getline(std::cin, state);

    cout << "Enter city: ";
    std::getline(std::cin, city);

    cout << "Enter phone number: ";
    std::getline(std::cin, phone);

    cout << "Enter recent check-up details: ";
    std::getline(std::cin, recentCheckUp);

    cout << "Enter recent medication details: ";
    std::getline(std::cin, recentMedication);

    cout << "Enter surgery history: ";
    std::getline(std::cin, surgery);

    cout << "Enter on-going medication details: ";
    std::getline(std::cin, onGoingMedication);

    cout << "Enter current health issue: ";
    std::getline(std::cin, currentHealthIssue);

    cout << "Enter allergic to medicines (if any): ";
    std::getline(std::cin, allergicMedicines);

    cout << "Enter emergency contact details: ";
    std::getline(std::cin, emergencyContact);

    // Generate a unique ID for the person
    std::string uniqueID = generateUUID();

    // Inform user to look into the camera for face registration
    std::cout << "Please look into the camera for face registration...\n";

    // Call face_detection.cpp using system command with absolute path
    std::string faceDetectionPath = "/Users/harshitnimesh/Desktop/opencv/build/face_detection";
    int result = std::system(faceDetectionPath.c_str());
    if (result != 0) {
        std::cerr << "Error executing face_detection. Returned error code: " << result << std::endl;
        return 1; // Exit with error
    }

    // Read the face detection data from file
    std::ifstream infile("face_detection_data.txt");
    if (!infile) {
        std::cerr << "Error opening face_detection_data.txt" << std::endl;
        return 1;
    }

    std::getline(infile, faceData);
    infile.close();

    // Construct the value string in table format
    std::string personData =name + "\n" +
       to_string(age) + "\n" +
        country + "\n" +
      state + "\n" +
        city + "\n" +
      phone + "\n" +
       recentCheckUp + "\n" +
        recentMedication + "\n" +
        surgery + "\n" +
        onGoingMedication + "\n" +
        currentHealthIssue + "\n" +
        allergicMedicines + "\n" +
        emergencyContact + "\n"  + faceData;

    // Store all data under the unique ID
    storePersonData(uniqueID, personData);

    std::cout << "Data stored with unique ID: " << uniqueID << std::endl;

    return 0;
}
