#!/bin/bash

verificationCode="3B3FACA5AD4FA91732BAA7A4A08779844546D5D704777AD7"

echo "------------Verification cert-------------------"
openssl genrsa -aes256 -passout pass:1234 -out ./private/verification-$verificationCode.key.pem 2048
openssl req -new -sha256 -passin pass:1234 -config ./openssl_device_intermediate_ca.cnf -subj "/CN=$verificationCode" -key ./private/verification-$verificationCode.key.pem -out ./csr/verification-$verificationCode.csr.pem
openssl ca -batch -config ./openssl_root_ca.cnf -passin pass:1234 -extensions v3_intermediate_ca -days 60 -notext -md sha256 -in ./csr/verification-$verificationCode.csr.pem -out ./certs/verification-$verificationCode.cert.pem
openssl x509 -noout -text -in ./certs/verification-$verificationCode.cert.pem
