/*
    Author: Diana Anton
    Date: 20.03.2022
*/
#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>

using namespace std;
//----------------------------------------------------------------------------//
/*
    encryption(string message, int key) - Encrypt a given message with the 
provided letter shift.
*/
string encryption(string message, int key)
{
    string encrypted = "";
    
    for (int i = 0; i < message.length(); i++)
    {
        if (message[i] == ' ')
        {
            encrypted += ' ';
        }
        else if (isupper(message[i]))
        {
            encrypted += char( int(message[i] + key - 65) % 26 + 65 );
        }
        else
        {
            encrypted += char( int(message[i] + key - 97) % 26 + 97 );
        }
    }
    return encrypted;
}
//----------------------------------------------------------------------------//
/*
    decryption(string message, int key) - Decrypt a given message with the 
provided letter shift.
*/
string decryption(string message, int key)
{
    string decrypted = "";

    for (int i = 0; i < message.length(); i++)
    {
        if (message[i] == ' ')
        {
            decrypted += ' ';
        }
        else if (isupper(message[i]))
        {
            decrypted += char( int(message[i] - key - 65 + 26) % 26 + 65 );
        }
        else
        {
            decrypted += char( int(message[i] - key - 97 + 26) % 26 + 97 );
        }
    }
    return decrypted;
}
//----------------------------------------------------------------------------//
int main() {
    // declaring the needed variables
    int shift;
    string plaintext, ciphertext, shift_str, encrypted, decrypted, brute_force;
    // setting files for read/write
    ifstream shift_file("shift.txt");
    ifstream plaintext_file("plaintext.txt");
    ofstream ciphertext_file("ciphertext.txt");
    ofstream decrypted_file("decrypted.txt");
    ofstream bruteforce_file("brute-force.txt");

    // reading the shift
    getline(shift_file, shift_str);
    shift = atoi(shift_str.c_str());
    // reading the plaintext
    getline(plaintext_file, plaintext);
    cout << "Shift: " << shift << endl;
    cout << "Plaintext: " << plaintext << endl << endl;

    // ecnrypt the message
    encrypted = encryption(plaintext, shift);
    cout << "Encrypted message: " << encrypted << endl << endl;
    ciphertext_file << encrypted;

    // decrypt the message
    decrypted = decryption(encrypted, shift);
    cout << "Decrypted message: " << decrypted << endl << endl;
    decrypted_file << decrypted;

    cout << "Brute Force the message" << endl;
    // brute-force the message
    for (int key = 1; key <= 25; key++)
    { // every shift variant will be tried and printed on the screen and in the
     // file for the user to pick the correct version
        brute_force = decryption(encrypted, key);
        cout << "Key is: " << key << " and the decrypted message is: " 
             << brute_force << endl;
        bruteforce_file << "Key: " << key << " Message: " << brute_force << endl;
        if (plaintext.compare(brute_force) == 0)
        {
            cout << endl << "Found the message: " << brute_force << endl;
            bruteforce_file << endl << "Found the message: " << brute_force 
                            << endl;
            break;
        }
    }
    // close the files
    shift_file.close(); plaintext_file.close(); ciphertext_file.close();
    decrypted_file.close(); bruteforce_file.close();

    return 0;
}
//----------------------------------------------------------------------------//