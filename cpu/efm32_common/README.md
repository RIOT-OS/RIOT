Silicon Labs EFM32 CPU implementation
Based on the existing STM32F0 CPU support and the support libraries available from Silicon Laboratories
For use (currently) with the ST3700 EFM32GG development kit

Layout:
	periph - peripheral drivers (RIOT-OS standard)
	CMSIS - Silicon Labs CMSIS files
	~~Device - Silicon Labs EFM32 files~~ Movced to family specific folders
	emlib - Silicon Labs BSP
	include - header files
	
Silicon labs files copied from the Simplicity Studio IDE
Care has been taken to avoid alteration to the provided files to ensure simple updates with new IDE releases

See: https://github.com/RIOT-OS/RIOT/issues/1123#issuecomment-52852285