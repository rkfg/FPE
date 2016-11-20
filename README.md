# About

This is a simple format preserving encryption library that only depends on OpenSSL (for encryption) and GNU GMP (for big number math). Here "format preservation" means that the output data consists of the same characters (alphabet) that the input data does and also that the output data length always equals the input data length.

It allows to encrypt arbitrary-radix data (up to 64 bit per value) to another arbitrary-radix data (up to 8 bit per value currently) and decrypt it back. It can be used to encrypt credit card numbers, for example, getting back other card numbers, i.e. data consisting of 0-9 digits instead of 0-255 bytes. Or you may encrypt the database IDs getting back YouTube-like video IDs that:
- never repeat themselves
- can't be predicted (bruteforced)
- can be easily decrypted back to plain integers and used to query the database (no need to change the existing logic or hack ORMs).

Unlike shortened hashes, you don't have to store these scrambled/original pairs anywhere and there won't be any collisions.

Based on [Virgil Security article](https://habrahabr.ru/company/virgilsecurity/blog/311200/) (in Russian).

# Compiling

No build system script provided as of yet, use `g++ -o fpe src/*.cpp -lcrypto -lgmp` to build.

# Usage

There's an example in `main.cpp`. `AES` class provides basic encryption/decryption methods, though in this algorithm decryption isn't used. Init the `AES` class instance with a key (128 bits) and IV (128 bits), then instantiate the LimitedAES class providing the AES instance and the size of the target alphabet (the number of symbols you'd like to use).

After that use the encrypt/decrypt methods of the LimitedAES instance. If your input has bigger elements than the output (more than one byte per element), you can use the overloaded methods that allow to specify the source radix (number of the source alphabet elements) or deduce it from the source element type. In that case the output vector will be larger than the input, check the `calcSize` method for details. Because of rounding errors you can get a bit longer result after the encryption/decryption cycle, for example, one unsigned long (64 bit) encrypts to 10 unsigned chars of radix 95 and then decrypts back to two unsigned longs. This happens because you should have 10 chars to cover all the possible values of a 64 bit input. It's a bit too much for one 64 bit value but at the same time too little for two 64 bit values. To prevent data loss the library decrypts to two unsigned longs. If you're sure you only need the first you can discard the second one.

However, this hasn't been thoroughly tested so use on your own risk. This is more of a proof-of-concept than a production-ready library, it certainly might be optimized for speed and readability. There also may be errors. Hope you've already figured that out.

# Example

In the test application ASCII symbols are supported, from SPACE (0x20) to ~ (0x7E), 95 chars overall. The same characters are used in the output. Note that unlike regular AES + base64 encoding (or other binary-to-ASCII transformation) the text length always stays the same and no padding needed.

```
> ./fpe -e 'This is a simple test.'
Source: This is a simple test. / 22
Encrypted: h00IDQ$3y8I:r3K$,@-,s:

> ./fpe -d 'h00IDQ$3y8I:r3K$,@-,s:'
Source: h00IDQ$3y8I:r3K$,@-,s: / 22
Decrypted: This is a simple test.
```

# Limitations

- Only `vector<unsigned char>` is allowed as output. However, this should be enough for most cases I can come up with. This is not a technical limitation, just an implementation detail to keep things simple. On the other hand, the input vector elements might be anything from 8 bit char to 64 bit long and have any radix.
- `aes-128-cbc` algorithm is hardcoded for simplicity as well, it's used only as a gamma source and most of the output is discarded in the encryption process. Probably, there's no point in using 256 bits or other modes but I'm not a cryptographer.
- Depends on two pretty heavy libraries for doing pretty simple things. Of course, I could've implemented AES myself or get any available implementation elsewhere but OpenSSL is available almost on any server so why not use it. If that concerns you, just replace the AES class with your own implementation. GMP, however, is really needed as AES output is used as a big number that's summed with the data being encrypted and then taken by modulo. It's not easy to implement such math working on arbitrary block size on any CPU (maybe I'm overcautious). GMP just works and does it right.
- No key derivation is done currently, the key should be exactly 128 bits or the rest will be just garbage from uninitialized memory, same for IV.
- Performance could be quite bad because of lots of GMP/bytes conversions back and forth. No profiling has been done but for short ID generation this should be enough (this will unlikely become a bottleneck).
