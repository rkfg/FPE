/*
 * AES.h
 *
 *  Created on: 19 Nov 2016
 *      Author: rkfg
 */

#ifndef AES_H_
#define AES_H_

#include <openssl/evp.h>
#include <string>
#include <vector>

class AES {
private:
	const EVP_CIPHER *cipher_;
	const std::string key_;
	const std::string iv_;
public:
	AES(const std::string& key, const std::string& iv);
	std::vector<unsigned char> encrypt(const std::vector<unsigned char>& data);
	std::vector<unsigned char> decrypt(const std::vector<unsigned char>& data);
};

#endif /* AES_H_ */
