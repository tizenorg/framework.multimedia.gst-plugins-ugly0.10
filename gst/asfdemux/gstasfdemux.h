/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


#ifndef __ASF_DEMUX_H__
#define __ASF_DEMUX_H__

#include <gst/gst.h>
#include <gst/base/gstadapter.h>

#include "asfheaders.h"

#include <gst/base/gstbitreader.h>
G_BEGIN_DECLS

#define GST_TYPE_ASF_DEMUX \
  (gst_asf_demux_get_type())
#define GST_ASF_DEMUX(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_ASF_DEMUX,GstASFDemux))
#define GST_ASF_DEMUX_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_ASF_DEMUX,GstASFDemuxClass))
#define GST_IS_ASF_DEMUX(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_ASF_DEMUX))
#define GST_IS_ASF_DEMUX_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_ASF_DEMUX))

GST_DEBUG_CATEGORY_EXTERN (asfdemux_dbg);
#define GST_CAT_DEFAULT asfdemux_dbg

#define LAST_STOP_SEGMENT

typedef struct _GstASFDemux GstASFDemux;
typedef struct _GstASFDemuxClass GstASFDemuxClass;

typedef struct {
  guint32	packet;
  guint16	count;
} AsfSimpleIndexEntry;

typedef struct {
  AsfPayloadExtensionID   id : 16;  /* extension ID; the :16 makes sure the
                                     * struct gets packed into 4 bytes       */
  guint16                 len;      /* save this so we can skip unknown IDs  */
} AsfPayloadExtension;


typedef struct
{
  gboolean        valid;               /* TRUE if structure is valid/filled */

  GstClockTime    start_time;
  GstClockTime    end_time;
  GstClockTime    avg_time_per_frame;
  guint32         data_bitrate;
  guint32         buffer_size;
  guint32         intial_buf_fullness;
  guint32         data_bitrate2;
  guint32         buffer_size2;
  guint32         intial_buf_fullness2;
  guint32         max_obj_size;
  guint32         flags;
  guint16         lang_idx;

  /* may be NULL if there are no extensions; otherwise, terminated by
   * an AsfPayloadExtension record with len 0 */
  AsfPayloadExtension  *payload_extensions;

  /* missing: stream names */
} AsfStreamExtProps;
#ifdef ASFDEMUX_MODIFICATION
typedef struct _GstMpeg4VideoObjectLayer        GstMpeg4VideoObjectLayer;
typedef struct _GstMpeg4VisualObject            GstMpeg4VisualObject;
typedef enum {
  GST_MPEG4_SQUARE        = 0x01,
  GST_MPEG4_625_TYPE_4_3  = 0x02,
  GST_MPEG4_525_TYPE_4_3  = 0x03,
  GST_MPEG4_625_TYPE_16_9 = 0x04,
  GST_MPEG4_525_TYPE_16_9 = 0x05,
  GST_MPEG4_EXTENDED_PAR  = 0x0f,
} GstMpeg4AspectRatioInfo;
typedef enum {
  GST_MPEG4_VIDEO_ID         = 0x01,
  GST_MPEG4_STILL_TEXTURE_ID = 0x02,
  GST_MPEG4_STILL_MESH_ID    = 0x03,
  GST_MPEG4_STILL_FBA_ID     = 0x04,
  GST_MPEG4_STILL_3D_MESH_ID = 0x05,
  /*... reserved */

} GstMpeg4VisualObjectType;
typedef enum {
  /* Other value are reserved */
  GST_MPEG4_CHROMA_4_2_0 = 0x01
} GstMpeg4ChromaFormat;
typedef enum {
  GST_MPEG4_RECTANGULAR,
  GST_MPEG4_BINARY,
  GST_MPEG4_BINARY_ONLY,
  GST_MPEG4_GRAYSCALE
} GstMpeg4VideoObjectLayerShape;
typedef enum {
  GST_MPEG4_SPRITE_UNUSED,
  GST_MPEG4_SPRITE_STATIC,
  GST_MPEG4_SPRITE_GMG
} GstMpeg4SpriteEnable;
typedef enum {
  GST_MPEG4_PARSER_OK,
  GST_MPEG4_PARSER_BROKEN_DATA,
  GST_MPEG4_PARSER_NO_PACKET,
  GST_MPEG4_PARSER_NO_PACKET_END,
  GST_MPEG4_PARSER_ERROR,
} GstMpeg4ParseResult;
struct _GstMpeg4VideoObjectLayer {
  guint8 random_accessible_vol;
  guint8 video_object_type_indication;

  guint8 is_object_layer_identifier;
  /* if is_object_layer_identifier */
  guint8 verid;
  guint8 priority;

  GstMpeg4AspectRatioInfo aspect_ratio_info;
  guint8 par_width;
  guint8 par_height;

