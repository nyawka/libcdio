/*
  $Id: udf1.c,v 1.1 2005/10/24 03:12:30 rocky Exp $

  Copyright (C)  2005 Rocky Bernstein <rocky@panix.com>
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* Simple program to show using libiso9660 to list files in a directory of
   an ISO-9660 image.
 */

/* This is the UDF image. */
#define UDF_IMAGE_PATH "../"
#define UDF_IMAGE "/src2/cd-images/udf/UDF102ISO.iso"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <sys/types.h>
#include <cdio/cdio.h>
#include <cdio/udf.h>

#include <stdio.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#define udf_PATH_DELIMITERS "/\\"

static 
udf_file_t *
list_directory(udf_t *p_udf, udf_file_t *p_udf_file, char *psz_token)
{
  if (!p_udf_file) return NULL;
  while (udf_get_next(p_udf, p_udf_file)) {
    printf("%s\n", psz_token);
    if (strcmp(psz_token, p_udf_file->psz_name) == 0) {
      char *next_tok = strtok(NULL, udf_PATH_DELIMITERS);
      
      if (p_udf_file->b_dir) {
	udf_file_t * p_udf_file2 = udf_get_sub(p_udf, p_udf_file);
	
	if (p_udf_file2) {
	  udf_file_t *p_udf_file3 = 
	    list_directory(p_udf, p_udf_file2, next_tok);
	  
	  if (!p_udf_file3) udf_file_free(p_udf_file2);
	  return p_udf_file3;
	}
      }
    }
  }
  return p_udf_file;
}

int
main(int argc, const char *argv[])
{
  udf_t *p_udf;
  char const *psz_fname;

  if (argc > 1) 
    psz_fname = argv[1];
  else 
    psz_fname = UDF_IMAGE;

  p_udf = udf_open (psz_fname);
  
  if (NULL == p_udf) {
    fprintf(stderr, "Sorry, couldn't open %s as something using UDF\n", 
	    psz_fname);
    return 1;
  } else {
    udf_file_t *p_udf_file = udf_get_root(p_udf, true, 0);
    if (NULL == p_udf_file) {
      fprintf(stderr, "Sorry, couldn't find / in %s\n", 
	      psz_fname);
      return 1;
    }

    list_directory(p_udf, p_udf_file, p_udf_file->psz_name);
    /* Go over: udf_file_free(p_udf_file);*/
  }
  
  udf_close(p_udf);
  return 0;
}
