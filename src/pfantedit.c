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
 */

#include <gtk/gtk.h>
#include "prg.h"

#define VERSION_STRING "Pinball Fantasies Table Editor v.0.2.4"
GtkWidget *window= NULL;
GtkWidget *da = NULL;
/* Pixmap for scribble area, to store current scribbles */
GdkPixmap *pixmap = NULL;

GtkWidget* mask_spinner[4];
GtkWidget* bgi_spinner[4];

struct prg_contents *current_table;

extern void load_prg(guchar* filename, struct prg_contents *table);
extern void patch_prg(struct prg_contents *table);
extern void decode_masks(FILE *fp, struct prg_contents *table);
extern void decode_bgimage(FILE *fp, struct prg_contents *table);
extern void write_png(char* buf, char* filename, struct color* pal);
extern void read_png(char* buf, char* filename, struct color* pal);

enum{
MASK0,
MASK1,
MASK2,
MASK3,
MASK4,
MASK5,
MASK6,
MASK7,
FLIPPER,
BGIMAGE
};

GdkRgbCmap* new_cmap_from_palette(struct color* palette){
	int i;
	guint32 pal[256];
	for (i=0;i<256;i++){
		pal[i] = palette[i].red;
		pal[i] = pal[i]*256 + palette[i].green;
		pal[i] = pal[i]*256 + palette[i].blue;
	}

	return gdk_rgb_cmap_new(pal,256);
}


void draw_bgimage(struct prg_contents *table){
	GdkGC *gc;	
	gc = gdk_gc_new (da->window);
	GdkRgbCmap* cmap= new_cmap_from_palette(table->palette);
//should we free this cmap?
	gdk_draw_indexed_image (pixmap, gc, 0, 0, 320, 576, GDK_RGB_DITHER_NONE, table->bgimage, 320, cmap);
	gtk_widget_queue_draw_area (da,0,0,320,576);
}

void draw_mask(guchar *mask, int xsize, int ysize){
	GdkGC *gc = gdk_gc_new (da->window);	
	guint32 pal[2] = {0x00,0xffffff};
	GdkRgbCmap* cmap = gdk_rgb_cmap_new(pal,2);
	gdk_draw_indexed_image (pixmap, gc, 0, 0, xsize, ysize, GDK_RGB_DITHER_NONE, mask, xsize, cmap);
	gtk_widget_queue_draw_area (da,0,0,xsize,576);
}

/* Get the selected filename and decode the respective PF table file */
static void file_ok_sel( GtkWidget        *w,
                         GtkFileSelection *fs )
{	
	if (!current_table)
		current_table = (struct prg_contents *) malloc(sizeof(struct prg_contents));
	load_prg( (guchar *) gtk_file_selection_get_filename(GTK_FILE_SELECTION (fs)), current_table );
	draw_bgimage( current_table );
	gtk_window_set_title (GTK_WINDOW (window), gtk_file_selection_get_filename(GTK_FILE_SELECTION (fs)));
}

void open_prg_fsel(GtkAction* action){
	GtkWidget *fsel;
	fsel = gtk_file_selection_new( "Select a .PRG file..." );
	
	g_signal_connect (G_OBJECT( GTK_FILE_SELECTION(fsel)->ok_button ), "clicked", G_CALLBACK (file_ok_sel), (gpointer) fsel);
	g_signal_connect_swapped (G_OBJECT (GTK_FILE_SELECTION (fsel)->ok_button), "clicked", G_CALLBACK (gtk_widget_destroy), G_OBJECT (fsel));
	
	g_signal_connect_swapped (G_OBJECT (GTK_FILE_SELECTION (fsel)->cancel_button), "clicked", G_CALLBACK (gtk_widget_destroy), G_OBJECT (fsel));

	gtk_widget_show(fsel);
}

void save_prg(GtkAction* action){
	if (current_table)
		patch_prg(current_table);
}

