struct color {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};


struct prg_contents {
	unsigned int bgi_addr[4],
		     mask_addr[8],
		     palette_addr,
		     flipper_addr;

	unsigned char bgimage[320*576],
		      mask[8][320*576],
		      flipper[64*576];

	struct color palette[256];
	char filename[256];
};

