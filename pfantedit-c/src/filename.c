/*----------------------------------------------------------------------
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *----------------------------------------------------------------------
 *
 * (c) 2007, 2012 Felipe Corrêa da Silva Sanches <juca@members.fsf.org>
 *
 *----------------------------------------------------------------------
 */

#include "malloc.h"
#include "string.h"

char *new_filename_change_extension(char *current, char* ext){
        char *new = (char *) malloc(strlen(current)+strlen(ext)+1);
        int i;
        //TODO: use strcpy ? 
        for( i = 0; (new[i] = current[i]) != '\0'; i++ );

        //now we'll look for the extension
        char *ptr = &new[strlen(new)-1];
        while (*ptr != '.' && ptr != new)
                ptr--;

        //when the original name does not contain an extension:
        if (ptr == new)
                ptr = &new[strlen(current)];

        snprintf(ptr, strlen(ext)+1, "%s", ext);

        return new;
}
