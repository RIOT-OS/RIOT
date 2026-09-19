#!/bin/bash

mkdir certs csr newcerts private
touch index.txt
openssl rand -hex 16 > serial

DeviceID="az-riot-pnp-module-test-02"

openssl genrsa -aes256 -passout pass:1234 -out ./private/azure-iot-test-only.root.ca.key.pem 2048
openssl req -new -x509 -config ./openssl_root_ca.cnf -passin pass:1234 -key ./private/azure-iot-test-only.root.ca.key.pem -subj '/CN=Azure IoT Hub CA Cert Test Only' -days 60 -sha256 -extensions v3_ca -out ./certs/azure-iot-test-only.root.ca.cert.pem
echo "---------------CA cert----------------"
openssl x509 -noout -text -in ./certs/azure-iot-test-only.root.ca.cert.pem
echo "------------Intermediate cert-------------------"
openssl genrsa -aes256 -passout pass:1234 -out ./private/azure-iot-test-only.intermediate.key.pem 2048
openssl req -new -sha256 -passin pass:1234 -config ./openssl_device_intermediate_ca.cnf -subj '/CN=Azure IoT Hub Intermediate Cert Test Only' -key ./private/azure-iot-test-only.intermediate.key.pem -out ./csr/azure-iot-test-only.intermediate.csr.pem
openssl ca -batch -config ./openssl_root_ca.cnf -passin pass:1234 -extensions v3_intermediate_ca -days 60 -notext -md sha256 -in ./csr/azure-iot-test-only.intermediate.csr.pem -out ./certs/azure-iot-test-only.intermediate.cert.pem
openssl x509 -noout -text -in ./certs/azure-iot-test-only.intermediate.cert.pem
echo "-------------Device cert-----------------"
openssl genrsa -out ./private/${DeviceID}.key.pem 2048
openssl req -config ./openssl_device_intermediate_ca.cnf -key ./private/${DeviceID}.key.pem -subj "/CN=$DeviceID" -new -sha256 -out ./csr/${DeviceID}.csr.pem
openssl ca -batch -config ./openssl_device_intermediate_ca.cnf -passin pass:1234 -extensions usr_cert -days 60 -notext -md sha256 -in ./csr/${DeviceID}.csr.pem -out ./certs/${DeviceID}.cert.pem
openssl x509 -noout -text -in ./certs/${DeviceID}.cert.pem
echo "---------------Chain----------------"
cat ./certs/${DeviceID}.cert.pem ./certs/azure-iot-test-only.intermediate.cert.pem ./certs/azure-iot-test-only.root.ca.cert.pem > ./certs/${DeviceID}-full-chain.cert.pem
openssl x509 -noout -text -in ./certs/${DeviceID}-full-chain.cert.pem

sudo chmod -R 777 certs csr newcerts private


