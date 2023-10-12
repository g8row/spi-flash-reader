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
static void transfer(int fd, u_int8_t* tx,u_int8_t* rx, int len)
{
        int ret;
        struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx,
                .rx_buf = (unsigned long)rx,
                .len = len,
                .delay_usecs = 0,
                .speed_hz = 410000,
                .bits_per_word = 8,
        };

        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
                pabort("can't send spi message");

        //puts("");
}
int main()
{
	int dev = open("/dev/spidev0.0", O_RDWR);
	int file = open("spictest.bin", O_RDWR | O_APPEND | O_CREAT , S_IRWXU); // | S_IRWXG | S_IRWXO);
	
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
	int speed = 410000;
	ret = ioctl(dev, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if(ret == -1)
		pabort("can't set max speed");
	ret = ioctl(dev, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if(ret == -1)
		pabort("can't get max speed");
	int sector = 256;	
	u_int8_t tx[4+256];
        u_int8_t rx[4+256] = {0, };
	
	for(int i=0;i<1024*16;i++){
		u_int8_t cmd = 0x03;
		int address = i*sector;	
		tx[0] = cmd;
		tx[1] = (address >> 16) & 0b11111111;
		tx[2] = (address >> 8) & 0b11111111;
		tx[3] = address & 0b11111111;
		printf("read half-sector %d\n",i);
		transfer(dev,tx,rx,4+sector);
		/*for(int j=0;j<8;j++){
			printf("in rx[%d] %x\n",j+4,rx[j+4]);
		}*/
		//lseek(file,0,SEEK_END);
		printf("%d\n",write(file,&(rx[4]),sector));
	}
	close(dev);
	close(file);
}