void save_to_png (GtkAction* actions){
	if (!current_table){
		printf("You must first load a .PRG\n");
		return;
	}
	write_png(current_table->bgimage, "bgimage.png", current_table->palette);
	write_png(current_table->mask[0], "mask0.png", current_table->palette);
	write_png(current_table->mask[1], "mask1.png", current_table->palette);
	write_png(current_table->mask[2], "mask2.png", current_table->palette);
	write_png(current_table->mask[3], "mask3.png", current_table->palette);
	write_png(current_table->mask[4], "mask4.png", current_table->palette);
	write_png(current_table->mask[5], "mask5.png", current_table->palette);
	write_png(current_table->mask[6], "mask6.png", current_table->palette);
	write_png(current_table->mask[7], "mask7.png", current_table->palette);
}

void load_from_png (GtkAction* actions){
	int i;
	if (!current_table){
		printf("Before modifying a table you must load its respective .PRG file.\n");
		return;
	}
	
	read_png(current_table->bgimage, "bgimage.png", current_table->palette);
	read_png(current_table->mask[0], "mask0.png", current_table->palette);
	read_png(current_table->mask[1], "mask1.png", current_table->palette);
	read_png(current_table->mask[2], "mask2.png", current_table->palette);
	read_png(current_table->mask[3], "mask3.png", current_table->palette);
	read_png(current_table->mask[4], "mask4.png", current_table->palette);
	read_png(current_table->mask[5], "mask5.png", current_table->palette);
	read_png(current_table->mask[6], "mask6.png", current_table->palette);
	read_png(current_table->mask[7], "mask7.png", current_table->palette);
	draw_bgimage(current_table); // <-- na verdaaaaaaaaaaaaaaade... não devia acontecer...
}



////////////////////////////////////////////////////////////////////////
/* Create a new pixmap of the appropriate size to store our scribbles */
static gboolean
scribble_configure_event (GtkWidget	    *widget,
			  GdkEventConfigure *event,
			  gpointer	     data)
{
  if (pixmap)
    g_object_unref (pixmap);

  pixmap = gdk_pixmap_new (widget->window,
			   widget->allocation.width,
			   widget->allocation.height,
			   -1);

  /* Initialize the pixmap to white */
  gdk_draw_rectangle (pixmap,
		      widget->style->white_gc,
		      TRUE,
		      0, 0,
		      widget->allocation.width,
		      widget->allocation.height);

  /* We've handled the configure event, no need for further processing. */
  return TRUE;
}
	
/* Redraw the screen from the pixmap */
static gboolean
scribble_expose_event (GtkWidget      *widget,
		       GdkEventExpose *event,
		       gpointer	       data)
{
  /* We use the "foreground GC" for the widget since it already exists,
   * but honestly any GC would work. The only thing to worry about
   * is whether the GC has an inappropriate clip region set.
   */
  
  gdk_draw_drawable (widget->window,
		     widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		     pixmap,
		     /* Only copy the area that was exposed. */
		     event->area.x, event->area.y,
		     event->area.x, event->area.y,
		     event->area.width, event->area.height);
  
  return FALSE;
}
static void
activate_action (GtkAction *action)
{
  g_message ("Action \"%s\" activated", gtk_action_get_name (action));
}

static void
activate_view_action (GtkAction *action, GtkRadioAction *current)
{
	if (current_table){
		switch (gtk_radio_action_get_current_value ( current )) {
			case BGIMAGE:
				draw_bgimage( current_table );
				break;
			case MASK0:
				draw_mask( current_table->mask[0], 320, 576);
				break;
			case MASK1:
				draw_mask( current_table->mask[1], 320, 576);
				break;
			case MASK2:
				draw_mask( current_table->mask[2], 320, 576);
				break;
			case MASK3:
				draw_mask( current_table->mask[3], 320, 576);
				break;
			case MASK4:
				draw_mask( current_table->mask[4], 320, 576);
				break;
			case MASK5:
				draw_mask( current_table->mask[5], 320, 576);
				break;
			case MASK6:
				draw_mask( current_table->mask[6], 320, 576);
				break;
			case MASK7:
				draw_mask( current_table->mask[7], 320, 576);
				break;
			case FLIPPER:
				draw_mask( current_table->flipper, 64, 576);
				break;
		}
	}
}

