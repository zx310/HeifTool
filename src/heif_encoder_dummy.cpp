/*
 * heif_encoder_dummy.cpp
 *
 *  Created on: Feb 28, 2023
 *      Author: zhouxin
 */


#include "libheif/heif.h"
#include "libheif/heif_plugin.h"

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <memory>
#include <string>
#include <string.h>
#include <stdio.h>


#define MAX_NALS 10
struct nal_struct
{
	uint8_t *mem;
	uint32_t  size;
};

struct dummy_encoder_struct
{
	uint8_t *image_buffer;
	uint32_t  image_size;
	int      nal_idx;

	struct nal_struct nals[MAX_NALS];
};


static const char kSuccess[] = "Success";
static struct heif_error error_Ok = { heif_error_Ok, heif_suberror_Unspecified, kSuccess };
static const int DUMMY_PLUGIN_PRIORITY = 110;

#define MAX_PLUGIN_NAME_LENGTH 80
static char plugin_name[MAX_PLUGIN_NAME_LENGTH];

#define MAX_NPARAMETERS 10
static const struct heif_encoder_parameter* dummy_encoder_parameter_ptrs[MAX_NPARAMETERS+1];

const char* dummy_plugin_name()
{
	printf("%s\n", __func__);
  strcpy(plugin_name, "creative hecv mux encoder");

  return plugin_name;
}


void dummy_init_plugin()
{
	printf("%s\n", __func__);
}


void dummy_cleanup_plugin()
{
	printf("%s\n", __func__);
}


struct heif_error dummy_set_parameter_quality(void* encoder_raw, int quality)
{
	printf("%s\n", __func__);
  return error_Ok;
}

struct heif_error dummy_get_parameter_quality(void* encoder_raw, int* quality)
{
	printf("%s\n", __func__);
  return error_Ok;
}


struct heif_error dummy_set_parameter_lossless(void* encoder_raw, int enable)
{
	printf("%s\n", __func__);
  return error_Ok;
}

struct heif_error dummy_get_parameter_lossless(void* encoder_raw, int* enable)
{
	printf("%s\n", __func__);
  return error_Ok;
}

struct heif_error dummy_set_parameter_logging_level(void* encoder_raw, int logging)
{
	printf("%s\n", __func__);
  return error_Ok;
}

struct heif_error dummy_get_parameter_logging_level(void* encoder_raw, int* loglevel)
{
	printf("%s\n", __func__);
  return error_Ok;
}


struct heif_error dummy_set_parameter_integer(void* encoder_raw, const char* name, int value)
{
	struct dummy_encoder_struct* encoder = (struct dummy_encoder_struct*)encoder_raw;
	printf("%s\n", __func__);
	if(strncmp(name, "file_size", 9) == 0)
	{
		encoder->image_size = (uint32_t)value;
	}
    return error_Ok;
}

struct heif_error dummy_get_parameter_integer(void* encoder_raw, const char* name, int* value)
{
	printf("%s\n", __func__);
	printf("name:%s\n", name);
	printf("value:%d\n", *value);
    return error_Ok;
}


struct heif_error dummy_set_parameter_string(void* encoder_raw, const char* name, const char* value)
{
	printf("%s\n", __func__);
    return error_Ok;
}


struct heif_error dummy_get_parameter_string(void* encoder_raw, const char* name,
                                            char* value, int value_size)
{
    return error_Ok;
}


const struct heif_encoder_parameter** dummy_list_parameters(void* encoder)
{
	printf("%s\n", __func__);
  return dummy_encoder_parameter_ptrs;
}



struct heif_error dummy_new_encoder(void** enc)
{
	printf("%s\n", __func__);
  struct dummy_encoder_struct* encoder = new dummy_encoder_struct();

  *enc = encoder;

  struct heif_error err = { heif_error_Ok, heif_suberror_Unspecified, kSuccess };
  return err;
}

void dummy_free_encoder(void* encoder_raw)
{
	printf("%s\n", __func__);
  struct dummy_encoder_struct* encoder = (struct dummy_encoder_struct*)encoder_raw;

  if (encoder->image_buffer)
	  free(encoder->image_buffer);
  encoder->image_size = 0;
  encoder->nal_idx = 0;

  delete encoder;
}


