#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define FILE_NAME "./log/log.txt"
#define BUFFER_SIZE 1024
#define NUM_SIZE 10
#define NAME_SIZE 20

void logging(char *msg);
void encryption(char *filename, int key, int flag);
void encryption_files(char **filenames, int len, int key, int flag);
void decryption(char *filename, int key, int flag);
void decryption_files(char **filenames, int len, int key, int flag);
void whole(char *filename, int en_key, int de_key);
void whole_files(char **filenames, int len, int en_key, int de_key);
void test(char *filename);
void test_files(char **filenames, int len, char *path);
void usage(char *argv);
void hello();
void itoa(int n, char s[]);
void reverse(char s[]);
int isDirectory(const char *path);
int isFile(const char *path);
int getList(char *path, char ***list);

enum
{
	ENCRYPTION_MODE,
	DECRYPTION_MODE,
	WHOLE_MODE,
	TEST_MODE
} mode = TEST_MODE;

int main(int argc, char *argv[])
{
	hello();
	logging("[Running] started cipher-helper program");

	int opt, fileCnt;
	char *fname;
	char **files = NULL;
	int encrypt_key;
	int decrypt_key;

	while ((opt = getopt(argc, argv, "e:d:")) != -1)
	{
		switch (opt)
		{
		case 'e':
			mode = ENCRYPTION_MODE;
			encrypt_key = atoi(optarg);
			break;
		case 'd':
			if (mode == ENCRYPTION_MODE)
				mode = WHOLE_MODE;
			else
				mode = DECRYPTION_MODE;
			decrypt_key = atoi(optarg);
			break;
		default:
			logging("[Terminating] illegal option");
			fprintf(stderr, "Error: illegal option\n");
			usage(argv[0]);
		}
	}

	if (argc - optind < 1)
	{
		logging("[Terminating] few arguments - filename");
		fprintf(stderr, "Error: too few arguments\n");
		usage(argv[0]);
	}

	fname = argv[optind++];

	if (!isDirectory(fname) && !isFile(fname))
	{
		logging("[Terminating] File not found - isDirectory() && isFile() have a same value");
		fprintf(stderr, "Error: file not found \'%s\'\n", fname);
		exit(EXIT_FAILURE);
	}

	if (isDirectory(fname))
		fileCnt = getList(fname, &files);

	switch (mode)
	{
	case ENCRYPTION_MODE:
		if (isFile(fname))
			encryption(fname, encrypt_key, 0);
		else
			encryption_files(files, fileCnt, encrypt_key, 0);
		break;
	case DECRYPTION_MODE:
		if (isFile(fname))
			decryption(fname, decrypt_key, 0);
		else
			decryption_files(files, fileCnt, decrypt_key, 0);
		break;
	case WHOLE_MODE:
		if (isFile(fname))
			whole(fname, encrypt_key, decrypt_key);
		else
			whole_files(files, fileCnt, encrypt_key, decrypt_key);
		break;
	case TEST_MODE:
		if (isFile(fname))
			test(fname);
		else
			test_files(files, fileCnt, fname);
		break;
	}

	logging("[Terminating] Exit program successful!");

	if (isDirectory(fname))
	{
		/*
		for (int i = 0; i < fileCnt; i++)
		{
			printf("%s\n", files[i]);
		}
		*/

		free(files);
	}

	exit(EXIT_SUCCESS);
}

void test(char *filename)
{
	logging("[Running] RUN TEST_MODE(FILE): view log about whole process");

	printf("***************************** 1. ENCRYPTION *****************************\n");
	printf("[TEST_MODE] ENCRYPTION(FILE): encrypt_key = 1234, filename = \'%s\'\n\n", filename);
	encryption(filename, 1234, 1);

	printf("\n\n*************************** 2. DECRYPTION ***************************\n");
	printf("[TEST_MODE] DECRYPTION(FILE): decrypt_key = 1234, filename = \'%s\'\n\n", filename);
	decryption(filename, 1234, 1);

	printf("\n\n[TEST_MODE] If you want to use character password, such as \'hihi\', you cna do it. There is no limitation about password\n");
}

void test_files(char **filenames, int len, char *path)
{
	logging("[Running] RUN TEST_MODE(DIR): view log about whole process");

	printf("***************************** 1. ENCRYPTION *****************************\n");
	printf("[TEST_MODE] ENCRYPTION(DIR): encrypt_key = 1234, path = \'%s\'\n\n", path);
	encryption_files(filenames, len, 1234, 1);

	printf("\n\n*************************** 2. DECRYPTION ***************************\n");
	printf("[TEST_MODE] DECRYPTION(DIR): decrypt_key = 1234, path = \'%s\'\n\n", path);
	decryption_files(filenames, len, 1234, 1);

	printf("\n\n[TEST_MODE] If you want to use character password, such as \'hihi\', you cna do it. There is no limitation about password\n");
}

void whole(char *filename, int en_key, int de_key)
{
	logging("[Running] RUN WHOLE_MODE(FILE): user want to use encryption & decryption at the same time");

	// 1. encryption step
	encryption(filename, en_key, 0);

	// 2. decryption step
	decryption(filename, de_key, 0);
}

