/*
 * gy-dialog-pref.c
 * Copyright (C) 2014 Jakub Czartek <kuba@linux.pl>
 *
 * gy-dialog-pref.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gy-dialog-pref.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gy-dialog-pref.h"
#include "gy-settings.h"

typedef struct _GyDialogPrefPrivate GyDialogPrefPrivate;

struct _GyDialogPrefPrivate
{
  GtkWidget *dict_table;
  GtkWidget *fonts_table;
  GtkWidget *default_paths_button;
  GtkWidget *system_fonts_button;
  GtkWidget *button_angpl;
  GtkWidget *button_plang;
  GtkWidget *button_depl_a;
  GtkWidget *button_depl_b;
  GtkWidget *text_font_button;
  GtkWidget *tree_font_button;

  GySettings *settings;
};

GtkWidget *prefs_dialog = NULL;

G_DEFINE_TYPE_WITH_PRIVATE (GyDialogPref, gy_dialog_pref, GTK_TYPE_DIALOG);

static gchar *
fixed_path(const gchar *path)
{
  gchar *path_fixed = NULL;

  if (path)
    {
      /* Delete "file://" */
      path_fixed = g_strdup (g_strstr_len (path, -1, "/") + 2);
      return path_fixed;
    }
  return NULL;
}

static void
tooltip_set (GtkWidget   *button,
             GySettings  *settings,
             const gchar *key)
{
  gchar *path;
  GSettings *settings_path = gy_settings_get_paths_settings (settings);

  path = g_settings_get_string (settings_path, key);
  gtk_widget_set_tooltip_text (button, path);
  g_free (path);
}

static void
file_set (GtkFileChooserButton *button,
          gpointer              data)
{
  const gchar *key = (const gchar *) data;
  gchar *path_fixed = NULL, *path = NULL;
  GySettings *settings = gy_settings_get();

  path = gtk_file_chooser_get_uri (GTK_FILE_CHOOSER (button));
  path_fixed = fixed_path (path);

  g_signal_emit_by_name (settings, "paths_changed",
                         FALSE, key, path_fixed);
  g_free (path);
  g_free (path_fixed);
  g_object_unref (settings);
}

static void
set_default_paths_file (GtkButton *button,
                        gpointer   data)
{
  GySettings *settings = GY_SETTINGS (data);
  
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)))
    {
      g_signal_emit_by_name (settings, "paths_changed", TRUE, NULL, NULL);
    }
}

static void
gy_dialog_pref_response (GtkDialog *dialog,
                         gint       response_id)
{
  (void) response_id;
  gtk_widget_destroy (GTK_WIDGET (dialog));
}

static void
gy_dialog_pref_init (GyDialogPref *self)
{
  GSettings *fonts = NULL, *paths = NULL;
  GyDialogPrefPrivate *priv = gy_dialog_pref_get_instance_private (self);

  priv->settings = gy_settings_get ();
  gtk_widget_init_template (GTK_WIDGET (self));
  /* Setup GSettings bindings */
  fonts = gy_settings_get_fonts_settings (priv->settings);
  paths = gy_settings_get_paths_settings (priv->settings);

  g_settings_bind (fonts, GY_SETTINGS_KEY_USE_FONTS, priv->system_fonts_button,
                   "active", G_SETTINGS_BIND_DEFAULT);
  g_settings_bind (fonts, GY_SETTINGS_KEY_USE_FONTS, priv->fonts_table,
                   "sensitive", G_SETTINGS_BIND_DEFAULT | G_SETTINGS_BIND_INVERT_BOOLEAN);
  g_settings_bind (paths, GY_SETTINGS_KEY_USE_PATH, priv->default_paths_button,
                   "active", G_SETTINGS_BIND_DEFAULT);
  g_settings_bind (paths, GY_SETTINGS_KEY_USE_PATH, priv->dict_table,
                   "sensitive", G_SETTINGS_BIND_DEFAULT | G_SETTINGS_BIND_INVERT_BOOLEAN);
  g_settings_bind (fonts, "font-text", priv->text_font_button,
                   "font-name", G_SETTINGS_BIND_DEFAULT);
  g_settings_bind (fonts, "font-list", priv->tree_font_button,
                   "font-name", G_SETTINGS_BIND_DEFAULT);
  /* Set hints */
  tooltip_set (priv->button_angpl, priv->settings, GY_SETTINGS_KEY_PATH_PWN_ANG);
  tooltip_set (priv->button_plang, priv->settings, GY_SETTINGS_KEY_PATH_PWN_PL);
  tooltip_set (priv->button_depl_a, priv->settings, GY_SETTINGS_KEY_PATH_DEPL_A);
  tooltip_set (priv->button_depl_b, priv->settings, GY_SETTINGS_KEY_PATH_DEPL_B);

  g_signal_connect (GTK_FILE_CHOOSER_BUTTON (priv->button_angpl),
                    "file-set", G_CALLBACK(file_set), GY_SETTINGS_KEY_PATH_PWN_ANG);
  g_signal_connect (GTK_FILE_CHOOSER_BUTTON (priv->button_plang),
                    "file-set", G_CALLBACK(file_set), GY_SETTINGS_KEY_PATH_PWN_PL);
  g_signal_connect (GTK_FILE_CHOOSER_BUTTON (priv->button_depl_a),
                    "file-set", G_CALLBACK(file_set), GY_SETTINGS_KEY_PATH_DEPL_A);
  g_signal_connect (GTK_FILE_CHOOSER_BUTTON (priv->button_depl_b),
                    "file-set", G_CALLBACK(file_set), GY_SETTINGS_KEY_PATH_DEPL_B);
  g_signal_connect (GTK_BUTTON (priv->default_paths_button), "clicked",
                    G_CALLBACK (set_default_paths_file), priv->settings);
}

