Silicon Labs EFM32 CPU implementation
Based on the existing STM32F0 CPU support
For use (currently) with the ST3700 EFM32GG development kit

Layout:
	periph - peripheral drivers (RIOT-OS standard)
	CMSIS - Silicon Labs CMSIS files
	Device - Silicon Labs EFM32 files
	emlib - Silicon Labs BSP
	include - header files
	
Silicon labs files copied from the Simplicity Studio IDE
Care has been taken to avoid alteration to the provided files to ensure simple updates with new IDE releases
