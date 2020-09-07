/* GB-Compress V1.2                                                           */

/* This algorithm tries to compress information. While I tried to optimize    */
/* it for Gamboy-GFX (font and font-map), it gives good results on text, too. */
/* I COULD implement more inteligent algorithms, but I wanted the GB-code to  */
/* be small, simple and fast, and the more complex stuff would need lots of   */
/* 16-Bit arithmentics.                                                       */

/* You can use decomp.inc in your own asm-projects to decompress it.          */

/* Codes: */
/*  00000000 - End of compressed block               */
/*  00  n    - copy following byte n+1 times         */
/*  01  n    - copy following word n+1 times         */
/*  10  n    - repeat the n+1 bytes from buffer      */
/*  11  n    - copy n+1 following bytes              */

/* History:                                          */
/*  1.0 - first version                              */
/*  1.1 - 10% better compression !                   */
/*  1.2 - fixed bug reported by Jeff Frohwein        */

#include <stdio.h>
#include <stdlib.h>

unsigned long int insize,outsize;
unsigned char *data;

void write_end(FILE *stream)
{
  unsigned char cmd;
  cmd=0;
  fwrite(&cmd,1,1,stream);
}

void write_byte(FILE *stream,unsigned char len,unsigned char data)
{
  len=(len-1)&63;outsize+=2;
  fwrite(&len,1,1,stream);
  fwrite(&data,1,1,stream);
}

void write_word(FILE *stream,unsigned char len,unsigned short data)
{
  len=((len-1)&63)|64;outsize+=3;
  fwrite(&len,1,1,stream);
  fwrite(&data,2,1,stream);
}

void write_string(FILE *stream,unsigned char len,unsigned short data)
{
  unsigned char hi,lo;
  len=((len-1)&63)|128;outsize+=3;
  hi=data>>8;lo=data&255;
  fwrite(&len,1,1,stream);
  fwrite(&lo,1,1,stream);fwrite(&hi,1,1,stream);
}

void write_trash(FILE *stream,unsigned char len,unsigned char *pos)
{
  unsigned char cmd;
  cmd=((len-1)&63)|192;outsize+=len+1;
  fwrite(&cmd,1,1,stream);
  fwrite(pos,len,1,stream);
}

int compress(char *name)
{
  FILE *file;

  if ((file=fopen(name,"wb"))!=NULL)
  {
    int bp,tb;

    bp=tb=0;

    outsize=0;
    while (bp<insize) {
      unsigned char x;
      unsigned short y;
      int rr,sr,rl;
      int r_rb,r_rw,r_rs;
      x=*(data+bp);
      r_rb=1;
      while ((*(data+bp+r_rb)==x)&&((bp+r_rb)<insize)&&(r_rb<64)) r_rb++;
      y=*((unsigned short *)&(*(data+bp)));
      r_rw=1;
      while ((*((unsigned short *)&(*(data+bp+r_rw*2)))==y)&&((bp+(r_rw*2))<insize)&&(r_rw<64)) r_rw++;
      rr=sr=r_rs=0;
      while (rr<bp) {
	rl=0;
	while ((*(data+rr+rl)==*(data+bp+rl))&&((rr+rl)<bp)&&(rl<64)) rl++;
	if (rl>r_rs) {
	  sr=rr-bp;r_rs=rl;
	}
	rr++;
      }
      if ((r_rb>2)&&(r_rb>r_rw)&&(r_rb>r_rs)) {
	if (tb>0) write_trash(file,tb,data+bp-tb);tb=0;
	write_byte(file,r_rb,x);
	bp+=r_rb;
      } else {
	if ((r_rw>2)&&((r_rw*2)>r_rs)) {
	  if (tb>0) write_trash(file,tb,data+bp-tb);tb=0;
	  write_word(file,r_rw,y);
	  bp+=r_rw*2;
	} else {
	  if (r_rs>3) {
	    if (tb>0) write_trash(file,tb,data+bp-tb);tb=0;
	    write_string(file,r_rs,sr);
	    bp+=r_rs;
	  } else {
	    tb++;bp++;
	  }
	}
      }
    }
    if (tb>0) write_trash(file,tb,data+bp-tb);
    write_end(file);
    fclose(file);
    printf("done, ratio: %u%%\n",outsize*100/insize);
  } else
    printf("error opening %s\n",name);
  return 0;
}

int main(int argc,char *argv[])
{
  if (argc==3)
  {
    FILE *file;
    if ((file=fopen(argv[1],"rb"))!=NULL)
    {
      fseek(file,0,SEEK_END);
      insize=ftell(file);
      fseek(file,0,SEEK_SET);
      if ((data=malloc(insize))!=NULL)
      {
	printf("reading ...\n");
	fread(data,insize,1,file);
	printf("writing ...\n");
	compress(argv[2]);
	free(data);
      } else
	printf("error allocating memory !\n");
      fclose(file);
    } else
      printf("error opening %s\n",argv[1]);
  } else
    printf("Syntax: comp <infile> <outfile>\n");
  return 0;
}
