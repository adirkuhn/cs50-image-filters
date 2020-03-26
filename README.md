Uncompressed BMP filter
===

Basic image filter as proposed in the cs50, this implementation covers:

 * blur
 * grayscale
 * reflect
 * edges
 
Blur (-b option)
-
Adds blur filter to BMP file.

![Original](./images/tower.bmp)
![Blur](./images/blur.bmp)

Grayscale
--
Turn a colored image to grayscale mode.

![Original](./images/tower.bmp)
![Grayscale](./images/grayscale.bmp)

Reflect
--
Mirror (reflect) the image.

![Original](./images/tower.bmp)
![Mirrored](./images/reflect.bmp)

Edges
--
Detects the image edges using the Sobel Algorithm.

![Original](./images/tower.bmp)
![Mirrored](./images/edges.bmp)

Compiling
--

Assuming that you have `clang` and `make` you type:

 > make filter

Usage
--

 > ./filter -[filter option] [input file] [output file]

e.g.

 > ./filter -b image.bmp blue.bmp
>
 > ./filter -g image.bmp grayscale.bmp
>
 > ./filter -r image.bmp reflected.bmp
>
 > ./filter -e image.bmp edges.bmp