void change_palette_addr(GtkWidget	    *widget,
			 gpointer	     spinner){
	FILE* fp;
		
	if ( (fp = fopen(current_table->filename, "r")) < 0 ) {
		printf("Couldn't open %s for reloading the palette.\n", current_table->filename);
		exit(-1);
	}
	current_table->palette_addr = gtk_spin_button_get_value_as_int(spinner);
	load_palette(fp, current_table);
	draw_bgimage(current_table);
	fclose(fp);
}

void change_bgimage_addr(GtkWidget	    *widget,
			 gpointer	     data){
	FILE* fp;
		
	if ( (fp = fopen(current_table->filename, "r")) < 0 ) {
		printf("Couldn't open %s for reloading the palette.\n", current_table->filename);
		exit(-1);
	}
	
	int i;
	for (i=0;i<4;i++){
 		current_table->bgi_addr[i] = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(bgi_spinner[i]));

	}

	//load_palette(fp, current_table);
	decode_bgimage(fp, current_table);
	draw_bgimage(current_table);
	fclose(fp);
}

void change_masks_addr(GtkWidget	    *widget,
			 int	    i){
	FILE* fp;
		
	if ( (fp = fopen(current_table->filename, "r")) < 0 ) {
		printf("Couldn't open %s for reloading masks.\n", current_table->filename);
		exit(-1);
	}
	current_table->mask_addr[i] = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(mask_spinner[i]));
	decode_masks(fp, current_table);
	fclose(fp);

	draw_mask(current_table->mask[i], 320, 576);
}



	

