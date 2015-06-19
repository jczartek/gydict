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

#ifndef __GY_HISTORY_H__
#define __GY_HISTORY_H__

G_BEGIN_DECLS	

/*
 * Type macros
 */
#define GY_TYPE_HISTORY 	   (gy_history_get_type ())
#define GY_HISTORY(obj) 	   (G_TYPE_CHECK_INSTANCE_CAST ((obj), GY_TYPE_HISTORY, GyHistory))
#define GY_IS_HISTORY(obj) 	   (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GY_TYPE_HISTORY))
#define GY_HISTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GY_TYPE_HISTORY,  GyHistoryClass))
#define GY_IS_HISTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GY_TYPE_HISTORY))
#define GY_HISTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GY_TYPE_HISTORY, GyHistoryClass))

typedef struct _GyHistory GyHistory;
typedef struct _GyHistoryClass GyHistoryClass;
typedef struct _GyHistoryPrivate GyHistoryPrivate;

struct _GyHistory
{
  GObject parent_instance;

  /* instance members */
  GyHistoryPrivate *priv;
};

struct _GyHistoryClass
{
  GObjectClass parent_class;

    /* class members */
};

/* used by GY_TYPE_HISTORY */
GType gy_history_get_type (void);

/*
 * Method definitions
 */
GyHistory* gy_history_new (void);
void gy_history_set_start_list (GyHistory *self, 
			        gboolean   start_list);
void gy_history_set_end_list (GyHistory *self, 
			      gboolean   end_list);
gboolean gy_history_get_start_list (GyHistory *self);
gboolean gy_history_get_end_list (GyHistory *self);
void gy_history_add_list (GyHistory   *self,
			  const gchar *string);
const gchar* gy_history_get_string_from_quark (GyHistory *self);
void gy_history_update_current_history (GyHistory *self);
void gy_history_go_back (GyHistory *self);
void gy_history_go_forward (GyHistory *self);

void gy_history_append (GyHistory   *obj,
			const gchar *str);
void gy_history_update (GyHistory   *obj);
G_END_DECLS

#endif /* end of include guard: __GY_HISTORY_H__ */
