From: Ms2ger <ms2ger@gmail.com>
No bug - Move friend APIs out of jstypedarray.h; rs=jorendorff

diff --git a/content/base/src/nsDOMBlobBuilder.cpp b/content/base/src/nsDOMBlobBuilder.cpp
--- a/content/base/src/nsDOMBlobBuilder.cpp
+++ b/content/base/src/nsDOMBlobBuilder.cpp
@@ -30,17 +30,17 @@
  * use your version of this file under the terms of the MPL, indicate your
  * decision by deleting the provisions above and replace them with the notice
  * and other provisions required by the GPL or the LGPL. If you do not delete
  * the provisions above, a recipient may use your version of this file under
  * the terms of any one of the MPL, the GPL or the LGPL.
  *
  * ***** END LICENSE BLOCK ***** */
 
-#include "jstypedarray.h"
+#include "jsfriendapi.h"
 #include "nsAutoPtr.h"
 #include "nsDOMClassInfoID.h"
 #include "nsDOMFile.h"
 #include "nsIMultiplexInputStream.h"
 #include "nsStringStream.h"
 #include "nsTArray.h"
 #include "nsJSUtils.h"
 #include "nsContentUtils.h"
diff --git a/content/base/src/nsDOMFileReader.cpp b/content/base/src/nsDOMFileReader.cpp
--- a/content/base/src/nsDOMFileReader.cpp
+++ b/content/base/src/nsDOMFileReader.cpp
@@ -74,17 +74,17 @@
 #include "nsCycleCollectionParticipant.h"
 #include "nsLayoutStatics.h"
 #include "nsIScriptObjectPrincipal.h"
 #include "nsFileDataProtocolHandler.h"
 #include "mozilla/Preferences.h"
 #include "xpcprivate.h"
 #include "xpcpublic.h"
 #include "XPCQuickStubs.h"
-#include "jstypedarray.h"
+#include "jsfriendapi.h"
 #include "nsDOMJSUtils.h"
 
 using namespace mozilla;
 
 #define LOAD_STR "load"
 #define LOADSTART_STR "loadstart"
 #define LOADEND_STR "loadend"
 
diff --git a/content/base/src/nsXMLHttpRequest.cpp b/content/base/src/nsXMLHttpRequest.cpp
--- a/content/base/src/nsXMLHttpRequest.cpp
+++ b/content/base/src/nsXMLHttpRequest.cpp
@@ -94,17 +94,17 @@
 #include "nsIPromptFactory.h"
 #include "nsIWindowWatcher.h"
 #include "nsCharSeparatedTokenizer.h"
 #include "nsIConsoleService.h"
 #include "nsIChannelPolicy.h"
 #include "nsChannelPolicy.h"
 #include "nsIContentSecurityPolicy.h"
 #include "nsAsyncRedirectVerifyHelper.h"
-#include "jstypedarray.h"
+#include "jsfriendapi.h"
 #include "nsStringBuffer.h"
 #include "nsDOMFile.h"
 #include "nsIFileChannel.h"
 
 using namespace mozilla;
 
 #define LOAD_STR "load"
 #define ERROR_STR "error"
diff --git a/content/canvas/src/CustomQS_Canvas2D.h b/content/canvas/src/CustomQS_Canvas2D.h
--- a/content/canvas/src/CustomQS_Canvas2D.h
+++ b/content/canvas/src/CustomQS_Canvas2D.h
@@ -175,30 +175,28 @@ CreateImageData(JSContext* cx,
 
     CheckedInt<uint32> len = CheckedInt<uint32>(w) * h * 4;
     if (!len.valid()) {
         return xpc_qsThrow(cx, NS_ERROR_DOM_INDEX_SIZE_ERR);
     }
 
     // Create the fast typed array; it's initialized to 0 by default.
     JSObject* darray =
-      js_CreateTypedArray(cx, js::TypedArray::TYPE_UINT8_CLAMPED, len.value());
+      js_CreateTypedArray(cx, TYPEDARRAY_UINT8_CLAMPED, len.value());
     js::AutoObjectRooter rd(cx, darray);
     if (!darray) {
         return false;
     }
 
     if (self) {
-        JSObject *tdest = js::TypedArray::getTypedArray(darray);
-
         // make the call
         nsresult rv =
             self->GetImageData_explicit(x, y, w, h,
-                                        static_cast<uint8_t*>(JS_GetTypedArrayData(tdest)),
-                                        JS_GetTypedArrayByteLength(tdest));
+                                        static_cast<uint8_t*>(JS_GetTypedArrayData(darray)),
+                                        JS_GetTypedArrayByteLength(darray));
         if (NS_FAILED(rv)) {
             return xpc_qsThrowMethodFailed(cx, rv, vp);
         }
     }
 
     // Do JS_NewObject after CreateTypedArray, so that gc will get
     // triggered here if necessary
     JSObject* result = JS_NewObject(cx, NULL, NULL, NULL);
diff --git a/content/canvas/src/CustomQS_WebGL.h b/content/canvas/src/CustomQS_WebGL.h
--- a/content/canvas/src/CustomQS_WebGL.h
+++ b/content/canvas/src/CustomQS_WebGL.h
@@ -35,17 +35,17 @@
  * the terms of any one of the MPL, the GPL or the LGPL.
  *
  * ***** END LICENSE BLOCK ***** */
 
 /*
  * Intended to be #included in dom_quickstubs.cpp via qsconf!
  */
 
-#include "jstypedarray.h"
+#include "jsfriendapi.h"
 
 #define GET_INT32_ARG(var, index) \
   int32 var; \
   do { \
     if (!JS_ValueToECMAInt32(cx, argv[index], &(var))) \
       return JS_FALSE; \
   } while (0)
 
