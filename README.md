# HeifTool
a tool use libheif only to make change from hecv to heic

# usage

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
