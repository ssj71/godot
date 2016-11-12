/*************************************************************************/
/*  openvr_interface.cpp                                                 */
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

#include "openvr_interface.h"

#include <stdio.h>
#include <string.h>

openVR_interface* openVR_singleton = NULL;

openVR_interface* openVR_interface::get_singleton(bool p_in_editor) {
	// not ideal parsing p_in_editor like this but as its virtually static it'll do...
	if (openVR_singleton == NULL) {
		openVR_singleton = new openVR_interface(p_in_editor);
	} else {
		openVR_singleton->refcount++;
	}
	return openVR_singleton;
};

void openVR_interface::release() {
	if (openVR_singleton != NULL) {
		openVR_singleton->refcount--;
		if (openVR_singleton->refcount == 0) {
			delete openVR_singleton;
			openVR_singleton = NULL;
		};
	};
};

openVR_interface::openVR_interface(bool p_in_editor) {
	// most of this code is copied straight from openVR sample files
	bool success = true;

	// reset some stuff
	refcount = 1;
	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
		ControllerIndices[i] = vr::k_unTrackedDeviceIndexInvalid;
		BaseStationIndices[i] = vr::k_unTrackedDeviceIndexInvalid;
	};

	// @TODO Find out if we can initialize some parts of OpenVR so we have access to certain things...
	// So far I've ran into trouble that having both editor and runtime use OpenVR at the same time leads to crashes.
	if (p_in_editor) {
		printf("Not initializing OpenVR, only available in runtime. You can access all OpenVR objects with dummy data.\n");

		hmd = NULL;
		renderModels = NULL;

		// Set up some dummy controllers and base stations
		ControllerIndices[0] = 1;
		ControllerIndices[1] = 2;
		BaseStationIndices[0] = 3;
		BaseStationIndices[1] = 4;
	} else {
		// Loading the SteamVR Runtime
		vr::EVRInitError error = vr::VRInitError_None;
		hmd = vr::VR_Init( &error, vr::VRApplication_Scene );

		if ( error != vr::VRInitError_None ) {
			success = false;
			printf( "Unable to init VR runtime: %s\n", vr::VR_GetVRInitErrorAsEnglishDescription( error ) );
		} else {
			printf("Main OpenVR interface has been initialized\n");
		}

		if (success) {
			// render models give us access to mesh representations of the various controllers
			renderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface( vr::IVRRenderModels_Version, &error );
			if( !renderModels ) {
				success = false;

				printf("Unable to get render model interface: %s\n", vr::VR_GetVRInitErrorAsEnglishDescription( error ) );
			} else {
				printf("Main render models interface has been initialized\n");
			};
		};

		if ( !vr::VRCompositor() ) {
			success = false;

			printf( "Compositor initialization failed. See log file for details\n" );
		};

		if (success) {
			for (uint32_t i = vr::k_unTrackedDeviceIndex_Hmd + 1; i < vr::k_unMaxTrackedDeviceCount; i++ ) {
				if( hmd->IsTrackedDeviceConnected(i) ) {
					attach_device(i);
				};
			};
		};

		if (!success) {
			vr::VR_Shutdown();
			hmd = NULL;
			renderModels = NULL;		
		};
	};
};

openVR_interface::~openVR_interface() {
	if (hmd != NULL) {
		vr::VR_Shutdown();
		hmd = NULL;
		renderModels = NULL;
	};
};

void openVR_interface::attach_device(uint32_t p_device_index) {
	char device_name[256];
	strcpy(device_name, getTrackedDeviceName(p_device_index, 255));
	printf( "Device %u attached (%s)", p_device_index, device_name );

	if (strstr(device_name, "basestation") != NULL) {
		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			if (BaseStationIndices[i] == vr::k_unTrackedDeviceIndexInvalid) {
				printf( " to basestation index %u", i+1 );
				BaseStationIndices[i] = p_device_index;
				i = vr::k_unMaxTrackedDeviceCount;
			};
		};
	} else {
		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			if (ControllerIndices[i] == vr::k_unTrackedDeviceIndexInvalid) {
				printf( " to controller index %u", i+1 );
				ControllerIndices[i] = p_device_index;
				i = vr::k_unMaxTrackedDeviceCount;
			};
		};
	};

	printf( ".\n");	
};

void openVR_interface::detach_device(uint32_t p_device_index) {
	printf( "Device %u detached", p_device_index );

	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
		if (ControllerIndices[i] == p_device_index) {
			printf( " from controller index %u", i+1);
			ControllerIndices[i] = vr::k_unTrackedDeviceIndexInvalid;
		};
		if (BaseStationIndices[i] == p_device_index) {
			printf( " from basestation index %u", i+1);
			BaseStationIndices[i] =  vr::k_unTrackedDeviceIndexInvalid;								
		};
	};

	printf( ".\n");
};

