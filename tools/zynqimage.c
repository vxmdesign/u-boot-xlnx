#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>

void usage(void){
  printf("genbin takes an rbl binary, and a uboot binary\n");
  printf("to build boot.bin");
  printf("Usage: genbin <u-boot> <rbl>\n");
}

unsigned char *loadFile(const char *pFilename, int pSize){
  unsigned char *f;
  int fd;
  fd = open(pFilename, O_RDONLY);
  if(fd < 0){
    printf("Could not open file %s\n", pFilename);
    return NULL;
  }
  f = (unsigned char*)malloc(pSize);
  if(read(fd, f, pSize) != pSize){
    printf("Could not read all the data\n");
    free(f);
    close(fd);
    return NULL;
  }
  close(fd);
  return f;
}

typedef struct{
  unsigned int addr;
  unsigned int data;
}rblentry;

#define HLEN (40 * 4)
#define RBLS 260
void updateHead(unsigned char *pHead, unsigned int pR44, unsigned int pUserdef, unsigned int pSize){
  unsigned int *head;
  int c;
  unsigned int chksum;
  head = (unsigned int *)pHead;
  for(c = 8; c < HLEN / 4; c++){
    head[c] = 0x0;
  }
  for(c = 0; c < 8; c++){
    head[c] = 0xeafffffe;
  }
  head[8]=0xaa995566;
  head[9]=0x584c4e58;
  head[10]=0x0;
  head[11]=pUserdef;
  head[12]=0x8c0;
  head[13]=pSize;
  head[14]=0x0;
  head[15]=0x0;
  head[16]=pSize;
  head[17]=pR44;
  chksum = 0;
  for(c = 8; c < 18; c++){
    chksum+=head[c];
  }
  chksum = chksum ^ 0xffffffff;
  head[18]=chksum;
  //  for(c =0; c < HLEN / 4; c++){
  // printf("0x%08x\n", head[c]);
  //}
}

int main(int argc, char **argv){
  unsigned char *ub;
  unsigned char *rbl;
  unsigned int *rh;
  rblentry rblout[RBLS];
  unsigned char head[HLEN];
  int rbllen;
  unsigned int r44;
  unsigned int userdef;
  int c;
  int fd;
  struct stat ustat;
  struct stat rstat;
  if(argc < 4){
    usage();
    return -1;
  }
  if(stat(argv[1], &ustat)!=0){
    printf("Invalid file: %s\n", argv[1]);
    usage();
    return -1;
  }
  if(stat(argv[2], &rstat)!=0){
    printf("Invalid file: %s\n", argv[2]);
    usage();
    return -1;
  }
  if(ustat.st_size <= 0 || ustat.st_size >= 0x30000){
    printf("Invalid uboot size\n");
    return -1;
  }
  if(rstat.st_size <= 0 || rstat.st_size >= 2060){ /*calculated....might be changed*/
    printf("Invalid rbl size\n");
    return -1;
  }
  if((ub = loadFile(argv[1], ustat.st_size)) == NULL){
    return -1;
  }
  if((rbl = loadFile(argv[2], rstat.st_size)) == NULL){
    return -1;
  }
  if((rstat.st_size % 4) != 0){
    printf("Rbl size must be a multiple of 4\n");
    return -1;
  }
  fd = open(argv[3], O_CREAT | O_RDWR , S_IRUSR | S_IWUSR);
  if(fd <= 0){
    printf("Could not open output file\n");
    return -1;
  }
  rbllen = rstat.st_size / 4;
  rh = (unsigned int*)rbl;
  if(rh[0] == 0xdeadbeef){
    r44= rh[1];
    userdef = rh[2];
    rh = &(rh[3]);
  }else if(rh[rbllen-3] == 0xdeadbeef){
    r44 = rh[rbllen-2];
    userdef = rh[rbllen-1];
  }else{
    printf("Rbl header not found\n");
    return -1;
  }
  rbllen = rbllen - 3;
  rbllen = rbllen / 2;
  printf("%d\n", rbllen);
  for(c = 0; c < RBLS; c++){
    if(c < rbllen){
      rblout[c].addr = rh[c*2];
      rblout[c].data = rh[c*2+1];
    }else{
      rblout[c].addr = 0xffffffff;
      rblout[c].data = 0x0;
    }
  }
  updateHead(head, r44,userdef, ustat.st_size);
  if(write(fd, head, HLEN) < 0){
    printf("Write error\n");
    return -1;
  }
  if(write(fd, (unsigned char*)rblout, sizeof(rblout))<0){
    printf("Write error\n");
    return -1;
  }
  if(write(fd, ub, ustat.st_size)<0){
    printf("Write error\n");
    return -1;
  }
  close(fd);
  return 0;
}
