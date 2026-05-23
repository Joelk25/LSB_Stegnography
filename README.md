# LSB Image Steganography

A command-line tool to hide and retrieve secret files inside BMP images using the Least Significant Bit (LSB) technique.

## How it works

Each pixel byte in a BMP image has its LSB replaced with one bit of the secret data. Since only 1 bit per byte changes, the image looks identical to the naked eye. The tool encodes a magic string, file extension, file size, and file data — all recoverable during decoding.

## Usage

## Build

```
gcc *.c
```

### Encode
```
./a.out -e <source.bmp> <secret_file> [output.bmp]
```
Hides `secret_file` inside `source.bmp`. Output defaults to `stego.bmp` if not specified.

### Decode
```
./a.out -d <stego.bmp> [output_file]
```
Extracts the hidden file from `stego.bmp`. Output defaults to `output` if not specified.

### Examples
```
./a.out -e beautiful.bmp secret.txt
./a.out -e beautiful.bmp secret.txt mystego.bmp
./a.out -d stego.bmp
./a.out -d stego.bmp recovered.txt
```


## Project Structure

```
.
├── main.c        # Entry point, argument parsing
├── encode.c/h    # Encoding logic
├── decode.c/h    # Decoding logic
├── common.h      # Magic string definition
├── types.h       # Shared types (Status, OperationType)
├── beautiful.bmp # Sample source image
├── secret.txt    # Sample secret file
└── stego.bmp     # Sample encoded output
```

## Constraints

- Source and stego images must be `.bmp` files
- Secret file must have an extension — any extension works (`.txt`, `.png`, `.c`, etc.)
- Image must have enough capacity to hold the secret data — capacity is checked before encoding