@@ -62,22 +62,22 @@
     if (argc > index) \
       if (!JS_ValueToECMAUint32(cx, argv[index], &(var))) \
         return JS_FALSE; \
   } while (0)
 
 
 static inline bool
 helper_isInt32Array(JSObject *obj) {
-    return js::GetObjectClass(obj) == &js::TypedArray::fastClasses[js::TypedArray::TYPE_INT32];
+    return JS_GetTypedArrayType(obj) == TYPEDARRAY_INT32;
 }
 
 static inline bool
 helper_isFloat32Array(JSObject *obj) {
-    return js::GetObjectClass(obj) == &js::TypedArray::fastClasses[js::TypedArray::TYPE_FLOAT32];
+    return JS_GetTypedArrayType(obj) == TYPEDARRAY_FLOAT32;
 }
 
 /*
  * BufferData takes:
  *    BufferData (int, int, int)
  *    BufferData_buf (int, js::ArrayBuffer *, int)
  *    BufferData_array (int, js::TypedArray *, int)
  */
@@ -115,17 +115,17 @@ nsIDOMWebGLRenderingContext_BufferData(J
 
     if (!nullobject) {
         if (!JSVAL_IS_PRIMITIVE(argv[1])) {
 
             JSObject *arg2 = JSVAL_TO_OBJECT(argv[1]);
             if (js_IsArrayBuffer(arg2)) {
                 wb = js::ArrayBuffer::getArrayBuffer(arg2);
             } else if (js_IsTypedArray(arg2)) {
-                wa = js::TypedArray::getTypedArray(arg2);
+                wa = arg);
             }
         }
 
         if (!wa && !wb &&
             !JS_ValueToECMAInt32(cx, argv[1], &size))
         {
             return JS_FALSE;
         }
@@ -190,17 +190,17 @@ nsIDOMWebGLRenderingContext_BufferSubDat
 
     JSBool nullobject = JSVAL_IS_NULL(argv[2]);
 
     if (!nullobject) {
         JSObject *arg3 = JSVAL_TO_OBJECT(argv[2]);
         if (js_IsArrayBuffer(arg3)) {
             wb = js::ArrayBuffer::getArrayBuffer(arg3);
         } else if (js_IsTypedArray(arg3)) {
-            wa = js::TypedArray::getTypedArray(arg3);
+            wa = arg3;
         } else {
             xpc_qsThrowBadArg(cx, NS_ERROR_FAILURE, vp, 2);
             return JS_FALSE;
         }
     }
 
     nsresult rv;
 
@@ -256,18 +256,17 @@ nsIDOMWebGLRenderingContext_ReadPixels(J
     GET_UINT32_ARG(argv5, 5);
 
     if (argc == 7 &&
         !JSVAL_IS_PRIMITIVE(argv[6]))
     {
         JSObject *argv6 = JSVAL_TO_OBJECT(argv[6]);
         if (js_IsTypedArray(argv6)) {
             rv = self->ReadPixels_array(argv0, argv1, argv2, argv3,
-                                        argv4, argv5,
-                                        js::TypedArray::getTypedArray(argv6));
+                                        argv4, argv5, argv6));
         } else {
             xpc_qsThrowBadArg(cx, NS_ERROR_FAILURE, vp, 6);
             return JS_FALSE;
         }
     } else {
         xpc_qsThrow(cx, NS_ERROR_FAILURE);
         return JS_FALSE;
     }
@@ -357,17 +356,17 @@ nsIDOMWebGLRenderingContext_TexImage2D(J
             }
             if (!js_IsTypedArray(obj_data))
             {
                 xpc_qsThrowBadArg(cx, NS_ERROR_FAILURE, vp, 5);
                 return JS_FALSE;
             }
             rv = self->TexImage2D_imageData(argv0, argv1, argv2,
                                             int_width, int_height, 0,
-                                            argv3, argv4, js::TypedArray::getTypedArray(obj_data));
+                                            argv3, argv4, obj_data);
         }
     } else if (argc > 8 &&
                JSVAL_IS_OBJECT(argv[8])) // here, we allow null !
     {
         // implement the variants taking a buffer/array as argv[8]
         GET_UINT32_ARG(argv2, 2);
         GET_INT32_ARG(argv3, 3);
         GET_INT32_ARG(argv4, 4);
@@ -380,17 +379,17 @@ nsIDOMWebGLRenderingContext_TexImage2D(J
         // then try to grab either a js::TypedArray, or null
         if (argv8 == nsnull) {
             rv = self->TexImage2D_array(argv0, argv1, argv2, argv3,
                                         argv4, argv5, argv6, argv7,
                                         nsnull);
         } else if (js_IsTypedArray(argv8)) {
             rv = self->TexImage2D_array(argv0, argv1, argv2, argv3,
                                         argv4, argv5, argv6, argv7,
-                                        js::TypedArray::getTypedArray(argv8));
+                                        argv8);
         } else {
             xpc_qsThrowBadArg(cx, NS_ERROR_FAILURE, vp, 8);
             return JS_FALSE;
         }
     } else {
         xpc_qsThrow(cx, NS_ERROR_XPC_NOT_ENOUGH_ARGS);
         return JS_FALSE;
     }
