#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#if FS
#include "fs.h"

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;


#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

/* YOUR CODE GOES HERE */

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2) {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}
     
int fs_mkfs(int dev, int num_inodes) {
  int i;
  
  if (dev == 0) {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  i = fsd.nblocks;
  while ( (i % 8) != 0) {i++;}
  fsd.freemaskbytes = i / 8; 
  
  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }
  
  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }
  
  fsd.inodes_used = 0;
  
  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
  
  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);

  return 1;
}

void fs_print_fsd(void) {

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit);
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
  return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}

int fs_create(char *filename, int mode)
{
	struct inode *in;
	if(mode != O_CREAT)
	{
		kprintf("Error: only O_CREAT mode is required while creating file");
		return SYSERR;
	}
	
	if((fsd.root_dir.numentries > NUM_FD)|| (fsd.root_dir.numentries <0)){
        kprintf("\nERROR: File System full, no new file can be created !");
        return SYSERR;
    }
	else{
		for(int i=0;i<fsd.root_dir.numentries;i++)
		{
			if(strcmp(filename,fsd.root_dir.entry[i].name) == 0)
			{	
				kprintf("\nERROR:File %s already exists!", fsd.root_dir.entry[i].name);
				return SYSERR;
			}
		}
	}
	in = getmem(sizeof(struct inode));
	in->id = fsd.inodes_used +1;
	fsd.inodes_used++;
	in->type = INODE_TYPE_FILE;
	in->nlink = 1;
	in->device = 0;
	in->size = 0;
	for(int i=0;i<INODEBLOCKS;i++){
		in->blocks[i] = -1;
	}
	fs_put_inode_by_num(0,in->id,in);
	fsd.root_dir.entry[fsd.root_dir.numentries].inode_num  = in->id;
	strncpy(fsd.root_dir.entry[fsd.root_dir.numentries].name,filename,strlen(filename));
	fsd.root_dir.numentries++; 
	return OK;

}

int fs_open(char *filename, int flags)
{
	int fd = next_open_fd;
	int fileExistFlag=0;
	if(flags<0 || flags >2){
		kprintf("\nERROR: Invalid Flag");
		return SYSERR;
	}
	if(fsd.root_dir.numentries <= 0){
		kprintf("\nERROR: File system empty");
		return SYSERR;
	}
	if (fsd.root_dir.numentries > NUM_FD){
		return SYSERR;
	}
	for(int i=0;i<fsd.root_dir.numentries;i++){
		
		if(strcmp(filename,fsd.root_dir.entry[i].name) == 0){
			fileExistFlag=1;//i.e file exists
			for(int j = 0;j < NUM_FD; j++){
				if((oft[j].state == FSTATE_OPEN)&&(strcmp(filename,oft[j].de->name) == 0)){
					kprintf("\nFile already open");
					fd=j;
					return fd;
				}
			}
			//write filetable
			oft[fd].state = FSTATE_OPEN; 
			oft[fd].fileptr = 0;
			oft[fd].flags = flags;
			oft[fd].de = getmem(sizeof(struct dirent));
			oft[fd].de->inode_num = fsd.root_dir.entry[i].inode_num;
			strncpy(oft[fd].de->name,fsd.root_dir.entry[i].name,strlen(fsd.root_dir.entry[i].name));
			fs_get_inode_by_num(0,oft[fd].de->inode_num,&oft[fd].in); 
		}
	}	
	
	if (fileExistFlag==0){
		kprintf("\nERROR: File %s does not exist",filename);
		return SYSERR;
	}
	printf("\nSuccess: File opened");
	return fd;
}

