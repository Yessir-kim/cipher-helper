#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

#define FILE_NAME "./log/log.txt"
#define BUFFER_SIZE 1024
#define NUM_SIZE 10

void logging(char *msg);
void encryption(char *filename, int key, int flag);
void decryption(char *filename, int key, int flag);
void whole(char *filename, int en_key, int de_key);
void test(char *filename);
void usage(char *argv);
void hello();
void itoa(int n, char s[]);
void reverse(char s[]);

enum { ENCRYPTION_MODE, DECRYPTION_MODE, WHOLE_MODE, TEST_MODE } mode = TEST_MODE;

int main(int argc, char *argv[])
{
								hello();
								logging("[Running] started cipher-helper program");

								int opt;
								char *fname;
								int encrypt_key;
								int decrypt_key;

								while((opt = getopt(argc, argv, "e:d:")) != -1)
								{
																switch(opt)
																{
																case 'e': mode = ENCRYPTION_MODE; encrypt_key = atoi(optarg); break;
																case 'd':
																								if(mode == ENCRYPTION_MODE) mode = WHOLE_MODE;
																								else mode = DECRYPTION_MODE;
																								decrypt_key = atoi(optarg); break;
																default:
																								logging("[Terminating] illegal option");
																								fprintf(stderr, "Error: illegal option\n");
																								usage(argv[0]);
																}
								}

								if(argc - optind < 1)
								{
																logging("[Terminating] few arguments - filename");
																fprintf(stderr, "Error: too few arguments\n");
																usage(argv[0]);
								}

								fname = argv[optind++];

								switch(mode)
								{
								case ENCRYPTION_MODE: encryption(fname, encrypt_key, 0); break;
								case DECRYPTION_MODE: decryption(fname, decrypt_key, 0); break;
								case WHOLE_MODE: whole(fname, encrypt_key, decrypt_key); break;
								case TEST_MODE: test(fname); break;
								}

								logging("[Terminating] Exit program successful!");

								exit(EXIT_SUCCESS);
}

void test(char *filename)
{
								logging("[Running] Run TEST_MODE: view log about whole process");

								printf("***************************** 1. Encryption *****************************\n");
								printf("[TEST_MODE] ENCRYPTION: encrypt_key = 1234, filename = \'%s\'\n\n", filename);
								encryption(filename, 1234, 1);

								printf("\n\n************************ 2. Decryption (correct) ************************\n");
								printf("[TEST_MODE] DECRYPTION: decrypt_key = 1234, filename = \'out_enc\'\n\n");
								decryption("out_enc", 1234, 1);

								printf("\n\n************************* 3. Decryption (wrong) *************************\n");
								printf("[TEST_MODE] DECRYPTION: decrypt_key = 4321, filename = \'out_enc\'\n");
								printf("[TEST_MODE] DECRYPTION: If you enter the wrong password, output file will be wrong\n\n");
								decryption("out_enc", 4321, 1);

								printf("\n\n[TEST_MODE] If you want to use character password, such as \'hihi\', you cna do it. There is no limitation about password\n");
}

void whole(char *filename, int en_key, int de_key)
{
								logging("[Running] Run WHOLE_MODE: user want to use encryption & decryption at the same time");

								// 1. encryption step
								encryption(filename, en_key, 0);
								// 2. decryption step
								decryption("out_enc", de_key, 0);
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

								if((ifp = fopen(filename, "r")) == NULL) // read file pointer
								{
																fprintf(stderr, "Error: file not found \'%s\'\n", filename);
																logging("[Terminating] Decryption -- file not found");
																exit(EXIT_FAILURE);
								}

								if((ofp = fopen("out_dec", "w")) == NULL) // write file pointer
								{
																fprintf(stderr, "Error: cannot open file \'out_dec\'");
																logging("[Terminating] Decryption -- cannot open file");
																exit(EXIT_FAILURE);
								}

								while((read = getline(&line, &len, ifp)) != -1)
								{
																char *ptr = strtok(line, " ");

																while(ptr != NULL)
																{
																								// 1. convert string to int
																								uni = atoi(ptr);
																								// 2. sub key value
																								uni -= key;
																								// 3. convert int to char
																								each = (char)uni;

																								total++;

																								if(flag) printf("%c", each);

																								fputc(each, ofp);

																								ptr = strtok(NULL, " ");
																}
								}

								fclose(ifp);
								fclose(ofp);

								if(line) free(line);

								printf("\nSuccess: Decryption process successful! (total = %d, output filename = \'out_dec\')\n", total);
								logging("[Running] Decryption process successful!");
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

								if((ifp = fopen(filename, "r")) == NULL) // read file pointer
								{
																fprintf(stderr, "Error: file not found \'%s\'\n", filename);
																logging("[Terminating] Encryption -- file not found");
																exit(EXIT_FAILURE);
								}

								if((ofp = fopen("out_enc", "w")) == NULL) // write file pointer
								{
																fprintf(stderr, "Error: cannot open file \'out_enc\'");
																logging("[Terminating] Encryption -- cannot open file");
																exit(EXIT_FAILURE);
								}

								while((read = getline(&line, &len, ifp)) != -1)
								{
																total += strlen(line);

																for(int i = 0; i < strlen(line); i++)
																{
																								// 1. convert each char to int
																								uni = (int)line[i];
																								// 2. add key value
																								uni += key;
																								// 3. convert int to string
																								itoa(uni, num);

																								if(flag) printf("%s", num);

																								fwrite(num, 1, strlen(num), ofp);
																								fwrite(" ", 1, 1, ofp);
																}
								}

								fclose(ifp);
								fclose(ofp);

								if(line) free(line);

								printf("\nSuccess: Encryption process successful! (total = %d, output filename = \'out_enc\')\n", total);
								logging("[Running] Encryption process successful!");
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
								if(fp != NULL)
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

								if((sign = n) < 0) n = -n;

								do { s[i++] = n % 10 + '0'; } while((n /= 10) > 0);

								if(sign < 0) s[i++] = '-';

								s[i] = '\0';

								reverse(s);
}

void reverse(char s[])
{
								int j;
								char c;

								for(int i = 0, j = strlen(s) - 1; i < j; i++, j--)
								{
																c = s[i];
																s[i] = s[j];
																s[j] = c;
								}
}