@@ -479,33 +478,33 @@ nsIDOMWebGLRenderingContext_TexSubImage2
             if (!js_IsTypedArray(obj_data))
             {
                 xpc_qsThrowBadArg(cx, NS_ERROR_FAILURE, vp, 6);
                 return JS_FALSE;
             }
             rv = self->TexSubImage2D_imageData(argv0, argv1, argv2, argv3,
                                                int_width, int_height,
                                                argv4, argv5,
-                                               js::TypedArray::getTypedArray(obj_data));
+                                               obj_data);
         }
     } else if (argc > 8 &&
                !JSVAL_IS_PRIMITIVE(argv[8]))
     {
         // implement the variants taking a buffer/array as argv[8]
         GET_INT32_ARG(argv4, 4);
         GET_INT32_ARG(argv5, 5);
         GET_UINT32_ARG(argv6, 6);
         GET_UINT32_ARG(argv7, 7);
 
         JSObject *argv8 = JSVAL_TO_OBJECT(argv[8]);
         // try to grab a js::TypedArray
         if (js_IsTypedArray(argv8)) {
             rv = self->TexSubImage2D_array(argv0, argv1, argv2, argv3,
                                            argv4, argv5, argv6, argv7,
-                                           js::TypedArray::getTypedArray(argv8));
+                                           argv8);
         } else {
             xpc_qsThrowBadArg(cx, NS_ERROR_FAILURE, vp, 8);
             return JS_FALSE;
         }
     } else {
         xpc_qsThrow(cx, NS_ERROR_XPC_NOT_ENOUGH_ARGS);
         return JS_FALSE;
     }