int fs_write(int fd, void *buf, int nbytes){
	if(fd>NUM_FD || fd<0 ||fd >next_open_fd){
		kprintf("\nERROR: Invalid file handle");
		return SYSERR;
	}
	if(oft[fd].flags < 1 || oft[fd].flags > 2 || oft[fd].state!=FSTATE_OPEN){
		kprintf("\nERROR: File is not opened for writing");
		return SYSERR;
	}
	if(nbytes <= 0){
		kprintf("\nERROR: Number of bytes to write are invalid");
		return SYSERR;
	}

	struct inode in = oft[fd].in;
	in.size = in.size - (in.size - oft[fd].fileptr);
	int blk = oft[fd].fileptr / dev0_blocksize;
    int blknum;
	int offset;
	for(int i=0;i<dev0_numblocks;i++){
		if(fs_getmaskbit(i) == 0){
			blknum=i;
			oft[fd].in.blocks[blk]=blknum;
		}
	}
	int byteswritten = 0;
	int writebytes = 0;
	int bytestowrite = nbytes;
	while (bytestowrite > 0){
		fs_setmaskbit(blknum);
		if(dev0_blocksize-offset > bytestowrite)
		{
			writebytes = bytestowrite;
		}
		else
		{
			writebytes = dev0_blocksize - offset;
		}
		if(bs_bwrite(dev0,blknum,offset,&buf[byteswritten],writebytes) == SYSERR)
		{
			kprintf("\nError: not able to write in file");
			oft[fd].in.size = oft[fd].in.size + byteswritten;
			return byteswritten;
		}
		byteswritten = byteswritten + writebytes;
		bytestowrite = bytestowrite - writebytes;
		blk++;
		
		for(int i=0;i<dev0_numblocks;i++){
			if(fs_getmaskbit(i) == 0){
				oft[fd].in.blocks[blk] = i;
			}
		}
		blknum =  oft[fd].in.blocks[blk]; 
		offset = 0;
	}
	oft[fd].fileptr = oft[fd].in.size;
	oft[fd].in.size = oft[fd].in.size + byteswritten; 
	oft[fd].fileptr = oft[fd].in.size;
	return byteswritten;
}

int fs_close(int  fd){
	if(oft[fd].state==FSTATE_CLOSED || fd <0){
		kprintf("\nERROR: File already closed or file handle is invalid");
		return SYSERR;
	}
	fs_put_inode_by_num(0,oft[fd].in.id,&oft[fd].in);
	freemem(oft[fd].de,sizeof(struct dirent));
	oft[fd].state=FSTATE_CLOSED;
	return OK;
}

int fs_read(int fd, void *buf, int nbytes)
{	
	if(oft[fd].flags != O_RDONLY && oft[fd].flags != O_RDWR)
	{
		kprintf("\nERROR: File mode is not read!\n");
		return SYSERR;
	}
	if(oft[fd].state == FSTATE_CLOSED)
    {
        kprintf("\nERROR: File is closed!\n");
        return SYSERR;
    }
	if(nbytes<=0){
		kprintf("\nERROR: Number of bytes to read are given 0");
		return SYSERR;
	}

	int blk = oft[fd].fileptr / dev0_blocksize;
	int offset = oft[fd].fileptr % dev0_blocksize;
	int blknum = oft[fd].in.blocks[blk];
	int bytestoread = nbytes;
	int bytesread = 0;
	int readbytes;
	while(bytesread < nbytes)
	{
		if(oft[fd].fileptr >= oft[fd].in.size) {
			return bytesread; 
        }
		if(blknum == -1){ 

			return SYSERR;
		}
		if(dev0_blocksize - offset >  bytestoread){
			readbytes = bytestoread;
		}	
		else{
			readbytes = dev0_blocksize - offset;
		}		
		if(bs_bread(dev0,blknum, offset, &buf[bytesread], readbytes) == SYSERR){
			kprintf("\nERROR: Not able to read file!\n");
			oft[fd].fileptr = oft[fd].fileptr + bytesread;
			return bytesread;
		}
		bytesread = bytesread + readbytes;
		oft[fd].fileptr = oft[fd].fileptr + readbytes;
		bytestoread = bytestoread - readbytes;
		blk++;
		blknum = oft[fd].in.blocks[blk];
		offset = 0;
	}
    return bytesread;
}

int fs_seek(int fd, int offset){
	int fileptr = oft[fd].fileptr;
	fileptr = fileptr + offset;
	if(fileptr > oft[fd].in.size || fileptr < 0 )
	{
		kprintf("\nERROR: fileptr out of range!");
		return SYSERR;
	}
	oft[fd].fileptr = fileptr;
	return OK;
}

#endif /* FS */
