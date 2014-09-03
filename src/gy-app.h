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

#ifndef __GY_APP_H__
#define __GY_APP_H__

G_BEGIN_DECLS	

/*
 * Type macros
 */
#define GY_TYPE_APP		(gy_app_get_type ())
#define GY_APP(obj)   	   	(G_TYPE_CHECK_INSTANCE_CAST ((obj), GY_TYPE_APP, GyApp))
#define GY_IS_APP(obj) 	   	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GY_TYPE_APP))
#define GY_APP_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GY_TYPE_APP, GyAppClass))
#define GY_IS_APP_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GY_TYPE_APP))
#define GY_APP_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GY_TYPE_APP, GyAppClass))

typedef struct _GyApp GyApp;
typedef struct _GyAppClass GyAppClass;

struct _GyApp
{
  GtkApplication parent_instance;
};

struct _GyAppClass
{
  GtkApplicationClass parent_class;
};

/* used by GY_TYPE_APP */
GType gy_app_get_type (void);

/*
 * Method definitions
 */
GyApp* gy_app_new (void);
void gy_app_new_window (GyApp *application);

G_END_DECLS

#endif /* end of include guard: __GY_APP_H__ */