@@ -554,26 +553,26 @@ helper_nsIDOMWebGLRenderingContext_Unifo
 
     JSObject *arg1 = JSVAL_TO_OBJECT(argv[1]);
 
     js::AutoValueRooter obj_tvr(cx);
 
     JSObject *wa = 0;
 
     if (helper_isInt32Array(arg1)) {
-        wa = js::TypedArray::getTypedArray(arg1);
+        wa = arg1;
     }  else if (JS_IsArrayObject(cx, arg1)) {
-        JSObject *nobj = js_CreateTypedArrayWithArray(cx, js::TypedArray::TYPE_INT32, arg1);
+        JSObject *nobj = js_CreateTypedArrayWithArray(cx, TYPEDARRAY_INT32, arg1);
         if (!nobj) {
             // XXX this will likely return a strange error message if it goes wrong
             return JS_FALSE;
         }
 
         *obj_tvr.jsval_addr() = OBJECT_TO_JSVAL(nobj);
-        wa = js::TypedArray::getTypedArray(nobj);
+        wa = nobj;
     } else {
         xpc_qsThrowBadArg(cx, NS_ERROR_FAILURE, vp, 1);
         return JS_FALSE;
     }
 
     if (nElements == 1) {
         rv = self->Uniform1iv_array(location, wa);
     } else if (nElements == 2) {
@@ -628,26 +627,26 @@ helper_nsIDOMWebGLRenderingContext_Unifo
 
     JSObject *arg1 = JSVAL_TO_OBJECT(argv[1]);
 
     js::AutoValueRooter obj_tvr(cx);
 
     JSObject *wa = 0;
 
     if (helper_isFloat32Array(arg1)) {
-        wa = js::TypedArray::getTypedArray(arg1);
+        wa = arg1;
     }  else if (JS_IsArrayObject(cx, arg1)) {
-        JSObject *nobj = js_CreateTypedArrayWithArray(cx, js::TypedArray::TYPE_FLOAT32, arg1);
+        JSObject *nobj = js_CreateTypedArrayWithArray(cx, TYPEDARRAY_FLOAT32, arg1);
         if (!nobj) {
             // XXX this will likely return a strange error message if it goes wrong
             return JS_FALSE;
         }
 
         *obj_tvr.jsval_addr() = OBJECT_TO_JSVAL(nobj);
-        wa = js::TypedArray::getTypedArray(nobj);
+        wa = nobj;
     } else {
         xpc_qsThrowBadArg(cx, NS_ERROR_FAILURE, vp, 1);
         return JS_FALSE;
     }
 
     if (nElements == 1) {
         rv = self->Uniform1fv_array(location, wa);
     } else if (nElements == 2) {
@@ -704,26 +703,26 @@ helper_nsIDOMWebGLRenderingContext_Unifo
 
     JSObject *arg2 = JSVAL_TO_OBJECT(argv[2]);
 
     js::AutoValueRooter obj_tvr(cx);
 
     JSObject *wa = 0;
 
     if (helper_isFloat32Array(arg2)) {
-        wa = js::TypedArray::getTypedArray(arg2);
+        wa = arg2;
     }  else if (JS_IsArrayObject(cx, arg2)) {
-        JSObject *nobj = js_CreateTypedArrayWithArray(cx, js::TypedArray::TYPE_FLOAT32, arg2);
+        JSObject *nobj = js_CreateTypedArrayWithArray(cx, TYPEDARRAY_FLOAT32, arg2);
         if (!nobj) {
             // XXX this will likely return a strange error message if it goes wrong
             return JS_FALSE;
         }
 
         *obj_tvr.jsval_addr() = OBJECT_TO_JSVAL(nobj);
-        wa = js::TypedArray::getTypedArray(nobj);
+        wa = nobj;
     } else {
         xpc_qsThrowBadArg(cx, NS_ERROR_FAILURE, vp, 2);
         return JS_FALSE;
     }
 
     if (nElements == 2) {
         rv = self->UniformMatrix2fv_array(location, transpose ? 1 : 0, wa);
     } else if (nElements == 3) {
@@ -769,26 +768,26 @@ helper_nsIDOMWebGLRenderingContext_Verte
 
     JSObject *arg1 = JSVAL_TO_OBJECT(argv[1]);
 
     js::AutoValueRooter obj_tvr(cx);
 
     JSObject *wa = 0;
 
     if (helper_isFloat32Array(arg1)) {
-        wa = js::TypedArray::getTypedArray(arg1);
+        wa = arg1;
     }  else if (JS_IsArrayObject(cx, arg1)) {
-        JSObject *nobj = js_CreateTypedArrayWithArray(cx, js::TypedArray::TYPE_FLOAT32, arg1);
+        JSObject *nobj = js_CreateTypedArrayWithArray(cx, TYPEDARRAY_FLOAT32, arg1);
         if (!nobj) {
             // XXX this will likely return a strange error message if it goes wrong
             return JS_FALSE;
         }
 
         *obj_tvr.jsval_addr() = OBJECT_TO_JSVAL(nobj);
-        wa = js::TypedArray::getTypedArray(nobj);
+        wa = nobj;
     } else {
         xpc_qsThrowBadArg(cx, NS_ERROR_FAILURE, vp, 1);
         return JS_FALSE;
     }
 
     nsresult rv = NS_OK;
     if (nElements == 1) {
         rv = self->VertexAttrib1fv_array(location, wa);
@@ -931,27 +930,27 @@ helper_nsIDOMWebGLRenderingContext_Unifo
     if (NS_FAILED(rv_convert_arg0)) {
         js_SetTraceableNativeFailed(cx);
         return;
     }
 
     JSObject *wa = 0;
 
     if (helper_isInt32Array(arg)) {
-        wa = js::TypedArray::getTypedArray(arg);
+        wa = arg;
     }  else if (JS_IsArrayObject(cx, arg)) {
-        JSObject *nobj = js_CreateTypedArrayWithArray(cx, js::TypedArray::TYPE_INT32, arg);
+        JSObject *nobj = js_CreateTypedArrayWithArray(cx, TYPEDARRAY_INT32, arg);
         if (!nobj) {
             // XXX this will likely return a strange error message if it goes wrong
             js_SetTraceableNativeFailed(cx);
             return;
         }
 
         *obj_tvr.jsval_addr() = OBJECT_TO_JSVAL(nobj);
-        wa = js::TypedArray::getTypedArray(nobj);
+        wa = nobj;
     } else {
         xpc_qsThrowMethodFailedWithDetails(cx, NS_ERROR_FAILURE, "nsIDOMWebGLRenderingContext", "uniformNiv");
         js_SetTraceableNativeFailed(cx);
         return;
     }
 
     nsresult rv = NS_OK;
     if (nElements == 1) {
@@ -1002,27 +1001,27 @@ helper_nsIDOMWebGLRenderingContext_Unifo
     if (NS_FAILED(rv_convert_arg0)) {
         js_SetTraceableNativeFailed(cx);
         return;
     }
 
     JSObject *wa = 0;
 
     if (helper_isFloat32Array(arg)) {
-        wa = js::TypedArray::getTypedArray(arg);
+        wa = arg;
     }  else if (JS_IsArrayObject(cx, arg)) {
-        JSObject *nobj = js_CreateTypedArrayWithArray(cx, js::TypedArray::TYPE_FLOAT32, arg);
+        JSObject *nobj = js_CreateTypedArrayWithArray(cx, TYPEDARRAY_FLOAT32, arg);
         if (!nobj) {
             // XXX this will likely return a strange error message if it goes wrong
             js_SetTraceableNativeFailed(cx);
             return;
         }
 
         *obj_tvr.jsval_addr() = OBJECT_TO_JSVAL(nobj);
-        wa = js::TypedArray::getTypedArray(nobj);
+        wa = nobj;
     } else {
         xpc_qsThrowMethodFailedWithDetails(cx, NS_ERROR_FAILURE, "nsIDOMWebGLRenderingContext", "uniformNfv");
         js_SetTraceableNativeFailed(cx);
         return;
     }
 
     nsresult rv = NS_OK;
     if (nElements == 1) {
@@ -1072,30 +1071,30 @@ helper_nsIDOMWebGLRenderingContext_Unifo
         = xpc_qsUnwrapThis(cx, locationobj, nsnull, &location,
                            &location_selfref.ptr, &location_anchor.get(),
                            nsnull);
     if (NS_FAILED(rv_convert_arg0)) {
         js_SetTraceableNativeFailed(cx);
         return;
     }
 
-    JSObject *wa = 0;
+    JSObject *wa = NULL;
 
     if (helper_isFloat32Array(arg)) {
-        wa = js::TypedArray::getTypedArray(arg);
+        wa = arg;
     }  else if (JS_IsArrayObject(cx, arg)) {
-        JSObject *nobj = js_CreateTypedArrayWithArray(cx, js::TypedArray::TYPE_FLOAT32, arg);
+        JSObject *nobj = js_CreateTypedArrayWithArray(cx, TYPEDARRAY_FLOAT32, arg);
         if (!nobj) {
             // XXX this will likely return a strange error message if it goes wrong
             js_SetTraceableNativeFailed(cx);
             return;
         }
 
         *obj_tvr.jsval_addr() = OBJECT_TO_JSVAL(nobj);
-        wa = js::TypedArray::getTypedArray(nobj);
+        wa = nobj;
     } else {
         xpc_qsThrowMethodFailedWithDetails(cx, NS_ERROR_FAILURE, "nsIDOMWebGLRenderingContext", "uniformMatrixNfv");
         js_SetTraceableNativeFailed(cx);
         return;
     }
 
     nsresult rv = NS_OK;
     if (nElements == 2) {
diff --git a/content/canvas/src/WebGLContextGL.cpp b/content/canvas/src/WebGLContextGL.cpp
--- a/content/canvas/src/WebGLContextGL.cpp
+++ b/content/canvas/src/WebGLContextGL.cpp
@@ -49,17 +49,17 @@
 //#include "nsIDOMHTMLCanvasElement.h"
 
 #include "nsContentUtils.h"
 #include "nsDOMError.h"
 #include "nsLayoutUtils.h"
 
 #include "CanvasUtils.h"
 
-#include "jstypedarray.h"
+#include "jsfriendapi.h"
 
 #if defined(USE_ANGLE)
 // shader translator
 #include "angle/ShaderLang.h"
 #endif
 
 #include "WebGLTexelConversions.h"
 #include "WebGLValidateStrings.h"
@@ -3394,23 +3394,23 @@ WebGLContext::ReadPixels_base(WebGLint x
 
     uint32_t bytesPerPixel = 0;
     int requiredDataType = 0;
 
     // Check the type param
     switch (type) {
         case LOCAL_GL_UNSIGNED_BYTE:
             bytesPerPixel = 1 * channels;
-            requiredDataType = js::TypedArray::TYPE_UINT8;
+            requiredDataType = TYPEDARRAY_UINT8;
             break;
         case LOCAL_GL_UNSIGNED_SHORT_4_4_4_4:
         case LOCAL_GL_UNSIGNED_SHORT_5_5_5_1:
         case LOCAL_GL_UNSIGNED_SHORT_5_6_5:
             bytesPerPixel = 2;
-            requiredDataType = js::TypedArray::TYPE_UINT16;
+            requiredDataType = TYPEDARRAY_UINT16;
             break;
         default:
             return ErrorInvalidEnum("readPixels: Bad type");
     }
 
     // Check the pixels param type
     if (dataType != requiredDataType)
         return ErrorInvalidOperation("readPixels: Mismatched type/pixels types");
@@ -4102,17 +4102,17 @@ WebGLContext::name(int32_t) {           
      return NS_ERROR_NOT_IMPLEMENTED;                                   \
 }                                                                       \
 NS_IMETHODIMP                                                           \
 WebGLContext::name##_array(nsIWebGLUniformLocation *ploc, JSObject *wa) \
 {                                                                       \
     if (mContextLost)                                                   \
         return NS_OK;                                                   \
     OBTAIN_UNIFORM_LOCATION(#name ": location")                         \
-    if (!wa || JS_GetTypedArrayType(wa) != js::TypedArray::arrayType)   \
+    if (!wa || JS_GetTypedArrayType(wa) != arrayType)   \
         return ErrorInvalidOperation(#name ": array must be " #arrayType);      \
     if (JS_GetTypedArrayLength(wa) == 0 || JS_GetTypedArrayLength(wa) % cnt != 0)\
         return ErrorInvalidValue(#name ": array must be > 0 elements and have a length multiple of %d", cnt); \
     MakeContextCurrent();                                               \
     gl->f##name(location, JS_GetTypedArrayLength(wa) / cnt, (ptrType *)JS_GetTypedArrayData(wa));            \
     return NS_OK;                                                       \
 }
 
@@ -4122,17 +4122,17 @@ WebGLContext::name(int32_t) {           
      return NS_ERROR_NOT_IMPLEMENTED;                                   \
 }                                                                       \
 NS_IMETHODIMP                                                           \
 WebGLContext::name##_array(nsIWebGLUniformLocation *ploc, WebGLboolean transpose, JSObject *wa)  \
 {                                                                       \
     if (mContextLost)                                                   \
         return NS_OK;                                                   \
     OBTAIN_UNIFORM_LOCATION(#name ": location")                         \
-    if (!wa || JS_GetTypedArrayType(wa) != js::TypedArray::arrayType)                   \
+    if (!wa || JS_GetTypedArrayType(wa) != arrayType)                   \
         return ErrorInvalidValue(#name ": array must be " #arrayType);      \
     if (JS_GetTypedArrayLength(wa) == 0 || JS_GetTypedArrayLength(wa) % (dim*dim) != 0)                 \
         return ErrorInvalidValue(#name ": array length must be >0 and multiple of %d", dim*dim); \
     if (transpose)                                                      \
         return ErrorInvalidValue(#name ": transpose must be FALSE as per the OpenGL ES 2.0 spec"); \
     MakeContextCurrent();                                               \
     gl->f##name(location, JS_GetTypedArrayLength(wa) / (dim*dim), transpose, (ptrType *)JS_GetTypedArrayData(wa)); \
     return NS_OK;                                                       \
@@ -4175,29 +4175,29 @@ SIMPLE_METHOD_UNIFORM_2(Uniform2i, Unifo
 SIMPLE_METHOD_UNIFORM_3(Uniform3i, Uniform3i, WebGLint, WebGLint, WebGLint)
 SIMPLE_METHOD_UNIFORM_4(Uniform4i, Uniform4i, WebGLint, WebGLint, WebGLint, WebGLint)
 
 SIMPLE_METHOD_UNIFORM_1(Uniform1f, Uniform1f, WebGLfloat)
 SIMPLE_METHOD_UNIFORM_2(Uniform2f, Uniform2f, WebGLfloat, WebGLfloat)
 SIMPLE_METHOD_UNIFORM_3(Uniform3f, Uniform3f, WebGLfloat, WebGLfloat, WebGLfloat)
 SIMPLE_METHOD_UNIFORM_4(Uniform4f, Uniform4f, WebGLfloat, WebGLfloat, WebGLfloat, WebGLfloat)
 
-SIMPLE_ARRAY_METHOD_UNIFORM(Uniform1iv, 1, TYPE_INT32, WebGLint)
-SIMPLE_ARRAY_METHOD_UNIFORM(Uniform2iv, 2, TYPE_INT32, WebGLint)
-SIMPLE_ARRAY_METHOD_UNIFORM(Uniform3iv, 3, TYPE_INT32, WebGLint)
-SIMPLE_ARRAY_METHOD_UNIFORM(Uniform4iv, 4, TYPE_INT32, WebGLint)
-
-SIMPLE_ARRAY_METHOD_UNIFORM(Uniform1fv, 1, TYPE_FLOAT32, WebGLfloat)
-SIMPLE_ARRAY_METHOD_UNIFORM(Uniform2fv, 2, TYPE_FLOAT32, WebGLfloat)
-SIMPLE_ARRAY_METHOD_UNIFORM(Uniform3fv, 3, TYPE_FLOAT32, WebGLfloat)
-SIMPLE_ARRAY_METHOD_UNIFORM(Uniform4fv, 4, TYPE_FLOAT32, WebGLfloat)
-
-SIMPLE_MATRIX_METHOD_UNIFORM(UniformMatrix2fv, 2, TYPE_FLOAT32, WebGLfloat)
-SIMPLE_MATRIX_METHOD_UNIFORM(UniformMatrix3fv, 3, TYPE_FLOAT32, WebGLfloat)
-SIMPLE_MATRIX_METHOD_UNIFORM(UniformMatrix4fv, 4, TYPE_FLOAT32, WebGLfloat)
+SIMPLE_ARRAY_METHOD_UNIFORM(Uniform1iv, 1, TYPEDARRAY_INT32, WebGLint)
+SIMPLE_ARRAY_METHOD_UNIFORM(Uniform2iv, 2, TYPEDARRAY_INT32, WebGLint)
+SIMPLE_ARRAY_METHOD_UNIFORM(Uniform3iv, 3, TYPEDARRAY_INT32, WebGLint)
+SIMPLE_ARRAY_METHOD_UNIFORM(Uniform4iv, 4, TYPEDARRAY_INT32, WebGLint)
+
+SIMPLE_ARRAY_METHOD_UNIFORM(Uniform1fv, 1, TYPEDARRAY_FLOAT32, WebGLfloat)
+SIMPLE_ARRAY_METHOD_UNIFORM(Uniform2fv, 2, TYPEDARRAY_FLOAT32, WebGLfloat)
+SIMPLE_ARRAY_METHOD_UNIFORM(Uniform3fv, 3, TYPEDARRAY_FLOAT32, WebGLfloat)
+SIMPLE_ARRAY_METHOD_UNIFORM(Uniform4fv, 4, TYPEDARRAY_FLOAT32, WebGLfloat)
+
+SIMPLE_MATRIX_METHOD_UNIFORM(UniformMatrix2fv, 2, TYPEDARRAY_FLOAT32, WebGLfloat)
+SIMPLE_MATRIX_METHOD_UNIFORM(UniformMatrix3fv, 3, TYPEDARRAY_FLOAT32, WebGLfloat)
+SIMPLE_MATRIX_METHOD_UNIFORM(UniformMatrix4fv, 4, TYPEDARRAY_FLOAT32, WebGLfloat)
 
 NS_IMETHODIMP
 WebGLContext::VertexAttrib1f(uint32_t index, WebGLfloat x0)
 {
     if (mContextLost)
         return NS_OK;
 
     MakeContextCurrent();
@@ -4288,17 +4288,17 @@ NS_IMETHODIMP                           
 WebGLContext::name(int32_t) {                                     \
      return NS_ERROR_NOT_IMPLEMENTED;                                   \
 }                                                                       \
 NS_IMETHODIMP                                                           \
 WebGLContext::name##_array(WebGLuint idx, JSObject *wa)           \
 {                                                                       \
     if (mContextLost)                                                   \
         return NS_OK;                                                   \
-    if (!wa || JS_GetTypedArrayType(wa) != js::TypedArray::arrayType)                   \
+    if (!wa || JS_GetTypedArrayType(wa) != arrayType)                   \
         return ErrorInvalidOperation(#name ": array must be " #arrayType); \
     if (JS_GetTypedArrayLength(wa) < cnt)                                               \
         return ErrorInvalidOperation(#name ": array must be >= %d elements", cnt); \
     MakeContextCurrent();                                               \
     ptrType *ptr = (ptrType *)JS_GetTypedArrayData(wa);                                  \
     if (idx) {                                                        \
         gl->f##name(idx, ptr);                                          \
     } else {                                                            \
@@ -4307,20 +4307,20 @@ WebGLContext::name##_array(WebGLuint idx
         mVertexAttrib0Vector[2] = cnt > 2 ? ptr[2] : ptrType(0);        \
         mVertexAttrib0Vector[3] = cnt > 3 ? ptr[3] : ptrType(1);        \
         if (gl->IsGLES2())                                              \
             gl->f##name(idx, ptr);                                      \
     }                                                                   \
     return NS_OK;                                                       \
 }
 
-SIMPLE_ARRAY_METHOD_NO_COUNT(VertexAttrib1fv, 1, TYPE_FLOAT32, WebGLfloat)
-SIMPLE_ARRAY_METHOD_NO_COUNT(VertexAttrib2fv, 2, TYPE_FLOAT32, WebGLfloat)
-SIMPLE_ARRAY_METHOD_NO_COUNT(VertexAttrib3fv, 3, TYPE_FLOAT32, WebGLfloat)
-SIMPLE_ARRAY_METHOD_NO_COUNT(VertexAttrib4fv, 4, TYPE_FLOAT32, WebGLfloat)
+SIMPLE_ARRAY_METHOD_NO_COUNT(VertexAttrib1fv, 1, TYPEDARRAY_FLOAT32, WebGLfloat)
+SIMPLE_ARRAY_METHOD_NO_COUNT(VertexAttrib2fv, 2, TYPEDARRAY_FLOAT32, WebGLfloat)
+SIMPLE_ARRAY_METHOD_NO_COUNT(VertexAttrib3fv, 3, TYPEDARRAY_FLOAT32, WebGLfloat)
+SIMPLE_ARRAY_METHOD_NO_COUNT(VertexAttrib4fv, 4, TYPEDARRAY_FLOAT32, WebGLfloat)
 
 NS_IMETHODIMP
 WebGLContext::UseProgram(nsIWebGLProgram *pobj)
 {
     if (mContextLost)
         return NS_OK;
 
     WebGLProgram *prog;
diff --git a/content/canvas/src/WebGLContextValidate.cpp b/content/canvas/src/WebGLContextValidate.cpp
--- a/content/canvas/src/WebGLContextValidate.cpp
+++ b/content/canvas/src/WebGLContextValidate.cpp
@@ -38,17 +38,17 @@
  * ***** END LICENSE BLOCK ***** */
 
 #include "WebGLContext.h"
 
 #include "mozilla/Preferences.h"
 
 #include "CheckedInt.h"
 
-#include "jstypedarray.h"
+#include "jsfriendapi.h"
 
 #if defined(USE_ANGLE)
 #include "angle/ShaderLang.h"
 #endif
 
 using namespace mozilla;
 
 /*
diff --git a/content/events/src/nsDOMNotifyAudioAvailableEvent.cpp b/content/events/src/nsDOMNotifyAudioAvailableEvent.cpp
--- a/content/events/src/nsDOMNotifyAudioAvailableEvent.cpp
+++ b/content/events/src/nsDOMNotifyAudioAvailableEvent.cpp
@@ -35,17 +35,17 @@
  * the provisions above, a recipient may use your version of this file under
  * the terms of any one of the MPL, the GPL or the LGPL.
  *
  * ***** END LICENSE BLOCK ***** */
 
 #include "nsDOMNotifyAudioAvailableEvent.h"
 #include "nsDOMClassInfoID.h" // DOMCI_DATA, NS_DOM_INTERFACE_MAP_ENTRY_CLASSINFO
 #include "nsContentUtils.h" // NS_DROP_JS_OBJECTS
-#include "jstypedarray.h"
+#include "jsfriendapi.h"
 
 nsDOMNotifyAudioAvailableEvent::nsDOMNotifyAudioAvailableEvent(nsPresContext* aPresContext,
                                                                nsEvent* aEvent,
                                                                uint32_t aEventType,
                                                                float* aFrameBuffer,
                                                                uint32_t aFrameBufferLength,
                                                                float aTime)
   : nsDOMEvent(aPresContext, aEvent),
diff --git a/content/html/content/src/nsHTMLAudioElement.cpp b/content/html/content/src/nsHTMLAudioElement.cpp
--- a/content/html/content/src/nsHTMLAudioElement.cpp
+++ b/content/html/content/src/nsHTMLAudioElement.cpp
@@ -54,17 +54,16 @@
 #include "prlock.h"
 #include "nsThreadUtils.h"
 
 #include "nsIScriptSecurityManager.h"
 #include "nsIXPConnect.h"
 #include "jsapi.h"
 #include "jscntxt.h"
 #include "jsfriendapi.h"
-#include "jstypedarray.h"
 #include "nsJSUtils.h"
 
 #include "nsITimer.h"
 
 #include "nsEventDispatcher.h"
 #include "nsIDOMProgressEvent.h"
 #include "nsContentUtils.h"
 
diff --git a/dom/workers/FileReaderSync.cpp b/dom/workers/FileReaderSync.cpp
--- a/dom/workers/FileReaderSync.cpp
+++ b/dom/workers/FileReaderSync.cpp
@@ -39,17 +39,17 @@
 
 #include "FileReaderSync.h"
 
 #include "nsIDOMFile.h"
 
 #include "jsapi.h"
 #include "jsatom.h"
 #include "jscntxt.h"
-#include "jstypedarray.h"
+#include "jsfriendapi.h"
 #include "nsJSUtils.h"
 #include "xpcprivate.h"
 
 #include "Exceptions.h"
 #include "File.h"
 #include "FileReaderSyncPrivate.h"
 #include "WorkerInlines.h"
 
diff --git a/dom/workers/XMLHttpRequestPrivate.cpp b/dom/workers/XMLHttpRequestPrivate.cpp
--- a/dom/workers/XMLHttpRequestPrivate.cpp
+++ b/dom/workers/XMLHttpRequestPrivate.cpp
@@ -40,17 +40,17 @@
 
 #include "nsIDOMEvent.h"
 #include "nsIDOMEventListener.h"
 #include "nsIDOMProgressEvent.h"
 #include "nsIRunnable.h"
 #include "nsIXMLHttpRequest.h"
 #include "nsIXPConnect.h"
 
-#include "jstypedarray.h"
+#include "jsfriendapi.h"
 #include "nsAutoPtr.h"
 #include "nsCOMPtr.h"
 #include "nsContentUtils.h"
 #include "nsJSUtils.h"
 #include "nsThreadUtils.h"
 #include "nsXMLHttpRequest.h"
 
 #include "Events.h"
diff --git a/js/src/Makefile.in b/js/src/Makefile.in
--- a/js/src/Makefile.in
+++ b/js/src/Makefile.in
@@ -216,17 +216,16 @@ INSTALLED_HEADERS = \
 		jspropertytree.h \
 		jsproto.tbl \
 		jsprvtd.h \
 		jspubtd.h \
 		jsreflect.h \
 		jsstdint.h \
 		jsstr.h \
 		jstracer.h \
-		jstypedarray.h \
 		jstypes.h \
 		jsutil.h \
 		jsversion.h \
 		jswrapper.h \
 		jsxdrapi.h \
 		jsval.h \
 		prmjtime.h \
 		$(NULL)
diff --git a/js/src/jsfriendapi.h b/js/src/jsfriendapi.h
--- a/js/src/jsfriendapi.h
+++ b/js/src/jsfriendapi.h
@@ -367,9 +367,92 @@ StringIsArrayIndex(JSLinearString *str, 
 #define JSITER_HIDDEN     0x10  /* also enumerate non-enumerable properties */
 
 /* When defining functions, JSFunctionSpec::call points to a JSNativeTraceInfo. */
 #define JSFUN_TRCINFO     0x2000
 
 } /* namespace js */
 #endif
 
+/* Typed Arrays */
+
+JS_FRIEND_API(JSObject *)
+js_InitTypedArrayClasses(JSContext *cx, JSObject *obj);
+
+JS_FRIEND_API(JSBool)
+js_IsTypedArray(JSObject *obj);
+
+JS_FRIEND_API(JSBool)
+js_IsArrayBuffer(JSObject *obj);
+
+JS_FRIEND_API(JSObject *)
+js_CreateArrayBuffer(JSContext *cx, jsuint nbytes);
+
+/*
+ * Create a new typed array of type atype (one of the TypedArray
+ * enumerant values above), with nelements elements.
+ */
+JS_FRIEND_API(JSObject *)
+js_CreateTypedArray(JSContext *cx, jsint atype, jsuint nelements);
+
+/*
+ * Create a new typed array of type atype (one of the TypedArray
+ * enumerant values above), and copy in values from the given JSObject,
+ * which must either be a typed array or an array-like object.
+ */
+JS_FRIEND_API(JSObject *)
+js_CreateTypedArrayWithArray(JSContext *cx, jsint atype, JSObject *arrayArg);
+
+/*
+ * Create a new typed array of type atype (one of the TypedArray
+ * enumerant values above), using a given ArrayBuffer for storage.
+ * The byteoffset and length values are optional; if -1 is passed, an
+ * offset of 0 and enough elements to use up the remainder of the byte
+ * array are used as the default values.
+ */
+JS_FRIEND_API(JSObject *)
+js_CreateTypedArrayWithBuffer(JSContext *cx, jsint atype, JSObject *bufArg,
+                              jsint byteoffset, jsint length);
+
+JS_FRIEND_API(JSUint32)
+JS_GetArrayBufferByteLength(JSObject *obj);
+
+JS_FRIEND_API(uint8 *)
+JS_GetArrayBufferData(JSObject *obj);
+
+JS_FRIEND_API(JSUint32)
+JS_GetTypedArrayLength(JSObject *obj);
+
+JS_FRIEND_API(JSUint32)
+JS_GetTypedArrayByteOffset(JSObject *obj);
+
+JS_FRIEND_API(JSUint32)
+JS_GetTypedArrayByteLength(JSObject *obj);
+
+enum {
+    TYPEDARRAY_INT8 = 0,
+    TYPEDARRAY_UINT8,
+    TYPEDARRAY_INT16,
+    TYPEDARRAY_UINT16,
+    TYPEDARRAY_INT32,
+    TYPEDARRAY_UINT32,
+    TYPEDARRAY_FLOAT32,
+    TYPEDARRAY_FLOAT64,
+
+    /*
+     * Special type that's a uint8, but assignments are clamped to 0 .. 255.
+     * Treat the raw data type as a uint8.
+     */
+    TYPEDARRAY_UINT8_CLAMPED,
+
+    TYPEDARRAY_MAX
+};
+
+JS_FRIEND_API(JSUint32)
+JS_GetTypedArrayType(JSObject *obj);
+
+JS_FRIEND_API(JSObject *)
+JS_GetTypedArrayBuffer(JSObject *obj);
+
+JS_FRIEND_API(void *)
+JS_GetTypedArrayData(JSObject *obj);
+
 #endif /* jsfriendapi_h___ */
diff --git a/js/src/jstypedarray.h b/js/src/jstypedarray.h
--- a/js/src/jstypedarray.h
+++ b/js/src/jstypedarray.h
@@ -278,76 +278,12 @@ struct JS_FRIEND_API(TypedArray) {
     static int dataOffset();
 };
 
 extern bool
 IsFastTypedArrayClass(const Class *clasp);
 
 } // namespace js
 
-/* Friend API methods */
-
-JS_FRIEND_API(JSObject *)
-js_InitTypedArrayClasses(JSContext *cx, JSObject *obj);
-
-JS_FRIEND_API(JSBool)
-js_IsTypedArray(JSObject *obj);
-
-JS_FRIEND_API(JSBool)
-js_IsArrayBuffer(JSObject *obj);
-
-JS_FRIEND_API(JSObject *)
-js_CreateArrayBuffer(JSContext *cx, jsuint nbytes);
-
-/*
- * Create a new typed array of type atype (one of the TypedArray
- * enumerant values above), with nelements elements.
- */
-JS_FRIEND_API(JSObject *)
-js_CreateTypedArray(JSContext *cx, jsint atype, jsuint nelements);
-
-/*
- * Create a new typed array of type atype (one of the TypedArray
- * enumerant values above), and copy in values from the given JSObject,
- * which must either be a typed array or an array-like object.
- */
-JS_FRIEND_API(JSObject *)
-js_CreateTypedArrayWithArray(JSContext *cx, jsint atype, JSObject *arrayArg);
-
-/*
- * Create a new typed array of type atype (one of the TypedArray
- * enumerant values above), using a given ArrayBuffer for storage.
- * The byteoffset and length values are optional; if -1 is passed, an
- * offset of 0 and enough elements to use up the remainder of the byte
- * array are used as the default values.
- */
-JS_FRIEND_API(JSObject *)
-js_CreateTypedArrayWithBuffer(JSContext *cx, jsint atype, JSObject *bufArg,
-                              jsint byteoffset, jsint length);
-
 extern int32 JS_FASTCALL
 js_TypedArray_uint8_clamp_double(const double x);
 
-JS_FRIEND_API(JSUint32)
-JS_GetArrayBufferByteLength(JSObject *obj);
-
-JS_FRIEND_API(uint8 *)
-JS_GetArrayBufferData(JSObject *obj);
-
-JS_FRIEND_API(JSUint32)
-JS_GetTypedArrayLength(JSObject *obj);
-
-JS_FRIEND_API(JSUint32)
-JS_GetTypedArrayByteOffset(JSObject *obj);
-
-JS_FRIEND_API(JSUint32)
-JS_GetTypedArrayByteLength(JSObject *obj);
-
-JS_FRIEND_API(JSUint32)
-JS_GetTypedArrayType(JSObject *obj);
-
-JS_FRIEND_API(JSObject *)
-JS_GetTypedArrayBuffer(JSObject *obj);
-
-JS_FRIEND_API(void *)
-JS_GetTypedArrayData(JSObject *obj);
-
 #endif /* jstypedarray_h */