void adjust_addr(){
	GtkWidget* AddressesWindow;
	GtkWidget* main_vbox;
	GtkWidget* vbox;
	GtkWidget* frame;
	GtkAdjustment* adj;
	GtkWidget* palette_spinner;
	
	AddressesWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (AddressesWindow), "Adjust Addresses");

	g_signal_connect_swapped (AddressesWindow, "destroy",
			G_CALLBACK (gtk_widget_destroy), AddressesWindow);

	main_vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_set_border_width (GTK_CONTAINER(main_vbox), 10);
	gtk_container_add (GTK_CONTAINER (AddressesWindow), main_vbox);

	if(!current_table){
		GtkWidget* label = gtk_label_new("You must first load a .PRG file.");
		GtkWidget* ok_button = gtk_button_new_with_label("OK");
		gtk_box_pack_start (GTK_BOX (main_vbox), label, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (main_vbox), ok_button, FALSE, TRUE, 0);

		g_signal_connect_swapped (ok_button, "clicked",
			G_CALLBACK (gtk_widget_destroy), AddressesWindow);
		gtk_widget_show_all(AddressesWindow);
		return;
	}
	
	frame = gtk_frame_new ("Background Image");
	gtk_box_pack_start (GTK_BOX(main_vbox), frame, TRUE, TRUE, 0);
	
	
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), vbox);

	int i;	
	for(i=0;i<4;i++){
		adj = (GtkAdjustment *) gtk_adjustment_new (current_table->bgi_addr[i], 0, 0xfffffff, 1.0,5.0, 0.0);
		bgi_spinner[i] = gtk_spin_button_new (adj, 0, 0);
		g_signal_connect (G_OBJECT (adj), "value_changed",
		    G_CALLBACK (change_bgimage_addr), NULL);

		gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (bgi_spinner[i]), TRUE);
  		gtk_box_pack_start (GTK_BOX (vbox), bgi_spinner[i], FALSE, TRUE, 0);
	}

	frame = gtk_frame_new ("Visibility Masks");
	gtk_box_pack_start (GTK_BOX(main_vbox), frame, TRUE, TRUE, 0);
	
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), vbox);

	for(i=0;i<2;i++){
		adj = (GtkAdjustment *) gtk_adjustment_new (current_table->mask_addr[i], 0, 0xffffff, 1.0,5.0, 0.0);
		mask_spinner[i] = gtk_spin_button_new (adj, 0, 0);
		g_signal_connect (G_OBJECT (adj), "value_changed",
		    G_CALLBACK (change_masks_addr),
		    (gpointer) i);

		gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (mask_spinner[i]), TRUE);
  		gtk_box_pack_start (GTK_BOX (vbox), mask_spinner[i], FALSE, TRUE, 0);
	}


	frame = gtk_frame_new ("Structure Masks");
	gtk_box_pack_start (GTK_BOX(main_vbox), frame, TRUE, TRUE, 0);
	
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), vbox);

	for(i=0;i<2;i++){
		adj = (GtkAdjustment *) gtk_adjustment_new(current_table->mask_addr[2+i], 0, 0xffffff, 1.0,5.0, 0.0);
		mask_spinner[2+i] = gtk_spin_button_new (adj, 0, 0);
		g_signal_connect (G_OBJECT (adj), "value_changed",
		    G_CALLBACK (change_masks_addr),
		    (gpointer) 2+i);

		gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (mask_spinner[2+i]), TRUE);
  		gtk_box_pack_start (GTK_BOX (vbox), mask_spinner[2+i], FALSE, TRUE, 0);
	}

	frame = gtk_frame_new ("Palette");
	gtk_box_pack_start (GTK_BOX(main_vbox), frame, TRUE, TRUE, 0);
	
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), vbox);

	adj = (GtkAdjustment *) gtk_adjustment_new(current_table->palette_addr, 0, 0xffffff, 1.0,5.0, 0.0);
	palette_spinner = gtk_spin_button_new (adj, 0, 0);
	
	g_signal_connect (G_OBJECT (adj), "value_changed",
		    G_CALLBACK (change_palette_addr),
		    (gpointer)  palette_spinner);
	
	gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (palette_spinner), TRUE);
	gtk_box_pack_start (GTK_BOX (vbox), palette_spinner, FALSE, TRUE, 0);
	
	gtk_widget_show_all(AddressesWindow);
}

void about_pfant(){
	GtkWidget* AboutWindow;
	GtkWidget* vbox;
	GtkWidget* label;
	GtkWidget* ok_button;
	AboutWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect_swapped (AboutWindow, "destroy",
			G_CALLBACK (gtk_widget_destroy), AboutWindow);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (AboutWindow), vbox);
	
	label = gtk_label_new(VERSION_STRING);
	ok_button = gtk_button_new_with_label("OK");
	g_signal_connect_swapped (ok_button, "clicked",
			G_CALLBACK (gtk_widget_destroy), AboutWindow);


	gtk_box_pack_start (GTK_BOX (vbox),
			  label,
			  FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox),
			  ok_button,
			  FALSE, FALSE, 0);

	gtk_widget_show_all(AboutWindow);
}

