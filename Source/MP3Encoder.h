#ifndef MP3_ENCODER
#define MP3_ENCODER

#include <SupportDefs.h>
#include <NodeInfo.h>
#include <Path.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "error.h"

class EncoderInterface;
//bitstream.h
typedef struct  bit_stream_struc {
    FILE        *pt;            /* pointer to bit stream device */
    unsigned char *buf;         /* bit stream buffer */
    int         buf_size;       /* size of buffer (in number of bytes) */
    long        totbit;         /* bit counter of bit stream */
    int         buf_byte_idx;   /* pointer to top byte in buffer */
    int         buf_bit_idx;    /* pointer to top bit of top byte in buffer */
    int         mode;           /* bit stream open in read or write mode */
    int         eob;            /* end of buffer index */
    int         eobs;           /* end of bit stream flag */
    char        format;
    
    /* format of file in rd mode (BINARY/ASCII) */
} bitstream_t;

/* "bit_stream.h" Definitions */

#define         MINIMUM         4    /* Minimum size of the buffer in bytes */
#define         MAX_LENGTH      32   /* Maximum length of word written or
                                        read from bit stream */
#define         READ_MODE       0
#define         WRITE_MODE      1
#define         ALIGNING        8
#define         BINARY          0
#define         ASCII           1

#define         TRUE            1
#define         FALSE           0

#ifndef BS_FORMAT
#define         BS_FORMAT       ASCII /* BINARY or ASCII = 2x bytes */
#endif

#define         BUFFER_SIZE     4096
#define 		BUFSIZE 4096

//#define         MIN(A, B)       ((A) < (B) ? (A) : (B))
//#define         MAX(A, B)       ((A) > (B) ? (A) : (B))
#define  hget1bit() hgetbits(1)
//End bitstream.h

//formatBitstream.h
#ifndef MAX_CHANNELS
#define MAX_CHANNELS 2
#endif

#ifndef MAX_GRANULES
#define MAX_GRANULES 2
#endif

typedef void (EncoderInterface::*BitsFcnPtr)( uint32 value, uint16 length );

typedef struct
{
    uint32 value;
    uint16 length;
} BF_BitstreamElement;

typedef struct
{
    uint32              nrEntries;
    BF_BitstreamElement *element;
} BF_BitstreamPart;



typedef struct BF_FrameResults
{
    int SILength;
    int mainDataLength;
    int nextBackPtr;
} BF_FrameResults;

typedef struct BF_PartHolder
{
    int              max_elements;
    BF_BitstreamPart *part;
} BF_PartHolder;

typedef struct
{
    int frameLength;
    int SILength;
    int nGranules;
    int nChannels;
    BF_PartHolder *headerPH;
    BF_PartHolder *frameSIPH;
    BF_PartHolder *channelSIPH[MAX_CHANNELS];
    BF_PartHolder *spectrumSIPH[MAX_GRANULES][MAX_CHANNELS];
} MYSideInfo;
typedef struct BF_FrameData
{
    BitsFcnPtr       putbits;  /* your low-level bitstream function */
    int              frameLength;
    int              nGranules;
    int              nChannels;
    BF_BitstreamPart *header;
    BF_BitstreamPart *frameSI;
    BF_BitstreamPart *channelSI[MAX_CHANNELS];
    BF_BitstreamPart *spectrumSI[MAX_GRANULES][MAX_CHANNELS];
    BF_BitstreamPart *scaleFactors[MAX_GRANULES][MAX_CHANNELS];
    BF_BitstreamPart *codedData[MAX_GRANULES][MAX_CHANNELS];
    BF_BitstreamPart *userSpectrum[MAX_GRANULES][MAX_CHANNELS];
    BF_BitstreamPart *userFrameData;
} BF_FrameData;
//end formatBitstream.h

//huffman.h
#define HUFFBITS unsigned long int
#define HTN	34
#define MXOFF	250
 
struct huffcodetab {
  unsigned int xlen; 	/*max. x-index+			      	*/ 
  unsigned int ylen;	/*max. y-index+				*/
  unsigned int linbits; /*number of linbits			*/
  unsigned int linmax;	/*max number to be stored in linbits	*/
  const HUFFBITS *table;	/*pointer to array[xlen][ylen]		*/
  const unsigned char *hlen;	/*pointer to array[xlen][ylen]		*/
};
//end huffman.h

//ieeefloat.h
typedef float Single;
 typedef double defdouble;
 typedef double Double;
