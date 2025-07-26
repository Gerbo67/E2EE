#include "CryptoUtils.h"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <stdexcept>
#include <memory>

// Función auxiliar para gestionar la memoria de OpenSSL BIO usando unique_ptr
struct BIO_deleter {
    void operator()(BIO* bio) const { BIO_free_all(bio); }
};
using unique_BIO = std::unique_ptr<BIO, BIO_deleter>;

/**
 * @summary Codifica un vector de bytes a una cadena Base64.
 */
std::string CryptoUtils::base64_encode(const std::vector<unsigned char>& data) {
    unique_BIO b64(BIO_new(BIO_f_base64()));
    BIO_set_flags(b64.get(), BIO_FLAGS_BASE64_NO_NL);

    BIO* sink = BIO_new(BIO_s_mem());
    BIO_push(b64.get(), sink);

    BIO_write(b64.get(), data.data(), data.size());
    BIO_flush(b64.get());

    BUF_MEM* mem;
    BIO_get_mem_ptr(sink, &mem);

    std::string encoded(mem->data, mem->length);
    return encoded;
}

/**
 * @summary Decodifica una cadena Base64 a un vector de bytes.
 */
std::vector<unsigned char> CryptoUtils::base64_decode(const std::string& input) {
    unique_BIO b64(BIO_new(BIO_f_base64()));
    BIO_set_flags(b64.get(), BIO_FLAGS_BASE64_NO_NL);

    BIO* source = BIO_new_mem_buf(input.data(), input.length());
    BIO_push(b64.get(), source);

    std::vector<unsigned char> decoded(input.length());
    int decoded_length = BIO_read(b64.get(), decoded.data(), decoded.size());

    if (decoded_length < 0) {
        throw std::runtime_error("Fallo en la decodificación Base64.");
    }

    decoded.resize(decoded_length);
    return decoded;
}
