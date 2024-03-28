/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// compile only if configured for Ogg / FLAC
#ifdef HAVE_FLAC_LIB

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "Exception.h"
#include "Util.h"
#include "FlacLibEncoder.h"
#include "CastSink.h"
#include <cstring>
#include <cstdlib>


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize the encoder
 *----------------------------------------------------------------------------*/
void
FlacLibEncoder :: init ( )
                                                            
{

    if ( getInBitsPerSample() != 16 && getInBitsPerSample() != 8 ) {
        throw Exception( __FILE__, __LINE__,
                         "specified bits per sample not supported",
                         getInBitsPerSample() );
    }

    if ( getInChannel() != 1 && getInChannel() != 2 ) {
        throw Exception( __FILE__, __LINE__,
                         "unsupported number of channels for the encoder",
                         getInChannel() );
    }

    if ( getOutSampleRate() == getInSampleRate() ) {
        converter     = 0;
    } else {
        throw Exception( __FILE__, __LINE__,
                         "Resampling not supported at the moment.");
    }

    encoderOpen = false;
}


/*------------------------------------------------------------------------------
 *  Start an encoding session
 *----------------------------------------------------------------------------*/
bool
FlacLibEncoder :: start ( void )
                                                            
{
    if ( isOpen() ) {
        close();
    }

    // open the underlying sink
    if ( !getSink()->open() ) {
        throw Exception( __FILE__, __LINE__,
                         "FLAC lib opening underlying sink error");
    }

    se = FLAC__stream_encoder_new();
    if (!se) {
        throw Exception( __FILE__, __LINE__,
                         "FLAC encoder creation error");
    }
    FLAC__stream_encoder_set_channels(se, getInChannel());
    FLAC__stream_encoder_set_ogg_serial_number(se, rand());
    FLAC__stream_encoder_set_bits_per_sample(se, getInBitsPerSample());
    FLAC__stream_encoder_set_sample_rate(se, getInSampleRate());
    FLAC__stream_encoder_set_compression_level(se, getOutQuality());
    FLAC__stream_encoder_init_ogg_stream(se, NULL, FlacLibEncoder::encoder_cb, NULL, NULL, NULL, this);
    
    encoderOpen = true;

    return true;
}

FLAC__StreamEncoderWriteStatus
FlacLibEncoder :: encoder_cb (const FLAC__StreamEncoder *encoder,
                              const FLAC__byte buffer[],
                              size_t bytes,
                              uint32_t samples, uint32_t current_frame,
                              void *client_data ) {
    //getSink()->write(data, len);
    return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
}

/*------------------------------------------------------------------------------
 *  Write data to the encoder
 *----------------------------------------------------------------------------*/
unsigned int
FlacLibEncoder :: write ( const void    * buf,
                          unsigned int    len )             
{
    if ( !isOpen() || len == 0 ) {
        return 0;
    }

    unsigned int    channels      = getInChannel();
    unsigned int    bitsPerSample = getInBitsPerSample();
    unsigned int    sampleSize = (bitsPerSample / 8) * channels;

    int totalProcessed = 0;
    return totalProcessed;
}

/*------------------------------------------------------------------------------
 *  Close the encoding session
 *----------------------------------------------------------------------------*/
void
FlacLibEncoder :: stop ( void )                    
{
    if ( isOpen() ) {

        FLAC__stream_encoder_finish(se);
        getSink()->flush();
        FLAC__stream_encoder_delete(se);
        se = NULL;

        encoderOpen = false;

        getSink()->close();
    }
}

#endif // HAVE_FLAC_LIB
