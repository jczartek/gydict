/* gy-app.c
 *
 * Copyright (C) 2014 Jakub Czartek <kuba@linux.pl>
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

#include "config.h"
#include <glib/gi18n-lib.h>
#include <egg-menu-manager.h>
#include "gy-app.h"
#include "window/gy-window.h"
#include "css/gy-css-provider.h"
#include "preferences/gy-preferences-window.h"
#include "dictionaries/gy-dict.h"
#include "shortcuts/gy-shortcuts-window.h"
#include "resources/gy-resources.h"


/**STATIC PROTOTYPES FUNCTIONS**/
static void new_window_cb (GSimpleAction *action,
                           GVariant      *parametr,
                           gpointer       data);
static void quit_cb (GSimpleAction *action,
                     GVariant      *parametr,
                     gpointer       data);
static void preferences_cb (GSimpleAction *action,
                            GVariant      *parametr,
                            gpointer       data);
static void about_cb (GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer       user_data);
static void shortcuts_cb (GSimpleAction *action,
                          GVariant      *variant,
                          gpointer       data);

struct _GyApp
{
  GtkApplication       __parent__;
  GyPreferencesWindow   *preferences_window;
  EggMenuManager        *menu_manager;
};

G_DEFINE_TYPE (GyApp, gy_app, GTK_TYPE_APPLICATION);

static GActionEntry app_entries[] =
{
  /* general action */
  { "new-window", new_window_cb, NULL, NULL, NULL},
  { "preferences", preferences_cb, NULL, NULL, NULL },
  { "about", about_cb, NULL, NULL, NULL},
  { "quit", quit_cb, NULL, NULL, NULL },
  { "shortcuts", shortcuts_cb, NULL, NULL, NULL },
};

inline static void
setup_actions_app (GyApp *application)
{
  g_action_map_add_action_entries (G_ACTION_MAP (application),
                                   app_entries,
                                   G_N_ELEMENTS (app_entries),
                                   application);
}

static void
new_window_cb (GSimpleAction *action G_GNUC_UNUSED,
               GVariant      *parametr G_GNUC_UNUSED,
               gpointer       data)
{
  GyApp *app = GY_APP (data);
  GtkWidget *window;

  window = gy_window_new (app);
  gtk_application_add_window (GTK_APPLICATION (app), GTK_WINDOW (window));
  gtk_widget_show_all (window);
}

static void
shortcuts_cb (GSimpleAction *action,
              GVariant      *variant,
              gpointer       data)
{
  GyApp *self = GY_APP (data);
  GtkWindow *window, *parent = NULL;
  GList *list;

  g_assert (GY_IS_APP (self));

  list = gtk_application_get_windows (GTK_APPLICATION (self));

  for (; list; list = list->next)
    {
      window = list->data;

      if (GY_IS_SHORTCUTS_WINDOW (window))
        {
          gtk_window_present (window);
          return;
        }

      if (GY_IS_WINDOW (window))
        parent = window;
    }

  window = g_object_new (GY_TYPE_SHORTCUTS_WINDOW,
                         "application", self,
                         "window-position", GTK_WIN_POS_CENTER,
                         "transient-for", parent, NULL);

  gtk_window_present (GTK_WINDOW (window));
}

static void
preferences_cb (GSimpleAction *action G_GNUC_UNUSED,
                GVariant      *parametr G_GNUC_UNUSED,
                gpointer       data)
{
  GyApp *self = GY_APP (data);

  if (self->preferences_window == NULL)
    {
      GyPreferencesWindow *prefs_window;

      prefs_window = g_object_new (GY_TYPE_PREFERENCES_WINDOW,
                                   "type-hint", GDK_WINDOW_TYPE_HINT_DIALOG,
                                   "window-position", GTK_WIN_POS_CENTER, NULL);

      self->preferences_window = prefs_window;
      g_object_add_weak_pointer (G_OBJECT (prefs_window),
                                 (gpointer *)&self->preferences_window);
    }

  gtk_window_present (GTK_WINDOW (self->preferences_window));
}

static void
about_cb (GSimpleAction *action G_GNUC_UNUSED,
          GVariant      *parameter G_GNUC_UNUSED,
          gpointer       user_data)
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
quit_cb (GSimpleAction *action G_GNUC_UNUSED,
         GVariant      *parametr G_GNUC_UNUSED,
         gpointer       data)
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

