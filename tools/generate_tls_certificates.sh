#! /bin/bash

# References:-
# 1. https://devopscube.com/create-self-signed-certificates-openssl/,
# 2. https://mariadb.com/docs/server/security/data-in-transit-encryption/create-self-signed-certificates-keys-openssl/
# Modification is done as per this project

DOMAIN=DiagClientLib

mkdir openssl && cd openssl

# Create root CA
openssl req -x509 \
            -newkey rsa:2048 \
            -sha256 -days 356 \
            -nodes \
            -subj "/CN=${DOMAIN}/C=DE/L=Berlin " \
            -keyout ${DOMAIN}RootCAKey.key -out ${DOMAIN}RootCA.crt

# Create server private key
openssl genrsa -out ${DOMAIN}Server.key 2048

# Create Certificate Signing Request Configuration
cat > csr.conf <<EOF
[ req ]
default_bits = 2048
prompt = no
default_md = sha256
req_extensions = req_ext
distinguished_name = dn

[ dn ]
C = DE
ST = BW
L = BERLIN
O = DiagClientLib
OU = DiagClientLib
CN = ${DOMAIN}

[ req_ext ]
subjectAltName = @alt_names

[ alt_names ]
DNS.1 = ${DOMAIN}
DNS.2 = www.${DOMAIN}.com
IP.1 = 172.16.25.127
IP.2 = 172.16.25.128

EOF

# Generate Certificate Signing Request (CSR) Using Server Private Key
openssl req -new -key ${DOMAIN}Server.key -out ${DOMAIN}.csr -config csr.conf

# Create a external config file for the certificate
cat > cert.conf <<EOF

authorityKeyIdentifier=keyid,issuer
basicConstraints=CA:FALSE
keyUsage = digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment
subjectAltName = @alt_names

[alt_names]
DNS.1 = ${DOMAIN}

EOF

# Generate SSL certificate With self signed CA
openssl x509 -req \
    -in ${DOMAIN}.csr \
    -CA ${DOMAIN}RootCA.crt -CAkey ${DOMAIN}RootCAKey.key \
    -CAcreateserial -out ${DOMAIN}Server.crt \
    -days 365 \
    -sha256 -extfile cert.conf


# Convert from CRT to PEM format
openssl x509 -in ${DOMAIN}RootCA.crt -out ${DOMAIN}RootCA.pem
openssl x509 -in ${DOMAIN}Server.crt -out ${DOMAIN}Server.pem

# Verify the certificates
openssl verify -CAfile ${DOMAIN}RootCA.pem ${DOMAIN}Server.pem

