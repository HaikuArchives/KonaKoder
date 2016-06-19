#ifndef MP3_ENCODER
#include "MP3Encoder.h"
#endif
//Put this here instead of MP3Encoder.h, so we don't have to recompile 20 files for
//no reason when we add a new message type.
#ifndef UTILITY_H
#include "util.h"
#endif
EncoderInterface::EncoderInterface() :
	BLooper("MP3 Encoder Looper", B_LOW_PRIORITY)
{
	// set everything to zero
//	memset ((char *)putmask, 0, sizeof(int)*9);
//	memset ((char *)clearmask, 0, sizeof(int)*9);
//	memset ((char *)mask, 0, sizeof(int)*8);
	memset ((char *)buf, 0, sizeof(int)*BUFSIZE);
	memset ((char *)w_real, 0, sizeof(double)*2*LOGBLKSIZE);
	memset ((char *)w_imag, 0, sizeof(double)*2*LOGBLKSIZE);
	memset ((char *)en_tot, 0, sizeof(int)*4);
	memset ((char *)en, 0, sizeof(int)*2*2*21);
	memset ((char *)xm, 0, sizeof(int)*2*2*21);
	memset ((char *)xrmax, 0, sizeof(int)*4);
	memset ((char *)ca, 0, sizeof(double)*8);
	memset ((char *)cs, 0, sizeof(double)*8);
	memset ((char *)win, 0, sizeof(double)*4*36);
	memset ((char *)cos_s, 0, sizeof(double)*6*12);
	memset ((char *)cos_l, 0, sizeof(double)*18*36);
	memset ((char *)l3_psy_ratio, 0, sizeof(double)*2*21);
	memset ((char *)ratio_s, 0, sizeof(double)*2*12*3);
	memset ((char *)window_s, 0, sizeof(float)*BLKSIZE_s);
	memset ((char *)cw, 0, sizeof(double)*HBLKSIZE);
	memset ((char *)eb, 0, sizeof(double)*CBANDS);
	memset ((char *)ctb, 0, sizeof(double)*CBANDS);
	memset ((char *)SNR_l, 0, sizeof(double)*CBANDS);
	memset ((char *)SNR_s, 0, sizeof(double)*CBANDS);
	memset ((char *)minval, 0, sizeof(double)*CBANDS);
	memset ((char *)qthr_l, 0, sizeof(double)*CBANDS);
	memset ((char *)norm_l, 0, sizeof(double)*CBANDS);
	memset ((char *)qthr_s, 0, sizeof(double)*CBANDS_s);
	memset ((char *)norm_s, 0, sizeof(double)*CBANDS_s);
	memset ((char *)nb_1, 0, sizeof(double)*2*CBANDS);
	memset ((char *)nb_2, 0, sizeof(double)*2*CBANDS);
	memset ((char *)s3_l, 0, sizeof(double)*CBANDS*CBANDS);
	memset ((char *)cbw_l, 0, sizeof(int)*SBMAX_l);
	memset ((char *)bu_l, 0, sizeof(int)*SBMAX_l);
	memset ((char *)bo_l, 0, sizeof(int)*SBMAX_l);
	memset ((char *)cbw_s, 0, sizeof(int)*SBMAX_s);
	memset ((char *)bu_s, 0, sizeof(int)*SBMAX_s);
	memset ((char *)bo_s, 0, sizeof(int)*SBMAX_s);
	memset ((char *)w1_l, 0, sizeof(double)*SBMAX_l);
	memset ((char *)w2_l, 0, sizeof(double)*SBMAX_l);
	memset ((char *)w1_s, 0, sizeof(double)*SBMAX_s);
	memset ((char *)w2_s, 0, sizeof(double)*SBMAX_s);
	memset ((char *)l3psy_en, 0, sizeof(double)*SBMAX_l);
	memset ((char *)thm, 0, sizeof(double)*SBMAX_l);
	memset ((char *)blocktype_old, 0, sizeof(int)*2);
	memset ((char *)partition_l, 0, sizeof(int)*HBLKSIZE);
	memset ((char *)partition_s, 0, sizeof(int)*HBLKSIZE_s);
	memset ((char *)energy_s, 0, sizeof(float)*3*256);
	memset ((char *)phi_s, 0, sizeof(float)*3*256);
	memset ((char *)numlines, 0, sizeof(int)*CBANDS);
	memset ((char *)partition, 0, sizeof(HBLKSIZE));
	memset ((char *)cbval, 0, sizeof(float)*CBANDS);
	memset ((char *)rnorm, 0, sizeof(float)*CBANDS);
	memset ((char *)window, 0, sizeof(float)*BLKSIZE);
	memset ((char *)tmn, 0, sizeof(double)*CBANDS);
	memset ((char *)s, 0, sizeof(float)*CBANDS*CBANDS);
	memset ((char *)lthr, 0, sizeof(float)*2*HBLKSIZE);
	memset ((char *)r, 0, sizeof(float)*2*2*HBLKSIZE);
	memset ((char *)phi_sav, 0, sizeof(float)*2*2*HBLKSIZE);
	memset ((char *)nb, 0, sizeof(float)*CBANDS);
	memset ((char *)cb, 0, sizeof(float)*CBANDS);
	memset ((char *)ecb, 0, sizeof(float)*CBANDS);
	memset ((char *)wsamp_r, 0, sizeof(float)*BLKSIZE);
	memset ((char *)wsamp_i, 0, sizeof(float)*BLKSIZE);
	memset ((char *)phi, 0, sizeof(float)*BLKSIZE);
	memset ((char *)energy, 0, sizeof(float)*BLKSIZE);
	memset ((char *)fthr, 0, sizeof(float)*HBLKSIZE);
	memset ((char *)l3subband_off, 0, sizeof(int)*2);
	memset ((char *)x, 0, sizeof(double)*2*HAN_SIZE);
	memset ((char *)filter, 0, sizeof(double)*SBLIMIT*64);
	memset ((char *)buffer, 0, sizeof(short)*2*1152);
	memset ((char *)sam, 0, sizeof(short)*2*1134);
	memset ((char *)snr32, 0, sizeof(float)*32);
	memset ((char *)pe, 0, sizeof(double)*2*2);
	memset ((char *)win_que, 0, sizeof(double)*2*HAN_SIZE);
	memset ((char *)l3_enc, 0, sizeof(int)*2*2*576);
	memset ((char *)l3_sb_sample, 0, sizeof(double)*2*3*18*SBLIMIT);
	memset ((char *)mdct_freq, 0, sizeof(double)*2*2*576);
	
	memset ((char *)&ratio, 0, sizeof(L3_psy_ratio_t));
	memset ((char *)&side_info, 0, sizeof(L3_side_info_t));
	memset ((char *)&scalefactor, 0, sizeof(L3_scalefac_t));
	memset ((char *)&bs, 0, sizeof(bitstream_t));
	memset ((char *)&config, 0, sizeof(config_t));

	frameResults = NULL;
	
	Run();
}

