/*
 * libheif example application "heif".
 * Copyright (c) 2017 struktur AG, Dirk Farin <farin@struktur.de>
 *
 * This file is part of heif, an example application using libheif.
 *
 * heif is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * heif is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with heif.  If not, see <http://www.gnu.org/licenses/>.
 */
#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>

#include "libheif/heif.h"
#include "heif_encoder_dummy.h"


#define MAX_ENCODERS 5

void show_help(const char *argv0)
{
	printf("heif-mux  libheif version: %s\n", heif_get_version());
	printf("------------------------------------\n");
	printf("usage: %s -w[width] -h[height] [options] image.h265\n", argv0);
	printf("options:\n");
	printf(" -w --width, set iutput img width\n");
	printf(" -h --height, set iutput img height\n");
	printf(" -o --ouput, output filename (optional)\n");
}

static struct option long_options[] = {
		{"width",     required_argument, 0, 'w' },
		{"height",     required_argument, 0, 'h' },
		{"output",     required_argument, 0, 'o' },
		{0,         0,                 0,  0 }
};


int main(int argc, char **argv)
{
	heif_context* context = NULL;
	const heif_encoder_descriptor* encoder_descriptors[MAX_ENCODERS];
	heif_image* image = NULL;
	heif_image_handle* handle = NULL;
	heif_error error;
	uint8_t *p = NULL;
	int size = 0;
	int stride = 0;
	int width, height;
	char* input_filename = NULL;
	char output_filename[64];
	memset(output_filename, '\0', sizeof(output_filename));
	heif_encoder *encoder = nullptr;

	while (true)
	{
		int option_index = 0;
		int c = getopt_long(argc, argv, "o:w:h:", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
			case 'o':
				strcpy(output_filename, optarg);
				break;
			case 'w':
				width = atoi(optarg);
				break;
			case 'h':
				height = atoi(optarg);
				break;
		}
	}

	if (optind != argc-1 || width == 0 || height == 0)
	{
		show_help(argv[0]);
		return 0;
	}

	input_filename = argv[optind];

	if (strlen(output_filename) == 0)
	{
		char* dot_position = strchr(input_filename, '.');
		if(dot_position != NULL)
		{
			strncpy(output_filename, input_filename, dot_position - input_filename);
		}else
		{
			strcat(output_filename, input_filename);
		}

		strcat(output_filename, ".heic");
	}

	heif_register_encoder_plugin(get_encoder_plugin_dummy());
	context = heif_context_alloc();
	if (!context)
	{
		printf("Could not create HEIF context\n");
		return -1;
	}

	int count = heif_context_get_encoder_descriptors(context, heif_compression_HEVC, NULL, encoder_descriptors, MAX_ENCODERS);
	printf("encoder count:%d\n", count);
	if (count > 0)
	{
		int i = 0;
		for(; i < count; i++)
		{
			printf("Encoder: %s = %s. \n",  heif_encoder_descriptor_get_id_name(encoder_descriptors[i]), heif_encoder_descriptor_get_name(encoder_descriptors[i]));
		}
	}
	else
	{
		printf("No HEVC encoder available.\n");
		return -1;
	}

	error = heif_context_get_encoder(context, encoder_descriptors[0], &encoder);
	if (error.code != 0)
	{
		printf("%s\n", error.message);
		return -1;
	}

	heif_image_create(width, height, heif_colorspace_YCbCr, heif_chroma_420, &image);
	heif_image_add_plane(image, heif_channel_Y, width, height, 8);
	p = heif_image_get_plane(image, heif_channel_Y, &stride);

	FILE *fp = fopen(input_filename, "rb");
	if (!fp)
	{
		printf("no input.hevc file\n");
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	size = (int)ftell(fp);
	fseek(fp, 0, SEEK_SET);

	fread(p, size, 1, fp);
	fclose(fp);

	error = heif_encoder_set_parameter_integer(encoder, "file_size", size);
	if (error.code != 0)
	{
		printf("%s\n", error.message);
		return -1;
	}

	error = heif_context_encode_image(context, image, encoder, nullptr, &handle);
	if (error.code != 0)
	{
		printf("Could not read HEIF file: %s\n",error.message);
		return -1;
	}

	heif_encoder_release(encoder);

	heif_image_handle_release(handle);

	error = heif_context_write_to_file(context, output_filename);
	if (error.code)
	{
		printf("%s\n", error.message);
		return -1;
	}

	heif_context_free(context);

	return 0;
}
