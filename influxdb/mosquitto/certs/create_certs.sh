#!/bin/bash

openssl genrsa -out m2mqtt_ca.key 2048

openssl req -new -x509 -days 3650 -subj "/C=IT/CN=mqtt-ca" -key m2mqtt_ca.key -out m2mqtt_ca.crt

openssl genrsa -out m2mqtt_srv.key 2048

openssl req -new -subj "/C=IT/CN=mqtt-srv" -out m2mqtt_srv.csr -key m2mqtt_srv.key

openssl x509 -req -in m2mqtt_srv.csr -CA m2mqtt_ca.crt -CAkey m2mqtt_ca.key -CAcreateserial -out m2mqtt_srv.crt -days 3650

rm m2mqtt_srv.csr

