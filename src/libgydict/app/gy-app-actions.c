/* gy-app-actions.c
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
#include "gui/gy-window.h"
#include "preferences/gy-prefs-window.h"

static void
gy_app_actions_new_window_cb (GSimpleAction *action   G_GNUC_UNUSED,
                              GVariant      *parametr G_GNUC_UNUSED,
                              gpointer                data)
{
  GyApp *app = GY_APP (data);
  GtkWidget *window;

  window = gy_window_new (app);
  gtk_application_add_window (GTK_APPLICATION (app), GTK_WINDOW (window));
  gtk_widget_show_all (window);
}

static void
gy_app_actions_preferences_cb (GSimpleAction *action   G_GNUC_UNUSED,
                               GVariant      *parametr G_GNUC_UNUSED,
                               gpointer                data)
{
  GyApp *self         = GY_APP (data);
  GtkWindow *toplevel = NULL;
  GtkWindow *window   = NULL;
  GList *windows      = NULL;


  windows = gtk_application_get_windows (GTK_APPLICATION (self));
  for (; windows != NULL; windows = windows->next)
    {
      GtkWindow *win = windows->data;

      if (GY_IS_PREFS_WINDOW (win))
        {
          gtk_window_present (win);
          return;
        }

      if (toplevel == NULL && GY_IS_WINDOW (win))
        toplevel = win;
    }


  window = g_object_new (GY_TYPE_PREFS_WINDOW, "transient-for", toplevel, NULL);
  gtk_application_add_window (GTK_APPLICATION (self), window);
  gtk_window_present (window);
}

static void
gy_app_actions_about_cb (GSimpleAction *action    G_GNUC_UNUSED,
                         GVariant      *parameter G_GNUC_UNUSED,
                         gpointer                 user_data)
{
  GyApp     *self = GY_APP (user_data);
  GList     *windows, *iter;
  GtkWindow *parent = NULL;
  const gchar  *authors[] =
  {
    "Jakub Czartek <kuba@linux.pl>",
    "Piotr Czartek",
    NULL
  };
  const gchar **documenters = NULL;
  const gchar  *translator_credits = _("translator_credits");

  g_return_if_fail (GY_IS_APP (self));

  windows = gtk_application_get_windows (GTK_APPLICATION (self));

  for (iter = windows; iter ;iter = iter->next)
    {
      if (GY_IS_WINDOW (iter->data))
        {
          parent = GTK_WINDOW (iter->data);
          break;
        }
    }

  gtk_show_about_dialog (parent,
                         "name", "Gydict",
                         "version", PACKAGE_VERSION,
                         "comments", _("Look up words in different (commercial and free) multimedia dictionaries"),
                         "license-type", GTK_LICENSE_GPL_2_0,
                         "authors", authors,
                         "documenters", documenters,
                         "translator-credits",
                         (strcmp (translator_credits, "translator_credits") != 0 ? translator_credits : NULL),
                         "logo-icon-name", PACKAGE_NAME,
                         NULL);
}

static void
gy_app_actions_quit_cb (GSimpleAction *action   G_GNUC_UNUSED,
                        GVariant      *parametr G_GNUC_UNUSED,
                        gpointer                data)
{
  GyApp *app = GY_APP(data);
  GList * windows;

  /* Remove all windows registered application */
  while ((windows = gtk_application_get_windows (GTK_APPLICATION (app))))
  {
    gtk_application_remove_window (GTK_APPLICATION (app),
                                   GTK_WINDOW (windows->data));
  }
}


static GActionEntry app_entries[] =
{
  /* general action */
  { "new-window", gy_app_actions_new_window_cb, NULL, NULL, NULL},
  { "prefs", gy_app_actions_preferences_cb, NULL, NULL, NULL },
  { "about", gy_app_actions_about_cb, NULL, NULL, NULL},
  { "quit", gy_app_actions_quit_cb, NULL, NULL, NULL },
};

void
_gy_app_action_init (GyApp *self)
{
  g_action_map_add_action_entries (G_ACTION_MAP (self), app_entries,
                                   G_N_ELEMENTS (app_entries), self);
}
