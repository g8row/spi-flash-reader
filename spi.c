#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>


static void pabort(const char *s)
{
		perror(s);
		abort();
}
int main()
{
	int dev = open("/dev/spidev0.0", O_RDWR);
	int file = open("spictest.bin", O_RDWR| O_TRUNC | O_CREAT , S_IRWXU);
	
	if (dev < 0)
		pabort("can't open device");
	int ret;
	int mode = SPI_MODE_1;	
	ret = ioctl(dev, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)	
		pabort("can't set spi mode");
	ret = ioctl(dev, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi1 mode");
	int bits = 8;
	ret = ioctl(dev, SPI_IOC_WR_BITS_PER_WORD ,&bits );
	if (ret == -1)
		pabort("can't set bits per word");
	ret = ioctl(dev, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");
	int speed = 80000000;
	ret = ioctl(dev, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if(ret == -1)
		pabort("can't set max speed");
	ret = ioctl(dev, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if(ret == -1)
		pabort("can't get max speed");

	int sector = 1024;	
	u_int8_t tx[4+1024];
        u_int8_t rx[4+1024] = {0, };
	
	u_int8_t cmd = 0x03;
	int address;	
	
        struct spi_ioc_transfer tr = {
               	.tx_buf = (unsigned long)tx,
               	.rx_buf = (unsigned long)rx,
               	.len = 4+sector,
               	.delay_usecs = 0,
               	.speed_hz = speed,
               	.bits_per_word = bits,
       	};

	int writes=1024*(4096/sector);
	
	for(int i=0;i<writes;i++){
		address = i*sector;	
		tx[0] = cmd;
		tx[1] = (address >> 16) & 0b11111111;
		tx[2] = (address >> 8) & 0b11111111;
		tx[3] = address & 0b11111111;
		
        	ret = ioctl(dev, SPI_IOC_MESSAGE(1), &tr);
        	if (ret < 1)
                	pabort("can't send spi message");

		lseek(file,0,SEEK_END);
		write(file,&(rx[4]),sector);
	}
	close(dev);
	close(file);
}
