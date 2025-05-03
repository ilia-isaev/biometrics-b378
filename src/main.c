#include "config.h"

#include "ftool.h"
#include "lib378.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <math.h>
#include <limits.h>

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _WIN32
#include <windows.h>
#endif

static int
check_file_extension(const char* name_end, const char* extension, const size_t ext_sz)
{
    return strncmp(name_end-ext_sz, extension, ext_sz);
}

int
incits378_check_sz(const size_t sz)
{
	return (sz >= 26);
}

int
incits378_number_of_minutiae(const void *const buffer, const size_t sz)
{
	const size_t idx = sz < 65535 ? 29 : 33;
	return ((const unsigned char *const)buffer)[idx];
}

/*int
incits378_minutiae_block(unsigned char **minutiae_block, void *buffer_in)
{
	unsigned char *buffer = buffer_in;

	const int len_short = (buffer[8]<<8)+buffer[9];

	if (len_short == 32)
		return 0;

	const int add_4_byte = len_short == 0 ? 4 : 0;

	unsigned char number_finger_views = buffer[24+add_4_byte];
	if (number_finger_views == 0)
		return 0;

	*minutiae_block = buffer+30+add_4_byte;

	return buffer[29+add_4_byte];
}*/

int
main (int argc, char **argv)
{
	int     help_opt = 0;
	int     vers_opt = 0;
	int     vrbs_opt = 0;

	int     mnum_opt = 0;
	int     mprt_opt = 0;
	int     less_opt = 0;
	int     gret_opt = 0;
	int     eqal_opt = 0;
	int     pair_opt = 0;
	int     scle_opt = 0;
	int     rott_opt = 0;
	int     eblk_opt = 0;
	int     core_opt = 0;
	int     delt_opt = 0;
	int     orih_opt = 0;

	int     cnt378    = 0;
	int     cnt_param = 0;
	char   *file378[argc];
	int     param[argc];

	/* Promgramm option check */
	for (int i = 1; i < argc; ++i)
	{
		const size_t len = strlen(argv[i]);
		if (argv[i][0] == '-')
		{
			for (int n = 1; n < len; ++n)
			{
				const char opt = argv[i][n];
				if ('V' == opt)
					vrbs_opt = 1;
				else if ('h' == opt)
					help_opt = 1;
				else if ('v' == opt)
					vers_opt = 1;
				else if ('n' == opt)
					mnum_opt = 1;
				else if ('N' == opt)
					mprt_opt = 1;
				else if ('l' == opt)
					less_opt = 1;
				else if ('e' == opt)
					eqal_opt = 1;
				else if ('g' == opt)
					gret_opt = 1;
				else if ('p' == opt)
					pair_opt = 1;
				else if ('E' == opt)
					eblk_opt = 1;
				else if ('c' == opt)
					core_opt = 1;
				else if ('d' == opt)
					delt_opt = 1;
				else if ('s' == opt)
					scle_opt = 1;
				else if ('r' == opt)
					rott_opt = 1;
				else if ('o' == opt)
					orih_opt = 1;
				else
					fprintf (stderr, "Bad option:'%s'\n", argv[i]);
			}
		}
		else
		{
			const int prm_opt = less_opt || gret_opt || eqal_opt || scle_opt || rott_opt || mprt_opt;
			if (!prm_opt && check_file_extension(argv[i] + len, "incits378", strlen("incits378")) == 0) { // argv[i] sz problem!!!
				file378[cnt378++] = argv[i];
			}
			else if (prm_opt)
				param[cnt_param++] = atoi(argv[i]);
		}
	}

	/* Standart imformation output */
	if (help_opt)
	{
		fprintf (stdout,
		    "Usage: %s *.incits378 [-Vvh]\n"
		    "Biometric incits378 tool\n"
		    "View and change incits378 template file\n"
			"   -r rot  rotate template by [rot] degree\n"
			"   -s val  scale  template by [val] times (val is x100)\n"
			"   -p      pair wsq extension for this template\n"
			"   -g      great minutiae number\n"
			"   -e      equal minutiae number\n"
			"   -l      less  minutiae number\n"
			"   -n      number of minutiae in template\n"
			"   -N n    print [n] minutiae in template\n"
			"   -E      extended data block info\n"
			"   -c      core  inforation\n"
			"   -d      delta inforation\n"
			"   -o      orientation of minutiae histogram\n"
		    "   -V      verbose output\n"
		    "   -v      show current version\n"
		    "   -h      show this help\n", PACKAGE_NAME);
	}
	if (vers_opt)
	{
		fprintf (stdout,
		    "   %s tool\n"
		    "version:   %s\n"
		    "date:       2 Ago 2023\n", PACKAGE_NAME, PACKAGE_VERSION);
	}
	if (help_opt || vers_opt)
	{
		return 0;
	}

	if (vrbs_opt)
	{
		for (int i = 0; i < cnt_param; ++i)
			fprintf(stdout, "prm[%2i]:%i\n", i+1, param[i]);
		if (less_opt)
			fprintf(stdout, "less opt\n");
		if (gret_opt)
			fprintf(stdout, "great opt\n");
		if (eqal_opt)
			fprintf(stdout, "equal opt\n");
	}

	float scale = 1.0;
	if (scle_opt && cnt_param > 0)
	{
		scale = param[0]/100.0;
		fprintf(stdout, "scale to: %.2f\n", scale);
		fprintf(stdout, "WARNING this is modification, continue? y[N]:");
		char c = 'n';
		int num = scanf("%c", &c);
		if (num == 0 || (num && (c != 'y' && c != 'Y')))
			return 0;
	}

	int   rot   = 0;
	float angle = 0;
	if (rott_opt && cnt_param > 0)
	{
		rot   = param[0]>>1;
		angle = rot*M_PI/90;
		fprintf(stdout, "rotete to: %.2f %.1f\n", angle, angle*180.0/M_PI);
		fprintf(stdout, "WARNING this is modification, continue? y[N]:");
		char c = 'n';
		int num = scanf("%c", &c);
		if (num == 0 || (num && (c != 'y' && c != 'Y')))
			return 0;
	}

	int hist[180] = {0};

	for (int i = 0; i < cnt378; ++i)
	{
		const char *name = file378[i];

		rbuf_s rd = rd_file(name);
		if (rd.err)
		{
			fprintf(stderr, "rd file: %s error: %s\n", name, strerror(rd.err));
			continue;
		}

		const size_t   mnum   = incits378_number_of_minutiae(rd.buf.ptr, rd.buf.sz);
		lib378_blk_s blk = incits378_dimension_block(rd.buf.ptr);
		const incits378_image_sz_s imgsz = incits378_image_size(blk.blk);

		if (!mnum_opt && !mprt_opt && !less_opt && !gret_opt && !eqal_opt && !orih_opt && !eblk_opt)
			fprintf(stdout, "%ix%i\t%s\n", imgsz.w, imgsz.h, name);
		else if (mnum_opt && !less_opt && !gret_opt && !eqal_opt)
			fprintf(stdout, "%3li\t%s\n", mnum, name);
		else if (eblk_opt)
		{
			lib378_blk_s b378 = incits378_extended_data_block(rd.buf.ptr);
			const short ext_len  = b378.blk.sz;
			if (ext_len > 0)
			{
				if (!core_opt && !delt_opt)
				{
					fprintf(stdout, "extended data block num: %i len: %i %s\n", b378.blk.num, b378.blk.sz, name);
					for (int i = 0; i < b378.blk.num; ++i)
					{
						unsigned short tc = incits378_extended_area_type_code(b378.blk, i+1);
						fprintf(stdout, "...[%2i] area type code: 0x%04x %s\n", i+1, tc,
							tc == 1 ? "(ridge count data)" : tc == 2 ? "(core and delta data)" : "");
					}
				}
				else
				{
					fprintf(stdout, "%s\n", name);
					incits378_block_s cd_blk = incits378_core_delta_block(b378.blk);
					if (cd_blk.sz)
					{
						/*fprintf(stdout, "core delta size: %i\n", cd_blk.sz);
						for (int i = 0; i < cd_blk.sz; ++i)
							fprintf(stdout, "[%5i]byte: %i\n", i, ((unsigned char*)cd_blk.buf)[i]);*/

						incits378_core_delta_info_s cdi = incits378_core_delta_info(cd_blk);
						/*fprintf(stdout, "core: %i\n", cdi.core.num);
						fprintf(stdout, "...ang: %s\n", cdi.core.has_angle ? "has" : "no");
						fprintf(stdout, "delta: %i\n", cdi.delta.num);
						fprintf(stdout, "...ang: %s\n", cdi.delta.has_angle ? "has" : "no");*/

						for (int i = 0; i < cdi.core.num && core_opt; ++i)
						{
							incits378_core_delta_s c = incits378_core(cdi.core, i+1);
							if (cdi.core.has_angle)
								fprintf(stdout, "...[%2i] core:  [%4i %4i]  %3i\n",
									i+1, c.x, c.y, c.a1*2);
							else
								fprintf(stdout, "...[%2i] core:  [%4i %4i]\n", i+1, c.x, c.y);
						}
						if (core_opt && !delt_opt && cdi.core.num == 0)
							fprintf(stdout, "... no core\n");
						for (int i = 0; i < cdi.delta.num && delt_opt; ++i)
						{
							incits378_core_delta_s d = incits378_delta(cdi.delta, i+1);
							if (cdi.delta.has_angle)
								fprintf(stdout, "...[%2i] delta: [%4i %4i]  %3i %3i %3i\n",
									i+1, d.x, d.y, d.a1*2, d.a2*2, d.a3*2);
							else
								fprintf(stdout, "...[%2i] delta: [%4i %4i]\n", i+1, d.x, d.y);
						}	
						if (!core_opt && delt_opt && cdi.delta.num == 0)
							fprintf(stdout, "... no delta\n");
					}
					else
						fprintf(stdout, "... no core delta\n");
				}
			}
			else if (vrbs_opt)
				fprintf(stdout, "no extended data block in: %s\n", name);
		}
		else if (orih_opt)
		{
			lib378_blk_s   m_blk  = incits378_minutiae_block(rd.buf.ptr);
			const int      m      = m_blk.blk.num;
			unsigned char *buffer = m_blk.blk.buf;

			for (int i = 0, ipos = 0; i < m; ++i, ipos+=6)
			{
				const int orient = buffer[ipos+4];

				hist[orient]++;
			}
		}
		else if (mprt_opt && cnt_param)
		{
			//unsigned char *buffer = NULL;
			//const int m = incits378_minutiae_block(&buffer, rd.buf.ptr);
			lib378_blk_s   m_blk  = incits378_minutiae_block(rd.buf.ptr);
			const int      m      = m_blk.blk.num;
			unsigned char *buffer = m_blk.blk.buf;

			for (int i = 0; i < cnt_param; ++i)
			{
				if (param[i] > 0 && param[i] <= m)
				{
					int ipos = (param[i]-1) * 6;

					const unsigned char type = (buffer[ipos]>>6) & 0x03;

					const short x = ((buffer[ipos  ] & 0x3F) << 8) + buffer[ipos+1];
					const short y = ((buffer[ipos+2] & 0x3F) << 8) + buffer[ipos+3];

					const int orient = buffer[ipos+4];

					fprintf(stdout, "[%3i] %i,%i %3i %c \t%s\n", param[i], x, y, orient*2,
						(type == 0 ? 'n' : type == 1 ? 'e' : 's'), name);
				}
			}
		}

		if (cnt_param)
		{
			if (((less_opt && gret_opt && cnt_param > 1) &&
			    ((param[0] > param[1] && param[0] > mnum && param[1] < mnum) ||
			     (param[0] < param[1] && param[0] < mnum && param[1] > mnum)   )) ||
			    (!gret_opt && less_opt && mnum <  param[0])                       ||
			    (!less_opt && gret_opt && mnum >  param[0])                       ||
			     (eqal_opt && ((!less_opt && !gret_opt      && mnum == param[0]) ||
			     (less_opt &&    gret_opt &&  cnt_param > 2 && mnum == param[2]))  )   )
			{
				if (mnum_opt)
					fprintf(stdout, "%3li\t%s\n", mnum, name);
				else
				{
					if (pair_opt)
					{
						char *root = strndup(name, strlen(name)-strlen("incits378"));
						fprintf(stdout, "%s %swsq\n", name, root);
						free(root);
					}
					else
						fprintf(stdout, "%s\n", name);
				}
			}

			if (scle_opt && cnt_param > 0)
			{
				lib378_blk_s   m_blk  = incits378_minutiae_block(rd.buf.ptr);
				const int      m      = m_blk.blk.num;
				unsigned char *buffer = m_blk.blk.buf;

				for (int i = 0, ipos = 0; i < m; ++i, ipos+=6)
				{
					const short x = ((buffer[ipos  ] & 0x3F) << 8) + buffer[ipos+1];
					const short y = ((buffer[ipos+2] & 0x3F) << 8) + buffer[ipos+3];

					const short sx = x*scale;
					const short sy = y*scale;

					const unsigned char type = (buffer[ipos]>>6) & 0x03;
					buffer[ipos  ] = ((sx>>8)&0x3F) + (type<<6);
					buffer[ipos+1] = ( sx    &0xFF);
					buffer[ipos+2] = ((sy>>8)&0x3F);
					buffer[ipos+3] = ( sy    &0xFF);
				}

				lib378_blk_s blk = incits378_dimension_block(rd.buf.ptr);
				int d  = blk.blk.sz;
				buffer = blk.blk.buf;
				if (d)
				{
					const incits378_image_sz_s sz = incits378_image_size(blk.blk);

					const short s_sz_x = sz.w*scale;
					const short s_sz_y = sz.h*scale;

					buffer[0] = s_sz_x >>    8;
					buffer[1] = s_sz_x &  0xFF;
					buffer[2] = s_sz_y >>    8;
					buffer[3] = s_sz_y &  0xFF;
				}

				int err = wr_file(name, rd.buf);
				if (err)
					fprintf(stderr, "%s, %s\n", name, strerror(err));
				else
					fprintf(stdout, "%s scaled to %.2f\n", name, scale);
			}

			if (rott_opt && cnt_param > 0)
			{
				double ax[256];
				double ay[256];
				double min_x = SHRT_MAX;
				double min_y = SHRT_MAX;
				double max_x = 0;
				double max_y = 0;
				lib378_blk_s   m_blk  = incits378_minutiae_block(rd.buf.ptr);
				const int      m      = m_blk.blk.num;
				unsigned char *buffer = m_blk.blk.buf;

				for (int i = 0, ipos = 0; i < m; ++i, ipos+=6)
				{
					const short x = ((buffer[ipos  ] & 0x3F) << 8) + buffer[ipos+1];
					const short y = ((buffer[ipos+2] & 0x3F) << 8) + buffer[ipos+3];

					min_x = fmin(x, min_x);
					min_y = fmin(y, min_y);
					max_x = fmax(x, max_x);
					max_y = fmax(y, max_y);
				}

				const double center_x = (max_x+min_x)/2;
				const double center_y = (max_y+min_y)/2;

				for (int i = 0, ipos = 0; i < m; ++i, ipos+=6)
				{
					int orient = buffer[ipos+4];

					if (179 < (orient + rot))
						orient = (rot+orient) - 180;
					else
						orient += rot;

					buffer[ipos+4] = orient;

					short x = ((buffer[ipos  ] & 0x3F) << 8) + buffer[ipos+1];
					short y = ((buffer[ipos+2] & 0x3F) << 8) + buffer[ipos+3];

					x -= center_x;
					y -= center_y;

					ax[i] =  x*cosf(angle)+y*sinf(angle) + center_x;
					ay[i] = -x*sinf(angle)+y*cosf(angle) + center_y;

					min_x = fmin(ax[i], min_x);
					min_y = fmin(ay[i], min_y);
					max_x = fmax(ax[i], max_x);
					max_y = fmax(ay[i], max_y);
				}

				if (min_x < 0)
				{
					for (int i = 0; i < m; ++i)
						ax[i] += fabs(min_x)+10;
					max_x += fabs(min_x)+10;
				}

				if (min_y < 0)
				{
					for (int i = 0; i < m; ++i)
						ay[i] += fabs(min_y)+10;
					max_y += fabs(min_y)+10;
				}

				for (int i = 0, ipos = 0; i < m; ++i, ipos+=6)
				{
					const short sx = ax[i];
					const short sy = ay[i];

					const unsigned char type = (buffer[ipos]>>6) & 0x03;
					buffer[ipos  ] = ((sx>>8)&0x3F) + (type<<6);
					buffer[ipos+1] = ( sx    &0xFF);
					buffer[ipos+2] = ((sy>>8)&0x3F);
					buffer[ipos+3] = ( sy    &0xFF);
				}

				lib378_blk_s blk = incits378_dimension_block(rd.buf.ptr);
				int d  = blk.blk.sz;
				buffer = blk.blk.buf;
				if (d)
				{
					const incits378_image_sz_s sz = incits378_image_size(blk.blk);

					const short s_sz_x = max_x + 10;
					const short s_sz_y = max_y + 10;

					if (sz.w < s_sz_x || sz.h < s_sz_y)
					{
						buffer[0] = s_sz_x >>    8;
						buffer[1] = s_sz_x &  0xFF;
						buffer[2] = s_sz_y >>    8;
						buffer[3] = s_sz_y &  0xFF;
					}
				}

				int err = wr_file(name, rd.buf);
				if (err)
					fprintf(stderr, "%s, %s\n", name, strerror(err));
				else
					fprintf(stdout, "%s rotated to %i\n", name, rot*2);
			}
		}
		free(rd.buf.ptr);
	}

	if (orih_opt)
	{
			int num = 0;
			for (int i = 0; i < 180; ++i)
				if (hist[i])
					num++;

			fprintf(stdout, "%i\n", num);
	}
}

