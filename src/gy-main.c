/* gy-main.c
 *
 * Copyright (C) 2017 Jakub Czartek <kuba@linux.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <gtk/gtk.h>
#include <locale.h>
#include <glib/gi18n.h>
#include <stdlib.h>

#include <gydict.h>

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

  /* Create new GyApp */
  application = gy_app_new ();

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