#define	kFloatLength	4
#define	kDoubleLength	8
#define	kExtendedLength	10
//end ieeefloat.h

//l3bitstream.h
typedef   bitstream_t L3_bitstream_t;
//end l3bitstream.h

//l3loop.h 
#define CBLIMIT       21
#define SFB_LMAX 22
#define SFB_SMAX 13
typedef struct
{
    unsigned region0_count;
    unsigned region1_count;
} subdv_table_type;
//end l3loop.h

//layer3.h
typedef struct {
        unsigned part2_3_length;
        unsigned big_values;
        unsigned count1;
        unsigned global_gain;
        unsigned scalefac_compress;
        unsigned window_switching_flag;
        unsigned block_type;
        unsigned mixed_block_flag;
        unsigned table_select[3];
int /*  unsigned */ subblock_gain[3];
        unsigned region0_count;
        unsigned region1_count;
        unsigned preflag;
        unsigned scalefac_scale;
        unsigned count1table_select;

        unsigned part2_length;
        unsigned sfb_lmax;
        unsigned sfb_smax;
        unsigned address1;
        unsigned address2;
        unsigned address3;
        double quantizerStepSize;
/* added for LSF */
        const unsigned *sfb_partition_table;
        unsigned slen[4];
} gr_info;

typedef struct {
    double  l[2][2][21];
    double  s[2][2][12][3];
} L3_psy_ratio_t;

typedef struct {
        double  l[2][2][21];
        double  s[2][2][12][3];
} L3_psy_xmin_t;

typedef struct {
    int l[2][2][22];            /* [cb] */
    int s[2][2][13][3];         /* [window][cb] */
} L3_scalefac_t;

typedef struct {
    int main_data_begin; /* unsigned -> int */
    unsigned private_bits;
    int resvDrain;
    unsigned scfsi[2][4];
    struct {
        struct {
            gr_info tt;
        } ch[2];
    } gr[2];
} L3_side_info_t;
//end layer3.h

//l3psy.h
#define HBLKSIZE    513
#define CBANDS       63 
#define CBANDS_s     42
#define BLKSIZE_s   256
#define HBLKSIZE_s  129
#define TCBMAX_l     63
#define TCBMAX_s     42
#define SBMAX_l      21
#define SBMAX_s      12
//end l3psy.h

//tables.h
struct scalefac_struct
{
   int l[23];
   int s[14];
};
//end tables.h


#include <Looper.h>
#include <Message.h>
#include <File.h>

class EncoderInterface : public BLooper
{
public:
	EncoderInterface();
	~EncoderInterface();
	
	void MessageReceived(BMessage *msg);

private:
	BFile output;
	BMessenger controlView;
	BLooper *dataSource;
	int32 blockNumber;
	
	void InitStream(BMessage *m);
	void CloseStream();
	void AddBlock(BMessage *m);
	
//bitstream.c
	static const int putmask[9];
	static const int clearmask[9];
	static const int mask[8];
	unsigned long offset,totbit, buf_byte_idx;
	unsigned int buf[BUFSIZE];
	unsigned int buf_bit_idx;
	int refill_buffer(bitstream_t *bs);
	void empty_buffer(bitstream_t *bs,int minimum);
	void open_bit_stream_w(bitstream_t *bs,char *bs_filenam,int size);
	void open_bit_stream_r(bitstream_t *bs,char *bs_filenam,int size);
	void close_bit_stream_r(bitstream_t *bs);
	void close_bit_stream_w(bitstream_t *bs);
	void alloc_buffer(bitstream_t *bs,int size);
	void desalloc_buffer(bitstream_t *bs);
	void back_track_buffer(bitstream_t *bs,int N);
	unsigned int get1bit(bitstream_t *bs);
	void put1bit(bitstream_t *bs,int bit);
	unsigned long look_ahead(bitstream_t *bs,int N);
	unsigned long getbits(bitstream_t *bs,int N);
	void putbits(bitstream_t *bs,unsigned int val,int N);
	void byte_ali_putbits(bitstream_t *bs,unsigned int val,int N);
	unsigned long byte_ali_getbits(bitstream_t *bs,int N);
	unsigned long sstell(bitstream_t *bs);
	int end_bs(bitstream_t *bs);
	int seek_sync(bitstream_t *bs,long sync,int N);
	unsigned long hgetbits(int N);
//End of bitstream.c

//fft.c
	int     	init;
	double   	w_real[2][LOGBLKSIZE], w_imag[2][LOGBLKSIZE];

