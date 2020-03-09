/* gy-app-cmd-line.c
 *
 * Copyright 2020 Jakub Czartek <kuba@linux.pl>
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
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "config.h"
#include <glib/gi18n-lib.h>
#include "gy-app.h"
#include "gy-app-private.h"


static gint
gy_app_handle_local_options (GApplication *application,
                             GVariantDict *options,
                             gpointer      user_data)
{
  if (g_variant_dict_contains (options, "version"))
    {
      g_print("%s\n",PACKAGE_STRING);
      return EXIT_SUCCESS;
    }
  else if (g_variant_dict_contains (options, "preferences"))
    {
      g_action_group_activate_action (G_ACTION_GROUP (application), "prefs", NULL);
      return EXIT_SUCCESS;
    }

  return -1;
}

void
_gy_app_cmd_line_add_options (GyApp *self)
{
  static const GOptionEntry options[] =
  {
    { "version", 'v', 0, G_OPTION_ARG_NONE, NULL, N_("Display the version program and exit") },
    { "preferences", 0, 0, G_OPTION_ARG_NONE, NULL, N_("Show the application preferences") },
    { NULL, }
  };

  g_application_add_main_option_entries (G_APPLICATION (self), options);

  g_signal_connect (GTK_APPLICATION (self), "handle-local-options",
                    G_CALLBACK (gy_app_handle_local_options), NULL);
}