void whole_files(char **filenames, int len, int en_key, int de_key)
{
	logging("[Running] RUN WHOLE_MODE(DIR): user want to use encryption & decryption at the same time");

	// 1. encryption step
	encryption_files(filenames, len, en_key, 0);

	// 2. decryption step
	decryption_files(filenames, len, de_key, 0);
}

void decryption(char *filename, int key, int flag)
{
	FILE *ifp = NULL;
	FILE *ofp = NULL;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int uni, total = 0;
	char each;
	char *ret;

	if ((ifp = fopen(filename, "r")) == NULL) // read file pointer
	{
		fprintf(stderr, "Error: file not found \'%s\'\n", filename);
		logging("[Terminating] DECRYPTION(FILE) -- file not found");
		exit(EXIT_FAILURE);
	}

	ret = strstr(filename, "_enc");
	if (ret)
		strcat(filename, "2dec");
	else
		strcat(filename, "_dec");

	if ((ofp = fopen(filename, "w")) == NULL) // write file pointer
	{
		fprintf(stderr, "Error: cannot open file \'%s\'", filename);
		logging("[Terminating] DECRYPTION(FILE) -- cannot open file");
		exit(EXIT_FAILURE);
	}

	while ((read = getline(&line, &len, ifp)) != -1)
	{
		char *ptr = strtok(line, " ");

		while (ptr != NULL)
		{
			// 1. convert string to int
			uni = atoi(ptr);
			// 2. sub key value
			uni -= key;
			// 3. convert int to char
			each = (char)uni;

			total++;

			if (flag)
				printf("%c", each);

			fputc(each, ofp);

			ptr = strtok(NULL, " ");
		}
	}

	fclose(ifp);
	fclose(ofp);

	if (line)
		free(line);

	printf("\nSuccess: DECRYPTION(FILE) process successful! (total = %d, output filename = \'%s\')\n\n", total, filename);
	logging("[Running] DECRYPTION(FILE) process successful!");
}

void encryption(char *filename, int key, int flag)
{
	FILE *ifp = NULL;
	FILE *ofp = NULL;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int uni, total = 0;
	char num[NUM_SIZE];
	char *ret;

	if ((ifp = fopen(filename, "r")) == NULL) // read file pointer
	{
		fprintf(stderr, "Error: file not found \'%s\'\n", filename);
		logging("[Terminating] ENCRYPTION(FILE) -- file not found");
		exit(EXIT_FAILURE);
	}

	ret = strstr(filename, "_dec");
	if (ret)
		strcat(filename, "2enc");
	else
		strcat(filename, "_enc");

	if ((ofp = fopen(filename, "w")) == NULL) // write file pointer
	{
		fprintf(stderr, "Error: cannot open file \'%s\'", filename);
		logging("[Terminating] ENCRYPTION(FILE) -- cannot open file");
		exit(EXIT_FAILURE);
	}

	while ((read = getline(&line, &len, ifp)) != -1)
	{
		total += strlen(line);

		for (int i = 0; i < strlen(line); i++)
		{
			// 1. convert each char to int
			uni = (int)line[i];
			// 2. add key value
			uni += key;
			// 3. convert int to string
			itoa(uni, num);

			if (flag)
				printf("%s", num);

			fwrite(num, 1, strlen(num), ofp);
			fwrite(" ", 1, 1, ofp);
		}
	}

	fclose(ifp);
	fclose(ofp);

	if (line)
		free(line);

	printf("\nSuccess: ENCRYPTION(FILE) process successful! (total = %d, output filename = \'%s\')\n\n", total, filename);
	logging("[Running] ENCRYPTION(FILE) process successful!");
}

void decryption_files(char **filenames, int len, int key, int flag)
{
	for (int i = 0; i < len; i++)
	{
		FILE *ifp = NULL;
		FILE *ofp = NULL;
		char *line = NULL;
		size_t len = 0;
		ssize_t read;
		int uni, total = 0;
		char each;
		char *ret;

		if ((ifp = fopen(filenames[i], "r")) == NULL) // read file pointer
		{
			fprintf(stderr, "Error: file not found \'%s\'\n", filenames[i]);
			logging("[Terminating] DECRYPTION(DIR) -- file not found");
			exit(EXIT_FAILURE);
		}

		ret = strstr(filenames[i], "_enc");

		if (ret)
			strcat(filenames[i], "2dec");
		else
			strcat(filenames[i], "_dec");

		if ((ofp = fopen(filenames[i], "w")) == NULL) // write file pointer
		{
			fprintf(stderr, "Error: cannot open file \'%s\'", filenames[i]);
			logging("[Terminating] DECRYPTION(DIR) -- cannot open file");
			exit(EXIT_FAILURE);
		}

		while ((read = getline(&line, &len, ifp)) != -1)
		{
			char *ptr = strtok(line, " ");

			while (ptr != NULL)
			{
				// 1. convert string to int
				uni = atoi(ptr);
				// 2. sub key value
				uni -= key;
				// 3. convert int to char
				each = (char)uni;

				total++;

				if (flag)
					printf("%c", each);

				fputc(each, ofp);

				ptr = strtok(NULL, " ");
			}
		}

		fclose(ifp);
		fclose(ofp);

		if (line)
			free(line);

		printf("\nSuccess: DECRYPTION(DIR) process successful! (total = %d, output filename = \'%s\')\n\n", total, filenames[i]);
	}

	logging("[Running] DECRYPTION(DIR) process successful!");
}