	void fft(float x_real[BLKSIZE],
         float x_imag[BLKSIZE],
         float energy[BLKSIZE],
         float phi[BLKSIZE],
         int N);
//End of fft.c

//formatBitstream.c
	int BitCount;
	int ThisFrameSize;
	int BitsRemaining;
	BitsFcnPtr PutBits;
	struct side_info_link *side_queue_head;
	struct side_info_link *side_queue_free;
	int n_si;
	
	int store_side_info(BF_FrameData *info);
	int side_queue_elements(int *frameLength, int *SILength);
	void free_side_queues();
	int write_side_info();
	MYSideInfo *get_side_info();
	void free_side_info_link(side_info_link *l);
	int writePartSideInfo( BF_BitstreamPart *part, BF_FrameResults *results );
	int writePartMainData(BF_BitstreamPart *part, BF_FrameResults *results);
	int main_data( BF_FrameData *frameInfo, BF_FrameResults *results );
	void WriteMainDataBits( unsigned val, unsigned nbits, BF_FrameResults *results );
	void BF_BitstreamFrame (BF_FrameData *frameInfo, BF_FrameResults *results );
	int  BF_PartLength( BF_BitstreamPart *part );
	void BF_FlushBitstream( BF_FrameData *frameInfo, BF_FrameResults *results );
	BF_PartHolder *BF_newPartHolder( int max_elements );
	BF_PartHolder *BF_resizePartHolder( BF_PartHolder *oldPH, int max_elements );
	BF_PartHolder *BF_addElement( BF_PartHolder *thePH, BF_BitstreamElement *theElement );
	BF_PartHolder *BF_addEntry( BF_PartHolder *thePH, uint32 value, uint16 length );
	BF_PartHolder *BF_NewHolderFromBitstreamPart( BF_BitstreamPart *thePart );
	BF_PartHolder *BF_LoadHolderFromBitstreamPart( BF_PartHolder *theHolder, BF_BitstreamPart *thePart );
	BF_PartHolder *BF_freePartHolder( BF_PartHolder *thePH );
//end formatBitstream.c

//huffman.c
	HUFFBITS dmask; 
	static const HUFFBITS t1HB[];
	static const HUFFBITS t2HB[];
	static const HUFFBITS t3HB[];
	static const HUFFBITS t5HB[];
	static const HUFFBITS t6HB[];
	static const HUFFBITS t7HB[];
	static const HUFFBITS t8HB[];
	static const HUFFBITS t9HB[];
	static const HUFFBITS t10HB[];
	static const HUFFBITS t11HB[];
	static const HUFFBITS t12HB[];
	static const HUFFBITS t13HB[];
	static const HUFFBITS t15HB[];
	static const HUFFBITS t16HB[];
	static const HUFFBITS t24HB[];
	static const HUFFBITS t32HB[];
	static const HUFFBITS t33HB[];
	static const uchar t1l[];
	static const uchar t2l[];
	static const uchar t3l[];
	static const uchar t5l[];
	static const uchar t6l[];
	static const uchar t7l[];
	static const uchar t8l[];
	static const uchar t9l[];
	static const uchar t10l[];
	static const uchar t11l[];
	static const uchar t12l[];
	static const uchar t13l[];
	static const uchar t15l[];
	static const uchar t16l[];
	static const uchar t24l[];
	static const uchar t32l[];
	static const uchar t33l[];
	unsigned int hs;

	static const struct huffcodetab ht[HTN];/* global memory block		*/
				/* array of all huffcodtable headers	*/
				/* 0..31 Huffman code table 0..31	*/
				/* 32,33 count1-tables			*/

	void huffman_coder(unsigned int, unsigned int,
                          struct huffcodetab*, bitstream_t*);
//end huffman.c

//ieeefloat.c
	defdouble ConvertFromIeeeSingle(char *bytes);
	void ConvertToIeeeSingle(defdouble num, char *bytes);
	defdouble ConvertFromIeeeDouble(char *bytes);
	void ConvertToIeeeDouble(defdouble num, char *bytes);
	defdouble ConvertFromIeeeExtended(char *bytes);
	void ConvertToIeeeExtended(defdouble num, char *bytes);
//end ieeefloat.c

//l3bitstream.c

