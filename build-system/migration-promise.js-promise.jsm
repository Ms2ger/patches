# vim: se ft=diff :
# HG changeset patch
# User Stefan Mirea <steven.mirea@gmail.com>
# Date 1371081891 25200
Bug 852411 - Migrating from promise.js to Promise.jsm; r?gps

# HG changeset patch
# User Stefan Mirea <steven.mirea@gmail.com>
# Date 1371081891 25200
#      Wed Jun 12 17:04:51 2013 -0700
# Node ID 62becf5deed7da0ad2b67c87319c0f4f467f34e2
# Parent  86413e921d5d5bdbefd6475619ebb0eab6185184
Bug 852411 - Migrating from promise.js to Promise.jsm; r?gps

diff --git a/services/common/bagheeraclient.js b/services/common/bagheeraclient.js
--- a/services/common/bagheeraclient.js
+++ b/services/common/bagheeraclient.js
@@ -17,17 +17,17 @@
   "BagheeraClient",
   "BagheeraClientRequestResult",
 ];
 
 const {classes: Cc, interfaces: Ci, utils: Cu} = Components;
 
 #endif
 
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://gre/modules/Services.jsm");
 Cu.import("resource://services-common/log4moz.js");
 Cu.import("resource://services-common/rest.js");
 Cu.import("resource://services-common/utils.js");
 
 
 /**
  * Represents the result of a Bagheera request.
diff --git a/services/common/utils.js b/services/common/utils.js
--- a/services/common/utils.js
+++ b/services/common/utils.js
@@ -1,17 +1,17 @@
 /* This Source Code Form is subject to the terms of the Mozilla Public
  * License, v. 2.0. If a copy of the MPL was not distributed with this file,
  * You can obtain one at http://mozilla.org/MPL/2.0/. */
 
 const {classes: Cc, interfaces: Ci, utils: Cu, results: Cr} = Components;
 
 this.EXPORTED_SYMBOLS = ["CommonUtils"];
 
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://gre/modules/Services.jsm");
 Cu.import("resource://gre/modules/XPCOMUtils.jsm");
 Cu.import("resource://gre/modules/osfile.jsm")
 Cu.import("resource://services-common/log4moz.js");
 
 this.CommonUtils = {
   exceptionStr: function exceptionStr(e) {
     if (!e) {
diff --git a/services/datareporting/policy.jsm b/services/datareporting/policy.jsm
--- a/services/datareporting/policy.jsm
+++ b/services/datareporting/policy.jsm
@@ -21,17 +21,17 @@
   "DataSubmissionRequest", // For test use only.
   "DataReportingPolicy",
 ];
 
 const {classes: Cc, interfaces: Ci, utils: Cu} = Components;
 
 #endif
 
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://services-common/log4moz.js");
 Cu.import("resource://services-common/utils.js");
 
 const MILLISECONDS_PER_DAY = 24 * 60 * 60 * 1000;
 
 // Used as a sanity lower bound for dates stored in prefs. This module was
 // implemented in 2012, so any earlier dates indicate an incorrect clock.
 const OLDEST_ALLOWED_YEAR = 2012;
@@ -64,42 +64,43 @@
  * user is away from the device). So, no event signaling implicit acceptance
  * is exposed.
  *
  * Receivers of instances of this type should treat it as a black box with
  * the exception of the on* functions.
  *
  * @param policy
  *        (DataReportingPolicy) The policy instance this request came from.
- * @param promise
+ * @param deferred
  *        (deferred) The promise that will be fulfilled when display occurs.
  */
-function NotifyPolicyRequest(policy, promise) {
+function NotifyPolicyRequest(policy, deferred) {
   this.policy = policy;
-  this.promise = promise;
+  this.deferred = deferred;
 }
 NotifyPolicyRequest.prototype = {
   /**
    * Called when the user is notified of the policy.
    *
    * This starts a countdown timer that will eventually signify implicit
    * acceptance of the data policy.
    */
   onUserNotifyComplete: function onUserNotified() {
-    this.promise.resolve();
+    this.deferred.resolve();
+    return this.deferred.promise;
   },
 
   /**
    * Called when there was an error notifying the user about the policy.
    *
    * @param error
    *        (Error) Explains what went wrong.
    */
   onUserNotifyFailed: function onUserNotifyFailed(error) {
-    this.promise.reject(error);
+    this.deferred.reject(error);
   },
 
   /**
    * Called when the user agreed to the data policy.
    *
    * @param reason
    *        (string) How the user agreed to the policy.
    */
@@ -153,62 +154,66 @@
    *
    * In the case of upload, this means there is no data to upload (perhaps
    * it isn't available yet). In case of remote deletion, it means that there
    * is no remote data to delete.
    */
   onNoDataAvailable: function onNoDataAvailable() {
     this.state = this.NO_DATA_AVAILABLE;
     this.promise.resolve(this);
+    return this.promise.promise;
   },
 
   /**
    * Data submission has completed successfully.
    *
    * In case of upload, this means the upload completed successfully. In case
    * of deletion, the data was deleted successfully.
    *
    * @param date
    *        (Date) When data submission occurred.
    */
   onSubmissionSuccess: function onSubmissionSuccess(date) {
     this.state = this.SUBMISSION_SUCCESS;
     this.submissionDate = date;
     this.promise.resolve(this);
+    return this.promise.promise;
   },
 
   /**
    * There was a recoverable failure when submitting data.
    *
    * Perhaps the server was down. Perhaps the network wasn't available. The
    * policy may request submission again after a short delay.
    *
    * @param reason
    *        (string) Why the failure occurred. For logging purposes only.
    */
   onSubmissionFailureSoft: function onSubmissionFailureSoft(reason=null) {
     this.state = this.SUBMISSION_FAILURE_SOFT;
     this.reason = reason;
     this.promise.resolve(this);
+    return this.promise.promise;
   },
 
   /**
    * There was an unrecoverable failure when submitting data.
    *
    * Perhaps the client is misconfigured. Perhaps the server rejected the data.
    * Attempts at performing submission again will yield the same result. So,
    * the policy should not try again (until the next day).
    *
    * @param reason
    *        (string) Why the failure occurred. For logging purposes only.
    */
   onSubmissionFailureHard: function onSubmissionFailureHard(reason=null) {
     this.state = this.SUBMISSION_FAILURE_HARD;
     this.reason = reason;
     this.promise.resolve(this);
+    return this.promise.promise;
   },
 };
 
 Object.freeze(DataSubmissionRequest.prototype);
 
 /**
  * Manages scheduling of Firefox Health Report data submission.
  *
diff --git a/services/datareporting/tests/xpcshell/test_policy.js b/services/datareporting/tests/xpcshell/test_policy.js
--- a/services/datareporting/tests/xpcshell/test_policy.js
+++ b/services/datareporting/tests/xpcshell/test_policy.js
@@ -133,17 +133,17 @@
 
   policy.dataSubmissionPolicyResponseDate = new Date();
   policy._dataSubmissionPolicyNotifiedDate = null;
   do_check_eq(policy.notifyState, policy.STATE_NOTIFY_COMPLETE);
 
   run_next_test();
 });
 
-add_test(function test_initial_submission_notification() {
+add_task(function test_initial_submission_notification() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("initial_submission_notification");
 
   do_check_eq(listener.notifyUserCount, 0);
 
   // Fresh instances should not do anything initially.
   policy.checkStateAndTrigger();
   do_check_eq(listener.notifyUserCount, 0);
 
@@ -155,126 +155,116 @@
   do_check_null(policy._dataSubmissionPolicyNotifiedDate);
   do_check_eq(policy.dataSubmissionPolicyNotifiedDate.getTime(), 0);
 
   // We have crossed the threshold. We should see notification.
   defineNow(policy, new Date(policy.firstRunDate.getTime() +
                              policy.SUBMISSION_NOTIFY_INTERVAL_MSEC));
   policy.checkStateAndTrigger();
   do_check_eq(listener.notifyUserCount, 1);
-  listener.lastNotifyRequest.onUserNotifyComplete();
+  yield listener.lastNotifyRequest.onUserNotifyComplete();
   do_check_true(policy._dataSubmissionPolicyNotifiedDate instanceof Date);
   do_check_true(policy.dataSubmissionPolicyNotifiedDate.getTime() > 0);
   do_check_eq(policy.dataSubmissionPolicyNotifiedDate.getTime(),
               policy._dataSubmissionPolicyNotifiedDate.getTime());
   do_check_eq(policy.notifyState, policy.STATE_NOTIFY_WAIT);
-
-  run_next_test();
 });
 
 add_test(function test_bypass_acceptance() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("bypass_acceptance");
 
   policyPrefs.set("dataSubmissionPolicyBypassAcceptance", true);
   do_check_false(policy.dataSubmissionPolicyAccepted);
   do_check_true(policy.dataSubmissionPolicyBypassAcceptance);
   defineNow(policy, new Date(policy.nextDataSubmissionDate.getTime()));
   policy.checkStateAndTrigger();
   do_check_eq(listener.requestDataUploadCount, 1);
 
   run_next_test();
 });
 
-add_test(function test_notification_implicit_acceptance() {
+add_task(function test_notification_implicit_acceptance() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("notification_implicit_acceptance");
 
   let now = new Date(policy.nextDataSubmissionDate.getTime() -
                      policy.SUBMISSION_NOTIFY_INTERVAL_MSEC + 1);
   defineNow(policy, now);
   policy.checkStateAndTrigger();
   do_check_eq(listener.notifyUserCount, 1);
-  listener.lastNotifyRequest.onUserNotifyComplete();
+  yield listener.lastNotifyRequest.onUserNotifyComplete();
   do_check_eq(policy.dataSubmissionPolicyResponseType, "none-recorded");
 
   do_check_true(5000 < policy.IMPLICIT_ACCEPTANCE_INTERVAL_MSEC);
   defineNow(policy, new Date(now.getTime() + 5000));
   policy.checkStateAndTrigger();
   do_check_eq(listener.notifyUserCount, 1);
   do_check_eq(policy.notifyState, policy.STATE_NOTIFY_WAIT);
   do_check_eq(policy.dataSubmissionPolicyResponseDate.getTime(), 0);
   do_check_eq(policy.dataSubmissionPolicyResponseType, "none-recorded");
 
   defineNow(policy, new Date(now.getTime() + policy.IMPLICIT_ACCEPTANCE_INTERVAL_MSEC + 1));
   policy.checkStateAndTrigger();
   do_check_eq(listener.notifyUserCount, 1);
   do_check_eq(policy.notifyState, policy.STATE_NOTIFY_COMPLETE);
   do_check_eq(policy.dataSubmissionPolicyResponseDate.getTime(), policy.now().getTime());
   do_check_eq(policy.dataSubmissionPolicyResponseType, "accepted-implicit-time-elapsed");
-
-  run_next_test();
 });
 
-add_test(function test_notification_rejected() {
+add_task(function test_notification_rejected() {
   // User notification failed. We should not record it as being presented.
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("notification_failed");
 
   let now = new Date(policy.nextDataSubmissionDate.getTime() -
                      policy.SUBMISSION_NOTIFY_INTERVAL_MSEC + 1);
   defineNow(policy, now);
   policy.checkStateAndTrigger();
   do_check_eq(listener.notifyUserCount, 1);
-  listener.lastNotifyRequest.onUserNotifyFailed(new Error("testing failed."));
+  yield listener.lastNotifyRequest.onUserNotifyFailed(new Error("testing failed."));
   do_check_null(policy._dataSubmissionPolicyNotifiedDate);
   do_check_eq(policy.dataSubmissionPolicyNotifiedDate.getTime(), 0);
   do_check_eq(policy.notifyState, policy.STATE_NOTIFY_UNNOTIFIED);
-
-  run_next_test();
 });
 
-add_test(function test_notification_accepted() {
+add_task(function test_notification_accepted() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("notification_accepted");
 
   let now = new Date(policy.nextDataSubmissionDate.getTime() -
                      policy.SUBMISSION_NOTIFY_INTERVAL_MSEC + 1);
   defineNow(policy, now);
   policy.checkStateAndTrigger();
-  listener.lastNotifyRequest.onUserNotifyComplete();
+  yield listener.lastNotifyRequest.onUserNotifyComplete();
   do_check_eq(policy.notifyState, policy.STATE_NOTIFY_WAIT);
   do_check_false(policy.dataSubmissionPolicyAccepted);
   listener.lastNotifyRequest.onUserNotifyComplete();
   listener.lastNotifyRequest.onUserAccept("foo-bar");
   do_check_eq(policy.notifyState, policy.STATE_NOTIFY_COMPLETE);
   do_check_eq(policy.dataSubmissionPolicyResponseType, "accepted-foo-bar");
   do_check_true(policy.dataSubmissionPolicyAccepted);
   do_check_eq(policy.dataSubmissionPolicyResponseDate.getTime(), now.getTime());
-
-  run_next_test();
 });
 
-add_test(function test_notification_rejected() {
+add_task(function test_notification_rejected() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("notification_rejected");
 
   let now = new Date(policy.nextDataSubmissionDate.getTime() -
                      policy.SUBMISSION_NOTIFY_INTERVAL_MSEC + 1);
   defineNow(policy, now);
   policy.checkStateAndTrigger();
-  listener.lastNotifyRequest.onUserNotifyComplete();
+  yield listener.lastNotifyRequest.onUserNotifyComplete();
   do_check_eq(policy.notifyState, policy.STATE_NOTIFY_WAIT);
   do_check_false(policy.dataSubmissionPolicyAccepted);
   listener.lastNotifyRequest.onUserReject();
   do_check_eq(policy.notifyState, policy.STATE_NOTIFY_COMPLETE);
   do_check_eq(policy.dataSubmissionPolicyResponseType, "rejected-no-reason");
   do_check_false(policy.dataSubmissionPolicyAccepted);
 
   // No requests for submission should occur if user has rejected.
   defineNow(policy, new Date(policy.nextDataSubmissionDate.getTime() + 10000));
   policy.checkStateAndTrigger();
   do_check_eq(listener.requestDataUploadCount, 0);
-
-  run_next_test();
 });
 
 add_test(function test_submission_kill_switch() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("submission_kill_switch");
 
   policy.firstRunDate = new Date(Date.now() - 3 * 24 * 60 * 60 * 1000);
   policy.nextDataSubmissionDate = new Date(Date.now() - 24 * 60 * 60 * 1000);
   policy.recordUserAcceptance("accept-old-ack");
@@ -326,94 +316,88 @@
   // The next trigger should try again.
   defineNow(policy, new Date(now.getTime() + 155 * 60 * 1000));
   policy.checkStateAndTrigger();
   do_check_eq(listener.requestDataUploadCount, 2);
 
   run_next_test();
 });
 
-add_test(function test_data_submission_submit_failure_hard() {
+add_task(function test_data_submission_submit_failure_hard() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("data_submission_submit_failure_hard");
 
   policy.dataSubmissionPolicyResponseDate = new Date(Date.now() - 24 * 60 * 60 * 1000);
   policy.dataSubmissionPolicyAccepted = true;
   let nextDataSubmissionDate = policy.nextDataSubmissionDate;
   let now = new Date(policy.nextDataSubmissionDate.getTime() + 1);
   defineNow(policy, now);
 
   policy.checkStateAndTrigger();
   do_check_eq(listener.requestDataUploadCount, 1);
-  listener.lastDataRequest.onSubmissionFailureHard();
+  yield listener.lastDataRequest.onSubmissionFailureHard();
   do_check_eq(listener.lastDataRequest.state,
               listener.lastDataRequest.SUBMISSION_FAILURE_HARD);
 
   let expected = new Date(now.getTime() + 24 * 60 * 60 * 1000);
   do_check_eq(policy.nextDataSubmissionDate.getTime(), expected.getTime());
 
   defineNow(policy, new Date(now.getTime() + 10));
   policy.checkStateAndTrigger();
   do_check_eq(listener.requestDataUploadCount, 1);
-
-  run_next_test();
 });
 
-add_test(function test_data_submission_submit_try_again() {
+add_task(function test_data_submission_submit_try_again() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("data_submission_failure_soft");
 
   policy.recordUserAcceptance();
   let nextDataSubmissionDate = policy.nextDataSubmissionDate;
   let now = new Date(policy.nextDataSubmissionDate.getTime());
   defineNow(policy, now);
   policy.checkStateAndTrigger();
-  listener.lastDataRequest.onSubmissionFailureSoft();
+  yield listener.lastDataRequest.onSubmissionFailureSoft();
   do_check_eq(policy.nextDataSubmissionDate.getTime(),
               nextDataSubmissionDate.getTime() + 15 * 60 * 1000);
-
-  run_next_test();
 });
 
-add_test(function test_submission_daily_scheduling() {
+add_task(function test_submission_daily_scheduling() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("submission_daily_scheduling");
 
   policy.dataSubmissionPolicyResponseDate = new Date(Date.now() - 24 * 60 * 60 * 1000);
   policy.dataSubmissionPolicyAccepted = true;
   let nextDataSubmissionDate = policy.nextDataSubmissionDate;
 
   // Skip ahead to next submission date. We should get a submission request.
   let now = new Date(policy.nextDataSubmissionDate.getTime());
   defineNow(policy, now);
   policy.checkStateAndTrigger();
   do_check_eq(listener.requestDataUploadCount, 1);
   do_check_eq(policy.lastDataSubmissionRequestedDate.getTime(), now.getTime());
 
   let finishedDate = new Date(now.getTime() + 250);
   defineNow(policy, new Date(finishedDate.getTime() + 50));
-  listener.lastDataRequest.onSubmissionSuccess(finishedDate);
+  yield listener.lastDataRequest.onSubmissionSuccess(finishedDate);
   do_check_eq(policy.lastDataSubmissionSuccessfulDate.getTime(), finishedDate.getTime());
 
   // Next scheduled submission should be exactly 1 day after the reported
   // submission success.
 
   let nextScheduled = new Date(finishedDate.getTime() + 24 * 60 * 60 * 1000);
   do_check_eq(policy.nextDataSubmissionDate.getTime(), nextScheduled.getTime());
 
   // Fast forward some arbitrary time. We shouldn't do any work yet.
   defineNow(policy, new Date(now.getTime() + 40000));
   policy.checkStateAndTrigger();
   do_check_eq(listener.requestDataUploadCount, 1);
 
   defineNow(policy, nextScheduled);
   policy.checkStateAndTrigger();
   do_check_eq(listener.requestDataUploadCount, 2);
-  listener.lastDataRequest.onSubmissionSuccess(new Date(nextScheduled.getTime() + 200));
+  yield listener.lastDataRequest.onSubmissionSuccess(new Date(nextScheduled.getTime() + 200));
   do_check_eq(policy.nextDataSubmissionDate.getTime(),
     new Date(nextScheduled.getTime() + 24 * 60 * 60 * 1000 + 200).getTime());
-
-  run_next_test();
 });
 
 add_test(function test_submission_far_future_scheduling() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("submission_far_future_scheduling");
 
   let now = new Date(Date.now() - 24 * 60 * 60 * 1000);
   defineNow(policy, now);
   policy.recordUserAcceptance();
@@ -430,17 +414,17 @@
   policy.checkStateAndTrigger();
   do_check_eq(listener.requestDataUploadCount, 0);
   do_check_eq(policy.nextDataSubmissionDate.getTime(),
               policy._futureDate(24 * 60 * 60 * 1000).getTime());
 
   run_next_test();
 });
 
-add_test(function test_submission_backoff() {
+add_task(function test_submission_backoff() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("submission_backoff");
 
   do_check_eq(policy.FAILURE_BACKOFF_INTERVALS.length, 2);
 
   policy.dataSubmissionPolicyResponseDate = new Date(Date.now() - 24 * 60 * 60 * 1000);
   policy.dataSubmissionPolicyAccepted = true;
 
   let now = new Date(policy.nextDataSubmissionDate.getTime());
@@ -448,17 +432,17 @@
   policy.checkStateAndTrigger();
   do_check_eq(listener.requestDataUploadCount, 1);
   do_check_eq(policy.currentDaySubmissionFailureCount, 0);
 
   now = new Date(now.getTime() + 5000);
   defineNow(policy, now);
 
   // On first soft failure we should back off by scheduled interval.
-  listener.lastDataRequest.onSubmissionFailureSoft();
+  yield listener.lastDataRequest.onSubmissionFailureSoft();
   do_check_eq(policy.currentDaySubmissionFailureCount, 1);
   do_check_eq(policy.nextDataSubmissionDate.getTime(),
               new Date(now.getTime() + policy.FAILURE_BACKOFF_INTERVALS[0]).getTime());
   do_check_eq(policy.lastDataSubmissionFailureDate.getTime(), now.getTime());
 
   // Should not request submission until scheduled.
   now = new Date(policy.nextDataSubmissionDate.getTime() - 1);
   defineNow(policy, now);
@@ -470,36 +454,34 @@
   defineNow(policy, now);
   policy.checkStateAndTrigger();
   do_check_eq(listener.requestDataUploadCount, 2);
 
   now = new Date(now.getTime() + 5000);
   defineNow(policy, now);
 
   // On second failure we should back off by more.
-  listener.lastDataRequest.onSubmissionFailureSoft();
+  yield listener.lastDataRequest.onSubmissionFailureSoft();
   do_check_eq(policy.currentDaySubmissionFailureCount, 2);
   do_check_eq(policy.nextDataSubmissionDate.getTime(),
               new Date(now.getTime() + policy.FAILURE_BACKOFF_INTERVALS[1]).getTime());
 
   now = new Date(policy.nextDataSubmissionDate.getTime());
   defineNow(policy, now);
   policy.checkStateAndTrigger();
   do_check_eq(listener.requestDataUploadCount, 3);
 
   now = new Date(now.getTime() + 5000);
   defineNow(policy, now);
 
   // On 3rd failure we should back off by a whole day.
-  listener.lastDataRequest.onSubmissionFailureSoft();
+  yield listener.lastDataRequest.onSubmissionFailureSoft();
   do_check_eq(policy.currentDaySubmissionFailureCount, 0);
   do_check_eq(policy.nextDataSubmissionDate.getTime(),
               new Date(now.getTime() + 24 * 60 * 60 * 1000).getTime());
-
-  run_next_test();
 });
 
 // Ensure that only one submission request can be active at a time.
 add_test(function test_submission_expiring() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("submission_expiring");
 
   policy.dataSubmissionPolicyResponseDate = new Date(Date.now() - 24 * 60 * 60 * 1000);
   policy.dataSubmissionPolicyAccepted = true;
@@ -516,17 +498,17 @@
                              policy.SUBMISSION_REQUEST_EXPIRE_INTERVAL_MSEC));
 
   policy.checkStateAndTrigger();
   do_check_eq(listener.requestDataUploadCount, 2);
 
   run_next_test();
 });
 
-add_test(function test_delete_remote_data() {
+add_task(function test_delete_remote_data() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("delete_remote_data");
 
   do_check_false(policy.pendingDeleteRemoteData);
   let nextSubmissionDate = policy.nextDataSubmissionDate;
 
   let now = new Date();
   defineNow(policy, now);
 
@@ -535,20 +517,18 @@
   do_check_neq(nextSubmissionDate.getTime(),
                policy.nextDataSubmissionDate.getTime());
   do_check_eq(now.getTime(), policy.nextDataSubmissionDate.getTime());
 
   do_check_eq(listener.requestRemoteDeleteCount, 1);
   do_check_true(listener.lastRemoteDeleteRequest.isDelete);
   defineNow(policy, policy._futureDate(1000));
 
-  listener.lastRemoteDeleteRequest.onSubmissionSuccess(policy.now());
+  yield listener.lastRemoteDeleteRequest.onSubmissionSuccess(policy.now());
   do_check_false(policy.pendingDeleteRemoteData);
-
-  run_next_test();
 });
 
 // Ensure that deletion requests take priority over regular data submission.
 add_test(function test_delete_remote_data_priority() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("delete_remote_data_priority");
 
   let now = new Date();
   defineNow(policy, policy._futureDate(-24 * 60 * 60 * 1000));
@@ -596,17 +576,17 @@
   policy.checkStateAndTrigger();
   do_check_eq(listener.requestRemoteDeleteCount, 2);
 
   run_next_test();
 });
 
 // If we request delete while an upload is in progress, delete should be
 // scheduled immediately after upload.
-add_test(function test_delete_remote_data_in_progress_upload() {
+add_task(function test_delete_remote_data_in_progress_upload() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("delete_remote_data_in_progress_upload");
 
   let now = new Date();
   defineNow(policy, policy._futureDate(-24 * 60 * 60 * 1000));
   policy.recordUserAcceptance();
   defineNow(policy, policy.nextDataSubmissionDate);
 
   policy.checkStateAndTrigger();
@@ -616,24 +596,22 @@
   // If we request a delete during a pending request, nothing should be done.
   policy.deleteRemoteData();
   policy.checkStateAndTrigger();
   do_check_eq(listener.requestDataUploadCount, 1);
   do_check_eq(listener.requestRemoteDeleteCount, 0);
 
   // Now wait a little bit and finish the request.
   defineNow(policy, policy._futureDate(10 * 1000));
-  listener.lastDataRequest.onSubmissionSuccess(policy._futureDate(1000));
+  yield listener.lastDataRequest.onSubmissionSuccess(policy._futureDate(1000));
   defineNow(policy, policy._futureDate(5000));
 
   policy.checkStateAndTrigger();
   do_check_eq(listener.requestDataUploadCount, 1);
   do_check_eq(listener.requestRemoteDeleteCount, 1);
-
-  run_next_test();
 });
 
 add_test(function test_polling() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("polling");
   let intended = 500;
   let acceptable = 250;     // Because nsITimer doesn't guarantee times.
 
   // Ensure checkStateAndTrigger is called at a regular interval.
@@ -742,41 +720,39 @@
   });
 
   policy.firstRunDate = new Date(Date.now() - 4 * 24 * 60 * 60 * 1000);
   policy.nextDataSubmissionDate = new Date(Date.now());
   start = Date.now();
   policy.startPolling();
 });
 
-add_test(function test_record_health_report_upload_enabled() {
+add_task(function test_record_health_report_upload_enabled() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("record_health_report_upload_enabled");
 
   // Preconditions.
   do_check_false(policy.pendingDeleteRemoteData);
   do_check_true(policy.healthReportUploadEnabled);
   do_check_eq(listener.requestRemoteDeleteCount, 0);
 
   // User intent to disable should immediately result in a pending
   // delete request.
   policy.recordHealthReportUploadEnabled(false, "testing 1 2 3");
   do_check_false(policy.healthReportUploadEnabled);
   do_check_true(policy.pendingDeleteRemoteData);
   do_check_eq(listener.requestRemoteDeleteCount, 1);
 
   // Fulfilling it should make it go away.
-  listener.lastRemoteDeleteRequest.onNoDataAvailable();
+  yield listener.lastRemoteDeleteRequest.onNoDataAvailable();
   do_check_false(policy.pendingDeleteRemoteData);
 
   // User intent to enable should get us back to default state.
   policy.recordHealthReportUploadEnabled(true, "testing 1 2 3");
   do_check_false(policy.pendingDeleteRemoteData);
   do_check_true(policy.healthReportUploadEnabled);
-
-  run_next_test();
 });
 
 add_test(function test_pref_change_initiates_deletion() {
   let [policy, policyPrefs, hrPrefs, listener] = getPolicy("record_health_report_upload_enabled");
 
   // Preconditions.
   do_check_false(policy.pendingDeleteRemoteData);
   do_check_true(policy.healthReportUploadEnabled);
diff --git a/services/datareporting/tests/xpcshell/test_session_recorder.js b/services/datareporting/tests/xpcshell/test_session_recorder.js
--- a/services/datareporting/tests/xpcshell/test_session_recorder.js
+++ b/services/datareporting/tests/xpcshell/test_session_recorder.js
@@ -1,16 +1,16 @@
 /* Any copyright is dedicated to the Public Domain.
  * http://creativecommons.org/publicdomain/zero/1.0/ */
 
 "use strict";
 
 const {utils: Cu} = Components;
 
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://gre/modules/services/datareporting/sessions.jsm");
 Cu.import("resource://gre/modules/Services.jsm");
 Cu.import("resource://services-common/utils.js");
 
 
 function run_test() {
   run_next_test();
 }
diff --git a/services/healthreport/healthreporter.jsm b/services/healthreport/healthreporter.jsm
--- a/services/healthreport/healthreporter.jsm
+++ b/services/healthreport/healthreporter.jsm
@@ -15,17 +15,17 @@
 Cu.import("resource://gre/modules/Metrics.jsm");
 Cu.import("resource://services-common/async.js");
 
 Cu.import("resource://services-common/bagheeraclient.js");
 #endif
 
 Cu.import("resource://services-common/log4moz.js");
 Cu.import("resource://services-common/utils.js");
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://gre/modules/osfile.jsm");
 Cu.import("resource://gre/modules/Preferences.jsm");
 Cu.import("resource://gre/modules/Services.jsm");
 Cu.import("resource://gre/modules/Task.jsm");
 Cu.import("resource://gre/modules/TelemetryStopwatch.jsm");
 Cu.import("resource://gre/modules/XPCOMUtils.jsm");
 
 XPCOMUtils.defineLazyModuleGetter(this, "UpdateChannel",
diff --git a/services/healthreport/modules-testing/utils.jsm b/services/healthreport/modules-testing/utils.jsm
--- a/services/healthreport/modules-testing/utils.jsm
+++ b/services/healthreport/modules-testing/utils.jsm
@@ -10,17 +10,17 @@
   "makeFakeAppDir",
   "createFakeCrash",
   "InspectedHealthReporter",
 ];
 
 
 const {classes: Cc, interfaces: Ci, results: Cr, utils: Cu} = Components;
 
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://gre/modules/FileUtils.jsm");
 Cu.import("resource://gre/modules/osfile.jsm");
 Cu.import("resource://gre/modules/XPCOMUtils.jsm");
 Cu.import("resource://gre/modules/services-common/utils.js");
 Cu.import("resource://gre/modules/HealthReport.jsm");
 
 
 let APP_INFO = {
diff --git a/services/healthreport/profile.jsm b/services/healthreport/profile.jsm
--- a/services/healthreport/profile.jsm
+++ b/services/healthreport/profile.jsm
@@ -17,17 +17,17 @@
 
 Cu.import("resource://gre/modules/Metrics.jsm");
 
 #endif
 
 const DEFAULT_PROFILE_MEASUREMENT_NAME = "age";
 const REQUIRED_UINT32_TYPE = {type: "TYPE_UINT32"};
 
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://gre/modules/osfile.jsm")
 Cu.import("resource://gre/modules/Task.jsm");
 Cu.import("resource://services-common/log4moz.js");
 Cu.import("resource://services-common/utils.js");
 
 // Profile creation time access.
 // This is separate from the provider to simplify testing and enable extraction
 // to a shared location in the future.
diff --git a/services/healthreport/providers.jsm b/services/healthreport/providers.jsm
--- a/services/healthreport/providers.jsm
+++ b/services/healthreport/providers.jsm
@@ -29,17 +29,17 @@
 ];
 
 const {classes: Cc, interfaces: Ci, utils: Cu} = Components;
 
 Cu.import("resource://gre/modules/Metrics.jsm");
 
 #endif
 
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://gre/modules/osfile.jsm");
 Cu.import("resource://gre/modules/Preferences.jsm");
 Cu.import("resource://gre/modules/Services.jsm");
 Cu.import("resource://gre/modules/Task.jsm");
 Cu.import("resource://gre/modules/XPCOMUtils.jsm");
 Cu.import("resource://services-common/utils.js");
 
 XPCOMUtils.defineLazyModuleGetter(this, "AddonManager",
diff --git a/services/healthreport/tests/xpcshell/test_healthreporter.js b/services/healthreport/tests/xpcshell/test_healthreporter.js
--- a/services/healthreport/tests/xpcshell/test_healthreporter.js
+++ b/services/healthreport/tests/xpcshell/test_healthreporter.js
@@ -2,17 +2,17 @@
  * http://creativecommons.org/publicdomain/zero/1.0/ */
 
 "use strict";
 
 const {classes: Cc, interfaces: Ci, utils: Cu} = Components;
 
 Cu.import("resource://services-common/observers.js");
 Cu.import("resource://services-common/utils.js");
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://gre/modules/Metrics.jsm");
 Cu.import("resource://gre/modules/osfile.jsm");
 Cu.import("resource://gre/modules/Preferences.jsm");
 let bsp = Cu.import("resource://gre/modules/services/healthreport/healthreporter.jsm");
 Cu.import("resource://gre/modules/services/healthreport/providers.jsm");
 Cu.import("resource://gre/modules/services/datareporting/policy.jsm");
 Cu.import("resource://gre/modules/Services.jsm");
 Cu.import("resource://gre/modules/Task.jsm");
diff --git a/services/healthreport/tests/xpcshell/test_profile.js b/services/healthreport/tests/xpcshell/test_profile.js
--- a/services/healthreport/tests/xpcshell/test_profile.js
+++ b/services/healthreport/tests/xpcshell/test_profile.js
@@ -7,17 +7,17 @@
 
 const MILLISECONDS_PER_DAY = 24 * 60 * 60 * 1000;
 
 // Create profile directory before use.
 // It can be no older than a day ago….
 let profile_creation_lower = Date.now() - MILLISECONDS_PER_DAY;
 do_get_profile();
 
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://gre/modules/Metrics.jsm");
 Cu.import("resource://gre/modules/services/healthreport/profile.jsm");
 Cu.import("resource://gre/modules/Task.jsm");
 
 
 function MockProfileMetadataProvider(name="MockProfileMetadataProvider") {
   this.name = name;
   ProfileMetadataProvider.call(this);
diff --git a/services/healthreport/tests/xpcshell/test_provider_sessions.js b/services/healthreport/tests/xpcshell/test_provider_sessions.js
--- a/services/healthreport/tests/xpcshell/test_provider_sessions.js
+++ b/services/healthreport/tests/xpcshell/test_provider_sessions.js
@@ -1,17 +1,17 @@
 /* Any copyright is dedicated to the Public Domain.
  * http://creativecommons.org/publicdomain/zero/1.0/ */
 
 "use strict";
 
 const {utils: Cu} = Components;
 
 
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://gre/modules/Metrics.jsm");
 Cu.import("resource://gre/modules/Task.jsm");
 Cu.import("resource://gre/modules/services-common/utils.js");
 Cu.import("resource://gre/modules/services/datareporting/sessions.jsm");
 Cu.import("resource://gre/modules/services/healthreport/providers.jsm");
 
 
 function run_test() {
diff --git a/services/metrics/dataprovider.jsm b/services/metrics/dataprovider.jsm
--- a/services/metrics/dataprovider.jsm
+++ b/services/metrics/dataprovider.jsm
@@ -12,17 +12,17 @@
 ];
 
 const {utils: Cu} = Components;
 
 const MILLISECONDS_PER_DAY = 24 * 60 * 60 * 1000;
 
 #endif
 
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://gre/modules/Preferences.jsm");
 Cu.import("resource://gre/modules/Task.jsm");
 Cu.import("resource://services-common/log4moz.js");
 Cu.import("resource://services-common/utils.js");
 
 
 
 /**
diff --git a/services/metrics/modules-testing/mocks.jsm b/services/metrics/modules-testing/mocks.jsm
--- a/services/metrics/modules-testing/mocks.jsm
+++ b/services/metrics/modules-testing/mocks.jsm
@@ -10,17 +10,17 @@
   "DummyConstantProvider",
   "DummyPullOnlyThrowsOnInitProvider",
   "DummyThrowOnInitProvider",
   "DummyThrowOnShutdownProvider",
 ];
 
 const {utils: Cu} = Components;
 
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://gre/modules/Metrics.jsm");
 Cu.import("resource://gre/modules/Task.jsm");
 
 this.DummyMeasurement = function DummyMeasurement(name="DummyMeasurement") {
   this.name = name;
 
   Metrics.Measurement.call(this);
 }
diff --git a/services/metrics/providermanager.jsm b/services/metrics/providermanager.jsm
--- a/services/metrics/providermanager.jsm
+++ b/services/metrics/providermanager.jsm
@@ -7,17 +7,17 @@
 #ifndef MERGED_COMPARTMENT
 this.EXPORTED_SYMBOLS = ["ProviderManager"];
 
 const {classes: Cc, interfaces: Ci, utils: Cu} = Components;
 
 Cu.import("resource://gre/modules/services/metrics/dataprovider.jsm");
 #endif
 
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://gre/modules/Task.jsm");
 Cu.import("resource://services-common/log4moz.js");
 Cu.import("resource://services-common/utils.js");
 
 
 /**
  * Handles and coordinates the collection of metrics data from providers.
  *
diff --git a/services/metrics/storage.jsm b/services/metrics/storage.jsm
--- a/services/metrics/storage.jsm
+++ b/services/metrics/storage.jsm
@@ -14,17 +14,17 @@
 ];
 
 const {utils: Cu} = Components;
 
 const MILLISECONDS_PER_DAY = 24 * 60 * 60 * 1000;
 
 #endif
 
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://gre/modules/Sqlite.jsm");
 Cu.import("resource://gre/modules/Task.jsm");
 Cu.import("resource://services-common/log4moz.js");
 Cu.import("resource://services-common/utils.js");
 
 
 // These do not account for leap seconds. Meh.
 function dateToDays(date) {
diff --git a/services/metrics/tests/xpcshell/test_metrics_provider_manager.js b/services/metrics/tests/xpcshell/test_metrics_provider_manager.js
--- a/services/metrics/tests/xpcshell/test_metrics_provider_manager.js
+++ b/services/metrics/tests/xpcshell/test_metrics_provider_manager.js
@@ -1,16 +1,16 @@
 /* Any copyright is dedicated to the Public Domain.
  * http://creativecommons.org/publicdomain/zero/1.0/ */
 
 "use strict";
 
 const {classes: Cc, interfaces: Ci, utils: Cu} = Components;
 
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://gre/modules/Metrics.jsm");
 Cu.import("resource://testing-common/services/metrics/mocks.jsm");
 
 const PULL_ONLY_TESTING_CATEGORY = "testing-only-pull-only-providers";
 
 function run_test() {
   let cm = Cc["@mozilla.org/categorymanager;1"]
              .getService(Ci.nsICategoryManager);
diff --git a/services/metrics/tests/xpcshell/test_metrics_storage.js b/services/metrics/tests/xpcshell/test_metrics_storage.js
--- a/services/metrics/tests/xpcshell/test_metrics_storage.js
+++ b/services/metrics/tests/xpcshell/test_metrics_storage.js
@@ -1,16 +1,16 @@
 /* Any copyright is dedicated to the Public Domain.
  * http://creativecommons.org/publicdomain/zero/1.0/ */
 
 "use strict";
 
 const {utils: Cu} = Components;
 
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 Cu.import("resource://gre/modules/Metrics.jsm");
 Cu.import("resource://services-common/utils.js");
 
 
 const MILLISECONDS_PER_DAY = 24 * 60 * 60 * 1000;
 
 
 function run_test() {
diff --git a/services/sync/tests/unit/test_bookmark_engine.js b/services/sync/tests/unit/test_bookmark_engine.js
--- a/services/sync/tests/unit/test_bookmark_engine.js
+++ b/services/sync/tests/unit/test_bookmark_engine.js
@@ -5,17 +5,17 @@
 Cu.import("resource://gre/modules/BookmarkJSONUtils.jsm");
 Cu.import("resource://services-common/async.js");
 Cu.import("resource://services-common/log4moz.js");
 Cu.import("resource://services-sync/engines.js");
 Cu.import("resource://services-sync/engines/bookmarks.js");
 Cu.import("resource://services-sync/service.js");
 Cu.import("resource://services-sync/util.js");
 Cu.import("resource://testing-common/services/sync/utils.js");
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 
 Service.engineManager.register(BookmarksEngine);
 var syncTesting = new SyncTestingInfrastructure();
 
 add_test(function bad_record_allIDs() {
   let syncTesting = new SyncTestingInfrastructure();
 
   _("Ensure that bad Places queries don't cause an error in getAllIDs.");
diff --git a/services/sync/tests/unit/test_corrupt_keys.js b/services/sync/tests/unit/test_corrupt_keys.js
--- a/services/sync/tests/unit/test_corrupt_keys.js
+++ b/services/sync/tests/unit/test_corrupt_keys.js
@@ -6,17 +6,17 @@
 Cu.import("resource://services-sync/engines.js");
 Cu.import("resource://services-sync/engines/tabs.js");
 Cu.import("resource://services-sync/engines/history.js");
 Cu.import("resource://services-sync/record.js");
 Cu.import("resource://services-sync/service.js");
 Cu.import("resource://services-sync/status.js");
 Cu.import("resource://services-sync/util.js");
 Cu.import("resource://testing-common/services/sync/utils.js");
-Cu.import("resource://gre/modules/commonjs/sdk/core/promise.js");
+Cu.import("resource://gre/modules/Promise.jsm");
 
 add_task(function test_locally_changed_keys() {
   let passphrase = "abcdeabcdeabcdeabcdeabcdea";
 
   let hmacErrorCount = 0;
   function counting(f) {
     return function() {
       hmacErrorCount++;

