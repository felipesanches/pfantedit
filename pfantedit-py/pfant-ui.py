#!/usr/bin/env python
'''
PfantEdit User Interface Classes
'''
import random, loadprg, saveprg, os
import gtk, gobject

try:
  from gettext import gettext
except ImportError:
  def gettext(message):
    return message
_ = gettext

def activate_action(action):
  print 'Action "%s" activated' % action.get_name()

entries = (
  ( "FileMenu", None, _("_File")
  ),

  ( "Open", gtk.STOCK_OPEN,    # name, stock id
    _("_Open"),"<control>O",   # label, accelerator
    _("Open a file"),          # tooltip
    activate_action
  ),

  ( "Quit", gtk.STOCK_QUIT,    
    _("_Quit"), "<control>Q",  
    _("Quit"),                 
    activate_action
  )
)

ui_info = \
'''<ui>
  <menubar name='MenuBar'>
    <menu action='FileMenu'>
      <menuitem action='Open'/>
      <menuitem action='Quit'/>
    </menu>
  </menubar>
</ui>'''

GAME_PARAMETERS = {
  "TABLE1.PRG": {
    "bgimage_addr": [0x53188,
                     0x5A4E6,
                     0x62834,
                     0x6B99E],
    "bgimage_maxaddr": [366163,
                        399769,
                        437153,
                        465711],
    "palette_addr": 0x6ABE0,
    "mask_addr": 0x300B0,
    "sensors_addr": 0x1AA3D
  }
}

class PinballTable():

  def __init__(self, prg_file="TABLE1.PRG"):
    self.prg_file = prg_file
    if prg_file.upper() in GAME_PARAMETERS.keys():
      self.params = GAME_PARAMETERS[prg_file]
    else:
      self.params = GAME_PARAMETERS["TABLE1.PRG"] #  <-- default params

  def load_resources(self):
    fp = open(self.prg_file, 'rb')

    self.image = []
    for i in range(4):
      self.image.append(loadprg.decodeimage(fp, self.params["bgimage_addr"][i], 320, 144))
    self.palette = loadprg.palette(fp, self.params["palette_addr"])
#    self.mask = loadprg.decode_mask(fp, 320,576, self.params["mask_addr"])
    self.mask = []
    for i in range(4):
      self.mask.append(loadprg.decode_mask(fp, 320, 144, self.params["mask_addr"] + i*320*144/8))
    self.sensors = loadprg.load_sensors(fp, self.params["sensors_addr"], 48)
    fp.close()

  def load_from_file(self, filename):
    pb = gtk.gdk.pixbuf_new_from_file_at_size(filename, 320, 576)
    self.image = pb.get_pixels()
    print "numero de pixels:", len(self.image)

  def save_resources(self):
    fd = os.open(self.prg_file, os.O_RDWR)

    for i in range(4):
      saveprg.save_image(fd,
                         self.image[i],
                         self.params["bgimage_addr"][i],
                         self.params["bgimage_maxaddr"][i],
                         320, 144)

    saveprg.palette(fd, self.params["palette_addr"], self.pal)
    #self.mask = loadprg.decode_mask(fp, 320, 576, self.params["mask_addr"])
    #self.sensors = loadprg.load_sensors(fp,  self.params["sensors_addr"], 48)
    os.close(fd)


class PfantUI(gtk.Window):

  def draw_bgimage(self):
    """
    Draws the pinball table background image on the pixmap.
    """
    for i in range(4):
      self.pixmap.draw_indexed_image(gtk.gdk.GC(self.window),
                                     0, 144*i,
                                     320, 144,
                                     gtk.gdk.RGB_DITHER_NORMAL,
                                     self.current_table.image[i],
                                     320,
                                     self.current_table.palette)
    self.da.queue_draw_area(0, 0, 320, 576)

  def draw_mask(self):
    """
    Draws a bitmap mask.
    """
    self.pixmap.draw_indexed_image(gtk.gdk.GC(self.window),
                                   0, 0, 320, 576,
                                   gtk.gdk.RGB_DITHER_NORMAL,
                                   self.current_table.mask,
                                   320,
                                   [0xffffff, 0x00ff00])
    self.da.queue_draw_area(0, 0, 320, 576)

  def draw_sensors(self):
    """
    Draws rectangles representing the sensors
    """
    self.current_table.sensors = [\
      [211, 252, 235, 276], [268, 263, 292, 287],
      [185, 283, 209, 307], [ 50, 415,  80, 470],
      [219, 415, 249, 470], [130, 196, 146, 204],
      [145, 274, 170, 330], [ 90,  15, 130,  40],
      [200,  15, 240,  40], [160,  35, 182,  55],
      [ 47, 122,  67, 146], [120, 165, 150, 185],
      [  3, 245,  22, 270], [220, 280, 260, 300],
      [280, 300, 320, 345], [260, 312, 268, 321],
      [ 25, 435,  35, 445], [263, 435, 273, 445],
      [  5, 455,  15, 465], [284, 455, 294, 465],
      [305, 455, 320, 512], [305, 512, 320, 576],
      [125,   5, 160,  50], [ 40,  40,  75, 100],
      [132,  58, 148,  63], [222,  58, 238,  63],
      [162,  64, 178,  69], [192,  64, 208,  69],
      [ 40,  80,  75, 120], [175, 100, 200, 130],
      [260, 130, 280, 150], [  3, 245,  22, 270],
      [ 10,  40,  30,  70], [ 10, 100,  40, 200],
      [300, 140, 320, 160], [215, 160, 235, 190],
      [ 50, 165,  85, 185], [275, 165, 300, 220],
      [ 60, 185, 100, 220], [195, 175, 215, 200],
      [300, 210, 320, 240], [300, 240, 320, 340],
      [ 20, 240,  50, 400], [  3, 245,  22, 270],
      [300, 360, 320, 400], [300, 400, 320, 500],
      [260, 450, 277, 470], [  0, 450,  50, 470]]