  guint8 control_parameters;
  /* if control_parameters */
  GstMpeg4ChromaFormat chroma_format;
  guint8 low_delay;
  guint8 vbv_parameters;
  /* if vbv_parameters */
  guint16 first_half_bitrate;
  guint16 latter_half_bitrate;
  guint16 first_half_vbv_buffer_size;
  guint16 latter_half_vbv_buffer_size;
  guint16 first_half_vbv_occupancy;
  guint16 latter_half_vbv_occupancy;

  /* Computed values */
  guint32 bit_rate;
  guint32 vbv_buffer_size;

  GstMpeg4VideoObjectLayerShape shape;
  /* if shape == GST_MPEG4_GRAYSCALE && verid =! 1 */
  guint8 shape_extension;

  guint16 vop_time_increment_resolution;
  guint8 vop_time_increment_bits;
  guint8 fixed_vop_rate;
  /* if fixed_vop_rate */
  guint16 fixed_vop_time_increment;

  guint16 width;
  guint16 height;
  guint8 interlaced;
  guint8 obmc_disable;

  GstMpeg4SpriteEnable sprite_enable;
  /* if vol->sprite_enable == SPRITE_GMG or SPRITE_STATIC*/
  /* if vol->sprite_enable != GST_MPEG4_SPRITE_GMG */
  guint16 sprite_width;
  guint16 sprite_height;
  guint16 sprite_left_coordinate;
  guint16 sprite_top_coordinate;

  guint8 no_of_sprite_warping_points;
  guint8 sprite_warping_accuracy;
  guint8 sprite_brightness_change;
  /* if vol->sprite_enable != GST_MPEG4_SPRITE_GMG */
  guint8 low_latency_sprite_enable;

  /* if shape != GST_MPEG4_RECTANGULAR */
  guint8 sadct_disable;

  guint8 not_8_bit;

  /* if no_8_bit */
  guint8 quant_precision;
  guint8 bits_per_pixel;

  /* if shape == GRAYSCALE */
  guint8 no_gray_quant_update;
  guint8 composition_method;
  guint8 linear_composition;

  guint8 quant_type;
  /* if quant_type */
  guint8 load_intra_quant_mat;
  guint8 intra_quant_mat[64];
  guint8 load_non_intra_quant_mat;
  guint8 non_intra_quant_mat[64];

  guint8 quarter_sample;
  guint8 complexity_estimation_disable;
  guint8 resync_marker_disable;
  guint8 data_partitioned;
  guint8 reversible_vlc;
  guint8 newpred_enable;
  guint8 reduced_resolution_vop_enable;
  guint8 scalability;
  guint8 enhancement_type;

  //GstMpeg4VideoPlaneShortHdr short_hdr;
};
struct _GstMpeg4VisualObject {
  guint8 is_identifier;
  /* If is_identifier */
  guint8 verid;
  guint8 priority;
 GstMpeg4VisualObjectType type;
};
#endif
#ifdef ASFDEMUX_MODIFICATION
typedef struct
{
  guint32           show_samples; /* no.of frames to show between two consecutive key frames = samples_bw_keyframes/rate */
  guint64           kpacket_dur_diff;
  GstClockTime      start_ts;
  GstClockTime      tp_preroll;
  GstClockTime      avg_dur;
  gboolean          is_eos;
  guint64           mo_len_type;
  guint32           prev_kpacket;
  guint32           next_kpacket;
  guint32           cur_kpacket;
  guint             prev_keyframe;
  guint             current_frame;
  GstClockTime      next_keyframe_ts;
  GstClockTime      cur_keyframe_ts;
  gboolean          first_video_push;
  gboolean          resume_nrl_play;
  gboolean          first_payload;
}AsfTrickplayInfo;

typedef struct
{
  GstClockTime key_time;
  gint key_packet;
  gint speed_count;
}AsfKeyPacketInfo;
#endif

typedef struct
{
  AsfStreamType      type;

  gboolean           active;  /* if the stream has been activated (pad added) */

  GstPad     *pad;
  guint16     id;

  /* video-only */
  gboolean    is_video;
  gboolean    fps_known;

  GstCaps    *caps;

  GstTagList *pending_tags;

  gboolean    discont;

  /* for new parsing code */
  GstFlowReturn   last_flow; /* last flow return */
  GArray         *payloads;  /* pending payloads */

  /* Video stream PAR & interlacing */
  guint8	par_x;
  guint8	par_y;
  gboolean      interlaced;

  /* extended stream properties (optional) */
  AsfStreamExtProps  ext_props;

} AsfStream;

typedef enum {
  GST_ASF_DEMUX_STATE_HEADER,
  GST_ASF_DEMUX_STATE_DATA,
  GST_ASF_DEMUX_STATE_INDEX
} GstAsfDemuxState;

