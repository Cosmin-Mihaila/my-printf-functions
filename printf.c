#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

static int write_stdout(const char *token, int length)
{
	int rc;
	int bytes_written = 0;

	do {
		rc = write(1, token + bytes_written, length - bytes_written);
		if (rc < 0)
			return rc;

		bytes_written += rc;
	} while (bytes_written < length);

	return bytes_written;
}

static int hexToDec (int auxNr) {
	char* hexStr = (char*)malloc(9 * sizeof(char));

	// Il punem intr-un unsigned pentru a rezolva si cazurile negative
	unsigned int n = auxNr;
	int i = 0, j;
	int rest  = 0;

	while(n) {   
		rest = n % 16; 
	// Vedem daca restul este o cifra sau litera si folosim ASCII pentru
	// A scrie caracterul corespunzator
		*(hexStr + i) = rest + ((rest < 10)? 48 : 87);

		i++;
		n = n/16; 
	} 
	*(hexStr + i) = '\0';

	for(j = i - 1; j >= 0; j--) {
		write_stdout(hexStr + j, 1);  
	}

    free(hexStr);
	return i;

}

static int longToStr(long long n){
	char* str = (char*)malloc(20 * sizeof(char));
	int i = 0, j;

	// Luam fiecare cifra si o scriem in string folosind codurile ASCII
	while(n){
		*(str + i) = (n % 10) + 48;
		n = n/10;
		i++;
	}
	*(str + i) = '\0';

	for(j = i - 1; j >= 0; j--) {
		write_stdout(str + j, 1);  
	}
	free(str);
	return i;
}

static int intToStr (int auxNr){
	long long n = auxNr;
	int count = 0;

	// Verificam daca intregul primit este negativ
	if(auxNr < 0){
		write_stdout("-", 1);
		n = n * (-1);
		count++;
	}

	return count + longToStr(n);
}

static int uIntToStr (unsigned int auxNr){
	long long n = auxNr;
	return longToStr(n);
}


int iocla_printf(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	int newSpec = 0;
	int count = 0;
	int simpleChar = 1;

	// Parcurgem fiecare caracter din sirul primit
	for(int i = 0; i < strlen(format); i++){

		// Verificam daca urmeaza un specificator
		if ( *(format + i) == '%' && !newSpec ) {
			newSpec = 1;
			simpleChar = 0;
			continue;
		}

		// Verificam ce specificator urmeaza
		if (newSpec) {
			simpleChar = 0;
			newSpec = 0;
			switch( *(format + i) ){
				case '%' : {
					write_stdout("%", 1);
					count++;
					break;
				}
				case 'd' : {
					count += intToStr(va_arg(args, int));
					break;
				}
				case 'u' : {
					count += uIntToStr(va_arg(args, unsigned int));
					break;
				}
				case 'x' : {
					count += hexToDec(va_arg(args, int));
					break;
				}
				case 'c' : {
					char* x = (char*)malloc(2 * sizeof(char));
					*x = (char)va_arg(args, int);
					*(x + 1) = '\0';
					write_stdout(x, 1);
					count ++;

					free(x);
					break;
				}
				case 's' : {
					char* auxStr = (char*)malloc(100 * sizeof(char));
					strcpy(auxStr, va_arg(args, char*));
					write_stdout(auxStr, strlen(auxStr));
					count += strlen(auxStr);

					free(auxStr);
					break;
				}

			}
		}

		// Verificam daca caracterul curent este unul simplu, adica
		// daca nu apartine niciunui specificator
		if(simpleChar) {
			write_stdout(format + i, 1);
			count ++;
		}
		simpleChar = 1;
	}

	return count;
}

