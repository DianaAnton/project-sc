#include <iostream>
#include <math.h>
#include <string>
#include <fstream>
#include <boost/multiprecision/cpp_int.hpp>

using namespace std;
using namespace boost::multiprecision;
//----------------------------------------------------------------------------//
#define MAXVALUE 10
#define MINVALUE 1

// public exponent
double encrypt = 2;
// private exponent
double decrypt;
// two random primes
double p, q;
// 
double num;
// 
double phi;
//----------------------------------------------------------------------------//
bool is_prime(int num)
{
    if (num == 0 || num== 1)
    {
        return false;
    }

    for (int i = 2; i <= num / 2; i++)
    {
        if (num % i == 0)
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------//
double generate_prime()
{
    // make them different for each execution
    srand((unsigned) time(0));
    double num;
    while (true)
    {
        // generate random number
        num= (rand() % MAXVALUE) + MINVALUE;
        if (is_prime(num))
        {
            return num;
        }
    }
}
//----------------------------------------------------------------------------//
int greatest_common_divisor(int a, int b)
{
    if ((a % b) == 0)
    {
        return b;
    }
    return greatest_common_divisor(b, (a % b));
}
//----------------------------------------------------------------------------//
double encryption(double plaintext)
{
    double encrypted = pow(plaintext, encrypt);
    encrypted = fmod(encrypted, num);
    return encrypted;
}
//----------------------------------------------------------------------------//
double decryption(double ciphertext)
{
    double decrypted = pow(ciphertext, decrypt);
    decrypted = fmod(decrypted, num);
    return decrypted;
}
//----------------------------------------------------------------------------//
void run_rsa()
{
    // two random primes
    // p = generate_prime(); q = generate_prime();
    // // should not be equal
    // while (true)
    // {
    //     if (p != q)
    //     {
    //         if (greatest_common_divisor(p, q) == 1)
    //         {
    //             break;
    //         }
    //     }
    //     q = generate_prime();
    // }
    // generate the number

    // set the files for read/write
    ifstream plaintext_file("./rsa/plaintext.txt");
    ifstream primes_file("./rsa/primes.txt");
    ofstream ciphertext_file("./rsa/ciphertext.txt");
    ofstream decrypt_file("./rsa/decrypted.txt");

    // get the primes
    string p_str, q_str, plaintext;
    getline(primes_file, p_str);
    getline(primes_file, q_str);
    // set the values
    p = atof(p_str.c_str());
    q = atof(q_str.c_str());
    num = p * q;
    // generate phi
    phi = (p - 1) * (q - 1);
    cout << "Variables" << endl << endl;
    cout << "p = " << p << endl;
    cout << "q = " << q << endl;
    cout << "num = " << num << endl;
    cout << "phi = " << phi << endl;

    // read the plaintext from the file
    getline(plaintext_file, plaintext);
    double plaintext_number = atof(plaintext.c_str());

    // generate encrypt - public key fragment
    int divisor;
    while (encrypt < phi)
    {
        // verify that phi and encrypt are co-prime, meaning their greatest 
        // commom divisor is 1
        divisor = greatest_common_divisor(encrypt, phi);
        if (divisor == 1)
        {
            break;
        }
        else
        {
            encrypt++;
        }
    }

    // constant value
    int constant = 4;
    // generate decrypt - private key fragment
    decrypt = (1 + (constant * phi)) / encrypt;

    cout << "public key = { " << encrypt << ", " << num << " }" << endl;
    cout << "private key = { " << decrypt << ", " << num << " }" << endl << endl;

    cout << "Plaintext number: " << plaintext_number << endl;

    double encrypted = encryption(plaintext_number);
    cout << "Encrypted: " << encrypted << endl;
    
    double decrypted = decryption(encrypted);
    cout << "Decrypted: " << decrypted << endl;
}
//----------------------------------------------------------------------------//