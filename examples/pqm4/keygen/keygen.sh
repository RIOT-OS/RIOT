#!/bin/bash

PQ_KEMS="kindi256342 sntrup4591761 kyber768 newhope1024cca ntruhrss701 saber sikep751"

for k in $PQ_KEMS; do
  make PQ_IMPL=ref PQ_KEM="$k" all;
  ./bin/native/pqm4-keygen.elf | tail -n +8 > "./pq_${k}_key.h";
done