#define GST_ASF_DEMUX_NUM_VIDEO_PADS   16
#define GST_ASF_DEMUX_NUM_AUDIO_PADS   32
#define GST_ASF_DEMUX_NUM_STREAMS      32
#define GST_ASF_DEMUX_NUM_STREAM_IDS  127

#ifdef CODEC_ENTRY
typedef struct __CODEC_ENTRY
{
	guint16 type;
	guint16 name_len;
	gchar* name;
	guint16 description_len;
	gchar* description;
	guint16 information_len;
	guint8* information;

} CODEC_ENTRY;
#endif

struct _GstASFDemux {
  GstElement 	     element;

  GstPad            *sinkpad;

  GstAdapter        *adapter;
  GstTagList        *taglist;
  GstAsfDemuxState   state;

  /* byte offset where the asf starts, which might not be zero on chained
   * asfs, index_offset and data_offset already are 'offseted' by base_offset */
  guint64            base_offset;

  guint64            index_offset; /* byte offset where index might be, or 0   */
  guint64            data_offset;  /* byte offset where packets start          */
  guint64            data_size;    /* total size of packet data in bytes, or 0 */
  guint64            num_packets;  /* total number of data packets, or 0       */
  gint64             packet;       /* current packet                           */
#ifdef LAST_STOP_SEGMENT
  gint64             packet_to_stop;
#endif
  guint              speed_packets; /* Known number of packets to get in one go*/

  gchar              **languages;
  guint                num_languages;

  GstCaps             *metadata;         /* metadata, for delayed parsing; one
                                          * structure ('stream-N') per stream */
  GstStructure	      *global_metadata;  /* metadata which isn't specific to one stream */
  GSList              *ext_stream_props; /* for delayed processing (buffers) */
  GSList              *mut_ex_streams;   /* mutually exclusive streams */

  guint32              num_audio_streams;
  guint32              num_video_streams;
  guint32              num_streams;
  AsfStream            stream[GST_ASF_DEMUX_NUM_STREAMS];
  gboolean             activated_streams;

  /* for chained asf handling, we need to hold the old asf streams until
   * we detect the new ones */
  AsfStream            old_stream[GST_ASF_DEMUX_NUM_STREAMS];
  gboolean             old_num_streams;

  GstClockTime         first_ts;        /* first timestamp found        */

  guint32              packet_size;
  guint32              timestamp;       /* in milliseconds              */
  guint64              play_time;

  guint64              preroll;

  gboolean             seekable;
  gboolean             broadcast;

  GstSegment           segment;          /* configured play segment                 */
  gboolean             accurate;

  gboolean             need_newsegment;  /* do we need to send a new-segment event? */
  GstClockTime         segment_ts;       /* streaming; timestamp for segment start */
  GstSegment           in_segment;       /* streaming; upstream segment info */
  GstClockTime         in_gap;           /* streaming; upstream initial segment gap for interpolation */
  gboolean             segment_running;  /* if we've started the current segment    */
  gboolean             streaming;        /* TRUE if we are operating chain-based    */
  GstClockTime         latency;

  /* Descrambler settings */
  guint8               span;
  guint16              ds_packet_size;
  guint16              ds_chunk_size;
  guint16              ds_data_size;

  /* for debugging only */
  gchar               *objpath;

  /* simple index, if available */
  GstClockTime         sidx_interval;    /* interval between entries in ns */
  guint                sidx_num_entries; /* number of index entries        */
  AsfSimpleIndexEntry *sidx_entries;     /* packet number for each entry   */
  gint16               audStrmId;
  gint16               vidStrmId;
  gboolean             firstAudBufSent;
  gboolean             firstVidBufSent;

#ifdef ASFDEMUX_MODIFICATION
  guint32              seek_idx;
  GstClockTime         stop_ts;
  GstClockTime         current_ts;
  AsfTrickplayInfo trickplay_info;
  guint                header_length;
  guint                header_size;
  GstClockTime         packet_send_time;
  gboolean             is_seeked;
  // For Enabling Playready content
  gchar                *file_location;
  gboolean              null_ready_state;
  // Playready content
  gboolean             playback_protected;
  // downloading mode
  gboolean             downloading_mode;
  guint                no_of_video_streams;
  guint                linked_video_stream_no;
  guint                index_table_no;
  gboolean             is_drm_file;
#endif

#ifdef CODEC_ENTRY
  /* Codec List Object */
  CODEC_ENTRY* codec_entry;
  int codec_entry_count;
#endif


};

struct _GstASFDemuxClass {
  GstElementClass parent_class;
};

GType           gst_asf_demux_get_type (void);

AsfStream     * gst_asf_demux_get_stream (GstASFDemux * demux, guint16 id);
void asfdemux_post_trusted_drm_error (GstASFDemux * demux, int drm_error);
G_END_DECLS

#endif /* __ASF_DEMUX_H__ */