#    print "lenghts:" #testing...
#    print len(loc)
#    print len(self.sensors)
#    print "loc:"
#    print loc
#    print "sensors:"
#    print self.sensors

    for i in self.current_table.sensors:
      self.pixmap.draw_rectangle(gtk.gdk.GC(self.window),
                                 False,
                                 i[0], i[1],
                                 i[2]-i[0], i[3]-i[1])
      self.da.queue_draw_area(0, 0, 320, 576)

  def configure_event (self, widget, event):
    """
    Create a new backing pixmap of the appropriate size
    """
    # Initialize the pixmap to white
    x, y, width, height = widget.get_allocation()
    self.pixmap = gtk.gdk.Pixmap(widget.window, width, height)
    self.pixmap.draw_rectangle(widget.get_style().white_gc, True, 0, 0, width, height)
    # We've handled the configure event, no need for further processing.
    return True

  def expose_event(self, widget, event):
    """
    Redraw the screen from the backing pixmap
    """
    if self.pixmap is None:
      self.configure_event(widget, event)
    x , y, width, height = event.area
    widget.window.draw_drawable(widget.get_style().fg_gc[gtk.STATE_NORMAL],
                                self.pixmap,
                                x, y,
                                x, y,
                                width, height)
    return False

  def __init__(self,
               parent=None,
               prg_file="TABLE1.PRG",
               write_bgimage_png=None,
               test_reload=False):

    self.current_table = PinballTable(prg_file)
    self.write_bgimage_png = write_bgimage_png
    self.test_reload = test_reload

    gtk.Window.__init__(self)
    self.set_title(_("Pinball Fantasies Editor"))
    self.set_border_width(0)

    actions = gtk.ActionGroup("Actions")
    actions.add_actions(entries)

    ui = gtk.UIManager()
    ui.insert_action_group(actions, 0)
#    self.add_accel_group(ui.get_accel_group())

    try:
      mergeid = ui.add_ui_from_string(ui_info)
    except gobject.GError, msg:
      print "building menus failed: %s" % msg

    box1 = gtk.VBox(False, 0)
    self.add(box1)

    box1.pack_start(ui.get_widget("/MenuBar"), False, False, 0)

#    logo = gtk.Image("")
#    box1.pack_start(logo, True, True, 0)
    self.da = gtk.DrawingArea()
    self.da.set_size_request(320, 576)
    box1.pack_start(self.da, False, False, 0)

    # Signals used to handle backing pixmap
    self.da.connect("expose_event", self.expose_event)
    self.da.connect("configure_event", self.configure_event)
    self.show_all()

    print "self.window: ",self.window
    self.pixmap = gtk.gdk.Pixmap(self.window, 320, 576)

    self.current_table.load_resources()

    if self.write_bgimage_png is not None:
      self.current_table.load_from_file(self.write_bgimage_png)
      image2 = []
      self.pal = []
      for j in range(4):
        imagechunk = []
        print "chunk ", j
        for i in range(320*144):
          pos = 4*(i+320*144*j)
          color = (ord(self.current_table.image[pos]),
                   ord(self.current_table.image[pos+1]),
                   ord(self.current_table.image[pos+2]))
          byte = None
          for index,c in enumerate(self.pal):
            if c==color:
              byte = index

          if len(self.pal) >= 200:
            byte = 0

#        print color, byte
          if byte == None:
            self.pal.append(color)
            imagechunk.append(len(self.pal))
          else:
            imagechunk.append(byte)

        image2.append(imagechunk)
#        print "a bit of a chunk:", [imagechunk[ii] for ii in range(5000)]

      self.image = image2

      if self.test_reload:
        self.current_table.save_resources()
        self.current_table.load_resources()

    self.draw_bgimage()
#    self.draw_mask() #testing...
    self.draw_sensors()

def main():
  # TODO: read command line args for:
  #
  # prg_file           -  Which of the 4 tables you want to see/edit ? (TABLE[1-4].PRG)
  # write_bgimage_png  -  Load a PNG file encode it and patch the original game file
  # test_reload        -  Try to reload resources after a binary patch operation

  PfantUI()
  gtk.main()

if __name__ == '__main__':
  main()
