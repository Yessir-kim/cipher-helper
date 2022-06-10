# cipher-helper

## 21600108 Youngbin Kim 
presentation link: https://youtu.be/5raEelO8UbA
## What does this project do?
This project is designed to provide cipher service.\
User can encrypt the private file, and also decrypt it based on user password. 
* support the single file path, also directory path is available 
* for the first-time user, this program provide TEST_MODE 

https://user-images.githubusercontent.com/52683010/173108459-d2d092ef-a863-4d02-923d-947057c756e0.png

## How to get started?
0. Clone this repository on your local 

1. Compile main.c file 
```
$ gcc main.c -o main
```

2. Run program 
```
$ ./main [-e password] [-d password] [file_path or dir_path]
```

* There are four modes: ENCRYPTION, DECRYPTION, WHOLE and TEST
  * ENCRYPTION MODE: ./main [-e password] [file_path or dir_path]
  * DECRYPTION MODE: ./main [-d password] [file_path or dir_path]
  * WHOLE MODE: ./main [-e password] [-d password] [file_path or dir_path]
  * TEST MODE: ./main [file_path or dir_path]


## Contribution 
If you have any idea for this project, please contact following email address. 
* apower321@naver.com 
