to start the openssl server:
	openssl s_server -key server-prk.pem -cert server-cer.pem -accept 10.0.110.71:44330 -www -debug

Please, change the IP address to your corresponding interface.

Kconfig should be configured before the build through make menuconfig.
