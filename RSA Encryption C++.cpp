// RSA Encryption C++.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <iomanip>  // For setw and setfill
#include <string>
#include <cmath>
#include <sstream>      // std::stringstream

using namespace std;

long long modExpo(long long base, long long exp, long long mod) {
    // Function to perform modular exponentiation
    long long result = 1;       // Initialize result
    base = base % mod;          // Update base if it is more than mod

    while (exp > 0) {
        // If exp is odd, multiply base with result
        if (exp % 2 == 1) {
            result = (result * base) % mod; // Prevent overflow
        }

        // Now exp must be even, divide it by 2
        exp = exp >> 1;          // Equivalent to exp = exp / 2
        base = (base * base) % mod; // Square the base
    }

    return result;
}

int extendedGCD(int a, int b, int& x, int& y) {
    // Extended Euclidean Algorithm
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    int x1, y1; // Temporary variables
    int gcd = extendedGCD(b, a % b, x1, y1);

    // Update x and y using results from recursion
    x = y1;
    y = x1 - (a / b) * y1;

    return gcd;
}

bool findE(int maybeE, int p, int q) {
    bool flag = true;
    int x, y; // dummy var
    int phi = (p - 1) * (q - 1);
    if (extendedGCD(maybeE, (p - 1), x, y) != 1) flag = false;
    if (extendedGCD(maybeE, (q - 1), x, y) != 1) flag = false;
    if (extendedGCD(maybeE, phi, x, y) != 1) flag = false;
    //cout << "flag: " << flag << endl;
    return flag;
}

int findD(int e, int phi) {
    int x, y; // Coefficients for the extendedGCD
    int gcd = extendedGCD(e, phi, x, y);

    // If e and phi are not coprime, no modular inverse exists
    if (gcd != 1) {
        cout << "Modular inverse does not exist!" << endl;
        return -1;
    }

    // Make x positive if necessary
    if (x < 0) {
        x += phi;
    }

    return x; // x is the modular inverse of e mod phi
}

string convertToNumeric(const string& input) {
    string numericString = "";

    for (char c : input) {
        if (isalpha(c)) {
            // Convert lowercase and uppercase letters to numbers (1-26)
            int numericValue = tolower(c) - 'a' + 1;

            // Add numeric value as two digits (01, 02, ..., 26)
            numericString += (numericValue < 10 ? "0" : "") + to_string(numericValue);
        }
        else if (c == ' ') {
            // Convert space to 32
            numericString += "32";
        }
    }

    return numericString;
}

string convertToLetters(const string& input) {
    string letterString = "";

    // Process the input two characters at a time
    for (size_t i = 0; i < input.length(); i += 2) {
        // Extract two digits as a substring
        string twoDigitStr = input.substr(i, 2);

        // Convert the two-digit string to an integer
        int numericValue = stoi(twoDigitStr);

        // Convert space (32) back to the space character
        if (numericValue == 32) {
            letterString += ' ';
        }
        // Convert numeric values (1-26) back to corresponding letters (a-z)
        else if (numericValue >= 1 && numericValue <= 26) {
            char letter = 'a' + (numericValue - 1);
            letterString += letter;
        }
    }

    return letterString;
}

int charToNumeric(char c) {
    // Convert character to a numeric value
    if (isalpha(c)) {
        return tolower(c) - 'a' + 1; // 1-26 for a-z
    }
    else if (c == ' ') {
        return 32; // Space is represented as 32
    }
    return 0; // For characters not handled, return 0
}

void encryption() {
    // Variables
    int p, q, phi, n, e = 0, d = 0;
    string userMessage = "";
    long long encryptedValue;
    int primeNumbers[25] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97 };

    // Prompt and input
    cout << "You've chosen Encryption" << endl;
    cout << "Pick two prime numbers p and q from the list." << endl;
    for (int i = 0; i < 25; i++) {
        cout << primeNumbers[i] << ' ';
    }
    cout << endl;
    cin >> p >> q;

    // Calculate n and phi
    n = p * q;
    phi = (p - 1) * (q - 1);

    // Find a valid e
    for (int i = 0; i < 25; i++) {
        if (findE(primeNumbers[i], p, q)) {
            e = primeNumbers[i];
            break;
        }
    }

    if (e == 0) {
        cout << "No valid e found. Exiting." << endl;
        return;
    }

    // Find d, the modular inverse of e mod phi
    d = findD(e, phi);
    if (d == -1) return; // Modular inverse not found

    // Prompt for message
    cout << "Enter message to encrypt" << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clears out the input buffer
    getline(cin, userMessage);

    cout << endl << "Public key: n = " << n << ", d = " << d << endl;

    // Encrypt each character and store encrypted values
    string encryptedMessage;
    for (char c : userMessage) {
        int numericValue = charToNumeric(c); // Get numeric value of character
        if (numericValue > 0) { // Ensure it's a valid character
            encryptedValue = modExpo(numericValue, e, n); // Encrypt using modExpo
            encryptedMessage += to_string(encryptedValue) + " ";
        }
        //cout << "Original numeric: " << numericValue << ", Encrypted: " << encryptedValue << endl;
    }
    cout << "Encrypted: " << encryptedMessage << endl << endl;
}

void decryption() {
    int n = 0, d = 0;
    long long decryptedMessage;
    string encryptedStringMessage;

    // Prompt and input
    cout << "You've chosen Decryption" << endl << "Please provide the public key n and d." << endl;
    cin >> n >> d;
    cout << "Please provide the encrypted message (space-separated values)" << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clears out the input buffer
    getline(cin, encryptedStringMessage);

    // Split the encrypted message by spaces and decrypt each part
    stringstream ss(encryptedStringMessage);
    string number;
    string decryptedStringMessage;

    while (ss >> number) {
        long long encryptedMessage = stoll(number);
        decryptedMessage = modExpo(encryptedMessage, static_cast<long long>(d), static_cast<long long>(n));

        // Wrap within 1-26 alphabet or 32 for space
        if (decryptedMessage > 26 && decryptedMessage != 32) {
            decryptedMessage = decryptedMessage % 27;
        }

        cout << "Adjusted decrypted numeric: " << decryptedMessage << endl; // For verification
        decryptedStringMessage += (decryptedMessage < 10 ? "0" : "") + to_string(decryptedMessage);
    }
    decryptedStringMessage = convertToLetters(decryptedStringMessage);

    cout << "message '" << decryptedStringMessage << "'" << endl << endl;
}

int main()
{
    cout << "Welcome to RSA. Developed by Matthew Dunfee." << endl;
    char userResponse;
    while (1) {
        cout << "Please enter 'a' for encryption, 'b' for decryption, or 'z' to quit." << endl;
        cin >> userResponse;
        switch (userResponse)
        {
        case 'a':
            encryption();
            break;
        case 'b':
            decryption();
            break;
        case 'z':
            return 0;
        default:
            cout << "bad input";
            break;
        }
    }
}