	static const unsigned slen1_tab[16];
	static const unsigned slen2_tab[16];
	static unsigned int crc;
	bitstream_t *l3_bitstream_bs;
	BF_FrameData    *frameData;
	BF_FrameResults *frameResults;
	int PartHoldersInitialized;
	BF_PartHolder *headerPH;
	BF_PartHolder *frameSIPH;
	BF_PartHolder *channelSIPH[ MAX_CHANNELS ];
	BF_PartHolder *spectrumSIPH[ MAX_GRANULES ][ MAX_CHANNELS ];
	BF_PartHolder *scaleFactorsPH[ MAX_GRANULES ][ MAX_CHANNELS ];
	BF_PartHolder *codedDataPH[ MAX_GRANULES ][ MAX_CHANNELS ];
	BF_PartHolder *userSpectrumPH[ MAX_GRANULES ][ MAX_CHANNELS ];
	BF_PartHolder *userFrameDataPH;
	void putMyBits(uint32 val, uint16 len);
	int encodeSideInfo( L3_side_info_t  *si );
	void encodeMainData(int l3_enc[2][2][576], L3_side_info_t  *si, L3_scalefac_t   *scalefac );
	void write_ancillary_data( char *theData, int lengthInBits );
	void drain_into_ancillary_data( int lengthInBits );
	void Huffmancodebits( BF_PartHolder **pph, int *ix, gr_info *gi );

	void L3_format_bitstream(int              l3_enc[2][2][576],
                         L3_side_info_t  *l3_side,
			 L3_scalefac_t   *scalefac,
			 L3_bitstream_t  *in_bs,
			 double           (*xr)[2][576],
			 char             *ancillary,
			 int              anc_bits);

	int HuffmanCode(int table_select, int x, int y, unsigned *code,
                unsigned int *extword, int *codebits, int *extbits);

	void L3_FlushBitstream();
	int abs_and_sign(int *x); /* returns signx and changes *x to abs(*x) */
	int L3_huffman_coder_count1( BF_PartHolder **pph, const struct huffcodetab *h, int v, int w, int x, int y );
//end l3bitstream.c

//l3loop.c
	float e_const;
	#define PRECALC_SIZE 1024 /* WAS 256 !!! */
	double pow43[PRECALC_SIZE];
	static const unsigned nr_of_sfb_block[6][3][4];
	static const int pretab[21];
	static const int scfsi_band_long[5];
	static const struct subdv_table_type subdv_table[23];
	static const int pow2[5];
	static const int slen1[16];
	static const int slen2[16];
	
	const int *scalefac_band_long;
	const int *scalefac_band_short;
    int firstcall;
    int en_tot[2][2]; /* ch,gr */
    int en[2][2][21];
    int xm[2][2][21];
    int xrmax[2][2];

	int inner_loop(double xr[2][2][576],  int l3_enc[2][2][576], int max_bits, gr_info *cod_info, int gr, int ch );
	int outer_loop( double xr[2][2][576],     /*  magnitudes of the spectral values */
                       int max_bits,
                       L3_psy_xmin_t  *l3_xmin,   /* the allowed distortion of the scalefactor */
                       int l3_enc[2][2][576],    /* vector of quantized values ix(0..575) */
                       L3_scalefac_t *scalefac, /* scalefactors */
                       int gr, int ch, L3_side_info_t *side_info );
   	int quantanf_init(double xr[576]);
	void calc_scfsi( double  xr[576], L3_side_info_t *l3_side, L3_psy_xmin_t *l3_xmin, int ch, int gr );
	int part2_length(L3_scalefac_t *scalefac, int gr, int ch, L3_side_info_t *si);
	int scale_bitcount(L3_scalefac_t *scalefac, gr_info *cod_info, int gr, int ch );
	void calc_noise(double xr[576], int ix[576], gr_info *cod_info, double xfsf[4][CBLIMIT] );
	int count_bits(int *ix /*int[576]*/, gr_info *cod_info);
	void gr_deco( gr_info *cod_info );
	int count_bit(int ix[576], unsigned int start, unsigned int end, unsigned int table );
	int bigv_bitcount(int ix[576], gr_info *gi);
	int choose_table( int max );
	int new_choose_table( int ix[576], unsigned int begin, unsigned int end );
	void bigv_tab_select( int ix[576], gr_info *cod_info );
	void subdivide(gr_info *cod_info);
	int count1_bitcount( int ix[ 576 ], gr_info *cod_info );
	void calc_runlen( int ix[576], gr_info *cod_info );
	void calc_xmin(double xr[2][2][576], L3_psy_ratio_t *ratio, gr_info *cod_info, L3_psy_xmin_t *l3_xmin, int gr, int ch );
	int loop_break(L3_scalefac_t *scalefac, gr_info *cod_info, int gr, int ch);
	void preemphasis(double xr[576], double xfsf[4][CBLIMIT], L3_psy_xmin_t  *l3_xmin, int gr, int ch, L3_side_info_t *l3_side );
	int amp_scalefac_bands(double xr[576], double xfsf[4][CBLIMIT], L3_psy_xmin_t    *l3_xmin, L3_side_info_t *l3_side, L3_scalefac_t  *scalefac, int gr, int ch, int iteration );
	void quantize( double xr[576], int ix[576], gr_info *cod_info );
	int ix_max( int ix[576], unsigned int begin, unsigned int end );
	double xr_max( double xr[576], unsigned int begin, unsigned int end );
	int bin_search_StepSize(int desired_rate, double start, int ix[576],
			double xrs[576], gr_info * cod_info);
	int count_bits();
	float worst_xfsf_to_xmin_ratio();

