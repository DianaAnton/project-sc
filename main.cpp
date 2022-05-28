#include <iostream>
#include "./caesar/caesar.cpp"
#include "./rsa/rsa.cpp"

using namespace std;
//----------------------------------------------------------------------------//
int main()
{
    int option;
    while (true)
    {
        cout << "1. Caesar cypher" << endl;
        cout << "2. RSA" << endl;
        cout << "3. TODO" << endl;
        cout << "4. Exit" << endl;
        cout << "Choose an algorithm: "; cin >> option;
        switch(option)
        {
            case 1:
            {
                cout << "\n------------------------Running Caesar cypher------------------------\n\n";
                run_caesar();
                cout << "\n------------------------Done Caesar cypher---------------------------\n\n";
                break;
            }
            case 2:
            {
                cout << "\n------------------------Running RSA cypher---------------------------\n\n";
                run_rsa();
                cout << "\n------------------------Done RSA cypher------------------------------\n\n";
                break;
            }
            case 3:
            {
                cout << "\n------------------------Running TODO cypher------------------------\n\n";

                cout << "\n------------------------Running TODO cypher------------------------\n\n";
                break;
            }
            case 4:
            {
                cout << "Goodbye...." << endl;
                exit(0);
                break;
            }
        }
    }
}
//----------------------------------------------------------------------------//