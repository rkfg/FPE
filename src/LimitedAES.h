/*
 * LimitedAES.h
 *
 *  Created on: 19 Nov 2016
 *      Author: rkfg
 */

#ifndef LIMITEDAES_H_
#define LIMITEDAES_H_

#include "AES.h"
#include <iostream>
#include <gmpxx.h>
#include <vector>

template<typename Elem>
class LimitedAES {
private:
	AES& aes_;
	mpz_class radix_;
	size_t rounds_;
	mpz_class elemPow_;

	template<typename V>
	mpz_class num(const std::vector<V>& data, const mpz_class& radix) {
		mpz_class result(0);
		for (int i = data.size() - 1; i >= 0; --i) {
			result = result * radix + data[i];
		}
		return result;
	}

	template<typename V>
	std::vector<V> numBack(const mpz_class& data, const mpz_class& radix, size_t len) {
		std::vector<V> result;
		mpz_class copy(data);
		for (size_t i = 0; i < len * sizeof(Elem) / sizeof(V); ++i) {
			mpz_class mod = copy % radix;
			result.push_back(mod.get_ui());
			copy /= radix;
		}
		return result;
	}
public:
	LimitedAES(AES& aes, int radix, size_t rounds = 8) :
			aes_(aes), radix_(radix), rounds_(rounds) {
		mpz_pow_ui(elemPow_.get_mpz_t(), mpz_class(2).get_mpz_t(), sizeof(Elem) * 8);
	}

	std::vector<Elem> encrypt(std::vector<Elem>& data) {
		auto len = data.size();
		auto half = len / 2;
		mpz_class a = num(std::vector<Elem>(data.begin(), data.begin() + half), radix_);
		mpz_class b = num(std::vector<Elem>(data.begin() + half, data.end()), radix_);
		mpz_class aPow, bPow;
		size_t aSize = half;
		size_t bSize = len - half;
		mpz_pow_ui(aPow.get_mpz_t(), mpz_class(radix_).get_mpz_t(), aSize);
		mpz_pow_ui(bPow.get_mpz_t(), mpz_class(radix_).get_mpz_t(), bSize);
		for (size_t r = 0; r < rounds_; r++) {
			auto bytes = numBack<unsigned char>(b, elemPow_, bSize);
			auto bEnc = aes_.encrypt(bytes);
			a = (a + num(bEnc, elemPow_)) % aPow;
			std::swap(a, b);
			std::swap(aPow, bPow);
			std::swap(aSize, bSize);
		}
		std::vector<Elem> result = numBack<Elem>(a, radix_, aSize);
		std::vector<Elem> bVec = numBack<Elem>(b, radix_, bSize);
		result.insert(result.end(), bVec.begin(), bVec.end());
		return result;
	}

	std::vector<Elem> decrypt(std::vector<Elem>& data) {
		auto len = data.size();
		auto half = len / 2;
		mpz_class a = num(std::vector<Elem>(data.begin(), data.begin() + half), radix_);
		mpz_class b = num(std::vector<Elem>(data.begin() + half, data.end()), radix_);
		mpz_class aPow, bPow;
		size_t aSize = half;
		size_t bSize = len - half;
		mpz_pow_ui(aPow.get_mpz_t(), mpz_class(radix_).get_mpz_t(), aSize);
		mpz_pow_ui(bPow.get_mpz_t(), mpz_class(radix_).get_mpz_t(), bSize);
		for (size_t r = 0; r < rounds_; r++) {
			std::swap(a, b);
			std::swap(aPow, bPow);
			std::swap(aSize, bSize);
			auto bytes = numBack<unsigned char>(b, elemPow_, bSize);
			auto bEnc = aes_.encrypt(bytes);
			a = (a - num(bEnc, elemPow_)) % aPow;
			if (a < 0) {
				a += aPow;
			}
		}
		std::vector<Elem> result = numBack<Elem>(a, radix_, aSize);
		std::vector<Elem> bVec = numBack<Elem>(b, radix_, bSize);
		result.insert(result.end(), bVec.begin(), bVec.end());
		return result;
	}
};

#endif /* LIMITEDAES_H_ */
