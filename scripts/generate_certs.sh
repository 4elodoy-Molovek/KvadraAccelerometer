#!/usr/bin/env bash
cd "$(dirname "$0")/.." || exit 1

CERTS_DIR="$(pwd)/certs"
mkdir -p "$CERTS_DIR"
cd "$CERTS_DIR" || exit 1

echo "Генерируем корневой сертификат (CA)..."
openssl req -x509 -newkey rsa:4096 -days 365 -nodes \
  -keyout ca.key -out ca.crt \
  -subj "/C=RU/O=KvadraOS/CN=Kvadra-Root-CA" >/dev/null 2>&1

generate_cert() {
    local filename=$1
    local common_name=$2
    echo "Генерируем сертификат для: $filename (CN=$common_name)..."
    
    openssl req -newkey rsa:4096 -nodes \
      -keyout "${filename}.key" -out "${filename}.csr" \
      -subj "/C=RU/O=KvadraOS/CN=${common_name}" >/dev/null 2>&1
      
    openssl x509 -req -in "${filename}.csr" \
      -CA ca.crt -CAkey ca.key -CAcreateserial \
      -out "${filename}.crt" -days 365 >/dev/null 2>&1
      
    rm "${filename}.csr"
}

generate_cert "server" "localhost"
generate_cert "client_a" "node-a"
generate_cert "client_b" "node-b"

rm -f ca.srl

echo "Все сертификаты успешно сгенерированы в папке $CERTS_DIR!"
ls -l *.crt *.key