	void L3_loop_initialise();

	void L3_iteration_loop(
			double pe[][2], 
			double mdct_freq_org[2][2][576], 
			L3_psy_ratio_t *ratio,
			L3_side_info_t *side_info, 
			int l3_enc[2][2][576],
			int mean_bits, 
			L3_scalefac_t  *scalefacitor );
//end l3loop.c

//l3mdct.c
	static const double c[8];
	double ca[8];
	double cs[8];

	double win[4][36];
	double cos_s[6][12];
	double cos_l[18][36];

	void L3_mdct_initialise();
	void L3_mdct_sub(double sb_sample[2][3][18][SBLIMIT], 
                 double (*mdct_freq)[2][576], 
                 L3_side_info_t *side_info);
	void mdct( double *in, double *out, int block_type );
//end l3mdct.c

//l3psy.c
     double l3_psy_ratio[2][21];
     double ratio_s[2][12][3];
     float   window_s[BLKSIZE_s] ;
     int     nnew, old , oldest;
     int     flush, sync_flush, syncsize;
     double 	cw[HBLKSIZE], eb[CBANDS];
     double 	ctb[CBANDS];
     double	SNR_l[CBANDS], SNR_s[CBANDS_s];
     double	minval[CBANDS],qthr_l[CBANDS],norm_l[CBANDS];
     double	qthr_s[CBANDS_s],norm_s[CBANDS_s];
     double	nb_1[2][CBANDS], nb_2[2][CBANDS];
     double	s3_l[CBANDS][CBANDS];
     int	cbw_l[SBMAX_l],bu_l[SBMAX_l],bo_l[SBMAX_l] ;
     int	cbw_s[SBMAX_s],bu_s[SBMAX_s],bo_s[SBMAX_s] ;
     double	w1_l[SBMAX_l], w2_l[SBMAX_l];
     double	w1_s[SBMAX_s], w2_s[SBMAX_s];
     double	l3psy_en[SBMAX_l],   thm[SBMAX_l] ;
     int	blocktype_old[2] ;
     int	partition_l[HBLKSIZE],partition_s[HBLKSIZE_s];
     const float   *absthr;
	 static const float   crit_band[27];
     float	energy_s[3][256];
     float   phi_s[3][256] ; /* 256 samples not 129 */
    	int     numlines[CBANDS];
     int     partition[HBLKSIZE];
     float   cbval[CBANDS];
     float   rnorm[CBANDS];
     float   window[BLKSIZE];
     double  tmn[CBANDS];
     float   s[CBANDS][CBANDS];
     float   lthr[2][HBLKSIZE];
     float   r[2][2][HBLKSIZE];
     float   phi_sav[2][2][HBLKSIZE];
	float   nb[CBANDS]; 
	float   cb[CBANDS];
	float   ecb[CBANDS];
	float   wsamp_r[BLKSIZE];
	float   wsamp_i[BLKSIZE];
	float   phi[BLKSIZE];
	float   energy[BLKSIZE];
	float   fthr[HBLKSIZE];

