
#include "AES.h"

AESCommon::~AESCommon() {
}

size_t AESCommon::blockSize() const {
    size_t returnValue;
    return 0;
}

void AESCommon::encryptBlock(uint8_t *output, const uint8_t *input) {
    aes_encrypt(&_aes_context, input, output);
}

void AESCommon::decryptBlock(uint8_t *output, const uint8_t *input) {
    aes_decrypt(&_aes_context, input, output);
}

void AESCommon::clear() {
}

AESCommon::AESCommon() {
}

void AESCommon::subBytesAndShiftRows(uint8_t *output, const uint8_t *input) {
}

void AESCommon::inverseShiftRowsAndSubBytes(uint8_t *output, const uint8_t *input) {
}

void AESCommon::mixColumn(uint8_t *output, uint8_t *input) {
}

void AESCommon::inverseMixColumn(uint8_t *output, const uint8_t *input) {
}

void AESCommon::keyScheduleCore(uint8_t *output, const uint8_t *input, uint8_t iteration) {
}

void AESCommon::applySbox(uint8_t *output, const uint8_t *input) {
}

AES128::AES128() {
    
}

AES128::~AES128() {
}

size_t AES128::keySize() const {
    return _key_size;
}

bool AES128::setKey(const uint8_t *key, size_t len) {
    if(len != AES_KEY_SIZE_128) {
        return false;
    }
    _key_size = len;
    int result = aes_init(&_aes_context, key, (uint8_t) len);
    return result == 1;
}

AES256::AES256() {
}

AES256::~AES256() {
}

size_t AES256::keySize() const {
    return _key_size;
}

bool AES256::setKey(const uint8_t *key, size_t len) {
    if(len != AES_KEY_SIZE_256) {
        return false;
    }
    _key_size = len;
    int result = aes_init(&_aes_context, key, (uint8_t) len);
    return result == 1;
}

AESTiny256::AESTiny256() {
}

AESTiny256::~AESTiny256() {
}

AESSmall256::AESSmall256() {
}

AESSmall256::~AESSmall256() {
}