/**
	Process openVR events and update trackers and controllers, this should be called once per frame
**/
void openVR_interface::process() {
	if (hmd != NULL) {
		// Process SteamVR events
		vr::VREvent_t event;
		while( hmd->PollNextEvent(&event, sizeof(event))) {
			switch( event.eventType ) {
				case vr::VREvent_TrackedDeviceActivated: {
					attach_device(event.trackedDeviceIndex);
				}; break;
				case vr::VREvent_TrackedDeviceDeactivated: {
					detach_device(event.trackedDeviceIndex);
				}; break;
				default: {
					// ignored for now...	
				}; break;
			};
		};

		// update our poses structure, this tracks our controllers
		vr::VRCompositor()->WaitGetPoses(trackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0 );

		// update our button state structure
		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			hmd->GetControllerState(i, &trackedDeviceState[i], sizeof(vr::VRControllerState_t));
		};
	};
};

vr::TrackedDeviceIndex_t openVR_interface::getControllerIndex(uint32_t p_index) {
	if (p_index <= vr::k_unMaxTrackedDeviceCount) {
		return ControllerIndices[p_index];
	} else {
		return vr::k_unTrackedDeviceIndexInvalid;
	}
};

vr::TrackedDeviceIndex_t openVR_interface::getBaseStationIndex(uint32_t p_index) {
	if (p_index < vr::k_unMaxTrackedDeviceCount) {
		return BaseStationIndices[p_index];
	} else {
		return vr::k_unTrackedDeviceIndexInvalid;
	}
};

bool openVR_interface::isButtonPressed(vr::TrackedDeviceIndex_t p_tracked_device_index, vr::EVRButtonId p_button) {
	if (hmd == NULL) {
		return false;
	} else if (p_tracked_device_index >= vr::k_unMaxTrackedDeviceCount) {
		return false;
	} else if (p_button >= vr::k_EButton_Max) {
		return false;
	} else {
		// note, if we're asking for one of our axis, we probably don't have a flag set so it ends up false
		return (trackedDeviceState[p_tracked_device_index].ulButtonPressed & ButtonMaskFromId(p_button)) != 0;
	};
};

Point2 openVR_interface::getAxis(vr::TrackedDeviceIndex_t p_tracked_device_index, vr::EVRButtonId p_axis) {
	int axis = p_axis - vr::k_EButton_Axis0;
	if (hmd == NULL) {
		return Point2(0.0, 0.0);
	} else if (p_tracked_device_index >= vr::k_unMaxTrackedDeviceCount) {
		return Point2(0.0, 0.0);
	} else if ((axis < 0) || (axis >= vr::k_unControllerStateAxisCount)) {
		printf("Unknown axis %i", axis);
		return Point2(0.0, 0.0);
	} else {
		vr::VRControllerAxis_t axisxy = trackedDeviceState[p_tracked_device_index].rAxis[axis];
		return Point2(axisxy.x, axisxy.y);
	};
};

Transform openVR_interface::getUpdatedTransform(vr::TrackedDeviceIndex_t p_tracked_device_index) {
	Transform newtransform;

	if ((hmd != NULL) && (p_tracked_device_index != vr::k_unTrackedDeviceIndexInvalid)) {
		if ( trackedDevicePose[p_tracked_device_index].bPoseIsValid ) {
			// bit wasteful copying it but I don't want to type so much!
			vr::HmdMatrix34_t matPose = trackedDevicePose[p_tracked_device_index].mDeviceToAbsoluteTracking;

			newtransform.basis.set(
				matPose.m[0][0], matPose.m[0][1], matPose.m[0][2],
				matPose.m[1][0], matPose.m[1][1], matPose.m[1][2],
				matPose.m[2][0], matPose.m[2][1], matPose.m[2][2]
				);

			newtransform.origin.x = matPose.m[0][3];
			newtransform.origin.y = matPose.m[1][3];
			newtransform.origin.z = matPose.m[2][3];

			// sample code inverts this but that is because its using it as the view matrix
			// in godot we don't need to do this as godot will do that automatically
			// just keeping it here for reference
			// if (p_tracked_device_index == vr::k_unTrackedDeviceIndex_Hmd) {
			//	newtransform.invert();
			// };
		};
	} else {
		if (p_tracked_device_index == 0) {
			/* just give our HMD some height */

			newtransform.origin.y = 1.75;
		} else if (p_tracked_device_index == 1) {
			/* position dummy controller 1 */

			newtransform.origin.x = -0.5;
			newtransform.origin.y = 1.45;
			newtransform.origin.z = -0.15;
		} else if (p_tracked_device_index == 2) {
			/* position dummy controller 2 */

			newtransform.origin.x = 0.5;
			newtransform.origin.y = 1.45;
			newtransform.origin.z = -0.15;
		} else if (p_tracked_device_index == 3) {
			/* position dummy basestation 1 */

			newtransform.basis.rotate(Vector3(0.0, 1.0, 0.0), -225.0 * Math_PI / 180.0);
			newtransform.origin.x = -2.0;
			newtransform.origin.y =  2.0;
			newtransform.origin.z = -2.0;
		} else if (p_tracked_device_index == 4) {
			/* position dummy basestation 2 */
 
			newtransform.basis.rotate(Vector3(0.0, 1.0, 0.0), -45.0 * Math_PI / 180.0);
			newtransform.origin.x =  2.0;
			newtransform.origin.y =  2.0;
			newtransform.origin.z =  2.0;
		};
	};

	return newtransform;
};

