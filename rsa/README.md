# RSA

## Algorithm

```
Begin
   1. Choose two prime numbers p and q.
   2. Compute n = p*q.
   3. Calculate phi = (p-1) * (q-1).
   4. Choose an integer e such that 1 < e < phi(n) and gcd(e, phi(n)) = 1; i.e., e and phi(n) are coprime.
   5. Calculate d as d ≡ e−1 (mod phi(n)); here, d is the modular multiplicative inverse of e modulo phi(n).
   6. For encryption, c = me mod n, where m = original message.
   7. For decryption, m = c d mod n.
End
```

## RSA Encrypt and Decrypt - Example

Let examine one **example of RSA encryption and decryption**, along with the calculations, following the above formulas. Assume we have generated the RSA public-private key pair:

- modulus **_n_** = 143
- public exponent **_e_** = 7
- private exponent **_d_** = 103
- public key = {**_n_**, **_e_**} = {143, 7}
- private key = {**_n_**, **_d_**} = {143, 103}

Let's **encrypt** a secret message **_msg_** = **83**. Just follow the formula:

- **_encryptedMsg_** = **_msge_** mod **_n_** = 837 mod 143 = 27136050989627 mod 143 = **8**

Now, let's **decrypt** the encrypted message back to its original value:

- **_decryptedMsg_** = **_encryptedMsgd_** mod n = 8103 mod 143 = 1042962419883256876169444192465601618458351817556959360325703910069443225478828393565899456512 mod 143 = **83**

The RSA calculations work correctly. This is because the key-pair meets the RSA property:

- (**_me_**)**_d_** ≡ **_m_** (mod **_n_**) for all **_m_** in the range [0...**_n_**)
- (**\*m**7)103 ≡ **m** (mod **143**) for all **m** in the range [0...**143\***)

In the real world, typically the RSA modulus **_n_** and the private exponent **_d_** are 3072-bit or 4096-bit integers and the public exponent **_e_** is 65537.

For further reading, look at this excellent explanation about **how RSA works** in detail with explainations and examples: [http://doctrina.org/How-RSA-Works-With-Examples.html](http://doctrina.org/How-RSA-Works-With-Examples.html).

Because RSA encryption is a **deterministic** (has no random component) attackers can successfully launch a **[chosen plaintext attack](https://en.wikipedia.org/wiki/Chosen-plaintext_attack)** against by encrypting likely plaintexts with the public key and test if they are equal to the ciphertext. This may not be a problem, but is a **weakness**, that should be considered when developers choose an encryption scheme.
