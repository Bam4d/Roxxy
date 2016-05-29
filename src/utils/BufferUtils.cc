/*
 * bufferUtils.cc
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#include "BufferUtils.h"

std::string BufferUtils::GetString(std::unique_ptr<folly::IOBuf> buf) {

	if(buf){
		return std::string(reinterpret_cast<const char*>(buf->data()),
	                           buf->length());
	}

	return "";
}

dynamic BufferUtils::GetJson(std::unique_ptr < folly::IOBuf > buf) {
	std::string jsonString = BufferUtils::GetString(std::move(buf));
	return parseJson(jsonString);
}

std::string BufferUtils::Base64Encode(const char* buffer, size_t size)
{
    BIO *p_bio_b64 = nullptr;
    BIO *p_bio_mem = nullptr;

    try
    {
        // make chain: p_bio_b64 <--> p_bio_mem
        p_bio_b64 = BIO_new(BIO_f_base64());
        if (!p_bio_b64) { throw std::runtime_error("BIO_new failed"); }
        BIO_set_flags(p_bio_b64, BIO_FLAGS_BASE64_NO_NL); //No newlines every 64 characters or less

        p_bio_mem = BIO_new(BIO_s_mem());
        if (!p_bio_mem) { throw std::runtime_error("BIO_new failed"); }
        BIO_push(p_bio_b64, p_bio_mem);

        // write input to chain
        // write sequence: input -->> p_bio_b64 -->> p_bio_mem
        if (BIO_write(p_bio_b64, buffer, size) <= 0)
            { throw std::runtime_error("BIO_write failed"); }

        if (BIO_flush(p_bio_b64) <= 0)
            { throw std::runtime_error("BIO_flush failed"); }

        // get result
        char *p_encoded_data = nullptr;
        auto  encoded_len    = BIO_get_mem_data(p_bio_mem, &p_encoded_data);
        if (!p_encoded_data) { throw std::runtime_error("BIO_get_mem_data failed"); }

        std::string result(p_encoded_data, encoded_len);

        // clean
        BIO_free_all(p_bio_b64);

        return result;
    }
    catch (...)
    {
        if (p_bio_b64) { BIO_free_all(p_bio_b64); }
        throw;
    }
}

std::string BufferUtils::Base64Decode(const std::string &input)
{
    BIO *p_bio_mem = nullptr;
    BIO *p_bio_b64 = nullptr;

    try
    {
        // make chain: p_bio_b64 <--> p_bio_mem
        p_bio_b64 = BIO_new(BIO_f_base64());
        if (!p_bio_b64) { throw std::runtime_error("BIO_new failed"); }
        BIO_set_flags(p_bio_b64, BIO_FLAGS_BASE64_NO_NL); //Don't require trailing newlines

        p_bio_mem = BIO_new_mem_buf((void*)input.c_str(), input.length());
        if (!p_bio_mem) { throw std::runtime_error("BIO_new failed"); }
        BIO_push(p_bio_b64, p_bio_mem);

        // read result from chain
        // read sequence (reverse to write): buf <<-- p_bio_b64 <<-- p_bio_mem
        std::vector<char> buf((input.size()*3/4)+1);
        std::string result;
        for (;;)
        {
            auto nread = BIO_read(p_bio_b64, buf.data(), buf.size());
            if (nread  < 0) { throw std::runtime_error("BIO_read failed"); }
            if (nread == 0) { break; } // eof

            result.append(buf.data(), nread);
        }

        // clean
        BIO_free_all(p_bio_b64);

        return result;
    }
    catch (...)
    {
        if (p_bio_b64) { BIO_free_all(p_bio_b64); }
        throw;
    }
}

