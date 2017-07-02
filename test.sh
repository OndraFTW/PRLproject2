#!/bin/bash

#Autor: Ondřej Šlampa, xslamp01@stud.fit.vutbr.cz
#Projekt: PRL proj1
#Popis: Implementace algoritmu pipeline merge sort pomocí OpenMPI.

#výpočet počtu procesorů
np=`echo "l($1)/l(2)+1" | bc -l`
np=${np%.*}

#pokud počet prvků není mocnina dvou, použije se o jeden více procesorů
p=`echo "(2^($np-1))!=$1" | bc -l`
if [ "$p" == "1" ] ; then
    np=`echo "$np+1" | bc -l`
fi

#překlad, tvorba testovacího souboru, běh algoritmu a úklid
mpic++ --prefix /usr/local/share/OpenMPI -o pms pms.cpp
dd if=/dev/urandom bs=1 count=$1 of=numbers &>/dev/null
mpirun --prefix /usr/local/share/OpenMPI -np $np pms
rm numbers pms