void encryption_files(char **filenames, int len, int key, int flag)
{
	for (int i = 0; i < len; i++)
	{
		FILE *ifp = NULL;
		FILE *ofp = NULL;
		char *line = NULL;
		size_t len = 0;
		ssize_t read;
		int uni, total = 0;
		char num[NUM_SIZE];
		char *ret;

		if ((ifp = fopen(filenames[i], "r")) == NULL) // read file pointer
		{
			fprintf(stderr, "Error: file not found \'%s\'\n", filenames[i]);
			logging("[Terminating] ENCRYPTION(DIR) -- file not found");
			exit(EXIT_FAILURE);
		}

		ret = strstr(filenames[i], "_dec");
		if (ret)
			strcat(filenames[i], "2enc");
		else
			strcat(filenames[i], "_enc");

		if ((ofp = fopen(filenames[i], "w")) == NULL) // write file pointer
		{
			fprintf(stderr, "Error: cannot open file \'%s\'", filenames[i]);
			logging("[Terminating] ENCRYPTION(DIR) -- cannot open file");
			exit(EXIT_FAILURE);
		}

		while ((read = getline(&line, &len, ifp)) != -1)
		{
			total += strlen(line);

			for (int i = 0; i < strlen(line); i++)
			{
				// 1. convert each char to int
				uni = (int)line[i];
				// 2. add key value
				uni += key;
				// 3. convert int to string
				itoa(uni, num);

				if (flag)
					printf("%s", num);

				fwrite(num, 1, strlen(num), ofp);
				fwrite(" ", 1, 1, ofp);
			}
		}

		fclose(ifp);
		fclose(ofp);

		if (line)
			free(line);

		printf("\nSuccess: ENCRYPTION(DIR) process successful! (total = %d, output filename = \'%s\')\n\n", total, filenames[i]);
	}

	logging("[Running] ENCRYPTION(DIR) process successful!");
}

void usage(char *argv)
{
	fprintf(stderr, "Usage: %s [-e password] [-d password] filename\n", argv);

	exit(EXIT_FAILURE);
}

void logging(char *msg)
{
	FILE *fp;
	char buf[BUFFER_SIZE];
	time_t uctTime;
	struct tm *locTime;

	(void)time(&uctTime);

	memset(buf, 0x00, sizeof(buf));
	locTime = localtime(&uctTime);

	strftime(buf, 255, "[%F %H:%M:%S] : ", locTime);

	fp = fopen(FILE_NAME, "a");
	if (fp != NULL)
	{
		fprintf(fp, "%s", buf);
		fprintf(fp, "%s\n", msg);

		fclose(fp);
		fp = NULL;
	}
}

void hello()
{
	printf("***************************** cipher-helper *****************************\n");
	printf("*                                                                       *\n");
	printf("*             This program will protect yout private files!             *\n");
	printf("*                                                                       *\n");
	printf("*                         Author : Yessir-kim                           *\n");
	printf("*               Github : https://github.com/Yessir-kim                  *\n");
	printf("*                                                                       *\n");
	printf("*************************************************************************\n");
}

void itoa(int n, char s[])
{
	int i = 0, sign;

	if ((sign = n) < 0)
		n = -n;

	do
	{
		s[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);

	if (sign < 0)
		s[i++] = '-';

	s[i] = '\0';

	reverse(s);
}

void reverse(char s[])
{
	int j;
	char c;

	for (int i = 0, j = strlen(s) - 1; i < j; i++, j--)
	{
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

// TRUE = 1, FALSE = 0
int isDirectory(const char *path)
{
	struct stat statbuf;
	stat(path, &statbuf);
	return S_ISDIR(statbuf.st_mode); // directory
}

// TRUE = 1, FALSE = 0
int isFile(const char *path)
{
	struct stat statbuf;
	stat(path, &statbuf);
	return S_ISREG(statbuf.st_mode);
}

int getList(char *path, char ***list)
{
	DIR *d;
	struct dirent *dir;
	int idx = 0, cnt = 0;

	d = opendir(path);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_REG)
			{
				printf("Find: find a file \'%s\' under the \'%s\' dir\n", dir->d_name, path);
				cnt++;
			}
		}

		rewinddir(d);

		*list = (char **)malloc(sizeof(char *) * cnt);

		strcat(path, "/");

		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_REG)
			{
				(*list)[idx] = (char *)malloc(sizeof(char) * NAME_SIZE);
				strcat((*list)[idx], path);
				strcat((*list)[idx], dir->d_name);
				// printf("%s\n", (*list)[idx]);
				idx++;
			}
		}

		closedir(d);
	}

	return cnt;
}