From: Ms2ger <ms2ger@gmail.com>

diff --git a/content/media/nsMediaCache.cpp b/content/media/nsMediaCache.cpp
--- a/content/media/nsMediaCache.cpp
+++ b/content/media/nsMediaCache.cpp
@@ -31,30 +31,31 @@
  * use your version of this file under the terms of the MPL, indicate your
  * decision by deleting the provisions above and replace them with the notice
  * and other provisions required by the GPL or the LGPL. If you do not delete
  * the provisions above, a recipient may use your version of this file under
  * the terms of any one of the MPL, the GPL or the LGPL.
  *
  * ***** END LICENSE BLOCK ***** */
 
-#include "mozilla/ReentrantMonitor.h"
-#include "mozilla/XPCOM.h"
+#include "nsMediaCache.h"
 
-#include "nsMediaCache.h"
 #include "nsDirectoryServiceUtils.h"
 #include "nsDirectoryServiceDefs.h"
 #include "nsNetUtil.h"
 #include "prio.h"
 #include "nsThreadUtils.h"
 #include "nsMediaStream.h"
 #include "nsMathUtils.h"
 #include "prlog.h"
 #include "nsIPrivateBrowsingService.h"
+#include "nsIObserverService.h"
 #include "mozilla/Preferences.h"
+#include "mozilla/ReentrantMonitor.h"
+#include "mozilla/TimeStamp.h"
 
 using namespace mozilla;
 
 #ifdef PR_LOGGING
 PRLogModuleInfo* gMediaCacheLog;
 #define LOG(type, msg) PR_LOG(gMediaCacheLog, type, msg)
 #else
 #define LOG(type, msg)
diff --git a/content/media/nsMediaCache.h b/content/media/nsMediaCache.h
--- a/content/media/nsMediaCache.h
+++ b/content/media/nsMediaCache.h
@@ -35,16 +35,18 @@
  * the terms of any one of the MPL, the GPL or the LGPL.
  *
  * ***** END LICENSE BLOCK ***** */
 
 #ifndef nsMediaCache_h_
 #define nsMediaCache_h_
 
 #include "nsTArray.h"
+#include "nsTHashtable.h"
+#include "nsHashKeys.h"
 #include "nsIPrincipal.h"
 #include "nsCOMPtr.h"
 
 class nsByteRange;
 namespace mozilla {
 class ReentrantMonitorAutoEnter;
 }
 
diff --git a/content/media/nsMediaDecoder.h b/content/media/nsMediaDecoder.h
--- a/content/media/nsMediaDecoder.h
+++ b/content/media/nsMediaDecoder.h
@@ -33,27 +33,28 @@
  * and other provisions required by the GPL or the LGPL. If you do not delete
  * the provisions above, a recipient may use your version of this file under
  * the terms of any one of the MPL, the GPL or the LGPL.
  *
  * ***** END LICENSE BLOCK ***** */
 #if !defined(nsMediaDecoder_h_)
 #define nsMediaDecoder_h_
 
-#include "mozilla/XPCOM.h"
+#include "nsIObserver.h"
 
 #include "nsIPrincipal.h"
 #include "nsSize.h"
 #include "prlog.h"
 #include "gfxContext.h"
 #include "gfxRect.h"
 #include "nsITimer.h"
 #include "ImageLayers.h"
+#include "mozilla/Mutex.h"
 #include "mozilla/ReentrantMonitor.h"
-#include "mozilla/Mutex.h"
+#include "mozilla/TimeStamp.h"
 
 class nsHTMLMediaElement;
 class nsMediaStream;
 class nsIStreamListener;
 class nsTimeRanges;
 
 // The size to use for audio data frames in MozAudioAvailable events.
 // This value is per channel, and is chosen to give ~43 fps of events,
diff --git a/content/media/nsMediaStream.h b/content/media/nsMediaStream.h
--- a/content/media/nsMediaStream.h
+++ b/content/media/nsMediaStream.h
@@ -34,24 +34,26 @@
  * the terms of any one of the MPL, the GPL or the LGPL.
  *
  * ***** END LICENSE BLOCK ***** */
 
 #if !defined(nsMediaStream_h_)
 #define nsMediaStream_h_
 
 #include "mozilla/Mutex.h"
-#include "mozilla/XPCOM.h"
+#include "mozilla/TimeStamp.h"
 #include "nsIChannel.h"
 #include "nsIPrincipal.h"
 #include "nsIURI.h"
 #include "nsIStreamListener.h"
 #include "nsIChannelEventSink.h"
 #include "nsIInterfaceRequestor.h"
 #include "nsMediaCache.h"
+#include "nsAutoPtr.h"
+#include "nsThreadUtils.h" // nsRevocableEventPtr
 
 // For HTTP seeking, if number of bytes needing to be
 // seeked forward is less than this value then a read is
 // done rather than a byte range request.
 #define SEEK_VS_READ_THRESHOLD (32*1024)
 
 #define HTTP_REQUESTED_RANGE_NOT_SATISFIABLE_CODE 416
 
diff --git a/content/media/ogg/nsOggReader.cpp b/content/media/ogg/nsOggReader.cpp
--- a/content/media/ogg/nsOggReader.cpp
+++ b/content/media/ogg/nsOggReader.cpp
@@ -39,16 +39,17 @@
  * ***** END LICENSE BLOCK ***** */
 #include "nsError.h"
 #include "nsBuiltinDecoderStateMachine.h"
 #include "nsBuiltinDecoder.h"
 #include "nsOggReader.h"
 #include "VideoUtils.h"
 #include "theora/theoradec.h"
 #include "nsTimeRanges.h"
