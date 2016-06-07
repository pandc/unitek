#include "BitmapsImmagini.h"



// Bitmap sizes for termometro
const unsigned char termometro_WidthPages = 3;
const unsigned char termometro_HeightPixels = 54;



//
//  Image data for gradi
//

const unsigned char gradi_bmp[] =
{
	0x06, //  ##
	0x09, // #  #
	0x09, // #  #
	0x06  //  ##
};

// Bitmap sizes for gradi
const unsigned char gradi_WidthPages = 1;
const unsigned char gradi_HeightPixels = 4;

/*
**  Image data for triangolino
*/

const unsigned char triangolino_bmp[] =
{
	0x01, // #
	0x03, // ##
	0x07, // ###
	0x0F, // ####
	0x1F, // #####
	0x3F, // ######
	0x1F, // #####
	0x0F, // ####
	0x07, // ###
	0x03, // ##
	0x01, // #
};

/* Bitmap sizes for triangolino */
const unsigned char triangolino_WidthPages = 1;
const unsigned char triangolino_HeightPixels = 11;


/*
**  Image data for beutaVuota
*/

const unsigned char beutaVuota_bmp [] =
{
	0xFE, 0x0F, //  ###########
	0x02, 0x08, //  #         #
	0x04, 0x04, //   #       #
	0x08, 0x02, //    #     #
	0x08, 0x02, //    #     #
	0x08, 0x02, //    #     #
	0x08, 0x02, //    #     #
	0x08, 0x02, //    #     #
	0x08, 0x02, //    #     #
	0x08, 0x02, //    #     #
	0x08, 0x02, //    #     #
	0x04, 0x04, //   #       #
	0x02, 0x08, //  #         #
	0x01, 0x10, // #           #
	0x01, 0x10, // #           #
	0x01, 0x10, // #           #
	0x01, 0x10, // #           #
	0x01, 0x10, // #           #
	0x01, 0x10, // #           #
	0x02, 0x08, //  #         #
	0x04, 0x04, //   #       #
	0xF8, 0x03, //    #######
};

/* Bitmap sizes for beutaVuota */
const unsigned char beutaVuotaWidthPages = 2;
const unsigned char beutaVuotaHeightPixels = 22;



/*
**  Image data for pompa_OK
*/

const unsigned char pompa_OK_bmp [] =
{
	0x00, 0x00, 0x10, //                     #
	0x00, 0x00, 0x38, //                    ###
	0x00, 0x1F, 0x7C, //         #####     #####
	0xC0, 0x60, 0x10, //       ##     ##     #
	0x20, 0x80, 0x10, //      #         #    #
	0x10, 0x00, 0x11, //     #           #   #
	0x10, 0x00, 0x11, //     #           #   #
	0x08, 0x02, 0x12, //    #     #       #  #
	0x08, 0x06, 0x12, //    #     ##      #  #
	0xFF, 0x0F, 0x12, // ############     #  #
	0x08, 0x06, 0x12, //    #     ##      #  #
	0x08, 0x02, 0x12, //    #     #       #  #
	0x10, 0x00, 0x01, //     #           #
	0x10, 0x00, 0x01, //     #           #
	0x20, 0x80, 0x00, //      #         #
	0xC0, 0x60, 0x00, //       ##     ##
	0x40, 0x5F, 0x00, //       # ##### #
	0x20, 0x80, 0x00, //      #         #
	0x10, 0x00, 0x01, //     #           #
	0x08, 0x00, 0x02, //    #             #
	0xFC, 0xFF, 0x07, //   #################
};

/* Bitmap sizes for pompa_OK */
const unsigned char pompa_OKWidthPages = 3;
const unsigned char pompa_OKHeightPixels = 21;


/*
**  Image data for termometro
*/

const unsigned char termometro_bmp [] =
{
	0xF0, 0x00, //     ####
	0x08, 0x01, //    #    #
	0x68, 0x01, //    # ## #
	0x68, 0x01, //    # ## #
	0x68, 0x01, //    # ## #
	0x68, 0x01, //    # ## #
	0x68, 0x01, //    # ## #
	0x68, 0x01, //    # ## #
	0x68, 0x01, //    # ## #
	0x68, 0x01, //    # ## #
	0x68, 0x01, //    # ## #
	0x68, 0x01, //    # ## #
	0x68, 0x01, //    # ## #
	0x64, 0x02, //   #  ##  #
	0x62, 0x04, //  #   ##   #
	0xF2, 0x04, //  #  ####  #
	0xF9, 0x09, // #  ######  #
	0xF1, 0x08, // #   ####   #
	0x62, 0x04, //  #   ##   #
	0x06, 0x06, //  ##      ##
	0x0C, 0x03, //   ##    ##
	0xF0, 0x00, //     ####
};

/* Bitmap sizes for termometro */
const unsigned char termometroWidthPages = 2;
const unsigned char termometroHeightPixels = 22;

/*
**  Image data for triangolino_up
*/

const unsigned char triangolino_up_bmp [] =
{
	0x10, 0x00, //     #
	0x38, 0x00, //    ###
	0x7C, 0x00, //   #####
	0xFE, 0x00, //  #######
	0xFF, 0x01, // #########
};

/* Bitmap sizes for triangolino_up */
const unsigned char triangolino_upWidthPages = 2;
const unsigned char triangolino_upHeightPixels = 5;


/*
**  Image data for triangolino_dn
*/

const unsigned char triangolino_dn_bmp [] =
{
	0xFF, 0x01, // #########
	0xFE, 0x00, //  #######
	0x7C, 0x00, //   #####
	0x38, 0x00, //    ###
	0x10, 0x00, //     #
};

/* Bitmap sizes for triangolino_dn */
const unsigned char triangolino_dnWidthPages = 2;
const unsigned char triangolino_dnHeightPixels = 5;


/*
**  Image data for riscaldatore
*/

const unsigned char riscaldatore_bmp [] =
{
	0x40, 0x08, 0x01, //       #    #    #
	0x40, 0x08, 0x01, //       #    #    #
	0x20, 0x84, 0x00, //      #    #    #
	0x20, 0x84, 0x00, //      #    #    #
	0x40, 0x08, 0x01, //       #    #    #
	0x40, 0x08, 0x01, //       #    #    #
	0x20, 0x84, 0x00, //      #    #    #
	0x20, 0x84, 0x00, //      #    #    #
	0x00, 0x00, 0x00, //
	0xFC, 0xFF, 0x0F, //   ##################
	0x03, 0x00, 0x30, // ##                  ##
	0xFC, 0xFF, 0x0F, //   ##################
	0x00, 0x00, 0x00, //
	0x00, 0x00, 0x00, //
	0xFC, 0xFF, 0x0F, //   ##################
	0x03, 0x00, 0x30, // ##                  ##
	0xFC, 0xFF, 0x0F, //   ##################
	0x00, 0x00, 0x00, //
	0x00, 0x00, 0x00, //
	0xFC, 0xFF, 0x0F, //   ##################
	0x03, 0x00, 0x30, // ##                  ##
	0xFC, 0xFF, 0x0F, //   ##################
};

/* Bitmap sizes for riscaldatore */
const unsigned char riscaldatoreWidthPages = 3;
const unsigned char riscaldatoreHeightPixels = 22;

