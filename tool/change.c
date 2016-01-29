#include "apue.h"
#include <fcntl.h>

#define	LOADER_INODE_ISIZE	0x9E18E4
#define	KERNEL_INODE_ISIZE	0x9e1904

int main(void)
{
	int	fd_img, fd_loader, fd_kernel;
	int	len;
	char	buf[MAXLINE];

	struct	stat	statbuf;

	if((fd_img = open("/Muse/80m.img", O_RDWR)) == -1)
		err_sys("open 80m.img error");
	if((fd_loader = open("/Muse/MuseLoader", O_RDWR)) == -1)
		err_sys("open MuseLoader error");
	if((fd_kernel = open("/Muse/MuseKernel", O_RDWR)) == -1)
		err_sys("open MuseKernel error");
	
	/* loader */
	if(lseek(fd_img, LOADER_INODE_ISIZE, SEEK_SET) == -1)
		err_sys("lseek loader inode wrong");

	if(fstat(fd_loader, &statbuf) == -1)
		err_sys("loader fstat err");

	if(write(fd_img, &statbuf.st_size, 4) != 4)
		err_sys("loader write err");

	/* kernel */
	if(lseek(fd_img, KERNEL_INODE_ISIZE, SEEK_SET) == -1)
		err_sys("lseek kernel inode wrong");

	if(fstat(fd_kernel, &statbuf) == -1)
		err_sys("kernel fstat err");

	if(write(fd_img, &statbuf.st_size, 4) != 4)
		err_sys("kernel write err");

	exit(0);
}
