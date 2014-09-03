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

#include "gy-settings.h"

#define SETTINGS_ID_DICTIONARY 	"org.gtk.gydict.path"
#define SETTINGS_ID_FONTS 	"org.gtk.gydict.fonts"
#define DEFAULT_FONT		"Normal 11"
#define DEFAULT_PATH(dict)	"/opt/gydict/data/" # dict

#define GET_PRIVATE(instance) G_TYPE_INSTANCE_GET_PRIVATE \
        (instance, GY_TYPE_SETTINGS, GySettingsPrivate)

G_DEFINE_TYPE (GySettings, gy_settings, G_TYPE_OBJECT);

static GySettings *singleton = NULL;

static void dispose (GObject *object);
static void finalize (GObject *object);
static void fonts_changed_cb (GSettings *settings G_GNUC_UNUSED, 
			      gchar     *key G_GNUC_UNUSED, 
			      gpointer   user_data);

struct 
{
  const gchar *path;
  const gchar *key; 
} default_paths[] = 
{
  {DEFAULT_PATH(angpol.win), GY_SETTINGS_KEY_PATH_PWN_ANG},
  {DEFAULT_PATH(polang.win), GY_SETTINGS_KEY_PATH_PWN_PL},
  {DEFAULT_PATH(a.data), GY_SETTINGS_KEY_PATH_DEPL_A},
  {DEFAULT_PATH(b.data), GY_SETTINGS_KEY_PATH_DEPL_B}
};

struct _GySettingsPrivate
{
    GSettings *settings_dictionary;
    GSettings *settings_fonts;
};

enum 
{
    FONTS_CHANGED,
    PATHS_CHANGED,
    LAST_SIGNAL
};

static gint signals[LAST_SIGNAL] = { 0 };

static void
gy_settings_init (GySettings *self)
{
    self->priv = GET_PRIVATE (self);
    self->priv->settings_dictionary = g_settings_new (SETTINGS_ID_DICTIONARY);
    self->priv->settings_fonts = g_settings_new (SETTINGS_ID_FONTS);

    /* setup GSettings notifications */
    g_signal_connect (self->priv->settings_fonts,
		      "changed", G_CALLBACK (fonts_changed_cb), self);
}

void paths_changed (GySettings  *settings,
		    gboolean     use_default,
		    const gchar *key_path,
		    const gchar *value_path)
{
  if (use_default)
  {
    guint i;
    for (i=0; i<G_N_ELEMENTS (default_paths); i++)
    {
#if 0
      g_message ("KEY: %s :: PATH: %s", default_paths[i].key, default_paths[i].path);
#endif
      gy_settings_set_path_dictionary (settings,
				       default_paths[i].key,
				       default_paths[i].path);
    }
  }
  else
  {
    gy_settings_set_path_dictionary (settings,
				     key_path,
				     value_path);
  }

  g_signal_stop_emission_by_name (settings, "paths_changed");
}

static void
gy_settings_class_init (GySettingsClass *klass)
{
    GObjectClass *object_klass = G_OBJECT_CLASS (klass);

    g_type_class_add_private (object_klass, sizeof (GySettingsPrivate));
    klass->paths_changed = paths_changed;
    object_klass->dispose = dispose;
    object_klass->finalize = finalize;

    signals[FONTS_CHANGED] =
		g_signal_new ("fonts-changed",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (GySettingsClass, fonts_changed),
			      NULL, NULL,
			      g_cclosure_marshal_generic,
			      G_TYPE_NONE,
			      2,
			      G_TYPE_STRING,
			      G_TYPE_STRING);

    signals[PATHS_CHANGED] =
		g_signal_new ("paths-changed",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_FIRST,
			      G_STRUCT_OFFSET (GySettingsClass, paths_changed),
			      NULL, NULL,
			      g_cclosure_marshal_generic,
			      G_TYPE_NONE,
			      3,
			      G_TYPE_BOOLEAN,
			      G_TYPE_STRING,
			      G_TYPE_STRING);
}

static void
dispose (GObject *object)
{
    GySettings *self = GY_SETTINGS (object);

    g_clear_object (&self->priv->settings_dictionary);
    g_clear_object (&self->priv->settings_fonts);

    G_OBJECT_CLASS (gy_settings_parent_class)->dispose (object);
}

