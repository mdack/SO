#! /bin/bash

DIRECTORY_T=tmp
DIRECTORY_O=out

FILE1=file1.txt
FILE2=file2.txt
FILE3=file3.dat
TAR_FILE=filetar.mtar

#1
if [ ! -e ./mytar ]; then
	echo "ERROR: mytar file doesn't exist!"
	exit 1

elif [ ! -x ./mytar ]; then
	echo "ERROR: mytar file is not an executable"
	exit 1

fi

#2
if [ -d $DIRECTORY_T ]; then
	rm -rf $DIRECTORY_T
	echo "$DIRECTORY_T directory deleted!"
fi


#3
mkdir $DIRECTORY_T 
cd $DIRECTORY_T

#4
if [ ! -e $FILE1 ]; then
	touch $FILE1
	echo "Hello World!" > $FILE1

	echo "File 1 created!"
fi

if [ ! -e $FILE2 ]; then
	touch $FILE2
	head -10 /etc/passwd > $FILE2

	echo "File 2 created!"
fi

if [ ! -e $FILE3 ]; then
	touch $FILE3
	head -c 1024 /dev/urandom > $FILE3

	echo "File 3 created!"
fi

#5
./../mytar -c -f $TAR_FILE $FILE1 $FILE2 $FILE3

#6
if [ ! -d $DIRECTORY_O ]; then
	mkdir $DIRECTORY_O

	echo "$DIRECTORY_O directory created!"
fi

cp $TAR_FILE $DIRECTORY_O

#7
cd $DIRECTORY_O
./../../mytar -x -f $TAR_FILE

#8
diff ../$FILE1 $FILE1

if [ $? = 0 ]; then
	echo "$FILE1 is identical"

	diff ../$FILE2 $FILE2
	if [ $? = 0 ]; then
		echo "$FILE2 is identical"

		diff ../$FILE3 $FILE3

		if [ $? = 0 ]; then
			echo "$FILE3 is identical"
			cd ../..
			echo "Correct!"
			exit 0
		else
			echo "Error: $FILE3 is different"
			cd ../..
			exit 1
		fi
	else
		echo "Error: $FILE2 is different"
		cd ../..
		exit 1
	fi
else
	echo "Error: $FILE1 is different"
	cd ../..
	exit 1
fi