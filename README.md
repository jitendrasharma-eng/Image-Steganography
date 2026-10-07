# Image Steganography in C

## 📌 Project Overview

Image Steganography is a C-based project that hides secret data inside a BMP image without noticeably changing the appearance of the image.

The project uses the **Least Significant Bit (LSB)** technique to encode and decode secret information.

## 🚀 Features

* Encode secret data into a BMP image
* Decode hidden data from a stego image
* Supports hiding text/data inside an image
* Uses LSB-based steganography
* File handling using C
* Command-line based application

## 🛠️ Technologies Used

* C Programming
* File Handling
* Bit Manipulation
* Pointers
* Structures
* BMP Image Processing
* LSB Steganography

## ⚙️ How It Works

During encoding, the project modifies the least significant bits of the image pixel data to store secret information.

During decoding, these bits are extracted to recover the hidden information.

## ▶️ Usage

### Encoding

```bash
./a.out -e source.bmp secret.txt stego.bmp
```

### Decoding

```bash
./a.out -d stego.bmp
```
## 🎯 Learning Outcomes

Through this project, I gained practical experience in:

* C programming
* Bitwise operations
* File I/O
* Pointers and structures
* BMP file handling
* Encoding and decoding algorithms
* Debugging and memory management

🚀 How to Run
## 🚀 How to Run

### 1. Clone the Repository

```bash
git clone <YOUR_GITHUB_REPOSITORY_URL>
cd Image-Stegnography_Project
```

### 2. Compile

```bash
gcc *.c
```

### 3. Encode

Hide your secret file inside a BMP image:

```bash
./a.out -e source.bmp secret.txt stego.bmp
```

* `source.bmp` → Original image
* `secret.txt` → Secret data
* `stego.bmp` → Encoded image

### 4. Decode

Extract the hidden data:

```bash
./a.out -d stego.bmp
```

The decoded data will be saved as:

```text
outputFile.txt
```

**Requirements:** GCC compiler and a BMP image with sufficient capacity.


## 👨‍💻 Author

**Jitendra Sharma**

Electronics & Communication Engineering
Embedded Systems Enthusiast
