#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

//extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int
copynFile(FILE * origin, FILE * destination, int nBytes)
{	int c = 0, wByte;
	int contBytes = 0;

	while((c != EOF) && (contBytes < nBytes)){
		c = getc(origin);
		if(c != EOF){
			wByte = putc((unsigned char) c, destination);

			if(wByte == EOF){
				perror("putc() failed!!");
				return -1;
			}
			contBytes++;
		}
	}

	// Complete the function
	return contBytes;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{	int c;
	int cont = 0;
	char *name = NULL;

	while((c = getc(file)) != '\0'){
		cont++;
	}

	if(cont > 0){
		name = (char *)malloc(sizeof(char)*(cont+1));

		fseek(file, -(cont+1), SEEK_CUR);

		for(c=0; c < cont+1; c++){
			name[c] = getc(file);
		}
	}

	// Complete the function
	return name;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{	stHeaderEntry* array = NULL;
	int nr_files = 0;
	int i = 0;
	char* name = NULL;
	unsigned int size;

	if(fread(&nr_files, sizeof(int), 1, tarFile) == 0){
		perror("Error reading the number of files\n");
	}

	array=malloc(sizeof(stHeaderEntry)*nr_files);

	do{
		name = loadstr(tarFile);
		if(name != NULL){
			array[i].name = name;
			if(fread(&size,sizeof(unsigned int), 1, tarFile) > 0){
				array[i].size = size;
			}
			i++;
		}	

	}while((i < nr_files) && (name != NULL));

	(*nFiles) = nr_files;
	
	// Complete the function
	return array;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *	-c -f filetar.mtar file1.txt file2.txt file3.dat
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{	FILE * tar;
	FILE * inputFile;
	stHeaderEntry* array = NULL;
	int i, bytes = 0;
	int tam, wBytes;
	int offData;

	if((tar = fopen(tarName, "w")) == NULL){
		perror("Error trying to open the tar file\n");
		return EXIT_FAILURE;
	}

	array = malloc(sizeof(stHeaderEntry)*nFiles);

	for(i=0; i < nFiles; i++){
		tam = strlen(fileNames[i])+1;
		bytes += tam;
		array[i].name = malloc(tam);
		strcpy(array[i].name, fileNames[i]);
	}

	offData=sizeof(int)+(nFiles*sizeof(unsigned int)) + bytes;

	if(fseek(tar, offData, SEEK_SET) < 0){
		perror("Error in function fseek!\n");
		return EXIT_FAILURE;
	}

	for(i=0; i < nFiles; i++){

		if((inputFile = fopen(fileNames[i], "r")) == NULL){
			perror("Error reading a file");
			fclose(tar);
			return EXIT_FAILURE;
		}

		wBytes = copynFile(inputFile, tar, INT_MAX);

		fclose(inputFile);

		array[i].size = wBytes;
	}

	if(fseek(tar, 0, SEEK_SET) == -1){
		perror("Error in function fseek!\n");
		return EXIT_FAILURE;
	}

	if(fwrite(&nFiles, sizeof(int), 1, tar) < 0){
		perror("Error writing in tarFile\n");
		return EXIT_FAILURE;
	}

	for (i = 0; i < nFiles; i++) {
		if(fwrite(array[i].name, strlen(array[i].name)+1, 1,tar) != -1){
			if(fwrite(&array[i].size, sizeof(unsigned int), 1, tar) == -1){
				perror("Error writing in tarFile\n");
				return EXIT_FAILURE;
			}
			free(array[i].name);
		}
		else{
			perror("Error writing in tarFile\n");
			return EXIT_FAILURE;
		}
	}

	free(array);
	if(fclose(tar) != 0){
		perror("Error closing tarFile\n");
		return EXIT_FAILURE;
	}
	else{
		printf("Mtar file created successfully\n");
	}
	// Complete the function
	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 * if (outputByte == EOF) { return -1;
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 * -x -f filetar.mtar
 */
int
extractTar(char tarName[])
{	FILE * tar;
	FILE * outputFile;
	stHeaderEntry* array = NULL;
	int i, nFiles, wBytes;
	char *name;

	if((tar=fopen(tarName, "r")) == NULL){
		perror("Error trying to open the tar file\n");
		return EXIT_FAILURE;
	}

	array = readHeader(tar, &nFiles);

	if(array == NULL){
		perror("Error reading the tarball's header\n");
		return EXIT_FAILURE;
	}

	for(i = 0; i < nFiles; i++){
		name = array[i].name;
		if((outputFile = fopen(name, "w")) == NULL){
			perror("Error trying to open a file\n");
			return EXIT_FAILURE;
		}

		if((wBytes = copynFile(tar, outputFile, array[i].size)) != array[i].size){
			perror("Error writing in a file\n");
			return EXIT_FAILURE;
		}
		else{
			printf("[%d] Creating file %s, size %d Bytes... OK\n", i,name, array[i].size);
		}

		if(fclose(outputFile) != 0){
			perror("Error closing file\n");
			return EXIT_FAILURE;
		}
	}

	if(fclose(tar) != 0){
		perror("Error closing tarFile\n");
		return EXIT_FAILURE;
	}
	
	free(name);
	free(array);

	// Complete the function
	return EXIT_SUCCESS;
}

/*
 * remName: fichero a borrar
 * tarName: tarball en el que se encuentra el fichero
 */
int removeFromTar(char remName[], char tarName[]){
	FILE * tar;
	FILE * new;
	stHeaderEntry* array = NULL;
	char *nameAux;
	int nFiles, i, tam, offData;
	int bytes = 0;

	if((tar=fopen(tarName, "r")) == NULL){
			perror("Error trying to open the tar file\n");
			return EXIT_FAILURE;
	}

	array = readHeader(tar, &nFiles);

	if(array == NULL){
		perror("Error reading the tarball's header\n");
		return EXIT_FAILURE;
	}

	tam = strlen(tarName)+strlen("_TMP_")+1;
	nameAux = malloc(tam);
	strcpy(nameAux, tarName);
	strcat(nameAux, "_TMP_");


	for(i=0; i < nFiles; i++){
		if(strcmp(array[i].name, remName) != 0){
			tam = strlen(array[i].name)+1;
			bytes += tam;
		}
	}

	offData=sizeof(int)+((nFiles-1)*sizeof(unsigned int)) + bytes;

	if((new=fopen(nameAux, "w")) == NULL){
			perror("Error trying to open the tar file\n");
			return EXIT_FAILURE;
	}

	if(fseek(new, offData, SEEK_SET) < 0){
			perror("Error in function fseek!\n");
			return EXIT_FAILURE;
	}

	for(i=0; i < nFiles; i++){
		if(strcmp(array[i].name, remName) != 0){
			if(copynFile(tar, new, array[i].size) != array[i].size){
				perror("Error writing in tmp File\n");
				return EXIT_FAILURE;
			}
		}
		else{
			if(fseek(tar, array[i].size, SEEK_CUR) == -1){
				perror("Error in fseek\n");
				return EXIT_FAILURE;
			}
		}
	}

	if(fclose(tar) != 0){
		perror("Error closing tarFile\n");
		return EXIT_FAILURE;
	}

	if(fseek(new, 0, SEEK_SET) == -1){
		perror("Error in function fseek!\n");
		return EXIT_FAILURE;
	}
	tam = nFiles -1;
	if(fwrite(&tam, sizeof(int), 1, new) < 0){
		perror("Error writing in tarFile\n");
		return EXIT_FAILURE;
	}
	for (i = 0; i < nFiles; i++) {
		if(strcmp(array[i].name, remName) != 0){
			if(fwrite(array[i].name, strlen(array[i].name)+1, 1,new) != -1){
				if(fwrite(&array[i].size, sizeof(unsigned int), 1, new) == -1){
					perror("Error writing in tarFile\n");
					return EXIT_FAILURE;
				}
				free(array[i].name);
			}
			else{
				perror("Error writing in tarFile\n");
				return EXIT_FAILURE;
			}
		}
	}

	if(fclose(new) != 0){
		perror("Error closing File _TMP_\n");
		return EXIT_FAILURE;
	}

	remove(tarName);
	rename(nameAux, tarName);

	return EXIT_SUCCESS;

}
