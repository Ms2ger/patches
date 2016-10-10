From: Ms2ger <ms2ger@gmail.com>

diff --git a/xpcom/base/PRUnichar.h b/xpcom/base/PRUnichar.h
new file mode 100644
--- /dev/null
+++ b/xpcom/base/PRUnichar.h
@@ -0,0 +1,30 @@
+/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
+/* vim: set ts=8 sts=2 et sw=2 tw=80: */
+/* This Source Code Form is subject to the terms of the Mozilla Public
+ * License, v. 2.0. If a copy of the MPL was not distributed with this
+ * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
+
+#ifndef PRUnichar_h
+#define PRUnichar_h
+
+#include "mozilla/StandardInteger.h"
+
+/* under VC++ (Windows), we don't have autoconf yet */
+#if defined(_MSC_VER)
+#define HAVE_CPP_2BYTE_WCHAR_T
+#endif
+
+#ifndef __PRUNICHAR__
+#define __PRUNICHAR__
+/* For now, don't use wchar_t on Unix because it breaks the Netscape
+ * commercial build.  When this is fixed there will be no need for the
+ * |reinterpret_cast| in nsLiteralString.h either.
+ */
+#if defined(HAVE_CPP_2BYTE_WCHAR_T) && defined(XP_WIN)
+typedef wchar_t PRUnichar;
+#else
+typedef uint16_t PRUnichar;
+#endif
+#endif
+
+#endif // PRUnichar_h
diff --git a/xpcom/base/moz.build b/xpcom/base/moz.build
--- a/xpcom/base/moz.build
+++ b/xpcom/base/moz.build
@@ -39,16 +39,17 @@ if CONFIG['MOZ_WIDGET_TOOLKIT'] == 'coco
 XPIDL_MODULE = 'xpcom_base'
 
 MODULE = 'xpcom'
 
 EXPORTS += [
     'ErrorList.h',
     'ErrorListCDefines.h',
     'ErrorListCxxDefines.h',
+    'PRUnichar.h',
     'nsAgg.h',
     'nsAtomicRefcnt.h',
     'nsAutoPtr.h',
     'nsAutoRef.h',
     'nsCom.h',
     'nsCycleCollector.h',
     'nsDebugImpl.h',
     'nsError.h',
diff --git a/xpcom/base/nscore.h b/xpcom/base/nscore.h
--- a/xpcom/base/nscore.h
+++ b/xpcom/base/nscore.h
@@ -18,21 +18,21 @@
 #if !defined(XPCOM_GLUE) && !defined(NS_NO_XPCOM) && !defined(MOZ_NO_MOZALLOC)
 #  include "mozilla/mozalloc.h"
 #  include "mozilla/mozalloc_macro_wrappers.h"
 #endif
 
 /**
  * Incorporate the integer data types which XPCOM uses.
  */
+#include "mozilla/NullPtr.h"
 #include "mozilla/StandardInteger.h"
+#include "PRUnichar.h"
 #include "stddef.h"
 
-#include "mozilla/NullPtr.h"
-
 /* Core XPCOM declarations. */
 
 /*----------------------------------------------------------------------*/
 /* Import/export defines */
 
 /**
  * Using the visibility("hidden") attribute allows the compiler to use
  * PC-relative addressing to call this function.  If a function does not
@@ -317,33 +317,16 @@
 typedef unsigned long nsrefcnt;
 #else
 typedef uint32_t nsrefcnt;
 #endif
 
 /* ------------------------------------------------------------------------ */
 /* Casting macros for hiding C++ features from older compilers */
 
-  /* under VC++ (Windows), we don't have autoconf yet */
-#if defined(_MSC_VER)
-  #define HAVE_CPP_2BYTE_WCHAR_T
-#endif
-
-#ifndef __PRUNICHAR__
-#define __PRUNICHAR__
-  /* For now, don't use wchar_t on Unix because it breaks the Netscape
-   * commercial build.  When this is fixed there will be no need for the
-   * |reinterpret_cast| in nsLiteralString.h either.
-   */
-  #if defined(HAVE_CPP_2BYTE_WCHAR_T) && defined(XP_WIN)
-    typedef wchar_t PRUnichar;
-  #else
-    typedef uint16_t PRUnichar;
-  #endif
-#endif
 
 /*
  * Use these macros to do 64bit safe pointer conversions.
  */
 
 #define NS_PTR_TO_INT32(x)  ((int32_t)  (intptr_t) (x))
 #define NS_PTR_TO_UINT32(x) ((uint32_t) (intptr_t) (x))
 #define NS_INT32_TO_PTR(x)  ((void *)   (intptr_t) (x))
