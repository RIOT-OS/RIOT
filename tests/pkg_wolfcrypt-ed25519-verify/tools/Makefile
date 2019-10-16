all: ed25519_sign_msg


ed25519_sign_msg: ed25519_sign_msg.c
	gcc -o $@ $^ -lwolfssl -DHAVE_ED25519 -DWOLFCRYPT_ONLY -DTFM_TIMING_RESISTANT -DNO_RSA -DECC_TIMING_RESISTANT

clean:
	@rm -f ed25519_sign_msg
