From: Ms2ger <ms2ger@gmail.com>

diff --git a/content/base/src/nsDOMFile.cpp b/content/base/src/nsDOMFile.cpp
--- a/content/base/src/nsDOMFile.cpp
+++ b/content/base/src/nsDOMFile.cpp
@@ -60,17 +60,16 @@
 #include "nsNetUtil.h"
 #include "nsIUUIDGenerator.h"
 #include "nsBlobProtocolHandler.h"
 #include "nsStringStream.h"
 #include "nsJSUtils.h"
 #include "mozilla/CheckedInt.h"
 #include "mozilla/Preferences.h"
 
-#include "plbase64.h"
 #include "prmem.h"
 #include "dombindings.h"
 
 using namespace mozilla;
 using namespace mozilla::dom;
 
 // XXXkhuey the input stream that we pass out of a DOMFile
 // can outlive the actual DOMFile object.  Thus, we must
diff --git a/content/html/content/src/nsHTMLAudioElement.cpp b/content/html/content/src/nsHTMLAudioElement.cpp
--- a/content/html/content/src/nsHTMLAudioElement.cpp
+++ b/content/html/content/src/nsHTMLAudioElement.cpp
@@ -41,17 +41,16 @@
 #include "nsGenericHTMLElement.h"
 #include "nsGkAtoms.h"
 #include "nsSize.h"
 #include "nsIFrame.h"
 #include "nsIDocument.h"
 #include "nsIDOMDocument.h"
 #include "nsDOMError.h"
 #include "nsNodeInfoManager.h"
-#include "plbase64.h"
 #include "nsNetUtil.h"
 #include "prmem.h"
 #include "nsXPCOMStrings.h"
 #include "prlock.h"
 #include "nsThreadUtils.h"
 
 #include "nsIScriptSecurityManager.h"
 #include "nsIXPConnect.h"
diff --git a/content/html/content/src/nsHTMLVideoElement.cpp b/content/html/content/src/nsHTMLVideoElement.cpp
--- a/content/html/content/src/nsHTMLVideoElement.cpp
+++ b/content/html/content/src/nsHTMLVideoElement.cpp
@@ -44,17 +44,16 @@
 #include "nsGenericHTMLElement.h"
 #include "nsGkAtoms.h"
 #include "nsSize.h"
 #include "nsIFrame.h"
 #include "nsIDocument.h"
 #include "nsIDOMDocument.h"
 #include "nsDOMError.h"
 #include "nsNodeInfoManager.h"
-#include "plbase64.h"
 #include "nsNetUtil.h"
 #include "prmem.h"
 #include "nsXPCOMStrings.h"
 #include "prlock.h"
 #include "nsThreadUtils.h"
 
 #include "nsIScriptSecurityManager.h"
 #include "nsIXPConnect.h"
diff --git a/gfx/thebes/gfxWindowsPlatform.cpp b/gfx/thebes/gfxWindowsPlatform.cpp
--- a/gfx/thebes/gfxWindowsPlatform.cpp
+++ b/gfx/thebes/gfxWindowsPlatform.cpp
@@ -48,17 +48,16 @@
 #include "nsUnicharUtils.h"
 
 #include "mozilla/Preferences.h"
 #include "nsServiceManagerUtils.h"
 #include "nsTArray.h"
 
 #include "nsIWindowsRegKey.h"
 #include "nsILocalFile.h"
-#include "plbase64.h"
 #include "nsIXULRuntime.h"
 
 #include "nsIGfxInfo.h"
 
 #include "gfxCrashReporterUtils.h"
 
 #include "gfxGDIFontList.h"
 #include "gfxGDIFont.h"
diff --git a/modules/libpref/src/prefapi.cpp b/modules/libpref/src/prefapi.cpp
--- a/modules/libpref/src/prefapi.cpp
+++ b/modules/libpref/src/prefapi.cpp
@@ -49,17 +49,16 @@
   #include <sys/types.h>
 #endif
 #ifdef _WIN32
   #include "windows.h"
 #endif /* _WIN32 */
 
 #include "plstr.h"
 #include "pldhash.h"
-#include "plbase64.h"
 #include "prlog.h"
 #include "prmem.h"
 #include "prprf.h"
 #include "nsQuickSort.h"
 #include "nsString.h"
 #include "nsPrintfCString.h"
 #include "prlink.h"
 
diff --git a/netwerk/protocol/http/nsHttpDigestAuth.cpp b/netwerk/protocol/http/nsHttpDigestAuth.cpp
--- a/netwerk/protocol/http/nsHttpDigestAuth.cpp
+++ b/netwerk/protocol/http/nsHttpDigestAuth.cpp
@@ -46,17 +46,16 @@
 #include "nsIServiceManager.h"
 #include "nsXPCOM.h"
 #include "nsISupportsPrimitives.h"
 #include "nsIURI.h"
 #include "nsString.h"
 #include "nsReadableUtils.h"
 #include "nsEscape.h"
 #include "nsNetCID.h"
-#include "plbase64.h"
 #include "plstr.h"
 #include "prprf.h"
 #include "prmem.h"
 #include "nsCRT.h"
 
 //-----------------------------------------------------------------------------
 // nsHttpDigestAuth <public>
 //-----------------------------------------------------------------------------
diff --git a/security/manager/ssl/src/nsCRLManager.cpp b/security/manager/ssl/src/nsCRLManager.cpp
--- a/security/manager/ssl/src/nsCRLManager.cpp
+++ b/security/manager/ssl/src/nsCRLManager.cpp
@@ -58,17 +58,16 @@ extern "C" {
 #include "cert.h"
 #include "secerr.h"
 #include "nssb64.h"
 #include "secasn1.h"
 #include "secder.h"
 }
 #include "ssl.h"
 #include "ocsp.h"
-#include "plbase64.h"
 
 static NS_DEFINE_CID(kNSSComponentCID, NS_NSSCOMPONENT_CID);
 
 NS_IMPL_ISUPPORTS1(nsCRLManager, nsICRLManager)
 
 nsCRLManager::nsCRLManager()
 {
 }