static void
setup_accels (GyApp *self)
{
  static const struct {gchar *action; gchar *accel_key;} accels_key[] = {
      {"app.quit", "<ctrl>q"},
      {"app.new-window", "<ctrl>n"},
      {"win.print", "<ctrl>p"},
      {"win.close", "<ctrl>w"},
      {"win.clip", "<ctrl>m"},
      {"win.gear-menu", "F10"},
      {"dockbin.top-visible", "<ctrl>f"},
      {"dockbin.left-visible", "F9"},
      {"workspace.alter-dict(\"GyEnglishPwn-english\")", "<ALT>1"},
      {"workspace.alter-dict(\"GyEnglishPwn-polish\")", "<ALT>2"},
      {"workspace.alter-dict(\"GyGermanPwn-german\")", "<ALT>3"},
      {"workspace.alter-dict(\"GyGermanPwn-polish\")", "<ALT>4"},
      {"workspace.alter-dict(\"GyDepl-german\")", "<ALT>5"},
      {NULL, NULL}
  };

  for (gint i = 0; accels_key[i].action; i++)
    {
      const gchar *accels[2] = {accels_key[i].accel_key, NULL};
      gtk_application_set_accels_for_action (GTK_APPLICATION (self),
                                             accels_key[i].action,
                                             accels);
    }
}

static void
gy_app_register_theme_overrides (GyApp *self)
{
  g_autoptr(GSettings)  settings = NULL;
  g_autoptr(GtkCssProvider) provider = NULL;
  GtkSettings          *gtk_settings;
  GdkScreen            *screen;

  provider = gy_css_provider_new ();
  screen = gdk_screen_get_default ();

  gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider),
                                             GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  gtk_settings = gtk_settings_get_for_screen (screen);
  settings = g_settings_new ("org.gtk.gydict");
  g_settings_bind (settings,     "night-mode",
                   gtk_settings, "gtk-application-prefer-dark-theme",
                   G_SETTINGS_BIND_DEFAULT);
}

static void
gy_app_register_menus (GyApp *self)
{
  GMenu *app_menu;

  g_assert (GY_IS_APP (self));

  self->menu_manager = egg_menu_manager_new ();
  egg_menu_manager_add_resource (self->menu_manager, "/org/gtk/gydict/gy-menus.ui", NULL);

  app_menu = egg_menu_manager_get_menu_by_id (self->menu_manager, "app-menu");
  gtk_application_set_app_menu (GTK_APPLICATION (self), G_MENU_MODEL (app_menu));
}

static void
startup (GApplication *application)
{
  GyApp *app = GY_APP (application);

  /* Chain up parent's class */
  G_APPLICATION_CLASS (gy_app_parent_class)->startup (application);

  /* Register resources */
  g_resources_register (gy_get_resource ());
  g_application_set_resource_base_path (application, "/org/gtk/gydict");

  /* Setup actions */
  setup_actions_app (app);

  /* Setup menu application */
  gy_app_register_menus (app);

  /* Setup accelerators */
  setup_accels (app);

  /* Setup theme */
  gy_app_register_theme_overrides (app);

  /* Register types */
  gy_dict_initialize ();
}

static void
gy_app_init (GyApp *application G_GNUC_UNUSED)
{
  g_set_application_name ("Gydict");
}

static void
gy_app_class_init (GyAppClass *klass)
{
  GApplicationClass *app_class = G_APPLICATION_CLASS (klass);

  app_class->startup = startup;
}

/**PUBLIC METHOD**/
GyApp *
gy_app_new(void)
{
  GyApp * application;

  application = g_object_new (GY_TYPE_APP,
                              "application-id",   "org.gnome.Gydict",
                              "flags",            G_APPLICATION_FLAGS_NONE,
                              "register-session", TRUE, NULL);

  return application;
}

void
gy_app_new_window (GyApp *self)
{
  g_return_if_fail (GY_IS_APP (self));

  g_action_group_activate_action (G_ACTION_GROUP (self),
                                  "new-window",NULL);
}

/**
 * gy_app_get_menu_by_id:
 * @self: An #GyApp.
 * @id: The id of the menu to lookup.
 *
 * Similar to gtk_application_get_menu_by_id() but takes into account merging
 * the menus provided by, and extended by, plugins.
 *
 * Returns: (transfer none): A #GMenu.
 */
GMenu *
gy_app_get_menu_by_id (GyApp          *self,
                       const gchar    *id)
{
  g_return_val_if_fail (GY_IS_APP (self), NULL);
  g_return_val_if_fail (id != NULL, NULL);

  if (self->menu_manager != NULL)
    return egg_menu_manager_get_menu_by_id (self->menu_manager, id);

  g_critical ("%s() called by non-UI process", G_STRFUNC);

  return NULL;
}