static void
dispose (GObject *object)
{
  GyDialogPrefPrivate *priv = gy_dialog_pref_get_instance_private (GY_DIALOG_PREF (object));

  g_clear_object (&priv->settings);
  G_OBJECT_CLASS (gy_dialog_pref_parent_class)->dispose (object);
}

static void
finalize (GObject *object)
{
  G_OBJECT_CLASS (gy_dialog_pref_parent_class)->finalize (object);
}

static void
gy_dialog_pref_class_init (GyDialogPrefClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkDialogClass *dialog_class = GTK_DIALOG_CLASS (klass);

  object_class->finalize = finalize;
  object_class->dispose = dispose;
  dialog_class->response = gy_dialog_pref_response;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/gydict/dialog-pref.ui");
  gtk_widget_class_bind_template_child_internal_private (widget_class, GyDialogPref, dict_table);
  gtk_widget_class_bind_template_child_internal_private (widget_class, GyDialogPref, fonts_table);
  gtk_widget_class_bind_template_child_internal_private (widget_class, GyDialogPref, default_paths_button);
  gtk_widget_class_bind_template_child_internal_private (widget_class, GyDialogPref, system_fonts_button);
  gtk_widget_class_bind_template_child_internal_private (widget_class, GyDialogPref, button_angpl);
  gtk_widget_class_bind_template_child_internal_private (widget_class, GyDialogPref, button_plang);
  gtk_widget_class_bind_template_child_internal_private (widget_class, GyDialogPref, button_depl_a);
  gtk_widget_class_bind_template_child_internal_private (widget_class, GyDialogPref, button_depl_b);
  gtk_widget_class_bind_template_child_internal_private (widget_class, GyDialogPref, text_font_button);
  gtk_widget_class_bind_template_child_internal_private (widget_class, GyDialogPref, tree_font_button);
}

/****************************METHOD PUBLIC************************************/
static GtkWidget *
gy_dialog_pref_new (void)
{
  return GTK_WIDGET (g_object_new (GY_TYPE_DIALOG_PREF, NULL));
}

void
gy_dialog_pref_show_dialog (GtkWindow *parent)
{
  g_return_if_fail (GTK_IS_WINDOW (parent));

  if (prefs_dialog == NULL)
    {
      prefs_dialog = gy_dialog_pref_new ();
      g_signal_connect (prefs_dialog, "destroy",
                        G_CALLBACK (gtk_widget_destroyed), &prefs_dialog);
    }

  if (parent != gtk_window_get_transient_for (GTK_WINDOW (prefs_dialog)))
    gtk_window_set_transient_for (GTK_WINDOW (prefs_dialog), parent);

  gtk_window_present (GTK_WINDOW (prefs_dialog));
}
