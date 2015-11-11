/*
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include "config.h"
#include <gtk/gtk.h>
#include <locale.h>
#include <glib/gi18n.h>
#include <stdlib.h>

#include "gy-app.h"

static gboolean option_version;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
static GOptionEntry options[] =
{
  { "version", 'v',
    0, G_OPTION_ARG_NONE, &option_version,
    N_("Display the version program and exit"),
    NULL
  },
  { NULL, }
};
#pragma GCC diagnostic pop

static void
activate_cb (GApplication *application,
             gpointer 	  *data G_GNUC_UNUSED)
{
  /* Create first instance */
  gy_app_new_window (GY_APP (application));
}

int
main (int argc, char **argv)
{
  GyApp *application;
  GError *error = NULL;
  gint status;

  /* GETTEXT */
  setlocale (LC_ALL, "");
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCAL_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  if (!gtk_init_with_args (&argc, &argv, NULL, options, GETTEXT_PACKAGE, &error))
  {
    g_printerr ("%s\n", error->message);
    return EXIT_FAILURE;
  }

  if (option_version)
  {
    g_print("%s\n",PACKAGE_STRING);
    return EXIT_SUCCESS;
  }

  /* Create new GyApp */
  application = gy_app_new ();
  g_signal_connect (application, "activate", G_CALLBACK (activate_cb), NULL);

  /* Set it as the default application */
  g_application_set_default (G_APPLICATION (application));

  /* Try to register the application... */
  if (!g_application_register (G_APPLICATION (application), NULL, &error)) 
  {
    g_printerr ("Couldn't register Gydict instance: '%s'\n",
                error ? error->message : "");
    g_object_unref (application);
    return EXIT_FAILURE;
  }

  /* And run the GtkApplication */
  status = g_application_run (G_APPLICATION (application), argc, argv);

  g_object_unref (application);

  return status;
}
