/*************************************************************************/
/*  openvr_hmd.cpp                                                       */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2016 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "openvr_hmd.h"
#include "core/core_string_names.h"
#include "scene/scene_string_names.h"
#include "scene/main/viewport.h"
#include "os/os.h"
 
void OpenVRHMD::_bind_methods() {
	ObjectTypeDB::bind_method( _MD("get_recommended_rendertarget_size"),&OpenVRHMD::get_recommended_rendertarget_size );
	ObjectTypeDB::bind_method( _MD("get_lefteye_global_transform"),&OpenVRHMD::get_lefteye_global_transform );
	ObjectTypeDB::bind_method( _MD("get_righteye_global_transform"),&OpenVRHMD::get_righteye_global_transform );
	ObjectTypeDB::bind_method( _MD("get_lefteye_frustum"),&OpenVRHMD::get_lefteye_frustum );
	ObjectTypeDB::bind_method( _MD("get_righteye_frustum"),&OpenVRHMD::get_righteye_frustum );

	ObjectTypeDB::bind_method(_MD("set_lefteye_viewport_path","path"),&OpenVRHMD::set_lefteye_viewport_path);
	ObjectTypeDB::bind_method(_MD("get_lefteye_viewport_path"),&OpenVRHMD::get_lefteye_viewport_path);

	ObjectTypeDB::bind_method(_MD("set_righteye_viewport_path","path"),&OpenVRHMD::set_righteye_viewport_path);
	ObjectTypeDB::bind_method(_MD("get_righteye_viewport_path"),&OpenVRHMD::get_righteye_viewport_path);

	ADD_PROPERTYNZ( PropertyInfo( Variant::NODE_PATH, "lefteye_viewport"), _SCS("set_lefteye_viewport_path"),_SCS("get_lefteye_viewport_path"));
	ADD_PROPERTYNZ( PropertyInfo( Variant::NODE_PATH, "righteye_viewport"), _SCS("set_righteye_viewport_path"),_SCS("get_righteye_viewport_path"));
};

Point2 OpenVRHMD::get_recommended_rendertarget_size() {
	if (ovr != NULL) {
		return ovr->getRecommendedRenderTargetSize();
	} else {
		return Point2(512.0,512.0);
	};
};

Transform OpenVRHMD::get_lefteye_global_transform() {
	Transform eyetransform;

	if (ovr != NULL) {
		eyetransform = ovr->getHeadToEyeTransform(Frustum::EYE_LEFT);
	} else {
		eyetransform.origin.x = -0.035;
	};

	/* is this the right way around? */
	eyetransform = get_global_transform() * eyetransform;

	return eyetransform;
};

Transform OpenVRHMD::get_righteye_global_transform() {
	Transform eyetransform;

	if (ovr != NULL) {
		eyetransform = ovr->getHeadToEyeTransform(Frustum::EYE_RIGHT);
	} else {
		eyetransform.origin.x = 0.035;
	};

	/* is this the right way around? */
	eyetransform = get_global_transform() * eyetransform;

	return eyetransform;
};

Rect2 OpenVRHMD::get_lefteye_frustum() {
	Frustum frustum;

	if (ovr != NULL) {
		frustum = ovr->getEyeFrustum(Frustum::EYE_LEFT);
	} else {
		frustum.set_frustum(60.0, Frustum::EYE_LEFT, 0.065, 1.0);
	};

	// FIXME find a nice way to return our frustum, maybe add it to variant
	return Rect2(frustum.left, frustum.top, frustum.right, frustum.bottom);
};

Rect2 OpenVRHMD::get_righteye_frustum() {
	Frustum frustum;

	if (ovr != NULL) {
		frustum = ovr->getEyeFrustum(Frustum::EYE_RIGHT);
	} else {
		frustum.set_frustum(60.0, Frustum::EYE_RIGHT, 0.065, 1.0);
	};

	// FIXME find a nice way to return our frustum, maybe add it to variant
	return Rect2(frustum.left, frustum.top, frustum.right, frustum.bottom);
};

void OpenVRHMD::set_lefteye_viewport_path(const NodePath& p_viewport) {

	lefteye_viewport_path=p_viewport;
	if (!is_inside_tree())
		return;

	if (lefteye_texture.is_valid()) {
		lefteye_texture->disconnect("changed",this,"update");
		lefteye_texture=Ref<Texture>();
	}

	if (lefteye_viewport_path.is_empty())
		return;

	Node *n = get_node(lefteye_viewport_path);
	ERR_FAIL_COND(!n);
	Viewport *vp=n->cast_to<Viewport>();
	ERR_FAIL_COND(!vp);

	Ref<RenderTargetTexture> rtt = vp->get_render_target_texture();
	lefteye_texture=rtt;

	if (lefteye_texture.is_valid()) {
		lefteye_texture->connect("changed",this,"update");
	}
}

NodePath OpenVRHMD::get_lefteye_viewport_path() const {

	return lefteye_viewport_path;
}