const char * openVR_interface::getTrackedDeviceName(vr::TrackedDeviceIndex_t p_tracked_device_index, int pMaxLen) const {
	static char returnstring[1025] = "Not initialised";

	// don't go bigger then this...
	if (pMaxLen > 1024) {
		pMaxLen = 1024;
	};

	if (hmd == NULL) {
		if (p_tracked_device_index == 0) {
			strcpy(returnstring, "dummy_hmd");
		} else if (p_tracked_device_index == 1) {
			strcpy(returnstring, "dummy_controller");
		} else if (p_tracked_device_index == 2) {
			strcpy(returnstring, "dummy_controller");
		} else if (p_tracked_device_index == 3) {
			strcpy(returnstring, "dummy_basestation");
		} else if (p_tracked_device_index == 4) {
			strcpy(returnstring, "dummy_basestation");
		};
	} else if (p_tracked_device_index != vr::k_unTrackedDeviceIndexInvalid) {
		uint32_t namelength = hmd->GetStringTrackedDeviceProperty( p_tracked_device_index, vr::Prop_RenderModelName_String, NULL, 0, NULL );
		if (namelength > 0 ) {
			if (namelength > pMaxLen) {
				namelength = pMaxLen;
			};
			
			hmd->GetStringTrackedDeviceProperty( p_tracked_device_index, vr::Prop_RenderModelName_String, returnstring, namelength, NULL );
		};
 	};

	return returnstring;
};

Point2 openVR_interface::getRecommendedRenderTargetSize() {
	if (hmd != NULL) {
		uint32_t width, height;

		hmd->GetRecommendedRenderTargetSize(&width, &height);

		return Point2(width, height);
	} else {
		return Point2(512,512);
	};
};	

Transform openVR_interface::getHeadToEyeTransform(Frustum::Eyes p_eye) {
	Transform newtransform;

	if (hmd != NULL) {
		vr::HmdMatrix34_t matrix = hmd->GetEyeToHeadTransform(p_eye == Frustum::EYE_LEFT ? vr::Eye_Left : vr::Eye_Right );

		newtransform.basis.set(
			matrix.m[0][0], matrix.m[0][1], matrix.m[0][2],
			matrix.m[1][0], matrix.m[1][1], matrix.m[1][2],
			matrix.m[2][0], matrix.m[2][1], matrix.m[2][2]
		);

		newtransform.origin.x = matrix.m[0][3];
		newtransform.origin.y = matrix.m[1][3];
		newtransform.origin.z = matrix.m[2][3];
	} else {
		if (p_eye == Frustum::EYE_LEFT) {
			newtransform.origin.x = -0.035;
		} else {
			newtransform.origin.x = 0.035;
		};
	};

	return newtransform;
};

Frustum openVR_interface::getEyeFrustum(Frustum::Eyes p_eye) {
	Frustum frustum;
	if (hmd != NULL) {
		Point2 size = getRecommendedRenderTargetSize();

		// note that openvr is upside down in relation to godot
		hmd->GetProjectionRaw(p_eye == Frustum::EYE_LEFT ? vr::Eye_Left : vr::Eye_Right, &frustum.left, &frustum.right, &frustum.bottom, &frustum.top);
	
		// Godot will (re)apply our aspect ratio so we need to unapply it
		frustum.left = frustum.left * size.y / size.x;
		frustum.right = frustum.right * size.y / size.x;
	} else {
		// just return a pretty basic stereoscopic frustum
		frustum.set_frustum(60.0, p_eye, 0.065, 1.0);
	};
	return frustum;
};

void openVR_interface::commitEyeTexture(Frustum::Eyes p_eye, Ref<Texture> p_texture) {
	if (hmd != NULL) {
		uint32_t texid;

		// added texture_get_texid to visual server (and rasterers), possibly temporary solution as gles3 may change access to the opengl texture id

		if (p_texture.is_valid()) {
			// upside down!
			vr::VRTextureBounds_t bounds;
			bounds.uMin = 0.0;
			bounds.uMax = 1.0;
			bounds.vMin = 1.0;
			bounds.vMax = 0.0;

			texid = VS::get_singleton()->texture_get_texid(p_texture->get_rid());

			vr::Texture_t eyeTexture = {(void*)texid, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
			vr::VRCompositor()->Submit(p_eye == Frustum::EYE_LEFT ? vr::Eye_Left : vr::Eye_Right, &eyeTexture, &bounds );
		};
	};
};
