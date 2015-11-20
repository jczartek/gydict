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
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "gy-app.h"
#include "gy-dialog-pref.h"
#include "gy-window.h"
#include "gy-preferences-window.h"

typedef struct _GyAppPrivate GyAppPrivate;

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

static void dispose (GObject *object);

struct _GyAppPrivate
{
  GyPreferencesWindow *preferences_window;
};

G_DEFINE_TYPE_WITH_PRIVATE (GyApp, gy_app, GTK_TYPE_APPLICATION);

/**STATIC DATA**/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
static GActionEntry app_entries[] =
{
  /* general action */
  { "new-window", new_window_cb, NULL, NULL, NULL},
  { "preferences", preferences_cb, NULL, NULL, NULL },
  { "about", about_cb, NULL, NULL, NULL},
  { "quit", quit_cb, NULL, NULL, NULL },
};
#pragma GCC diagnostic pop

/**FUNCTION STATIC**/
static GtkWindow *
gy_app_peek_first_window (GyApp *app)
{
  GList *l;

  g_return_val_if_fail (GY_IS_APP (app), NULL);

  for (l = gtk_application_get_windows (GTK_APPLICATION (app)); 
       l; 
       l = g_list_next (l)) 
  {
    if (GY_IS_WINDOW (l->data)) 
      return GTK_WINDOW (l->data);
  }

  /* Create a new window */
  g_action_group_activate_action (G_ACTION_GROUP (app), 
                                  "new-window", NULL);

  /* And look for the newly created window again */
  return gy_app_peek_first_window (app);
}

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
preferences_cb (GSimpleAction *action G_GNUC_UNUSED,
                GVariant      *parametr G_GNUC_UNUSED,
                gpointer       data)
{
  GyApp *app = GY_APP(data);
  GtkWindow *window;

  window = gy_app_peek_first_window (app);
  gy_dialog_pref_show_dialog (window);

  /*GyApp *self = GY_APP (data);
  GyAppPrivate *priv = gy_app_get_instance_private (self);

  if (priv->preferences_window == NULL)
    {
      GyPreferencesWindow *prefs_window;

      prefs_window = g_object_new (GY_TYPE_PREFERENCES_WINDOW,
                                   "type-hint", GDK_WINDOW_TYPE_HINT_DIALOG,
                                   "window-position", GTK_WIN_POS_CENTER, NULL);

      priv->preferences_window = prefs_window;
      g_object_add_weak_pointer (G_OBJECT (prefs_window),
                                 (gpointer *)&priv->preferences_window);
    }

  gtk_window_present (GTK_WINDOW (priv->preferences_window));*/
}

static void
about_cb (GSimpleAction *action G_GNUC_UNUSED,
          GVariant      *parameter G_GNUC_UNUSED,
          gpointer       user_data G_GNUC_UNUSED)
{
  const gchar  *authors[] = 
  {
    "Jakub Czartek <kuba@linux.pl>",
    "Piotr Czartek <kuba@linux.pl>",
    NULL
  };

  const gchar **documenters = NULL;
  const gchar  *translator_credits = _("translator_credits");

  gtk_show_about_dialog (NULL,
                         "name", "Gydict",
                         "version", PACKAGE_VERSION,
                         "comments", _("This program supports the various dictionaries. "
                                       "Currently, the program provides support to the dictionaries: "
                                       "PWN (2006, 2007) and DEPL (www.depl.pl)"),
                         "license-type", GTK_LICENSE_GPL_2_0,
                         "authors", authors,
                         "documenters", documenters,
                         "translator-credits",
                         (strcmp (translator_credits, "translator_credits") != 0 ? translator_credits : NULL),
                         "logo-icon-name", PACKAGE_TARNAME,
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
      {"win.copy", "<ctrl>c"},
      {"win.paste", "<ctrl>v"},
      {"win.clip", "<ctrl>m"},
      {"win.close", "<ctrl>w"},
      {"win.gear-menu", "F10"},
      {"win.find-menu", "<ctrl>f"},
      {"win.dict(\"dict-pwn-angpol\")", "<ALT>1"},
      {"win.dict(\"dict-pwn-polang\")", "<ALT>2"},
      {"win.dict(\"dict-depl-a\")", "<ALT>3"},
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
setup_menu_app (GyApp *application)
{
  GtkBuilder *builder;
  GMenuModel *model;
  GError *error = NULL;

  builder = gtk_builder_new ();

  if (!gtk_builder_add_from_resource (builder, "/org/gtk/gydict/menu.ui", &error)) 
  {
    g_error ("%s",error ? error->message : "unknown error");
    g_clear_error (&error);
  }

  model = G_MENU_MODEL (gtk_builder_get_object (builder, "app-menu"));
  gtk_application_set_app_menu (GTK_APPLICATION (application), model);

  g_object_unref (builder);
}

static void
startup (GApplication *application)
{
  GyApp *app = GY_APP (application);

  /* Chain up parent's class */
  G_APPLICATION_CLASS (gy_app_parent_class)->startup (application);

  /* Setup actions */
  setup_actions_app (app);

  /* Setup menu application */
  setup_menu_app (app);

  /* Setup accelerators */
  setup_accels (app);
}

static void
gy_app_init (GyApp *application G_GNUC_UNUSED)
{
  g_set_application_name ("Gydict");
}

static void
gy_app_class_init (GyAppClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GApplicationClass *app_class = G_APPLICATION_CLASS (klass); 

  object_class->dispose = dispose;
  app_class->startup = startup;
}

static void
dispose (GObject *object G_GNUC_UNUSED)
{
  G_OBJECT_CLASS (gy_app_parent_class)->dispose;
}

/**PUBLIC METHOD**/
GyApp *
gy_app_new(void)
{
  GyApp * application;

  application = g_object_new (GY_TYPE_APP,
                              "application-id",   "org.gtk.Gydict",
                              "flags",            G_APPLICATION_FLAGS_NONE,
                              "register-session", TRUE, NULL);

  return application;
}

void
gy_app_new_window (GyApp *application)
{
  g_action_group_activate_action (G_ACTION_GROUP (application), 
                                  "new-window",NULL);
}
