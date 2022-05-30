#include <iostream>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <stdint.h>
using namespace std;
#define E 2.71828182845904523536
//----------------------------------------------------------------------------//
unsigned int word_size = 32, rounds = 20, key_len_bytes = 16, log_word_size;
int64_t modulo;
string mode, plaintext, key;
unsigned int *store, *len;
//----------------------------------------------------------------------------//
void rc6_constraints(unsigned int &word_size, unsigned int &p, unsigned int &q)
{
    // cout << "(e-2): " << E - 2 << endl;
    // cout << "pow(2, wordsize): " << E - 2 << endl;
    p = (unsigned int) ceil(((E - 2) * pow(2, word_size)));
    // cout << "p " << p << endl;
    // Golden Ratio
    q = (unsigned int) ((1.618033988749895 - 1) * pow(2, word_size));
    // cout << "q " << q << endl;
}
//----------------------------------------------------------------------------//
int rotate_left(unsigned int first, unsigned int second, unsigned int word_size)
{
    second <<= word_size - log_word_size;
    second >>= word_size - log_word_size;
    return (first << second) | (first >> (word_size + second));
}
//----------------------------------------------------------------------------//
int rotate_right(unsigned int first, unsigned int second, unsigned int word_size)
{
    second <<= word_size - log_word_size;
    second >>= word_size - log_word_size;
    return (first >> second) | (first << (word_size + second));
}
//----------------------------------------------------------------------------//
// convert the string to little endian format
string little_endian(string text)
{
    string endian_text;

    if (text.length() % 2 == 0)
    {
        // cout << "little_endian(): text " << text << endl;
        for (string::reverse_iterator it = text.rbegin(); it != text.rend(); it++)
        {

            // cout << "little_endian(): it " << *it << endl;
            endian_text.push_back(*it + 1);
            endian_text.push_back(*it);
        }
    }
    else
    {
        // add padding
        text = "0" + text;
        // cout << "little_endian(): text " << text << endl;
        for (string::reverse_iterator it = text.rbegin(); it != text.rend(); it++)
        {
            // cout << "little_endian(): it " << *it << endl;
            endian_text.push_back(*it + 1);
            endian_text.push_back(*it);
        }
    }
    // cout << "little_endian(): endian_text " << endian_text << endl;
    return endian_text;
}
//----------------------------------------------------------------------------//
string hex_to_string(unsigned int first, unsigned int second, 
                     unsigned int third, unsigned int fourth)
{
    string first_str, second_str, third_str, fourth_str, result;
    stringstream ss;

    ss << setfill('0') << setw(4) << hex << first;
    first_str = little_endian(ss.str());
    ss.str(""); ss.clear();

    ss << setfill('0') << setw(4) << hex << second;
    second_str = little_endian(ss.str());
    ss.str(""); ss.clear();

    ss << setfill('0') << setw(4) << hex << third;
    third_str = little_endian(ss.str());
    ss.str(""); ss.clear();

    ss << setfill('0') << setw(4) << hex << fourth;
    fourth_str = little_endian(ss.str());
    ss.str(""); ss.clear();

    result = first_str + second_str + third_str + fourth_str;
    return result;
}
//----------------------------------------------------------------------------//
void key_schedule(string key)
{
    unsigned int word_bytes = ceil((float) word_size / 8);
    // cout << "word_bytes " << word_bytes << endl;
    unsigned int count = ceil((float) key_len_bytes / word_bytes);
    // cout << "count " << count << endl;
    int v = 3 * max(count, (2 * rounds + 4));
    // cout << "v " << v << endl;
    unsigned int p, q, first, second;

    // cout << 'before rc6_\n';
    rc6_constraints(word_size, p, q);
    // cout << 'after rc6_\n';

    len = new unsigned int[count];
    for (int i = 0; i < count; i++)
    {
        // cout << "key_sch(): i = " << i << endl;
        len[i] = strtoul(little_endian(
            key.substr(word_bytes * 2 * i, word_bytes * 2)).c_str(), NULL, 16);
    }

    store[0] = p;
    // cout << "key_sch(): store[0] " << store[0] << endl;
    for (int i = 1; i <= (2 * rounds + 3); i++)
    {
        // cout << "key_sch(): i = " << i << endl;
        store[i] = (store[i - 1] + q) % modulo;
        // cout << "key_sch(): store[i] " << store[i] << endl;
    }

    unsigned int i = 0, j = 0;
    for (int k = 1; k <= v; k++)
    {
        first = store[i] = rotate_left((store[i] + first + second) % modulo, 
                                        3, word_size);
        second = len[j] = rotate_left((len[j] + first + second) % modulo, 
                                        (first + second), word_size);
        i = (i + 1) % (2 * rounds + 4);
        j = (j + 1) % count;
    }
}
//----------------------------------------------------------------------------//
string encryption(string plaintext)
{
    string encrypted;
    int32_t trial, upper, aux;
    unsigned int first  = strtoul(little_endian(plaintext.substr(0, 8)).c_str(), 
                            NULL, 16);
    unsigned int second = strtoul(little_endian(plaintext.substr(8, 8)).c_str(), 
                            NULL, 16);
    unsigned int third = strtoul(little_endian(plaintext.substr(16, 8)).c_str(), 
                            NULL, 16);
    unsigned int fourth = strtoul(little_endian(plaintext.substr(24, 8)).c_str(), 
                            NULL, 16);

    second += store[0];
    fourth += store[1];
    for (int i = 1; i <= rounds; i++)
    {
        trial = rotate_left((second * (2 * second + 1)) % modulo, 
                            log_word_size, word_size);
        upper = rotate_left((fourth * (2 * fourth + 1)) % modulo, 
                            log_word_size, word_size);                            
        first = rotate_left((first ^ trial), upper, word_size) + store[2 * i];
        third = rotate_left((third ^ upper), trial, word_size) + store[2 * i + 1];

        aux = first; first = second; second = third; third = fourth; fourth = aux;
    }
    first += store[2 * rounds + 2];
    third += store[2 * rounds + 3];
    encrypted = hex_to_string(first, second, third, fourth);

    return encrypted;
}
//----------------------------------------------------------------------------//
string decryption(string ciphertext)
{
    string decrypted;
    unsigned int trial, upper, aux;
    unsigned int first  = strtoul(little_endian(ciphertext.substr(0, 8)).c_str(), 
                            NULL, 16);
    unsigned int second = strtoul(little_endian(ciphertext.substr(8, 8)).c_str(), 
                            NULL, 16);
    unsigned int third = strtoul(little_endian(ciphertext.substr(16, 8)).c_str(), 
                            NULL, 16);
    unsigned int fourth = strtoul(little_endian(ciphertext.substr(24, 8)).c_str(), 
                            NULL, 16);

    // cout << "store[2 * rounds + 3] " << store[2 * rounds + 3] << endl;
    // cout << "store[2 * rounds + 2] " << store[2 * rounds + 2] << endl;
    third -= store[2 * rounds + 3];
    first -= store[2 * rounds + 2];
    // cout << "third " << third << endl;
    // cout << "first " << first << endl;
    for (int i = rounds; i >= 1; i--)
    {
        aux = fourth; fourth = third; third = second; second = first; first = aux;

        upper = rotate_left((fourth * (2 * fourth +1)) % modulo, 
                            log_word_size, word_size);
        trial = rotate_left((second * (2 * second + 1)) % modulo, 
                            log_word_size, word_size);
        third = rotate_right((third - store[2 * i + 1]) % modulo, trial, 
                            word_size) ^ upper;
        first = rotate_right((first - store[2 * i]) % modulo, upper, 
                            word_size) ^ trial;
    }
    fourth -= store[1];
    second -= store[0];
    decrypted = hex_to_string(first, second, third, fourth);

    return decrypted;
}
//----------------------------------------------------------------------------//
void run_rc6()
{
    ifstream plaintext_file("./rc6/plaintext.txt");
    ifstream key_file("./rc6/key.txt");
    ofstream ciphertext_file("./rc6/ciphertext.txt");
    ofstream decrypt_file("./rc6/decrypted.txt");

    string plaintext, key, encrypted, decrypted, aux;
    int size = 2 * rounds + 4;
    store = new unsigned int[size];
    log_word_size = (unsigned int) log2(word_size);
    modulo = pow(2, word_size);

    // cout << "modulo " << modulo << endl;
    // cout << "log_word_size " << log_word_size << endl;
    // cout << "size " << size << endl;

    if (plaintext_file.is_open())
    {
        getline(plaintext_file >> ws, plaintext);
    }
    else
    {
        cout << "plaintext not open\n";
    }
    getline(key_file >> ws, key);

    cout << "Plaintext: " << plaintext << endl; 
    // cout << 'before key_sche\n';
    cout << "Key: " << key << endl;

    // remove whitespaces
    plaintext.erase(std::remove_if(plaintext.begin(), 
                              plaintext.end(),
                              [](unsigned char x){return std::isspace(x);}),
               plaintext.end());
    key.erase(std::remove_if(key.begin(), 
                              key.end(),
                              [](unsigned char x){return std::isspace(x);}),
               key.end());

    cout << "Plaintext: " << plaintext << endl; 
    // cout << 'before key_sche\n';
    cout << "Key: " << key << endl;
    // cout << 'before key_sche\n';
    key_schedule(key);
    // cout << 'after key_sche\n';
    
    aux = encryption(plaintext);
    for (string::iterator it = aux.begin(); it != aux.end(); it += 2)
    {
        encrypted.push_back(*it);
        encrypted.push_back(*(it + 1));
        encrypted += " ";
    }
    cout << "Encrypted: " << encrypted << endl;
    encrypted.erase(std::remove_if(encrypted.begin(), 
                              encrypted.end(),
                              [](unsigned char x){return std::isspace(x);}),
               encrypted.end());

    aux = decryption(encrypted);
    for (string::iterator it = aux.begin(); it != aux.end(); it += 2)
    {
        decrypted.push_back(*it);
        decrypted.push_back(*(it + 1));
        decrypted += " ";
    }
    cout << "Decrypted: " << decrypted << endl;
    plaintext_file.close(); key_file.close(); ciphertext_file.close();
    decrypt_file.close();
}
//----------------------------------------------------------------------------//
int main()
{
    run_rc6();
}