void OpenVRHMD::set_righteye_viewport_path(const NodePath& p_viewport) {

	righteye_viewport_path=p_viewport;
	if (!is_inside_tree())
		return;

	if (righteye_texture.is_valid()) {
		righteye_texture->disconnect("changed",this,"update");
		righteye_texture=Ref<Texture>();
	}

	if (righteye_viewport_path.is_empty())
		return;

	Node *n = get_node(righteye_viewport_path);
	ERR_FAIL_COND(!n);
	Viewport *vp=n->cast_to<Viewport>();
	ERR_FAIL_COND(!vp);

	Ref<RenderTargetTexture> rtt = vp->get_render_target_texture();
	righteye_texture=rtt;

	if (righteye_texture.is_valid()) {
		righteye_texture->connect("changed",this,"update");
	}
}

NodePath OpenVRHMD::get_righteye_viewport_path() const {

	return righteye_viewport_path;
}

String OpenVRHMD::get_configuration_warning() const {

	if (!has_node(lefteye_viewport_path) || !get_node(lefteye_viewport_path) || !get_node(lefteye_viewport_path)->cast_to<Viewport>()) {
		return TTR("Left eye path property must point to a valid Viewport node to work. Such Viewport must be set to 'render target' mode.");
	} else {
		Node *n = get_node(lefteye_viewport_path);
		if (n) {
			Viewport *vp = n->cast_to<Viewport>();
			if (!vp->is_set_as_render_target()) {
				return TTR("The Viewport set in the left eye path property must be set as 'render target' in order for this sprite to work.");
			}
		}
	}

	if (!has_node(righteye_viewport_path) || !get_node(righteye_viewport_path) || !get_node(righteye_viewport_path)->cast_to<Viewport>()) {
		return TTR("Right eye path property must point to a valid Viewport node to work. Such Viewport must be set to 'render target' mode.");
	} else {
		Node *n = get_node(righteye_viewport_path);
		if (n) {
			Viewport *vp = n->cast_to<Viewport>();
			if (!vp->is_set_as_render_target()) {
				return TTR("The Viewport set in the right eye path property must be set as 'render target' in order for this sprite to work.");
			}
		}
	}

	return String();
}

void OpenVRHMD::process(float pDelta) {
	if (ovr != NULL) {
		// @TODO: Commit last frames textures, really need to fine a way to move this to the end of the render
		// loop but seeing the end of the last render loop and the start of this one is right after eachother...
		ovr->commitEyeTexture(Frustum::EYE_LEFT, lefteye_texture);
		ovr->commitEyeTexture(Frustum::EYE_RIGHT, righteye_texture);

		// @TODO: Move this into a better place, it should be called only once per frame
		// For now we only run it for our HMD but if this isn't the first node we could be in trouble
		ovr->process();

		// @TODO: we should find a way to update the transforms of all our trackers so we don't trickle this data in during processing..
		// update our tracked device data...
		set_transform(ovr->getUpdatedTransform(vr::k_unTrackedDeviceIndex_Hmd));

		// Done...
		return;
	};
};

void OpenVRHMD::_notification(int p_what) {
	switch (p_what ) {
		case NOTIFICATION_ENTER_TREE: {
			if (ovr == NULL) {
				ovr = openVR_interface::get_singleton(get_tree()->is_editor_hint());
			}

			if (!lefteye_viewport_path.is_empty()) {

				Node *n = get_node(lefteye_viewport_path);
				ERR_FAIL_COND(!n);
				Viewport *vp=n->cast_to<Viewport>();
				ERR_FAIL_COND(!vp);

				Ref<RenderTargetTexture> rtt = vp->get_render_target_texture();
				lefteye_texture=rtt;
				lefteye_texture->connect("changed",this,"update");
			}

			if (!righteye_viewport_path.is_empty()) {

				Node *n = get_node(righteye_viewport_path);
				ERR_FAIL_COND(!n);
				Viewport *vp=n->cast_to<Viewport>();
				ERR_FAIL_COND(!vp);

				Ref<RenderTargetTexture> rtt = vp->get_render_target_texture();
				righteye_texture=rtt;
				righteye_texture->connect("changed",this,"update");
			}

			set_process(true);
		}; break;
		case NOTIFICATION_EXIT_TREE: {
			set_process(false);

			if (lefteye_texture.is_valid()) {

				lefteye_texture->disconnect("changed",this,"update");
				lefteye_texture=Ref<Texture>();
			}

			if (righteye_texture.is_valid()) {

				righteye_texture->disconnect("changed",this,"update");
				righteye_texture=Ref<Texture>();
			}

			if (ovr != NULL) {
				openVR_interface::release();
				ovr = NULL;
			};
		}; break;
		case NOTIFICATION_PROCESS: {
			process(get_process_delta_time());
		}; break;
		default : {
			// don't do anything here
		}; break;
	};
};

OpenVRHMD::OpenVRHMD() {
	ovr = NULL;
};

OpenVRHMD::~OpenVRHMD() {
	if (ovr != NULL) {
		openVR_interface::release();
		ovr = NULL;
	};
};
