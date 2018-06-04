#include <stdio.h>
#include <string.h>
#include <xtimer.h>
#include <math.h>

/*RFID*/
#include "mlx90109.h"
#include "mlx90109_params.h"



int sleepTime = 2;

	/*RFID */
	mlx90109_t mlx90109_dev;
	mlx90109_fdxb_tagdata neuerTag;
	
//Read Data
void interrupt_mlx90109(void *args)
{
	if (args){}
	int16_t temp = 0;
	temp = mlx90109_read_fdxb(&mlx90109_dev);
	if (temp == MLX90109_DATA_OK)
	{
		//Raw Data to Tag Data and CRC
		temp = mlx90109_format_fdxb(&mlx90109_dev, &neuerTag);
		if (temp == MLX90109_OK){
			neuerTag.newTag = 1;
		}
	}
	return;
}

int main(void)
{
	puts("MLX90901 AnimalTag driver test application\n");
 
	mlx90109_init(&mlx90109_dev, &mlx90109_params[0], interrupt_mlx90109); 
 
    while (1) {
		if (neuerTag.newTag == 1){
			neuerTag.newTag = 0;
			printf("TagId: 0x%x%x \n" , (unsigned int)(neuerTag.tagId>>32),(unsigned int)neuerTag.tagId);
			neuerTag.tagId = 0;
		}
		xtimer_sleep(sleepTime);
    }
}