static void
finalize (GObject *object)
{
    singleton = NULL;
    G_OBJECT_CLASS (gy_settings_parent_class)->finalize (object);
}

static void
get_selected_fonts (GySettings *settings,
		    gchar      **text_font,
		    gchar      **list_font)
{
    g_return_if_fail (text_font != NULL && *text_font == NULL);
    g_return_if_fail (list_font != NULL && *list_font == NULL);
    
    gboolean use_system_font = g_settings_get_boolean (settings->priv->settings_fonts,
						       GY_SETTINGS_KEY_USE_FONTS);
    if (use_system_font)
    {
	*text_font = g_strdup (DEFAULT_FONT);
	*list_font = g_strdup (DEFAULT_FONT);
    }
    else
    {
	*text_font = g_settings_get_string (settings->priv->settings_fonts,
					    GY_SETTINGS_KEY_FONT_TEXT);
	*list_font = g_settings_get_string (settings->priv->settings_fonts,
					    GY_SETTINGS_KEY_FONT_TREE);
    }
}

static void 
fonts_changed_cb (GSettings *settings G_GNUC_UNUSED, 
		  gchar     *key G_GNUC_UNUSED, 
  		  gpointer   user_data)
{
    GySettings *self = GY_SETTINGS (user_data);
    gchar *text_font = NULL, *list_font = NULL;

    get_selected_fonts (self, &text_font, &list_font);
    g_signal_emit (self, signals[FONTS_CHANGED], 0, text_font, list_font);

#if 0
    g_message ("text_view = %s\t tree_font = %s.", text_font, list_font);
#endif
    g_free (text_font);
    g_free (list_font);
}

/* Public Method */
GySettings *
gy_settings_get (void)
{
    if (!singleton)
    {
	singleton = GY_SETTINGS (g_object_new (GY_TYPE_SETTINGS, NULL));
    }
    else
    {
	g_object_ref (singleton);
    }

    g_assert (singleton);
    return singleton;
}

gchar *
gy_settings_get_font_text (GySettings *settings)
{
    return g_settings_get_string (settings->priv->settings_fonts,
				  GY_SETTINGS_KEY_FONT_TEXT);
}

gchar *
gy_settings_get_font_tree (GySettings *settings)
{
    return g_settings_get_string (settings->priv->settings_fonts,
				  GY_SETTINGS_KEY_FONT_TREE);
}

gchar * 
gy_settings_get_path_dictionary (GySettings  *settings,
				 const gchar *key)
{
    return g_settings_get_string (settings->priv->settings_dictionary,
				  key);
}

void 
gy_settings_set_path_dictionary (GySettings  *settings,
				 const gchar *key,
				 const gchar *value)
{
    if (!g_settings_set_string (settings->priv->settings_dictionary, key, value))
    {
	g_warning ("Cannot set path to dictionary: %s", value);
    }
}

gboolean
gy_settings_get_use_fonts_system (GySettings *settings) 
{
    return g_settings_get_boolean (settings->priv->settings_fonts, 
				   GY_SETTINGS_KEY_USE_FONTS);
}

void
gy_settings_set_use_fonts_system (GySettings  *settings,
				  const gchar *key,
				  gboolean     value)
{
    if (!g_settings_set_boolean (settings->priv->settings_fonts, key, value))
    {
	g_warning ("Cannot set use system fonts");
    }
}

gboolean
gy_settings_get_use_paths_system (GySettings  *settings,
				  const gchar *key)
{
    return g_settings_get_boolean (settings->priv->settings_dictionary, 
				   key);
}

void
gy_settings_set_use_paths_system (GySettings  *settings,
				  const gchar *key,
				  gboolean     value)
{
    if (!g_settings_set_boolean (settings->priv->settings_dictionary, key, value))
    {
	g_warning ("Cannot set use system paths");
    }
}

GSettings *
gy_settings_get_fonts_settings (GySettings *settings)
{
    return settings->priv->settings_fonts;
}

GSettings *
gy_settings_get_paths_settings (GySettings *settings)
{
    return settings->priv->settings_dictionary;
}
