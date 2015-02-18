#include "Lzss.h"
  
#include <cstring>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif
  
static uint32_t textsize = 0,    /* text size counter */
    codesize = 0;   /* code size counter */
static uint8_t   text_buf[N + F - 1];    /* ring buffer of size N,
            with extra F-1 bytes to facilitate string comparison */
static int  match_position, match_length,  /* of longest match.  These are
            set by the InsertNode() procedure. */
    lson[N + 1], rson[N + 257], dad[N + 1];  /* left & right children &
            parents -- These constitute binary search trees. */
  
static void InitTree(void)
{
    int  i;
  
    for (i = N + 1; i <= N + 256; i++) rson[i] = NIL;
    for (i = 0; i < N; i++) dad[i] = NIL;
}
  
static void InsertNode(int r)
{
    int  i, p, cmp;
    unsigned char  *key;
  
    cmp = 1;  key = &text_buf[r];  p = N + 1 + key[0];
    rson[r] = lson[r] = NIL;  match_length = 0;
    for(;;)
    {
        if (cmp >= 0)
        {
            if(rson[p]!=NIL) p=rson[p];
            else {rson[p]=r; dad[r]=p; return;}
        }
        else
        {
            if(lson[p]!=NIL) p=lson[p];
            else {lson[p]=r; dad[r]=p; return;}
        }
        for (i = 1; i < F; i++) if ((cmp = key[i] - text_buf[p + i]) != 0)  break;
        if(i>match_length)
        {
            match_position=p;
            if((match_length=i)>=F) break;
        }
    }
    dad[r] = dad[p];  lson[r] = lson[p];  rson[r] = rson[p];
    dad[lson[p]] = r;  dad[rson[p]] = r;
    if (rson[dad[p]] == p) rson[dad[p]] = r;
    else                   lson[dad[p]] = r;
    dad[p] = NIL;  /* remove p */
}
  
void DeleteNode(int p)
{
    int  q;
      
    if (dad[p] == NIL) return;  /* not in tree */
    if (rson[p] == NIL) q = lson[p];
    else if (lson[p] == NIL) q = rson[p];
    else
    {
        q = lson[p];
        if (rson[q] != NIL)
        {
            do {  q = rson[q];  } while (rson[q] != NIL);
            rson[dad[q]] = lson[q];  dad[lson[q]] = dad[q];
            lson[q] = lson[p];  dad[lson[p]] = q;
        }
        rson[q] = rson[p];  dad[rson[p]] = q;
    }
    dad[q] = dad[p];
    if (rson[dad[p]] == p) rson[dad[p]] = q;  else lson[dad[p]] = q;
    dad[p] = NIL;
}
  
int ff4psp::compression::LzssEncode(uint8_t* &dest, uint8_t* src, int src_size)
{
    int dest_seek=4, src_seek=0;
    int  i, c, len, r, s, last_match_length, code_buf_ptr;
    uint8_t code_buf[17], mask;
  
    dest=new uint8_t[src_size*2];
    InitTree();
    code_buf[0]=0;
    code_buf_ptr=mask=1;
    s=0;
    r=N-F;
    // clear the buffer with any character that will appear often
    memset(&text_buf[s],0,r);
    // read F bytes into the last F bytes of the buffer
    for(len=0; len<F && len<src_size; len++) text_buf[r+len]=src[src_seek++];
    // text of size zero
    if((textsize=len)==0) return 0;
    for(i=1; i<=F; i++) InsertNode(r-i);
    InsertNode(r);
      
    for(; len>0; )
    {
        if(match_length>len) match_length=len;
  
        if(match_length<=THRESHOLD)
        {
            match_length=1;  /* Not long enough match.  Send one byte. */
            code_buf[0]|=mask;  /* 'send one byte' flag */
            code_buf[code_buf_ptr++]=text_buf[r];  /* Send uncoded. */
        }
        else
        {
            code_buf[code_buf_ptr++]=match_position;
            code_buf[code_buf_ptr++]=((match_position >> 4) & 0xf0)|(match_length-(THRESHOLD+1));
        }
        // shift mask left one bit
        if((mask<<=1)==0)
        {
            for(i=0; i<code_buf_ptr; i++) dest[dest_seek++]=code_buf[i];
            codesize+=code_buf_ptr;
            code_buf[0]=0;
            code_buf_ptr=mask=1;
        }
        last_match_length=match_length;
        for(i=0; i<last_match_length; i++)
        {
            if(src_seek>=src_size) break;
            c=src[src_seek++];
            DeleteNode(s);      // delete old strings
            text_buf[s]=c;      // read new bytes
            if(s<F-1) text_buf[s+N]=c;
            s=(s+1)%N;
            r=(r+1)%N;
            InsertNode(r);
        }
        while(i++<last_match_length)
        {
            DeleteNode(s);                  // no need to read, but
            s=(s+1)%N;
            r=(r+1)%N;
            if(--len) InsertNode(r);        // buffer may not be empty
        }
    }
  
    // Send remaining code. */
    if(code_buf_ptr>1)
    {
        for(i=0; i<code_buf_ptr; i++) dest[dest_seek++]=code_buf[i];
        codesize+=code_buf_ptr;
    }
  
    *((uint32_t*)&dest[0])=src_size;
      
    return dest_seek;
}
  
int ff4psp::compression::LzssDecode(uint8_t* &dest, uint8_t* src, int src_size)
{
    int r, fpos;
    int dest_seek, src_seek;
    uint8_t flags, c;
  
	int dest_size=*((uint32_t*)src);
    dest=new uint8_t[dest_size];
    //MEM_STREAM str;
    //MemStreamCreate(&str);
    memset(text_buf,0,N-F);
  
    for(fpos=0, r=N-F, flags=0, dest_seek=0, src_seek=4; src_seek<src_size && dest_seek<dest_size; flags>>=1, fpos++)
    {
        // read flag
        if((fpos%=8)==0) flags=src[src_seek++];
  
        if(flags&1)
        {
            c=src[src_seek++];
            dest[dest_seek++]=c;
            //MemStreamWriteByte(&str,c);
            text_buf[r++]=c;
            r%=N;
        }
        else
        {
            int i=src[src_seek++];
            int j=src[src_seek++];
  
            i|=((j&0xf0)<<4);
            j=(j&0x0f)+THRESHOLD;
            // decompress string
            for(int k=0; k<=j; k++)
            {
                c=text_buf[(i+k)%N];
                dest[dest_seek++]=c;
                //MemStreamWriteByte(&str,c);
                text_buf[r++]=c;
                r%=N;
            }
        }
    }
    return dest_seek;
  
    //dest=str.data;
    //return str.pos;
} 