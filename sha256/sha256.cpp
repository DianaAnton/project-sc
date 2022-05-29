#include <string>
#include <array>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
using namespace std;
//----------------------------------------------------------------------------//
uint8_t  data[64];
uint32_t block_len;
uint64_t bit_len;
uint32_t m_state[8]; //A, B, C, D, E, F, G, H

static constexpr array<uint32_t, 64> K = 
{
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
    0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
    0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
    0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
    0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
    0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
    0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
    0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
    0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

//----------------------------------------------------------------------------//
static uint32_t rotr(uint32_t x, uint32_t n)
{
    return (x >> n) | (x << (32 - n));
}
//----------------------------------------------------------------------------//
static uint32_t choose(uint32_t e, uint32_t f, uint32_t g)
{
    return (e & f) ^ (~e & g);
}
//----------------------------------------------------------------------------//
static uint32_t majority(uint32_t a, uint32_t b, uint32_t c)
{
    return (a & (b | c)) | (b & c);
}
//----------------------------------------------------------------------------//
static uint32_t sig0(uint32_t x)
{
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}
//----------------------------------------------------------------------------//
static uint32_t sig1(uint32_t x)
{
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}
//----------------------------------------------------------------------------//
void transform()
{
    uint32_t major, xor_A, ch, xor_E, sum, new_A, new_E, m[64];
	uint32_t state[8];

	for (int i = 0, j = 0; i < 16; i++, j += 4) 
    { // Split data in 32 bit blocks for the 16 first words
		m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
	}

	for (int k = 16 ; k < 64; k++)
    { // Remaining 48 blocks
		m[k] = sig1(m[k - 2]) + m[k - 7] + sig0(m[k - 15]) + m[k - 16];
	}

	for(int i = 0 ; i < 8 ; i++)
    {
		state[i] = m_state[i];
	}

	for (int i = 0; i < 64; i++) 
    {
		major   = majority(state[0], state[1], state[2]);
		xor_A  = rotr(state[0], 2) ^ rotr(state[0], 13) ^ rotr(state[0], 22);

		ch = choose(state[4], state[5], state[6]);

		xor_E  = rotr(state[4], 6) ^ rotr(state[4], 11) ^ rotr(state[4], 25);

		sum  = m[i] + K[i] + state[7] + ch + xor_E;
		new_A = xor_A + major + sum;
		new_E = state[3] + sum;

		state[7] = state[6]; state[6] = state[5]; state[5] = state[4];
		state[4] = new_E;
		state[3] = state[2]; state[2] = state[1]; state[1] = state[0];
		state[0] = new_A;
	}

	for(int i = 0 ; i < 8 ; i++)
    {
		m_state[i] += state[i];
	}
}
//----------------------------------------------------------------------------//
void pad()
{
    uint64_t i = block_len;
	uint8_t end = block_len < 56 ? 56 : 64;

    // Append a bit 1
	data[i++] = 0x80;

    // Pad with zero
	while (i < end) 
    {
		data[i++] = 0x00; 
	}

	if(block_len >= 56) 
    {
		transform();
		memset(data, 0, 56);
	}

	// Append the message's length in bits and transform.
	bit_len += block_len * 8;
	data[63] = bit_len;
	data[62] = bit_len >> 8;
	data[61] = bit_len >> 16;
	data[60] = bit_len >> 24;
	data[59] = bit_len >> 32;
	data[58] = bit_len >> 40;
	data[57] = bit_len >> 48;
	data[56] = bit_len >> 56;
	transform();
}
//----------------------------------------------------------------------------//
void revert(uint8_t * hash)
{
    for (uint8_t i = 0 ; i < 4 ; i++) 
    {
		for(uint8_t j = 0 ; j < 8 ; j++) 
        {
			hash[i + (j * 4)] = (m_state[j] >> (24 - i * 8)) & 0x000000ff;
		}
	}
}
//----------------------------------------------------------------------------//
void update(const uint8_t * text, size_t length)
{
    for (size_t i = 0 ; i < length ; i++) 
    {
		data[block_len++] = text[i];
		if (block_len == 64) 
        {
			transform();
			// End of the block
			bit_len += 512;
			block_len = 0;
		}
	}
}
//----------------------------------------------------------------------------//
void update(const std::string &text)
{
    update(reinterpret_cast<const uint8_t*> (text.c_str()), text.size());
}
//----------------------------------------------------------------------------//
uint8_t * digest_hash()
{
    uint8_t * hash = new uint8_t[32];
	pad();
	revert(hash);

	return hash;
}
//----------------------------------------------------------------------------//
static string toString(const uint8_t * digest)
{
    stringstream ss;
	ss << setfill('0') << hex;
	for(uint8_t i = 0 ; i < 32 ; i++) 
    {
		ss << setw(2) << (unsigned int) digest[i];
	}

	return ss.str();
}
//----------------------------------------------------------------------------//
void run_sha256()
{
    m_state[0] = 0x6a09e667;
    m_state[1] = 0xbb67ae85;
    m_state[2] = 0x3c6ef372;
    m_state[3] = 0xa54ff53a;
    m_state[4] = 0x510e527f;
    m_state[5] = 0x9b05688c;
    m_state[6] = 0x1f83d9ab;
    m_state[7] = 0x5be0cd19;

    ifstream plaintext_file("./sha256/plaintext.txt");
    ofstream ciphertext_file("./sha256/ciphertext.txt");

    string plaintext, ciphertext;
    getline(plaintext_file, plaintext);
    cout << "Plaintext: " << plaintext << endl;

    update(plaintext);
    uint8_t *digest = digest_hash();

    cout << "Ciphertext: " << toString(digest) << endl;
    ciphertext += toString(digest) + "\n";

    delete [] digest;
    ciphertext_file << ciphertext;
    plaintext_file.close();
    ciphertext_file.close();
}
//----------------------------------------------------------------------------//
int main()
{
    run_sha256();
}
//----------------------------------------------------------------------------//