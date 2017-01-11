all: inc inc/get_tweetnacl_h src/get_tweetnacl_c

inc:
	mkdir include

inc/get_tweetnacl_h:
	wget http://tweetnacl.cr.yp.to/20140427/tweetnacl.h -O include/tweetnacl.h

src/get_tweetnacl_c:
	wget http://tweetnacl.cr.yp.to/20140427/tweetnacl.c -O src/tweetnacl.c

clean:
	rm -rf include
	rm -f src/tweetnacl.c
