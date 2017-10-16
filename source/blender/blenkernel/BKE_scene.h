/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 */
#ifndef __BKE_SCENE_H__
#define __BKE_SCENE_H__

/** \file BKE_scene.h
 *  \ingroup bke
 *  \since March 2001
 *  \author nzc
 */

#ifdef __cplusplus
extern "C" {
#endif

struct AviCodecData;
struct BaseLegacy;
struct Depsgraph;
struct EvaluationContext;
struct Main;
struct Object;
struct RenderData;
struct SceneLayer;
struct SceneRenderLayer;
struct Scene;
struct SceneCollection;
struct SceneLayer;
struct UnitSettings;
struct ViewRender;
struct WorkSpace;

typedef enum eSceneCopyMethod {
	SCE_COPY_NEW       = 0,
	SCE_COPY_EMPTY     = 1,
	SCE_COPY_LINK_OB   = 2,
	SCE_COPY_LINK_DATA = 3,
	SCE_COPY_FULL      = 4,
} eSceneCopyMethod;

/* Use as the contents of a 'for' loop: for (SETLOOPER(...)) { ... */
#define SETLOOPER(_sce_basis, _sce_iter, _base)                               \
	_sce_iter = _sce_basis, _base = _setlooper_base_step(&_sce_iter, BKE_scene_layer_from_scene_get(_sce_basis), NULL); \
	_base;                                                                    \
	_base = _setlooper_base_step(&_sce_iter, NULL, _base)

#define SETLOOPER_SCENE_LAYER(_sce_basis, _scene_layer, _sce_iter, _base)     \
	_sce_iter = _sce_basis, _base = _setlooper_base_step(&_sce_iter, _scene_layer, NULL);   \
	_base;                                                                    \
	_base = _setlooper_base_step(&_sce_iter, NULL, _base)

#define SETLOOPER_SET_ONLY(_sce_basis, _sce_iter, _base)     \
	_sce_iter = _sce_basis, _base = _setlooper_base_step(&_sce_iter, NULL, NULL);   \
	_base;                                                                    \
	_base = _setlooper_base_step(&_sce_iter, NULL, _base)

struct Base *_setlooper_base_step(struct Scene **sce_iter, struct SceneLayer *scene_layer, struct Base *base);

void free_avicodecdata(struct AviCodecData *acd);

void BKE_scene_free_ex(struct Scene *sce, const bool do_id_user);
void BKE_scene_free(struct Scene *sce);
void BKE_scene_init(struct Scene *sce);
struct Scene *BKE_scene_add(struct Main *bmain, const char *name);

void BKE_scene_remove_rigidbody_object(struct Scene *scene, struct Object *ob);

/* base functions */
struct BaseLegacy *BKE_scene_base_find_by_name(struct Scene *scene, const char *name);
struct BaseLegacy *BKE_scene_base_find(struct Scene *scene, struct Object *ob);
struct BaseLegacy *BKE_scene_base_add(struct Scene *sce, struct Object *ob);
void         BKE_scene_base_unlink(struct Scene *sce, struct BaseLegacy *base);
void         BKE_scene_base_deselect_all(struct Scene *sce);
void         BKE_scene_base_select(struct Scene *sce, struct BaseLegacy *selbase);

/* Scene base iteration function.
 * Define struct here, so no need to bother with alloc/free it.
 */
typedef struct SceneBaseIter {
	struct ListBase *duplilist;
	struct DupliObject *dupob;
	float omat[4][4];
	struct Object *dupli_refob;
	int phase;
} SceneBaseIter;

int BKE_scene_base_iter_next(
        const struct EvaluationContext *eval_ctx, struct SceneBaseIter *iter,
        struct Scene **scene, int val, struct BaseLegacy **base, struct Object **ob);

void BKE_scene_base_flag_to_objects(struct SceneLayer *sl);
void BKE_scene_base_flag_from_objects(struct Scene *scene);
void BKE_scene_base_flag_sync_from_base(struct BaseLegacy *base);
void BKE_scene_base_flag_sync_from_object(struct BaseLegacy *base);
void BKE_scene_object_base_flag_sync_from_base(struct Base *base);
void BKE_scene_object_base_flag_sync_from_object(struct Base *base);

void BKE_scene_set_background(struct Main *bmain, struct Scene *sce);
struct Scene *BKE_scene_set_name(struct Main *bmain, const char *name);

void BKE_scene_copy_data(struct Main *bmain, struct Scene *sce_dst, const struct Scene *sce_src, const int flag);
struct Scene *BKE_scene_copy(struct Main *bmain, struct Scene *sce, int type);
void BKE_scene_groups_relink(struct Scene *sce);

void BKE_scene_make_local(struct Main *bmain, struct Scene *sce, const bool lib_local);

struct Scene *BKE_scene_find_from_collection(const struct Main *bmain, const struct SceneCollection *scene_collection);

struct Object *BKE_scene_camera_find(struct Scene *sc);
#ifdef DURIAN_CAMERA_SWITCH
struct Object *BKE_scene_camera_switch_find(struct Scene *scene); // DURIAN_CAMERA_SWITCH
#endif
int BKE_scene_camera_switch_update(struct Scene *scene);

char *BKE_scene_find_marker_name(struct Scene *scene, int frame);
char *BKE_scene_find_last_marker_name(struct Scene *scene, int frame);

/* checks for cycle, returns 1 if it's all OK */
bool BKE_scene_validate_setscene(struct Main *bmain, struct Scene *sce);

float BKE_scene_frame_get(const struct Scene *scene);
float BKE_scene_frame_get_from_ctime(const struct Scene *scene, const float frame);
void  BKE_scene_frame_set(struct Scene *scene, double cfra);

/* **  Scene evaluation ** */
void BKE_scene_update_tagged(struct EvaluationContext *eval_ctx, struct Main *bmain, struct Scene *sce);
void BKE_scene_update_for_newframe(struct EvaluationContext *eval_ctx, struct Main *bmain, struct Scene *sce);

struct SceneRenderLayer *BKE_scene_add_render_layer(struct Scene *sce, const char *name);
bool BKE_scene_remove_render_layer(struct Main *main, struct Scene *scene, struct SceneRenderLayer *srl);

struct SceneRenderView *BKE_scene_add_render_view(struct Scene *sce, const char *name);
bool BKE_scene_remove_render_view(struct Scene *scene, struct SceneRenderView *srv);

/* render profile */
int get_render_subsurf_level(const struct RenderData *r, int level, bool for_render);
int get_render_child_particle_number(const struct RenderData *r, int num, bool for_render);
int get_render_shadow_samples(const struct RenderData *r, int samples);
float get_render_aosss_error(const struct RenderData *r, float error);

bool BKE_scene_use_new_shading_nodes(const struct Scene *scene);
bool BKE_scene_use_shading_nodes_custom(struct Scene *scene);
bool BKE_scene_use_world_space_shading(struct Scene *scene);
bool BKE_scene_use_spherical_stereo(struct Scene *scene);

bool BKE_scene_uses_blender_internal(const struct Scene *scene);
bool BKE_scene_uses_blender_game(const struct Scene *scene);
bool BKE_scene_uses_blender_eevee(const struct Scene *scene);

void BKE_scene_disable_color_management(struct Scene *scene);
bool BKE_scene_check_color_management_enabled(const struct Scene *scene);
bool BKE_scene_check_rigidbody_active(const struct Scene *scene);

int BKE_scene_num_threads(const struct Scene *scene);
int BKE_render_num_threads(const struct RenderData *r);

int BKE_render_preview_pixel_size(const struct RenderData *r);

/**********************************/

struct ViewRender *BKE_viewrender_get(struct Scene *scene, struct WorkSpace *workspace);
void BKE_viewrender_init(struct ViewRender *view_render);
void BKE_viewrender_free(struct ViewRender *view_render);
void BKE_viewrender_copy(struct ViewRender *view_render_dst, const struct ViewRender *view_render_src);
bool BKE_viewrender_use_new_shading_nodes(const struct ViewRender *view_render);
bool BKE_viewrender_use_shading_nodes_custom(const struct ViewRender *view_render);
bool BKE_viewrender_use_world_space_shading(const struct ViewRender *view_render);
bool BKE_viewrender_use_spherical_stereo(const struct ViewRender *view_render);
bool BKE_viewrender_uses_blender_internal(const struct ViewRender *view_render);
bool BKE_viewrender_uses_blender_game(const struct ViewRender *view_render);
bool BKE_viewrender_uses_blender_eevee(const struct ViewRender *view_render);

/**********************************/

double BKE_scene_unit_scale(const struct UnitSettings *unit, const int unit_type, double value);

/* multiview */
bool        BKE_scene_multiview_is_stereo3d(const struct RenderData *rd);
bool        BKE_scene_multiview_is_render_view_active(const struct RenderData *rd, const struct SceneRenderView *srv);
bool        BKE_scene_multiview_is_render_view_first(const struct RenderData *rd, const char *viewname);
bool        BKE_scene_multiview_is_render_view_last(const struct RenderData *rd, const char *viewname);
int         BKE_scene_multiview_num_views_get(const struct RenderData *rd);
struct SceneRenderView *BKE_scene_multiview_render_view_findindex(const struct RenderData *rd, const int view_id);
const char *BKE_scene_multiview_render_view_name_get(const struct RenderData *rd, const int view_id);
int         BKE_scene_multiview_view_id_get(const struct RenderData *rd, const char *viewname);
void        BKE_scene_multiview_filepath_get(struct SceneRenderView *srv, const char *filepath, char *r_filepath);
void        BKE_scene_multiview_view_filepath_get(const struct RenderData *rd, const char *filepath, const char *view, char *r_filepath);
const char *BKE_scene_multiview_view_suffix_get(const struct RenderData *rd, const char *viewname);
const char *BKE_scene_multiview_view_id_suffix_get(const struct RenderData *rd, const int view_id);
void        BKE_scene_multiview_view_prefix_get(struct Scene *scene, const char *name, char *rprefix, const char **rext);
void        BKE_scene_multiview_videos_dimensions_get(const struct RenderData *rd, const size_t width, const size_t height, size_t *r_width, size_t *r_height);
int         BKE_scene_multiview_num_videos_get(const struct RenderData *rd);

/* depsgraph */
struct Depsgraph *BKE_scene_get_depsgraph(struct Scene *scene, struct SceneLayer *scene_layer);

#ifdef __cplusplus
}
#endif

#endif