EncoderInterface::~EncoderInterface()
{
	if (output.InitCheck() == B_NO_ERROR) {
		output.Unset();
	};
}

void EncoderInterface::MessageReceived(BMessage *m)
{
//printf ("EI: got a message...\n");
	switch (m->what) {
		case NEW_STREAM:
			InitStream(m);
			break;
		case CLOSE_STREAM:
			CloseStream();
			break;
		case AUDIO_BLOCK:
			AddBlock(m);
			break;
		default:
			BLooper::MessageReceived(m);
	};
}

void EncoderInterface::InitStream(BMessage *m)
{
//printf ("Encoder creating stream...\n");
//Initialize all of our fixed-duration variables here, that way they are fresh for
//each new encode.
	blockNumber = 0;
//bitstream.c
	offset = 0;
	totbit = 0;
	buf_byte_idx = 0;
	buf_bit_idx = 8;
//fft.c
	init=0;
//formatBitstream.c
	BitCount = 0;
	ThisFrameSize = 0;
	BitsRemaining = 0;
	PutBits = NULL;
	side_queue_head   = NULL;
	side_queue_free   = NULL;
	n_si = 0;
//huffman.c
	dmask = 1 << (sizeof(HUFFBITS)*8-1);
	hs  = sizeof(HUFFBITS)*8;
//l3bitstream.c
	l3_bitstream_bs = NULL;
	frameData = NULL;
	PartHoldersInitialized = 0;
//l3loop.c
	scalefac_band_long  = &sfBandIndex[3].l[0];
	scalefac_band_short = &sfBandIndex[3].s[0];
	firstcall = 1;
//reservoir.h
	ResvSize = 0;
	ResvMax = 0;
	e_const = 2.71828182845;
//l3psy.c
	nnew = 0;
	old = 1;
	oldest = 0;
//l3subband.c
	l3subband_off[0] = 0;
	l3subband_off[1] = 0;
//layer3.c
	sent_bits = 0;
	frame_bits = 0;

// Stuff to configure config
	entry_ref ref;
	if (m->FindRef("output", &ref) != B_OK) {
		aerror("InitStream", "No entry_ref output");
	};
	
	BNode n(&ref);
	BNodeInfo ni(&n);
	ni.SetType(MIME_TYPE);
	
	config.byte_order = order_bigEndian;
	
	BEntry entry;
	entry.SetTo(&ref);
	if (entry.InitCheck() != B_NO_ERROR) {
		aerror("InitStream", "InitStream couldn't SetTo");
	};
//	output.SetTo(&entry, B_WRITE_ONLY | B_CREATE_FILE);
	BPath path(&entry);

	config.outfile = strdup(path.Path());
	


// End configuration of config

// Start to configure config.wave
	config.wave.channels = 2;
	config.wave.samplerate = 44100;
// End configuring config.wave

// Start to configure config.mpeg
	config.mpeg.type = TYPE_MPEG_I;
	config.mpeg.layr = LAYR_III;
	config.mpeg.mode = MODE_STEREO;
	
	int32 tempint;
	if (m->FindInt32("bitrate", &tempint) != B_OK) {
		config.mpeg.bitr = 128;
		config.mpeg.bitrate_index = 9;
	} else {
		config.mpeg.bitr = tempint;
	};
	
	if (m->FindInt32("bitrateIndex", &tempint) != B_OK) {
		config.mpeg.bitr = 128;
		config.mpeg.bitrate_index = 9;
	} else {
		config.mpeg.bitrate_index = tempint;
	};
	
	config.mpeg.psyc = PSYC_ATT;
	config.mpeg.emph = EMPH_NONE; 
	config.mpeg.crc  = 0;
	config.mpeg.ext  = 0;
	config.mpeg.mode_ext  = 0;
	config.mpeg.copyright = 1;
	config.mpeg.original  = 0;  
	
	config.mpeg.samplerate_index = 0;
// Thru with junk from main.c

// Taken from layer3.c
    open_bit_stream_w(&bs, config.outfile, BUFFER_SIZE);
    
    memset((char*)snr32,0,sizeof(snr32));
    memset((char *)sam,0,sizeof(sam));
    memset((char *)&side_info,0,sizeof(L3_side_info_t));

    L3_psycho_initialise();
    L3_subband_initialise();
    L3_mdct_initialise();
    L3_loop_initialise();

    config.mpeg.mode_gr           = (config.mpeg.type==TYPE_MPEG_I)?   2:  1;
    config.mpeg.samples_per_frame = (config.mpeg.type==TYPE_MPEG_I)?1152:576;
//    config.mpeg.total_frames      = config.wave.total_samples/config.mpeg.samples_per_frame;
    config.mpeg.bits_per_slot     = 8;
    frames_processed              = 0;

    sideinfo_len = 32;
    if(config.mpeg.type==TYPE_MPEG_I)
    {   /* MPEG 1 */
        if(config.wave.channels==1) sideinfo_len += 136;
        else                        sideinfo_len += 256;
    }
    else
    {   /* MPEG 2 */
        if(config.wave.channels==1) sideinfo_len += 72;
        else                        sideinfo_len += 136;
    }
    if(config.mpeg.crc) sideinfo_len += 16;

/* Figure average number of 'slots' per frame. */
    avg_slots_per_frame   = ((double)config.mpeg.samples_per_frame / 
                             ((double)config.wave.samplerate/1000)) *
                            ((double)config.mpeg.bitr /
                             (double)config.mpeg.bits_per_slot);
    whole_slots_per_frame = (int)avg_slots_per_frame;
    frac_slots_per_frame  = avg_slots_per_frame - (double)whole_slots_per_frame;
    slot_lag              = -frac_slots_per_frame;
    if(frac_slots_per_frame==0) config.mpeg.padding = 0;
// End stuff from layer3.c

	
	if (m->FindPointer("source", (void **)&dataSource) != B_OK) {
		aerror("InitStream", "InitStream: No pointer source");
	};
	
	controlView = m->ReturnAddress();
	
	BMessage *msg = new BMessage(SEND_AUDIO_BLOCK);
	msg->AddPointer("looper", this);
	msg->AddInt32("Block", ++blockNumber);
	dataSource->Lock();
	dataSource->PostMessage(msg);
	dataSource->Unlock();
	delete msg;
//printf ("Encoder: stream created...\n");
}

