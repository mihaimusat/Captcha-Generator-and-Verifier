build: captcha_codes.c bmp_header.h
	gcc -Wall captcha_codes.c -o captcha

run: captcha
	./captcha

clean:
	rm captcha