static GtkActionEntry entries[] = {
//------------
  { "FileMenu", NULL, "_File" },               /* name, stock id, label */
  { "New", GTK_STOCK_NEW,                      /* name, stock id */
    "_New Table", "<control>N",                      /* label, accelerator */
    "Create a new table project",                       /* tooltip */ 
    G_CALLBACK (activate_action) },      
  { "Open", GTK_STOCK_OPEN,                    /* name, stock id */
    "_Open .PRG File","<control>O",                      /* label, accelerator */     
    "Open a .PRG File",                             /* tooltip */
    G_CALLBACK (open_prg_fsel) }, 
  { "Save", GTK_STOCK_SAVE,                    /* name, stock id */
    "_Save","<control>S",                      /* label, accelerator */     
    "Save current table project",                       /* tooltip */
    G_CALLBACK (save_prg) },
  { "SaveAs", GTK_STOCK_SAVE,                  /* name, stock id */
    "Save _As...", NULL,                       /* label, accelerator */     
    "Save project to a file",                          /* tooltip */
    G_CALLBACK (activate_action) },
  { "Quit", GTK_STOCK_QUIT,                    /* name, stock id */
    "_Quit", "<control>Q",                     /* label, accelerator */     
    "Quit",                                    /* tooltip */
    G_CALLBACK (gtk_main_quit) },
  { "About", NULL,                             /* name, stock id */
    "_About PfantEdit...", "<control>A",                    /* label, accelerator */     
    "About",                                   /* tooltip */  
    G_CALLBACK (about_pfant) },
//------------
  { "ViewMenu", NULL, "_View" },	       /* name, stock id, label */
//------------
  { "OptionsMenu", NULL, "_Options" },	       /* name, stock id, label */
  { "AdjustAddresses", NULL,                      /* name, stock id */
    "_Adjust Addresses", "<control>A",                      /* label, accelerator */
    "Adjusts the addresses for this file.",                       /* tooltip */ 
    G_CALLBACK (adjust_addr) },      
{ "SaveToPNG", NULL,                      /* name, stock id */
    "_Save to PNG files", "<control>P",                      /* label, accelerator */
    "Saves the images to a set of PNG files.",                       /* tooltip */ 
    G_CALLBACK (save_to_png) },      
{ "LoadFromPNG", NULL,                      /* name, stock id */
    "_Load from PNG images", "<control>L",                      /* label, accelerator */
    "Loads the content of a set of PNG images to the current table struct",                       /* tooltip */ 
    G_CALLBACK (load_from_png) },      
//------------
  { "HelpMenu", NULL, "_Help" },               /* name, stock id, label */
};
static guint n_entries = G_N_ELEMENTS (entries);

static GtkRadioActionEntry view_entries[] = {
  { "BGimage", NULL,                               /* name, stock id */
    "_BGimage", "<control>B",                      /* label, accelerator */     
    "Background image", BGIMAGE },
  { "Mask0", NULL,                    /* name, stock id */
     "_Visibility Mask #0", NULL,                    /* label, accelerator */     
    "Visibility Mask #0" , MASK0},
  { "Mask1", NULL,                    /* name, stock id */
     "_Visibility Mask #1", NULL,                    /* label, accelerator */     
    "Visibility Mask #1" , MASK1},
  { "Mask2", NULL,                    /* name, stock id */
     "_StructureMask #0", NULL,                    /* label, accelerator */     
    "Structure Mask #0" , MASK2},
  { "Mask3", NULL,                    /* name, stock id */
     "_Structure Mask #1", NULL,                    /* label, accelerator */     
    "Structure Mask #1" , MASK3},

  { "Mask4", NULL,                    /* name, stock id */
     "_Unknown Mask A0", NULL,                    /* label, accelerator */     
    "Unknown Mask A0" , MASK4},
  { "Mask5", NULL,                    /* name, stock id */
     "_Unknown Mask A1", NULL,                    /* label, accelerator */     
    "Unknown Mask A1" , MASK5},
  { "Mask6", NULL,                    /* name, stock id */
     "_Unknown Mask B0", NULL,                    /* label, accelerator */     
    "Unknown Mask B0" , MASK6},
  { "Mask7", NULL,                    /* name, stock id */
     "_Unknown Mask B1", NULL,                    /* label, accelerator */     
    "Unknown Mask B1" , MASK7},
{ "Flipper", NULL,                    /* name, stock id */
     "_flipper test", NULL,                    /* label, accelerator */     
    "flipper test" , FLIPPER},

};
static guint n_view_entries = G_N_ELEMENTS (view_entries);