void EncoderInterface::CloseStream()
{
	output.Unset();
	BMessage *m = new BMessage(STREAM_DONE);

    L3_FlushBitstream();
    close_bit_stream_w(&bs);
	
	controlView.SendMessage(m, this);
}

void EncoderInterface::AddBlock(BMessage *m)
{
//printf ("EI: Got an audio block...\n");
	const int16 *lbuff;
	const int16 *rbuff;
	int32 tBlock;
	
	ssize_t s; 
	
	if (m->FindInt32("Block", &tBlock) != B_OK) {
		aerror("","MP3Encoder could not find Block number!");
	};
	if (tBlock != blockNumber) {
		aerror("","MP3Encoder received the wrong block number!");
	};
	
	if ((m->FindData("Left Buffer", B_RAW_TYPE, (const void **)&lbuff, &s) != B_OK) ||
	   (m->FindData("Right Buffer", B_RAW_TYPE, (const void **)&rbuff, &s) != B_OK)) {
	   		CloseStream();
	   		return;
	};
	memcpy (buffer[0], lbuff, sizeof(int16)*MP3_BUFFER_SIZE);
	memcpy (buffer[1], rbuff, sizeof(int16)*MP3_BUFFER_SIZE);
swap_data(B_INT16_TYPE, buffer[0], sizeof (int16)  * MP3_BUFFER_SIZE, B_SWAP_LENDIAN_TO_HOST);
swap_data(B_INT16_TYPE, buffer[1], sizeof (int16)  * MP3_BUFFER_SIZE, B_SWAP_LENDIAN_TO_HOST);
//printf ("EI: Read the data out of it...\n");	
	if (s != sizeof(int16)*MP3_BUFFER_SIZE) {
		aerror("AddBlock", "AddBlock found data of the wrong size.");
	};
        buffer_window[0] = buffer[0];
        buffer_window[1] = buffer[1];

        if(frac_slots_per_frame!=0)
            if(slot_lag>(frac_slots_per_frame-1.0))
            { /* No padding for this frame */
                slot_lag    -= frac_slots_per_frame;
                config.mpeg.padding = 0;
            }
            else 
            { /* Padding for this frame  */
                slot_lag    += (1-frac_slots_per_frame);
                config.mpeg.padding = 1;
            }
       config.mpeg.bits_per_frame = 8*(whole_slots_per_frame + config.mpeg.padding);
       mean_bits = (config.mpeg.bits_per_frame - sideinfo_len) / config.mpeg.mode_gr;


/* psychoacousic model */
        for(gr=0;gr<config.mpeg.mode_gr;gr++)
            for(channel=0;channel<config.wave.channels;channel++)
              L3_psycho_analize(channel,
                                  &buffer[channel][gr*576],
                                  &sam[channel][0],&snr32[0],
                                  &ratio.l[gr][channel][0],
                                  &ratio.s[gr][channel][0],
                                  &pe[gr][channel],
                                  &side_info.gr[gr].ch[channel].tt);

/* polyphase filtering */
        for(gr=0;gr<config.mpeg.mode_gr;gr++)
            for(channel=0;channel<config.wave.channels;channel++)
                for(i=0;i<18;i++)
                {

                    L3_window_subband(&buffer_window[channel],
                                      &win_que[channel][0],
                                      channel);
                    L3_filter_subband(&win_que[channel][0],
                                      &l3_sb_sample[channel][gr+1][i][0]);
                }

/* apply mdct to the polyphase output */
        L3_mdct_sub(l3_sb_sample, mdct_freq, &side_info);


/* bit and noise allocation */
        L3_iteration_loop(pe,mdct_freq,&ratio,&side_info,
                          l3_enc, mean_bits,&scalefactor);


/* write the frame to the bitstream */

       L3_format_bitstream(l3_enc,&side_info,&scalefactor, 
                           &bs,mdct_freq,NULL,0);

       frame_bits = sstell(&bs) - sent_bits;

       if(frame_bits%config.mpeg.bits_per_slot) ERROR("This shouldn't happen, program error.");

       sent_bits += frame_bits;

///////
// Original "write to file" version.
//	int16 outbuff[2 * MP3_BUFFER_SIZE];
//	for (int x = 0; x < MP3_BUFFER_SIZE; x++) {
//		outbuff[x * 2] = lbuff[x];
//		outbuff[x * 2 + 1] = rbuff[x];
//	};
//	
//	swap_data(B_INT16_TYPE, outbuff, sizeof (int16) * 2 * MP3_BUFFER_SIZE, B_SWAP_LENDIAN_TO_HOST);
//	output.Write(outbuff, sizeof (int16) * 2 * MP3_BUFFER_SIZE);
//printf ("EI: wrote the buffer out to disk...\n");
	BMessage *msg = new BMessage(SEND_AUDIO_BLOCK);
	msg->AddPointer("looper", this);
	msg->AddInt32("Block", ++blockNumber);
	
	//If we can't lock the dataSource, it is almost certainly because it no longer
	//exists.  This happens when the application quits.
	if (!dataSource->Lock()) {
		return;
	}
	dataSource->PostMessage(msg);
	dataSource->Unlock();
	delete msg;
//printf ("EI: All done with that block...\n");
}

