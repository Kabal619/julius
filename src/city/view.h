#ifndef CITY_VIEW_H
#define CITY_VIEW_H

#include "core/buffer.h"

// TODO get rid of these
#define VIEW_X_MAX 165
#define VIEW_Y_MAX 325

typedef void (map_callback)(int x, int y, int grid_offset);

void city_view_init(void);

int city_view_orientation(void);

void city_view_reset_orientation(void);

void city_view_get_camera(int *x, int *y);

void city_view_set_camera(int x, int y);

int city_view_scroll(int direction);

int city_view_to_grid_offset(int x_view, int y_view);

void city_view_grid_offset_to_xy_view(int grid_offset, int *x_view, int *y_view);

void city_view_get_selected_tile_pixels(int *x_pixels, int *y_pixels);

int city_view_pixels_to_grid_offset(int x_pixels, int y_pixels);

void city_view_go_to_grid_offset(int grid_offset);

void city_view_rotate_left(void);

void city_view_rotate_right(void);

void city_view_set_viewport(int screen_width, int screen_height);

void city_view_get_viewport(int *x, int *y, int *width, int *height);
void city_view_get_viewport_size_tiles(int *width, int *height);

int city_view_is_sidebar_collapsed(void);

void city_view_start_sidebar_toggle(void);

void city_view_toggle_sidebar(void);

void city_view_save_state(buffer *orientation, buffer *camera);

void city_view_load_state(buffer *orientation, buffer *camera);

void city_view_load_scenario_state(buffer *camera);

void city_view_foreach_map_tile(map_callback *callback);

void city_view_foreach_valid_map_tile(map_callback *callback1, map_callback *callback2, map_callback *callback3);

void city_view_foreach_minimap_tile(int x_offset, int y_offset, int absolute_x, int absolute_y, int width_tiles, int height_tiles, map_callback *callback);

#endif // CITY_VIEW_H
