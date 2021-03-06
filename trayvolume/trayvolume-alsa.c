// Applet for changing volume with mouse wheel (alsa version)

#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <values.h>

#include <alsa.h>
#include <alsa.c>

GtkBuilder *builder;
GtkWidget *winTray1;
GtkImage *imgTray1;

void update_icon(long int v);
G_MODULE_EXPORT void on_eventbox1_scroll_event (GtkObject *object, GdkEventScroll *scroll);

void update_icon(long int v) {
  // update icon depending on current volume
  gtk_image_set_from_icon_name(imgTray1,"audio-volume-muted",GTK_ICON_SIZE_SMALL_TOOLBAR);
  if (v > 10) gtk_image_set_from_icon_name(imgTray1,"audio-volume-low",GTK_ICON_SIZE_SMALL_TOOLBAR);
  if (v > 80) gtk_image_set_from_icon_name(imgTray1,"audio-volume-medium",GTK_ICON_SIZE_SMALL_TOOLBAR);
  if (v > 95) gtk_image_set_from_icon_name(imgTray1,"audio-volume-high",GTK_ICON_SIZE_SMALL_TOOLBAR);
}

G_MODULE_EXPORT void
on_eventbox1_scroll_event (GtkObject *object, GdkEventScroll *scroll) {
  // eventbox sroll event (1=down, 0=up)
  int r;
  if (scroll->direction)
    r = alsa_volume_down();
  else
    r = alsa_volume_up();
  update_icon(r);
  printf("eventbox scroll: volume=%d\n",r);
}

int main (int argc, char *argv[]) {
  // initialize gtk application
  gtk_init (&argc, &argv);

  // glade builder
  builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, "trayvolume.glade", NULL);
  gtk_builder_add_from_file (builder, PREFIX"/share/jwmtools/trayvolume.glade", NULL);

  // main window
  winTray1 = GTK_WIDGET (gtk_builder_get_object (builder, "winTray1"));
  gtk_builder_connect_signals (builder, NULL);          
  
  // tray components
  imgTray1 = (GtkImage*)gtk_builder_get_object(builder, "imgTray1");
  //gtk_image_set_from_icon_name(imgClouds1,"gtk-dialog-error",64);

  // initial volume settings
  update_icon(alsa_get_volume());

  // main loop
  //g_object_unref (G_OBJECT (builder));
  gtk_widget_show (winTray1);       
  gtk_main ();
  
  return 0;
}
