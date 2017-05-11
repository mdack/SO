#!/bin/bash

#Nombre del fichero de ejemplo que desea simular

nombArch=""
ok=false

while [ "$ok" != "true" ]; do
	
	echo "Nombre del fichero que desea simular: "
	read nombre

	nombArch=("./examples/"$nombre"")

	if [ -f "$nombArch" ]; then
		ok=true
	else
		echo "No se ha encontrado el fichero $nomArch"
	fi

done

#Numero maximo de CPUs que se desean simular

maxCPUs=0
ok=false

while [ "$ok" != "true" ]; do
	echo "Máximo de CPUs: "
	read maxCPUs

	if (( "$maxCPUs" > 0 &&  "$maxCPUs" <= 8 )); then
		ok=true
	else
		echo "Error: Como maximo 8 CPUs"
	fi

done

directorio="resultados"
i=0
j=0

if [ -d "$directorio" ]; then
	rm -rf -- $directorio  
fi

mkdir $directorio 

for line in $(./schedsim -L); do
	if (( i >= 2 )); then
		cpus=1
		echo "Algoritmo: $line"
		algoritmo=$line

		while (( cpus <= maxCPUs )); do
			./schedsim -i $nombArch -n $cpus -s $algoritmo

			while (( j < cpus )); do
				auxDirectory=($directorio"/"$algoritmo"-CPU-"$j".log")
				aux=("CPU_"$j".log")
				mv $aux $auxDirectory

				((j+=1))
			done

			cd "../gantt-gplot"
			./generate_gantt_chart "../schedsim/"$auxDirectory
			cd "../schedsim"

			((cpus += 1))
			((j=0))
		done
	fi

	((i+=1))
done