static const gchar *ui_info = 
"<ui>"
"  <menubar name='MenuBar'>"
"    <menu action='FileMenu'>"
"      <menuitem action='New'/>"
"      <menuitem action='Open'/>"
"      <menuitem action='Save'/>"
"      <menuitem action='SaveAs'/>"
"      <separator/>"
"      <menuitem action='Quit'/>"
"    </menu>"
"    <menu action='ViewMenu'>"
"      <menuitem action='BGimage'/>"
"      <menuitem action='Mask0'/>"
"      <menuitem action='Mask1'/>"
"      <menuitem action='Mask2'/>"
"      <menuitem action='Mask3'/>"
"      <menuitem action='Mask4'/>"
"      <menuitem action='Mask5'/>"
"      <menuitem action='Mask6'/>"
"      <menuitem action='Mask7'/>"
"      <menuitem action='Flipper'/>"
"    </menu>"
"    <menu action='OptionsMenu'>"
"      <menuitem action='AdjustAddresses'/>"
"      <menuitem action='LoadFromPNG'/>"
"      <menuitem action='SaveToPNG'/>"
"    </menu>"
"    <menu action='HelpMenu'>"
"      <menuitem action='About'/>"
"    </menu>"
"  </menubar>"
"</ui>";

void construct_ui(int *argc, char** argv[]){
      GtkWidget *box1;
      GtkWidget *label;
      GtkWidget *button;
      GtkWidget *frame;
      //GtkWidget *da; //drawing area
      GtkUIManager *uim; //user interface manager
      //GtkWidget *window;
      GtkActionGroup *actions;
      GError *error = NULL;

      gtk_init (argc, argv);

      gdk_rgb_init();
	
      window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	
      g_signal_connect (window, "destroy",
			G_CALLBACK (gtk_widget_destroyed), &window);
      g_signal_connect (window, "delete-event",
			G_CALLBACK (gtk_true), NULL);

      actions = gtk_action_group_new ("Actions");
      gtk_action_group_add_actions (actions, entries, n_entries, NULL);
      gtk_action_group_add_radio_actions (actions, 
					  view_entries, n_view_entries, 
					  BGIMAGE,
					  G_CALLBACK (activate_view_action), 
					  NULL);
      uim = gtk_ui_manager_new ();
      gtk_ui_manager_insert_action_group (uim, actions, 0);
      gtk_window_add_accel_group (GTK_WINDOW (window), 
				  gtk_ui_manager_get_accel_group (uim));
      gtk_window_set_title (GTK_WINDOW (window), "Pinball Fantasies editor");
      //gtk_container_set_border_width (GTK_CONTAINER (window), 10);
      
      if (!gtk_ui_manager_add_ui_from_string (uim, ui_info, -1, &error))
	{
	  g_message ("building menus failed: %s", error->message);
	  g_error_free (error);
	}
      box1 = gtk_vbox_new (FALSE, 0);
      gtk_container_add (GTK_CONTAINER (window), box1);
      
      gtk_box_pack_start (GTK_BOX (box1),
			  gtk_ui_manager_get_widget (uim, "/MenuBar"),
			  FALSE, FALSE, 0);

      frame = gtk_frame_new (NULL);
      gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
      gtk_box_pack_start (GTK_BOX (box1), frame, TRUE, TRUE, 0);
      

      da = gtk_drawing_area_new ();
      /* set a minimum size */
      gtk_widget_set_size_request (da, 320, 576);

      gtk_container_add (GTK_CONTAINER (frame), da);
      
      /* Signals used to handle backing pixmap */
      
      g_signal_connect (da, "expose_event",
			G_CALLBACK (scribble_expose_event), NULL);
      g_signal_connect (da,"configure_event",
			G_CALLBACK (scribble_configure_event), NULL);

      gtk_widget_show_all (window);
}

int main(int argc, char* argv[]){
	construct_ui(&argc, &argv);
	gtk_main();
      return 0;
}