void dummy_query_input_colorspace(heif_colorspace* colorspace, heif_chroma* chroma)
{
	printf("%s\n", __func__);
  *colorspace = heif_colorspace_YCbCr;
  *chroma = heif_chroma_420;
}

struct heif_error dummy_encode_image(void* encoder_raw, const struct heif_image* image, heif_image_input_class input_class)
{
	printf("%s\n", __func__);
	struct dummy_encoder_struct *encoder = NULL;
	int stride = 0;
	int i = 0;
	uint32_t size = 0;
	uint8_t *p = NULL;

	encoder = (struct dummy_encoder_struct *)encoder_raw;
	size = encoder->image_size;
	printf("file size:%d\n", size);

	p = (uint8_t *)heif_image_get_plane_readonly(image, heif_channel_Y, &stride);
	encoder->nal_idx = 0;


	/* scan NALs */
	while (size > 0)
	{
		/* skip start bit 0x0 0x0 0x0 0x1 */
		if (p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 1 && size > 0)
		{
			if (i > 0)
			{
				encoder->nals[i-1].size = p - encoder->nals[i-1].mem;
				printf("nal[%d] size: %d\n", i -1, (int)encoder->nals[i-1].size);
			}

			p = p + 4;
			size = size - 4;
			encoder->nals[i].mem = p;
			i++;
		}

		p++;
		size--;
	}

	if (i > 0)
	{
		encoder->nals[i-1].size = p - encoder->nals[i-1].mem;
		printf("nal[%d] size: %d\n", i -1, (int)encoder->nals[i-1].size);
	}

	encoder->nals[i].size = 0;
	encoder->nals[i].mem = nullptr;
	printf("Got %d nals\n", i);

	return error_Ok;
}


struct heif_error dummy_get_compressed_data(void* encoder_raw, uint8_t** data, int* size,
                                           enum heif_encoded_data_type* type)
{
	printf("%s\n", __func__);
	struct dummy_encoder_struct *enc = (struct dummy_encoder_struct *)encoder_raw;

	*data = enc->nals[enc->nal_idx].mem;
	*size = (int)enc->nals[enc->nal_idx].size;
	enc->nal_idx++;

	struct heif_error err = { heif_error_Ok, heif_suberror_Unspecified, kSuccess };
	return err;
}

static const struct heif_encoder_plugin encoder_plugin_dummy
{
  /*.plugin_api_version*/1,
  /*compression_format*/heif_compression_HEVC,
  /*id_name*/ "vts265",
  /*priority*/ DUMMY_PLUGIN_PRIORITY,
  /*supports_lossy_compression*/ true,
  /*supports_lossless_compression*/ true,
  /*get_plugin_name*/ dummy_plugin_name,
  /*init_plugin*/ dummy_init_plugin,
  /*cleanup_plugin*/ dummy_cleanup_plugin,
  /*new_encoder*/ dummy_new_encoder,
  /*free_encoder*/ dummy_free_encoder,
  /*set_parameter_quality*/ dummy_set_parameter_quality,
  /*get_parameter_quality*/ dummy_get_parameter_quality,
  /*set_parameter_lossless*/ dummy_set_parameter_lossless,
  /*get_parameter_lossless*/ dummy_get_parameter_lossless,
  /*set_parameter_logging_level*/ dummy_set_parameter_logging_level,
  /*get_parameter_logging_level*/ dummy_get_parameter_logging_level,
  /*list_parameters*/ dummy_list_parameters,
  /*set_parameter_integer*/ dummy_set_parameter_integer,
  /*get_parameter_integer*/ dummy_get_parameter_integer,
  /*set_parameter_boolean*/ dummy_set_parameter_integer, // boolean also maps to integer function
  /*get_parameter_boolean*/ dummy_get_parameter_integer, // boolean also maps to integer function
  /*set_parameter_string*/ dummy_set_parameter_string,
  /*get_parameter_string*/ dummy_get_parameter_string,
  /*query_input_colorspace*/ dummy_query_input_colorspace,
  /*encode_image*/ dummy_encode_image,
  /*get_compressed_data*/ dummy_get_compressed_data
};

const struct heif_encoder_plugin* get_encoder_plugin_dummy()
{
  return &encoder_plugin_dummy;
}

