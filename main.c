/*
 * The main file of the pellet burner control program
 */

#include "lc1.h"
#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <stdint.h>

#include <glib.h>

#define GETTEXT_PACKAGE "myapp"

static gchar *device = "/dev/ttyUSB0";

static GOptionEntry entries[] =
{
  { "device", 'd', 0, G_OPTION_ARG_STRING, device, "Device name", "N" },
  { NULL }
};

int main (int argc, char *argv[])
{
    char *portname = "/dev/ttyUSB0";
    char chr;

    GError *error = NULL;
    GOptionContext *context;

    setlocale (LC_ALL, "");
    bindtextdomain ("hello", "/usr/share/locale");
    textdomain ("hello");

    context = g_option_context_new ("- Pellet burner control system");
    g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);
//    g_option_context_add_group (context, gtk_get_option_group (TRUE));
    if (!g_option_context_parse (context, &argc, &argv, &error))
    {
        g_print ("option parsing failed: %s\n", error->message);
        exit (1);
    }

    int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
            printf ("error %d opening %s: %s", errno, portname, strerror (errno));
            return 0;
    }

    set_interface_attribs (fd, B19200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
    set_blocking (fd, 0);                   // set no blocking

    while (1)
    {
        if (read (fd, &chr, 1) == 1) {
            printf ("Value: 0x%02x", chr);
            if ((chr & 0xa2) == 0xa2) printf (" Sync");
            printf ("\n");
        }
    }
}