	void L3para_read();
	void L3_psycho_initialise();
	void L3_psycho_analize(int      channel,
                       short   *buffer, 
                       short    savebuf[1344],
                       float    snr32[32],
                       double   ratio_d[21],
                       double   ratio_ds[12][3],
                       double  *pe, 
                       gr_info *cod_info);


//end l3psy.c

//l3subband.c
	int l3subband_off[2];
	double x[2][HAN_SIZE];
	double filter[SBLIMIT][64];

void L3_subband_initialise();
void L3_window_subband(short **buffer, double z[HAN_SIZE], int k);
void L3_filter_subband(double z[HAN_SIZE], double s[SBLIMIT]);
//end l3subband.c

//portableio.c
	int ReadByte(FILE *fp);
	int Read16BitsLowHigh(FILE *fp);
	int Read16BitsHighLow(FILE *fp);
	void Write8Bits(FILE *fp, int i);
	void Write16BitsLowHigh(FILE *fp, int i);
	void Write16BitsHighLow(FILE *fp, int i);
	int Read24BitsHighLow(FILE *fp);
	int Read32Bits(FILE *fp);
	int Read32BitsHighLow(FILE *fp);
	void Write32Bits(FILE *fp, int i);
	void Write32BitsLowHigh(FILE *fp, int i);
	void Write32BitsHighLow(FILE *fp, int i);
	void ReadBytes(FILE *fp, char *p, int n);
	void ReadBytesSwapped(FILE *fp, char *p, int n);
	void WriteBytes(FILE *fp, char *p, int n);
	void WriteBytesSwapped(FILE *fp, char *p, int n);
	defdouble ReadIeeeFloatHighLow(FILE *fp);
	defdouble ReadIeeeFloatLowHigh(FILE *fp);
	defdouble ReadIeeeDoubleHighLow(FILE *fp);
	defdouble ReadIeeeDoubleLowHigh(FILE *fp);
	defdouble ReadIeeeExtendedHighLow(FILE *fp);
	defdouble ReadIeeeExtendedLowHigh(FILE *fp);
	void WriteIeeeFloatLowHigh(FILE *fp, defdouble num);
	void WriteIeeeFloatHighLow(FILE *fp, defdouble num);
	void WriteIeeeDoubleLowHigh(FILE *fp, defdouble num);
	void WriteIeeeDoubleHighLow(FILE *fp, defdouble num);
	void WriteIeeeExtendedLowHigh(FILE *fp, defdouble num);
	void WriteIeeeExtendedHighLow(FILE *fp, defdouble num);


	enum e_byte_order DetermineByteOrder();
	void SwapBytesInWords(short *loc,int words);


#define	Read32BitsLowHigh(f)	Read32Bits(f)
#define WriteString(f,s)	fwrite(s,strlen(s),sizeof(char),f)


//end portableio.c

//reservoir.c
	 int ResvSize; /* in bits */
	 int ResvMax; /* in bits */

	void ResvFrameBegin(L3_side_info_t *l3_side, int mean_bits, int frameLength );
	int  ResvMaxBits   (L3_side_info_t *l3_side, double *pe, int mean_bits );
	void ResvAdjust    (gr_info *gi, L3_side_info_t *l3_side, int mean_bits );
	void ResvFrameEnd  (L3_side_info_t *l3_side, int mean_bits );
//end reservoir.c

//sqrttab.c
	const static short int2idx[10000];
	const static float idx2dbl[1001];
//psy_data.c
	const static char *psy_data[];
//table.c
	const static struct scalefac_struct sfBandIndex[6]; 
	const static float  absthr_0[];
	const static float  absthr_1[];
	const static float  absthr_2[];
	const static double enwindow[];
//end table.c

//layer3.c
    int             frames_processed;
    short    buffer[2][1152];
    int             channel;

    int             i;
    int             gr;
    short           sam[2][1344];
    float           snr32[32];
    L3_psy_ratio_t  ratio;
    double          pe[2][2];
    L3_side_info_t  side_info;
    short          *buffer_window[2];
    double          win_que[2][HAN_SIZE];
    int             l3_enc[2][2][576];
    L3_scalefac_t   scalefactor;
    bitstream_t     bs;
    double          l3_sb_sample[2][3][18][SBLIMIT];
    double          mdct_freq[2][2][576]; 
    double          avg_slots_per_frame;
    double          frac_slots_per_frame;
    long            whole_slots_per_frame;
    double          slot_lag;
    
    int             mean_bits;
    unsigned long   sent_bits;
    unsigned long   frame_bits;
    int             sideinfo_len;
    config_t		config;
    
    void	L3_compress();
    void	L3_start(BMessage *m);
    void	L3_end();
//end layer3.c
};

#endif 