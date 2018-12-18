char spi_write(int fd, uint8_t buf);
char spi_read(int fd, uint8_t buf);
void spi_close(int fd);
int spi_setup(int argc, char *argv[]);

extern struct spi_ioc_transfer tr; 
