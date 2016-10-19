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

#include <gtk/gtk.h>
#include "prg.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

int hex2int(char * str){
	int rv=0;
	if (str[0] == '0' && str[1] == 'x')
		str+=2;
	while(*str){
		rv*=16;
		if (str[0] >= 96 && str[0] <= 101) rv = rv + str[0] - 86;
		if (*str >= 65 && *str <= 70) rv = rv + str[0] - 55;
		if (*str >= 48 && *str <= 57) rv = rv + str[0] - 48;
		str++;
	}
	return rv;
}

static void parseAddresses (xmlDocPtr doc, xmlNodePtr cur, int count, unsigned int *address) {

	xmlChar *key;
	cur = cur->xmlChildrenNode;
	int i=0;

	while ((cur != NULL) && count--) {
		while(!xmlStrcmp(cur->name, (const xmlChar *)"text")) 
			cur = cur->next;
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"address"))) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			address[i++]= hex2int(key);
			xmlFree(key);
 		} else {
			printf("unexpected tag %s !\n",cur->name);
		}
	cur = cur->next;
	}
	if (!count)
	  printf("Too much addresses...\n");
	return;
}

void parsexml(char *docname, struct prg_contents* table) {

	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile(docname);
	
	if (doc == NULL ) {
		fprintf(stderr,"Document not parsed successfully. \n");
		return;
	}
	
	cur = xmlDocGetRootElement(doc);
	
	if (cur == NULL) {
		fprintf(stderr,"empty document\n");
		xmlFreeDoc(doc);
		return;
	}
	
	if (xmlStrcmp(cur->name, (const xmlChar *) "pftable")) {
		fprintf(stderr,"document of the wrong type, root node != pftable\n");
		xmlFreeDoc(doc);
		return;
	}
	
	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
	  if ((!xmlStrcmp(cur->name, (const xmlChar *)"bgimage"))){
	    parseAddresses(doc, cur, 4, table->bgi_addr);
	  }      
	  if ((!xmlStrcmp(cur->name, (const xmlChar *)"palette"))){
	    parseAddresses(doc, cur, 1, &(table->palette_addr));
	  }      
	  if ((!xmlStrcmp(cur->name, (const xmlChar *)"masks"))){
	    parseAddresses(doc, cur, 8, table->mask_addr);
	  }      
	  if ((!xmlStrcmp(cur->name, (const xmlChar *)"flipper"))){
	    parseAddresses(doc, cur, 1, &(table->flipper_addr));
	  }      
	  cur = cur->next;
	}
	
	xmlFreeDoc(doc);
	return;
}