+#include "nsISeekableStream.h"
 #include "mozilla/TimeStamp.h"
 
 using namespace mozilla;
 
 // Un-comment to enable logging of seek bisections.
 //#define SEEK_LOGGING
 
 #ifdef PR_LOGGING
diff --git a/content/media/wave/nsWaveReader.cpp b/content/media/wave/nsWaveReader.cpp
--- a/content/media/wave/nsWaveReader.cpp
+++ b/content/media/wave/nsWaveReader.cpp
@@ -37,16 +37,17 @@
  * ***** END LICENSE BLOCK ***** */
 #include "nsError.h"
 #include "nsBuiltinDecoderStateMachine.h"
 #include "nsBuiltinDecoder.h"
 #include "nsMediaStream.h"
 #include "nsWaveReader.h"
 #include "nsTimeRanges.h"
 #include "VideoUtils.h"
+#include "nsISeekableStream.h"
 
 using namespace mozilla;
 
 // Un-comment to enable logging of seek bisections.
 //#define SEEK_LOGGING
 
 #ifdef PR_LOGGING
 extern PRLogModuleInfo* gBuiltinDecoderLog;
diff --git a/dom/base/nsGlobalWindow.h b/dom/base/nsGlobalWindow.h
--- a/dom/base/nsGlobalWindow.h
+++ b/dom/base/nsGlobalWindow.h
@@ -38,71 +38,71 @@
  * the provisions above, a recipient may use your version of this file under
  * the terms of any one of the MPL, the GPL or the LGPL.
  *
  * ***** END LICENSE BLOCK ***** */
 
 #ifndef nsGlobalWindow_h___
 #define nsGlobalWindow_h___
 
-#include "mozilla/XPCOM.h" // for TimeStamp/TimeDuration
+#include "nsPIDOMWindow.h"
+#include "nsIScriptGlobalObject.h"
+#include "nsIDOMJSWindow.h"
+#include "nsIScriptObjectPrincipal.h"
+#include "nsIDOMEventTarget.h"
+#include "nsIDOMStorageIndexedDB.h"
+#include "nsWeakReference.h"
+#include "nsIInterfaceRequestor.h"
+#include "nsWrapperCache.h"
+#include "prclist.h"
 
 // Local Includes
 // Helper Classes
 #include "nsCOMPtr.h"
 #include "nsAutoPtr.h"
-#include "nsWeakReference.h"
 #include "nsHashtable.h"
 #include "nsDataHashtable.h"
 #include "nsCycleCollectionParticipant.h"
 #include "nsDOMScriptObjectHolder.h"
 
 // Interfaces Needed
 #include "nsDOMWindowList.h"
 #include "nsIBaseWindow.h"
 #include "nsIBrowserDOMWindow.h"
 #include "nsIDocShellTreeOwner.h"
 #include "nsIDocShellTreeItem.h"
 #include "nsIDOMClientInformation.h"
-#include "nsIDOMEventTarget.h"
 #include "nsIDOMNavigator.h"
 #include "nsIDOMNavigatorGeolocation.h"
 #include "nsIDOMNavigatorDesktopNotification.h"
 #include "nsIDOMLocation.h"
-#include "nsIInterfaceRequestor.h"
 #include "nsIInterfaceRequestorUtils.h"
-#include "nsIDOMJSWindow.h"
 #include "nsIDOMChromeWindow.h"
-#include "nsIScriptGlobalObject.h"
 #include "nsIScriptContext.h"
-#include "nsIScriptObjectPrincipal.h"
 #include "nsIScriptTimeoutHandler.h"
 #include "nsITimer.h"
 #include "nsIWebBrowserChrome.h"
-#include "nsPIDOMWindow.h"
 #include "nsIDOMModalContentWindow.h"
 #include "nsIScriptSecurityManager.h"
 #include "nsEventListenerManager.h"
 #include "nsIDOMDocument.h"
 #ifndef MOZ_DISABLE_DOMCRYPTO
 #include "nsIDOMCrypto.h"
 #endif
 #include "nsIPrincipal.h"
 #include "nsPluginArray.h"
 #include "nsMimeTypeArray.h"
 #include "nsIXPCScriptable.h"
 #include "nsPoint.h"
 #include "nsSize.h"
 #include "nsRect.h"
 #include "mozFlushType.h"
-#include "prclist.h"
 #include "nsIDOMStorageObsolete.h"
 #include "nsIDOMStorageList.h"
 #include "nsIDOMStorageEvent.h"
-#include "nsIDOMStorageIndexedDB.h"
 #include "nsIDOMOfflineResourceList.h"
 #include "nsIArray.h"
 #include "nsIContent.h"
 #include "nsIIDBFactory.h"
 #include "nsFrameMessageManager.h"
 #include "mozilla/TimeStamp.h"
 
 // JS includes
diff --git a/dom/base/nsJSEnvironment.cpp b/dom/base/nsJSEnvironment.cpp
--- a/dom/base/nsJSEnvironment.cpp
+++ b/dom/base/nsJSEnvironment.cpp
@@ -92,16 +92,17 @@
 #include "jsxdrapi.h"
 #include "nsIArray.h"
 #include "nsIObjectInputStream.h"
 #include "nsIObjectOutputStream.h"
 #include "nsDOMScriptObjectHolder.h"
 #include "prmem.h"
 #include "WrapperFactory.h"
 #include "nsGlobalWindow.h"
+#include "nsTextFormatter.h"
 
 #ifdef XP_MACOSX
 // AssertMacros.h defines 'check' and conflicts with AccessCheck.h
 #undef check
 #endif
 #include "AccessCheck.h"
 
 #ifdef MOZ_JSDEBUGGER
