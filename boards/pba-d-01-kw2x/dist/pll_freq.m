1;
# not a function file:

F = [2405:5:2480];
i = 1;
PLL_INT0 = [];
PLL_FRAC0 = [];
tmp_frac = 0;
tmp_int = 11;

while (i <= length(F))
	tmp_frac = (F(i)./32 - tmp_int - 64).*65536;
	if tmp_frac >= 65536
		tmp_int++;
	else
		PLL_FRAC0 = [PLL_FRAC0 tmp_frac];
		PLL_INT0 = [PLL_INT0 tmp_int];
		i++;
	endif
endwhile

%F
%Fcalc = ((PLL_INT0 + 64) + (PLL_FRAC0./65536)).*32
%PLL_INT0
%PLL_FRAC0

printf("static const uint8_t pll_int_lt[%d] = {\n", length(F));
for i = 1:4:length(F)
	printf("    ");
	printf("%d,", PLL_INT0(i:1:i+3));
	printf("\n");
endfor
printf("};\n");
printf("\n");
printf("static const uint16_t pll_frac_lt[%d] = {\n", length(F));
for i = 1:4:length(F)
	printf("    ");
	printf("%d,", PLL_FRAC0(i:1:i+3));
	printf("\n");
endfor
printf("};\n\n");

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

POWER_MAX=8;
POWER_MIN=-35;
RANGE_MAX=31;
RANGE_MIN=3;

i = [0:1:abs(diff([POWER_MAX POWER_MIN]))];
pow_lt = round(i .* (RANGE_MAX - RANGE_MIN) ./ length(i) + RANGE_MIN);

printf("static const uint8_t pow_lt[%d] = {", length(pow_lt));
printf("    ");
for i = 0:1:length(pow_lt)-1
	if (rem(i,4) == 0)
		printf("\n    ");
	endif
	printf("%d,", pow_lt(i+1));
endfor
printf("\n};\n\n");

i = [0:1:abs(diff([RANGE_MAX RANGE_MIN]))];
level_lt = round(i .* (POWER_MAX - POWER_MIN) ./ length(i) + POWER_MIN);
length(level_lt)

printf("static const int level_lt[%d] = {", length(level_lt));
for i = 0:1:length(level_lt)-1
	if (rem(i,4) == 0)
		printf("\n    ");
	endif
	printf("%d,", level_lt(i+1));
endfor
printf("\n};\n\n");
