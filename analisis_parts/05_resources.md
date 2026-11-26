# Bagian 5 — Manifest, resource, native library & SDK pihak ketiga

# LAPORAN INVENTARISASI TEKNIS LENGKAP
## MineTuner+ 2.2.3 (XAPK) — Manifest, Resources, Native Libraries & Dependencies Pihak Ketiga

**Objek audit:** `/home/kunbu8u7b/NBT-EDITOR-P/MineTuner+-+NBT+Editor_2.2.3_APKPure.xapk`
**Workspace:** `/home/kunbu8u7b/NBT-EDITOR-P/`
**Tanggal inventarisasi:** 23 November 2026
**Metode:** `aapt2 dump xmltree/badging/resources/strings`, `unzip -l`, `sha256sum`, `apksigner verify --print-certs`, inspeksi `apkbase/META-INF/*.properties`, serta verifikasi silang terhadap 5.222 sumber JADX di `.ref_extract/jadx_out/sources/` (+ `jadx_c12`, `jadx_c6`) dan `apktool_out/`.

---

## 0. RINGKASAN EKSEKUTIF

| Dimensi | Temuan utama |
|---|---|
| Identitas | `com.hextetrahedral.minetuner`, versi `2.2.3` (versionCode `16`), label **MineTuner** |
| Target platform | minSdk **26**, targetSdk **36**, compileSdk **36** (Android 16) |
| XAPK | 1 base + 3 split (`arm64_v8a`, `en`, `mdpi`), total 109.767.468 byte |
| Komponen manifest | **177** komponen: 129 activity, 19 provider, 14 service, 15 receiver |
| Aplikasi inti | Hanya **5 activity** + 1 FileProvider; sisanya **172 komponen iklan/SDK/AndroidX** |
| Resources | 527 layout, 6 menu, 954 drawable, 9 mipmap, 742 string, 1.208 color, 1.437 style |
| Tema | `Theme.Minetuner` → `Base.Theme.Minetuner` → `Theme.Material3.Light.NoActionBar`, hijau `#429E46` |
| Native | 12 file `.so` arm64-v8a; `libleveldb.so` adalah jantung editor.world Bedrock |
| DEX | **13** file `classes*.dex` + 1 `assets/audience_network.dex` |
| Monetisasi | ≥ 25 jaringan iklan/SDK terdeteksi (Appodeal/MAX sebagai orkestrator utama) |
| Licensing | **PairIP** (`com.pairip.licensecheck`) memblokir app di emulator tanpa Play Store |

---

## 1. IDENTITAS & BUILD METADATA

### 1.1 Identitas aplikasi (dari `aapt2 dump badging` + xmltree manifest)

| Field | Nilai |
|---|---|
| `package` | `com.hextetrahedral.minetuner` |
| `versionCode` | `16` |
| `versionName` | `2.2.3` |
| `platformBuildVersionName/Code` | `16` / `36` |
| `compileSdkVersion` | `36` (`16`) |
| `minSdkVersion` | `26` (Android 8.0 Oreo) |
| `targetSdkVersion` | `36` (Android 16) |
| `application-label` | `MineTuner` |
| `application-icon` | `res/mipmap-anydpi-v26/ic_launcher.xml` (semua densitas) |
| `launchable-activity` | `com.hextetrahedral.minetuner.MainActivity` |
| `requiredSplitTypes` | `base__abi,base__density` |
| `splitTypes` | kosong (base adalah aplikasi utama) |
| Densitas didukung | 160, 240, 320, 480, 640, 65534 (anydpi), 65535 (nodpi) |
| Layar | small, normal, large, xlarge; `supports-any-density=true` |
| Locale | `--_--` (default;资源 bahasa Inggris ada di `config.en.apk`) |
| `extractNativeLibs` | `false` (`.so` dimuat langsung dari APK) |
| `allowBackup` | `true` |
| `hardwareAccelerated` | `true` |
| `supportsRtl` | `true` |
| `networkSecurityConfig` | `@xml/network_security_config` |
| `appComponentFactory` | `androidx.core.app.CoreComponentFactory` |

### 1.2 Hash & ukuran file

| File | SHA-256 | Ukuran (byte) |
|---|---|---|
| `MineTuner+-+NBT+Editor_2.2.3_APKPure.xapk` | `bb1a658a76de7d6d0d9d23bbf756c01c62b2499d8abaa597a6c315b9c1e5148c` | 109.767.468 |
| `com.hextetrahedral.minetuner.apk` (base) | `13c2cf98dd2421ddd8412097d983792528037ff7facd496ee41a17bd3cde649b` | 105.397.608 |
| `config.arm64_v8a.apk` | `defd011b8a421e7c93dbee24d0ed3c6d528c0fab97b658018a81e32374a2dbcd` | 4.048.134 |
| `config.en.apk` | `b3023e9a7af9d6631ceb2aa8067008099d085d2aa6a4355b189edbba1b2e94b5` | 99.054 |
| `config.mdpi.apk` | `aae9fa9de4092d521be6f25d2bcc6e2e78ea33f94754e2bd44443419a19bc544` | 220.643 |

Jumlah keempat APK = **109.765.439 byte**, identik dengan `total_size` di `manifest.json` (selisih 2.029 byte dengan ukuran container XAPK = metadata ZIP).

### 1.3 Struktur `manifest.json` (XAPK v2)

```json
{
  "xapk_version": 2,
  "package_name": "com.hextetrahedral.minetuner",
  "name": "MineTuner",
  "version_code": "16",
  "version_name": "2.2.3",
  "min_sdk_version": "26",
  "target_sdk_version": "36",
  "permissions": [18 entri, identik dengan manifest],
  "split_configs": ["config.mdpi", "config.en", "config.arm64_v8a"],
  "total_size": 109765439,
  "icon": "icon.png",
  "split_apks": [
    {"file":"com.hextetrahedral.minetuner.apk","id":"base"},
    {"file":"config.mdpi.apk","id":"config.mdpi"},
    {"file":"config.en.apk","id":"config.en"},
    {"file":"config.arm64_v8a.apk","id":"config.arm64_v8a"}
  ]
}
```

### 1.4 Tanda tangan (apksigner 36.0.0)

| Item | Nilai |
|---|---|
| Verifikasi | **Verifies**; v1 = false, **v2 = true**, **v3 = true**, v3.1 = false, v4 = false |
| SourceStamp | **true**, timestamp `1762712700` (≈ 9 Nov 2025) |
| Jumlah signer | 1 |
| DN signer | `CN=Android, OU=Android, O=Google Inc., L=Mountain View, ST=California, C=US` |
| SHA-256 sertifikat | `ed6faf5d24b74b9faebce13ebc9277a1245bb4aecefe7df827eece2fbb26818d` |
| SHA-1 sertifikat | `6bbc297f59a8a4ee384a6584efbf24afd97de58a` |
| MD5 sertifikat | `bb9888e228e39d6a6a3295c61b8768ff` |
| Kunci | RSA 4096-bit; public key SHA-256 `a85f63e3e99ad545356131e95b21f7b01756a27c646d8f36135a380ab5b3b15f` |
| SourceStamp signer DN | sama (Google Android), SHA-256 `3257d599a49d2c961a471ca9843f59d341a405884583fc087df4237b733bbd6d` |
| File META-INF | `BNDLTOOL.SF`, `BNDLTOOL.RSA`, `MANIFEST.MF` (bundletool, khas App Bundle → APK set) |

Metadata tambahan: `com.android.vending.splits.required=true`, `com.android.stamp.source=https://play.google.com/store`, `com.android.stamp.type=STAMP_TYPE_DISTRIBUTION_APK`, `com.android.vending.splits=@xml/splits0`, `com.android.vending.derived.apk.id=3`.

---

## 2. MANIFEST LENGKAP

### 2.1 `<uses-permission>` — 18 izin

| # | Izin | Kategori / pemilik |
|---|---|---|
| 1 | `android.permission.INTERNET` | Inti/iklan |
| 2 | `android.permission.ACCESS_NETWORK_STATE` | Inti/iklan |
| 3 | `android.permission.READ_EXTERNAL_STORAGE` | Inti — baca world legacy & ekspor |
| 4 | `android.permission.WRITE_EXTERNAL_STORAGE` | Inti — impor/ekspor world |
| 5 | `com.google.android.gms.permission.AD_ID` | AdMob / GMS |
| 6 | `android.permission.ACCESS_WIFI_STATE` | Iklan / konektivitas |
| 7 | `com.android.vending.BILLING` | Google Play Billing 7.1.1 |
| 8 | `android.permission.VIBRATE` | UI/feedback |
| 9 | `com.applovin.array.apphub.permission.BIND_APPHUB_SERVICE` | AppLovin AppHub |
| 10 | `android.permission.ACCESS_ADSERVICES_TOPICS` | Android Privacy Sandbox Ads |
| 11 | `android.permission.ACCESS_ADSERVICES_ATTRIBUTION` | Android Privacy Sandbox Ads |
| 12 | `android.permission.RECEIVE_BOOT_COMPLETED` | StartApp / WorkManager |
| 13 | `com.google.android.finsky.permission.BIND_GET_INSTALL_REFERRER_SERVICE` | Play Install Referrer |
| 14 | `android.permission.ACCESS_ADSERVICES_AD_ID` | Privacy Sandbox Ad ID |
| 15 | `android.permission.WAKE_LOCK` | Iklan / foreground work |
| 16 | `android.permission.FOREGROUND_SERVICE` | Iklan / WorkManager |
| 17 | `com.hextetrahedral.minetuner.DYNAMIC_RECEIVER_NOT_EXPORTED_PERMISSION` | AndroidX Core (protectionLevel `signature`) |
| 18 | `com.android.vending.CHECK_LICENSE` | **PairIP / Google Play Licensing** |

Tidak ada `READ_MEDIA_*`, `POST_NOTIFICATIONS`, `MANAGE_EXTERNAL_STORAGE`, `CAMERA`, atau `RECORD_AUDIO`.

### 2.2 `<queries>` — 21 entri visibilitas paket

| Bentuk | Nilai | Alasan |
|---|---|---|
| intent + scheme | `ACTION_VIEW`, `https` | browser/link iklan |
| package | `com.pubmatic.openwrapapp` | PubMatic OpenWrap |
| package | `com.android.chrome` | Chrome Custom Tabs |
| intent | `android.support.customtabs.action.CustomTabsService` | androidx.browser |
| intent | `com.android.vending.billing.InAppBillingService.BIND` | Billing |
| intent | `com.google.android.apps.play.billingtestcompanion.BillingOverrideService.BIND` | Billing sandbox |
| package | `com.facebook.katana` | Audience Network |
| intent | `com.applovin.am.intent.action.APPHUB_SERVICE` | AppLovin AppHub |
| intent | `VIEW` + scheme `http` / `https` | klik iklan |
| intent | `androidx.browser.customtabs.CustomTabsService` | Custom Tabs |
| intent | `VIEW` + `BROWSABLE` + `https` / `http` | deep link |
| intent | `VIEW` + scheme `market` | Play Store |
| intent | `com.attribution.REFERRAL_PROVIDER` | attribution |
| intent | `ACTION_MAIN` | resolver umum |
| intent | `INSERT` + mime `vnd.android.cursor.dir/event` | kalender (iklan) |
| intent | `VIEW` + scheme `sms`, `DIAL` + path `tel:` | iklan click-through |
| package | `com.google.android.webview`, `com.android.webview` | WebView |
| package | `com.android.vending` | Play |
| intent | `VIEW` | umum |
| intent | `com.digitalturbine.ignite.cl.IgniteRemoteService` | Digital Turbine / DT Exchange |

### 2.3 `<uses-feature>` — 11 entri

Wajib (implied): `android.hardware.faketouch`, `android.hardware.screen.landscape`, `android.hardware.screen.portrait`, `android.hardware.wifi`.
Opsional (`required=false`): `android.hardware.sensor.light`, `.compass`, `.gyroscope`, `.barometer`, `.proximity`.

### 2.4 Atribut `<application>`

`theme=@style/Theme.Minetuner`, `label=@string/app_name`, `icon=@mipmap/ic_launcher`, `roundIcon=@mipmap/ic_launcher_round`, `allowBackup=true`, `hardwareAccelerated=true`, `supportsRtl=true`, `extractNativeLibs=false`, `networkSecurityConfig=@xml/network_security_config`, `appComponentFactory=androidx.core.app.CoreComponentFactory`.

`uses-library` (keduanya `required=false`): `org.apache.http.legacy`, `android.ext.adservices`.

### 2.5 Komponen APLIKASI INTI (5 activity + 1 provider)

| Komponen | exported | Keterangan |
|---|---|---|
| `com.hextetrahedral.minetuner.MainActivity` | **true** | Launcher: `MAIN` + `LAUNCHER`. Daftar world, FAB New/Import, menu Help/About |
| `com.hextetrahedral.minetuner.DrawerActivity` | false | Editor NBT world: DrawerLayout + 12 navigasi |
| `com.hextetrahedral.minetuner.app.HelpActivity` | false | Tutorial ekspor texture-pack (“Exporting Worlds”) |
| `com.hextetrahedral.minetuner.app.CreateWorldActivity` | false | Pembuat flat world |
| `com.hextetrahedral.minetuner.app.EntitiesNBTActivity` | false | Daftar entitas chunk |
| `androidx.core.content.FileProvider` | false | authority `com.hextetrahedral.minetuner.provider`, `grantUriPermissions=true`, `@xml/file_paths` |

Catatan: kelima activity tersebut **tidak memiliki intent-filter selain MainActivity**. Seluruh 172 komponen lain adalah pihak ketiga/AndroidX.

### 2.6 Statistik & komponen yang `exported=true`

Total komponen = **177** (activity 129, provider 19, service 14, receiver 15).

| Komponen exported | Jenis | Pelindung |
|---|---|---|
| `MainActivity` | activity | launcher (disengaja) |
| `com.amazon.device.ads.DTBInterstitialActivity` | activity | — |
| `com.amazon.aps.ads.activity.ApsInterstitialActivity` | activity | — |
| `com.google.android.gms.auth.api.signin.RevocationBoundService` | service | `...permission.REVOCATION_NOTIFICATION` |
| `com.appodeal.ads.AppodealPackageAddedReceiver` | receiver | `PACKAGE_ADDED` |
| `io.appmetrica.analytics.internal.PreloadInfoContentProvider` | provider | authority `...appmetrica.preloadinfo.retail` |
| `com.startapp.sdk.adsbase.remoteconfig.BootCompleteListener` | receiver | `BOOT_COMPLETED` |
| `com.mbridge.msdk.foundation.same.broadcast.NetWorkChangeReceiver` | receiver | `CONNECTIVITY_CHANGE` |
| `com.ogury.core.internal.LogEnablerReceiver` | receiver | `com.ogury.sdk.intent.ENABLE_LOGS` |
| `androidx.work.impl.background.systemjob.SystemJobService` | service | `BIND_JOB_SERVICE` |
| `androidx.work.impl.diagnostics.DiagnosticsReceiver` | receiver | `android.permission.DUMP` |
| `androidx.profileinstaller.ProfileInstallReceiver` | receiver | `android.permission.DUMP` |

### 2.7 Daftar LENGKAP komponen per SDK (177 entri)

**Aplikasi (5):** MainActivity · DrawerActivity · app.HelpActivity · app.CreateWorldActivity · app.EntitiesNBTActivity

**AndroidX / Google Infrastructure (15):** androidx.core.content.FileProvider *(auth `com.hextetrahedral.minetuner.provider`)* · androidx.startup.InitializationProvider *(auth `...androidx-startup`)* · androidx.work.impl.background.systemalarm.SystemAlarmService · androidx.work.impl.background.systemjob.SystemJobService · androidx.work.impl.foreground.SystemForegroundService · androidx.room.MultiInstanceInvalidationService · androidx.work.impl.utils.ForceStopRunnable$BroadcastReceiver · androidx.work.impl.background.systemalarm.ConstraintProxy$BatteryChargingProxy · ...$BatteryNotLowProxy · ...$StorageNotLowProxy · ...$NetworkStateProxy · ...RescheduleReceiver · ...ConstraintProxyUpdateReceiver · androidx.work.impl.diagnostics.DiagnosticsReceiver · androidx.profileinstaller.ProfileInstallReceiver

**Google Mobile Ads / GMS / Play (13):** com.google.android.gms.ads.AdActivity · OutOfContextTestingActivity · NotificationHandlerActivity · GoogleApiActivity · auth.api.signin.internal.SignInHubActivity · com.google.android.gms.ads.MobileAdsInitProvider *(auth `...mobileadsinitprovider`, initOrder 100)* · com.google.android.gms.ads.AdService · auth.api.signin.RevocationBoundService · com.android.billingclient.api.ProxyBillingActivity · ProxyBillingActivityV2 · com.google.android.datatransport.runtime.backends.TransportBackendDiscovery · ...jobscheduling.JobInfoSchedulerService · ...AlarmManagerSchedulerBroadcastReceiver

**AppLovin / MAX (20):** com.applovin.adview.AppLovinFullscreenActivity · sdk.AppLovinWebViewActivity · mediation.MaxDebuggerActivity · MaxDebuggerDetailActivity · MaxDebuggerMultiAdActivity · MaxDebuggerAdUnitsListActivity · MaxDebuggerAdUnitWaterfallsListActivity · MaxDebuggerAdUnitDetailActivity · MaxDebuggerCmpNetworksListActivity · MaxDebuggerTcfConsentStatusesListActivity · MaxDebuggerTcfInfoListActivity · MaxDebuggerTcfStringActivity · MaxDebuggerTestLiveNetworkActivity · MaxDebuggerTestModeNetworkActivity · MaxDebuggerUnifiedFlowActivity · MaxDebuggerWaterfallSegmentsActivity · creative.MaxCreativeDebuggerActivity · creative.MaxCreativeDebuggerDisplayedAdActivity · sdk.AppLovinInitProvider *(auth `...applovininitprovider`, initOrder 101)* · impl.adview.activity.FullscreenAdService

**Appodeal (4):** ads.adapters.iab.unified.VideoPlayerActivity · ads.TestActivity · consent.form.ConsentActivity · ads.AppodealPackageAddedReceiver

**Chartboost (4):** view.CBImpressionActivity · internal.clickthrough.EmbeddedBrowserActivity · view.FullscreenAdActivity · internal.video.repository.exoplayer.VideoRepositoryDownloadService *(action `com.google.android.exoplayer.downloadService.action.RESTART`)*

**Fyber / Inneractive (5):** InneractiveInternalBrowserActivity · InneractiveFullscreenAdActivity · InneractiveRichMediaVideoPlayerActivityCore · InternalStoreWebpageActivity · FyberReportAdActivity

**Smaato (4):** rewarded.view.RewardedInterstitialAdActivity · interstitial.view.InterstitialAdActivity · core.browser.SmaatoSdkBrowserActivity · core.lifecycle.ProcessLifecycleOwnerInitializer *(auth `...smaato-lifecycle-process`)*

**Amazon DTB / APS (3):** device.ads.DTBInterstitialActivity *(exported)* · device.ads.DTBAdActivity *(scheme `amazonmobile://intent`)* · amazon.aps.ads.activity.ApsInterstitialActivity *(exported)*

**PubMatic / OpenWrap (3):** webrendering.mraid.POBVideoPlayerActivity · webrendering.ui.POBFullScreenActivity · common.browser.POBInternalBrowserActivity

**Yandex Mobile Ads (4):** common.AdActivity · features.debugpanel.ui.IntegrationInspectorActivity · core.initializer.MobileAdsInitializeProvider *(auth `...MobileAdsInitializeProvider`)* · features.debugpanel.data.local.DebugPanelFileProvider *(auth `...monetization.ads.inspector.fileprovider`)*

**InMobi (1):** ads.rendering.InMobiAdActivity

**Pangle / ByteDance (15):** activity.TTCeilingLandingPageActivity · TTLandingPageActivity · TTPlayableLandingPageActivity · TTVideoLandingPageLink2Activity · TTDelegateActivity · TTWebsiteActivity · TTAppOpenAdActivity · TTRewardVideoActivity · TTRewardExpressVideoActivity · TTFullScreenVideoActivity · TTFullScreenExpressVideoActivity · TTInterstitialActivity · TTInterstitialExpressActivity · TTAdActivity · multipro.aidl.BinderPoolService

**Unity Ads (5 + 1 initializer):** ads.adunit.AdUnitActivity · AdUnitTransparentActivity · AdUnitTransparentSoftwareActivity · AdUnitSoftwareActivity · ads.adplayer.FullScreenWebViewDisplay · (Startup meta-data `com.unity3d.services.core.configuration.AdsSdkInitializer`)

**ironSource / LevelPlay (6):** sdk.controller.ControllerActivity · InterstitialActivity · OpenUrlActivity · mediationsdk.testSuite.TestSuiteActivity · lifecycle.IronsourceLifecycleProvider · lifecycle.LevelPlayActivityLifecycleProvider

**Mintegral / Mbridge (4):** activity.MBCommonActivity · out.LoadingActivity · reward.player.MBRewardVideoActivity · foundation.same.broadcast.NetWorkChangeReceiver *(exported)*

**Vungle (2):** internal.ui.VungleActivity · VungleProvider *(auth `...vungle-provider`, initOrder 102)*

**BIGO (11):** ad.splash.AdSplashActivity · LandscapeAdSplashActivity · controller.form.AdFormActivity · api.AdActivity · api.PopupAdActivity · api.LandingStyleableActivity · api.LandscapeAdActivity · api.CompanionAdActivity · api.LandscapeCompanionAdActivity · core.mraid.MraidVideoActivity · controller.provider.BigoAdsProvider *(auth `...BigoAdsProvider`)*

**TaurusX (9):** ui.TaxWebViewActivity · ui.TaxVideoActivity · ui.AppOpenAdActivity · ui.AppOpenNativeAdActivity · utils.webview.TaxBrowserActivity · mraid.MraidBrowser · mraid.MraidVideoPlayerActivity · ui.TaurusxH5Activity · core.TaurusXProvider *(auth `...taurusxfileprovider`)*

**OGury (4):** interstitial.ui.InterstitialActivity · InterstitialAndroid8TransparentActivity · InterstitialAndroid8RotableActivity · core.internal.LogEnablerReceiver *(exported)*

**StartApp (6):** consent.ConsentActivity · ads.list3d.List3DActivity · ads.interstitials.OverlayActivity · cachedservice.BackgroundService · jobs.SchedulerService · remoteconfig.BootCompleteListener

**MobileFuse (5):** MobileFuseFullscreenActivity · MobileFuseFullscreenTransparentActivity · experimental.NativeInterstitialActivity · splashad.MobileFuseSplashAdActivity · MobileFuseSdkInitProvider *(auth `...mobilefusesdkinitprovider`)*

**Moloco (4):** Xenoss FullscreenWebviewActivity · VastActivity · StaticAdActivity · MraidActivity · (Startup meta-data `com.moloco.sdk.internal.android_context.StartupComponentInitialization`)

**MyTarget (2):** common.MyTargetActivity · common.MyTargetContentProvider *(auth `...mytargetcontentprovider`)*

**Facebook Audience Network (2):** ads.AudienceNetworkActivity · AudienceNetworkContentProvider *(auth `...AudienceNetworkContentProvider`)*

**BidMachine (5):** rendering.ad.fullscreen.FullScreenActivity · nativead.view.VideoPlayerActivity · iab.mraid.MraidActivity · iab.vast.activity.VastActivity · BidMachineInitProvider *(auth `...bidmachineinitprovider`, initOrder 100)*

**ExploreStack / IAB (3):** iab.mraid.MraidActivity · MraidDialogActivity · iab.vast.activity.VastActivity

**PubNative (8):** consent.UserConsentActivity · interstitial.activity.MraidInterstitialActivity · VastInterstitialActivity · rewarded.activity.MraidRewardedActivity · VastRewardedActivity · contentinfo.AdFeedbackActivity · utils.browser.BrowserActivity · receiver.VolumeChangedActionReceiver

**AppMetrica (2):** internal.AppMetricaService · internal.PreloadInfoContentProvider *(exported)*

**Picasso (1):** com.squareup.picasso.PicassoProvider *(auth `...com.squareup.picasso`)*

**PairIP (2):** com.pairip.licensecheck.LicenseActivity · LicenseContentProvider *(auth `...com.pairip.licensecheck.LicenseContentProvider`)*

**Sentry:** tidak ada komponen; aktif via meta-data + native `libsentry*.so`.

### 2.8 SELURUH meta-data (30 entri)

| # | Nama | Nilai |
|---|---|---|
| 1 | `android.support.FILE_PROVIDER_PATHS` | `@xml/file_paths` (FileProvider app) |
| 2 | `com.google.android.gms.ads.APPLICATION_ID` | `ca-app-pub-3353299071120459~1150848728` |
| 3 | `com.moloco.sdk.internal.android_context.StartupComponentInitialization` | `androidx.startup` |
| 4 | `androidx.lifecycle.ProcessLifecycleInitializer` | `androidx.startup` |
| 5 | `com.unity3d.services.core.configuration.AdsSdkInitializer` | `androidx.startup` |
| 6 | `androidx.emoji2.text.EmojiCompatInitializer` | `androidx.startup` |
| 7 | `androidx.work.WorkManagerInitializer` | `androidx.startup` |
| 8 | `androidx.profileinstaller.ProfileInstallerInitializer` | `androidx.startup` |
| 9 | `com.google.android.gms.ads.flag.OPTIMIZE_INITIALIZATION` | `true` |
| 10 | `com.google.android.gms.ads.flag.OPTIMIZE_AD_LOADING` | `true` |
| 11 | `com.yandex.mobile.ads.AUTOMATIC_SDK_INITIALIZATION` | `false` |
| 12 | `android.support.FILE_PROVIDER_PATHS` | `@xml/debug_panel_file_paths` (Yandex) |
| 13 | `com.google.android.play.billingclient.version` | `7.1.1` |
| 14 | `com.mobilefuse.sdk.disable_auto_init` | `true` |
| 15 | `com.startapp.sdk.SPLASH_ENABLED` | `false` |
| 16 | `com.google.android.gms.version` | `@integer/google_play_services_version` |
| 17 | `com.bytedance.sdk.pangle.version` | `7.3.0.5` |
| 18 | `io.sentry.dsn` | `https://19bf3a1f35004cbe8b0e90999cb10c39@sentry.appodeal.com/42` |
| 19 | `io.sentry.release` | `3.11.0` |
| 20 | `io.sentry.auto-init` | `false` |
| 21 | `io.sentry.ndk.scope-sync.enable` | `true` |
| 22 | `io.sentry.proguard-uuid` | `2fc9fc4a-a2ed-4e19-9561-5e9e83c126cb` |
| 23 | `android.support.FILE_PROVIDER_PATHS` | `@xml/nath_tax_file_path` (TaurusX) |
| 24 | `backend:com.google.android.datatransport.cct.CctBackendFactory` | `cct` |
| 25 | `android.webkit.WebView.EnableSafeBrowsing` | `true` (ironSource TestSuite) |
| 26 | `com.android.vending.splits.required` | `true` |
| 27 | `com.android.stamp.source` | `https://play.google.com/store` |
| 28 | `com.android.stamp.type` | `STAMP_TYPE_DISTRIBUTION_APK` |
| 29 | `com.android.vending.splits` | `@xml/splits0` |
| 30 | `com.android.vending.derived.apk.id` | `3` |

---

## 3. RESOURCES — INVENTARISASI LENGKAP

### 3.1 Rekap tipe resource (`aapt2 dump resources`)

| Tipe | Jumlah | Tipe | Jumlah |
|---|---:|---|---:|
| `attr` | 1.655 | `drawable` | 954 |
| `id` | 1.652 | `string` | 742 |
| `dimen` | 1.484 | `layout` | 527 |
| `style` | 1.437 | `integer` | 92 |
| `color` | 1.208 | `anim` | 68 |
| `plurals` | 4 | `mipmap` | 9 |
| `menu` | 6 | `raw` | 17 |
| `xml` | 13 | `font` | 11 |
| `bool` | 11 | `array` | 10 |
| `animator` | 40 | `interpolator` | 18 |

### 3.2 SELURUH 527 layout

```text
abc_action_bar_title_item · abc_action_bar_up_container · abc_action_menu_item_layout · abc_action_menu_layout · abc_action_mode_bar · abc_action_mode_close_item_material · abc_activity_chooser_view · abc_activity_chooser_view_list_item · abc_alert_dialog_button_bar_material · abc_alert_dialog_material · abc_alert_dialog_title_material · abc_cascading_menu_item_layout
abc_dialog_title_material · abc_expanded_menu_layout · abc_list_menu_item_checkbox · abc_list_menu_item_icon · abc_list_menu_item_layout · abc_list_menu_item_radio · abc_popup_menu_header_item_layout · abc_popup_menu_item_layout · abc_screen_content_include · abc_screen_simple · abc_screen_simple_overlay_action_mode · abc_screen_toolbar
abc_search_dropdown_item_icons_2line · abc_search_view · abc_select_dialog_material · abc_tooltip · activity_create_world · activity_debug · activity_drawer · activity_entities_nbt · activity_help · activity_internal_browser · activity_main · ad_mob_native_banner_template_basic_layout
ad_mob_native_banner_template_icon_text_layout · ad_mob_native_banner_template_rect_layout · ad_mob_native_banner_template_text_cta_layout · admob_empty_layout · apd_admob_banner · applovin_debugger_list_item_detail · applovin_native_ad_media_view · aps_interstitial_activity · bigo_ad_activity_form · bigo_ad_activity_interstitial · bigo_ad_activity_interstitial_banner · bigo_ad_activity_interstitial_bottom_card
bigo_ad_activity_interstitial_landingpage · bigo_ad_activity_interstitial_landingpage_7_8 · bigo_ad_activity_interstitial_native_center · bigo_ad_activity_interstitial_native_top · bigo_ad_activity_interstitial_percent_warning · bigo_ad_activity_interstitial_percent_warning_landscape · bigo_ad_activity_interstitial_rich_video · bigo_ad_activity_interstitial_rich_video_19_29 · bigo_ad_activity_interstitial_rich_video_2 · bigo_ad_activity_interstitial_rich_video_3 · bigo_ad_activity_interstitial_rich_video_4 · bigo_ad_activity_interstitial_rich_video_5
bigo_ad_activity_interstitial_rich_video_component_19 · bigo_ad_activity_interstitial_rich_video_component_20 · bigo_ad_activity_interstitial_rich_video_component_21 · bigo_ad_activity_interstitial_rich_video_component_22 · bigo_ad_activity_interstitial_rich_video_component_23 · bigo_ad_activity_interstitial_rich_video_component_24 · bigo_ad_activity_interstitial_rich_video_component_25 · bigo_ad_activity_interstitial_rich_video_component_26 · bigo_ad_activity_interstitial_rich_video_download_10 · bigo_ad_activity_interstitial_rich_video_download_11 · bigo_ad_activity_interstitial_rich_video_download_12 · bigo_ad_activity_interstitial_rich_video_download_6
bigo_ad_activity_interstitial_rich_video_download_7 · bigo_ad_activity_interstitial_rich_video_download_8 · bigo_ad_activity_interstitial_rich_video_download_9 · bigo_ad_activity_interstitial_rich_video_end · bigo_ad_activity_interstitial_rich_video_end_14_15 · bigo_ad_activity_interstitial_rich_video_end_16_17 · bigo_ad_activity_interstitial_rich_video_end_18 · bigo_ad_activity_interstitial_rich_video_end_19 · bigo_ad_activity_interstitial_rich_video_end_2 · bigo_ad_activity_interstitial_rich_video_end_3 · bigo_ad_activity_interstitial_rich_video_end_4 · bigo_ad_activity_interstitial_rich_video_end_6
bigo_ad_activity_interstitial_rich_video_end_7 · bigo_ad_activity_interstitial_rich_video_end_8 · bigo_ad_activity_interstitial_rich_video_end_9 · bigo_ad_activity_interstitial_rich_video_end_landscape · bigo_ad_activity_interstitial_rich_video_end_landscape_2 · bigo_ad_activity_interstitial_rich_video_land_material_31_32 · bigo_ad_activity_interstitial_rich_video_left_material_31 · bigo_ad_activity_interstitial_rich_video_multi_img_13 · bigo_ad_activity_interstitial_rich_video_multi_img_14 · bigo_ad_activity_interstitial_rich_video_multi_img_15 · bigo_ad_activity_interstitial_rich_video_multi_img_16 · bigo_ad_activity_interstitial_rich_video_multi_img_17
bigo_ad_activity_interstitial_rich_video_multi_img_item_layout · bigo_ad_activity_interstitial_rich_video_multi_img_media_layout · bigo_ad_activity_interstitial_rich_video_right_material_32 · bigo_ad_activity_interstitial_sab · bigo_ad_activity_interstitial_style_landscape_1 · bigo_ad_activity_interstitial_style_landscape_1_full_media · bigo_ad_activity_interstitial_style_landscape_2 · bigo_ad_activity_interstitial_style_landscape_2_full_media · bigo_ad_activity_interstitial_style_landscape_3 · bigo_ad_activity_interstitial_style_landscape_3_no_gp_element · bigo_ad_activity_interstitial_style_landscape_4 · bigo_ad_activity_interstitial_style_landscape_4_full_media
bigo_ad_activity_interstitial_vpaid · bigo_ad_activity_pop_up_style_1 · bigo_ad_activity_pop_up_style_2 · bigo_ad_activity_pop_up_style_3 · bigo_ad_activity_pop_up_style_4 · bigo_ad_activity_pop_up_style_5 · bigo_ad_activity_pop_up_style_6 · bigo_ad_activity_popup · bigo_ad_activity_popup_banner · bigo_ad_activity_popup_vpaid · bigo_ad_activity_reward · bigo_ad_activity_webview
bigo_ad_banner_placeholder_img · bigo_ad_default_loading_layout · bigo_ad_dialog_reward_retain · bigo_ad_download_msg · bigo_ad_download_msg_white · bigo_ad_form_content · bigo_ad_form_content_dark · bigo_ad_form_fill_question · bigo_ad_form_fill_question_dark · bigo_ad_form_layout · bigo_ad_form_layout_dark · bigo_ad_form_notice_privacy
bigo_ad_form_notice_privacy_dark · bigo_ad_form_question · bigo_ad_form_question_dark · bigo_ad_form_select_question · bigo_ad_form_select_question_dark · bigo_ad_item_inter_countdown_bg · bigo_ad_item_interaction_horizontal · bigo_ad_item_interaction_vertical · bigo_ad_item_splash_countdown_bg · bigo_ad_label_layout · bigo_ad_layout_gesture_slide · bigo_ad_layout_gesture_zoom
bigo_ad_layout_ic_item · bigo_ad_layout_interstitial_ad_mark_top_left · bigo_ad_layout_interstitial_ad_warning_bottom · bigo_ad_layout_interstitial_icon_item_cta_des_style1 · bigo_ad_layout_interstitial_icon_item_cta_des_style2 · bigo_ad_layout_interstitial_icon_item_cta_style1 · bigo_ad_layout_interstitial_icon_item_cta_style2 · bigo_ad_layout_interstitial_icon_item_style · bigo_ad_layout_interstitial_icon_view_flow · bigo_ad_layout_interstitial_mid_page_loading_view · bigo_ad_layout_interstitial_mid_page_loading_view_landscape · bigo_ad_layout_interstitial_mid_page_native_fallback_view_download_info_landscape
bigo_ad_layout_interstitial_mid_page_native_fallback_view_landscape · bigo_ad_layout_interstitial_mid_page_native_view · bigo_ad_layout_interstitial_mid_page_native_view_landscape · bigo_ad_layout_interstitial_rich_video_multi_img_bottom · bigo_ad_layout_more · bigo_ad_layout_space · bigo_ad_layout_space_black · bigo_ad_native_banner_medium · bigo_ad_native_banner_small · bigo_ad_optionview_feedback · bigo_ad_pop_up_ad_msg · bigo_ad_popup_close_btn
bigo_ad_splash_ad_label · bigo_ad_splash_endpage1 · bigo_ad_splash_endpage1_slide · bigo_ad_splash_endpage_item_click_guide · bigo_ad_splash_endpage_item_cta · bigo_ad_splash_endpage_item_flash · bigo_ad_splash_endpage_item_slide · bigo_ad_splash_item_arrow · bigo_ad_splash_item_click_guide · bigo_ad_splash_item_cta · bigo_ad_splash_item_cta_bg · bigo_ad_splash_item_flash
bigo_ad_splash_item_ripple · bigo_ad_splash_item_slide · bigo_ad_splash_style_3_item_cta · bigo_ad_splash_style_3_multi_img · bigo_ad_splash_style_4_cta_widget · bigo_ad_splash_style_5_card_widget · bigo_ad_splash_style_fullscreen · bigo_ad_splash_style_fullscreen_immersive · bigo_ad_splash_style_fullscreen_interaction · bigo_ad_splash_style_fullscreen_interaction_immersive · bigo_ad_splash_style_fullscreen_vpaid · bigo_ad_splash_style_halfscreen
bigo_ad_splash_style_halfscreen_interaction · bigo_ad_splash_style_halfscreen_vpaid · bigo_ad_splash_style_root · bigo_ad_success_submit · bigo_ad_success_submit_dark · bigo_ad_view_click_guide · bigo_ad_view_click_guide_1 · bigo_ad_view_click_guide_2 · bigo_ad_view_click_guide_3 · bigo_ad_view_click_guide_landscape_1 · bigo_ad_view_click_guide_landscape_2 · bigo_ad_view_click_guide_landscape_3
bigo_ad_view_slide_gesture · bigo_ad_webview_loading · biome_item · bm_exo_legacy_player_control_view · bm_exo_list_divider · bm_exo_player_control_ffwd_button · bm_exo_player_control_rewind_button · bm_exo_player_control_view · bm_exo_player_view · bm_exo_styled_settings_list · bm_exo_styled_settings_list_item · bm_exo_styled_sub_settings_list_item
bm_exo_track_selection_dialog · bm_privacy_sheet · browser_actions_context_menu_page · browser_actions_context_menu_row · content_info_left_aligned_layout · controls · creative_debugger_displayed_ad_detail_activity · custom_dialog · debug_panel_item_action_button · debug_panel_item_ad_unit · debug_panel_item_ad_units · debug_panel_item_divider
debug_panel_item_mediation_adapter · debug_panel_item_switch · debug_panel_toolbar · design_bottom_navigation_item · design_bottom_sheet_dialog · design_layout_snackbar · design_layout_snackbar_include · design_layout_tab_icon · design_layout_tab_text · design_menu_item_action_area · design_navigation_item · design_navigation_item_header
design_navigation_item_separator · design_navigation_item_subheader · design_navigation_menu · design_navigation_menu_item · design_text_input_end_icon · design_text_input_start_icon · deubg_panel_item_header · deubg_panel_item_key_value · dialog_fullscreen · drawer_header · dt_store_promo_layout · exo_legacy_player_control_view
exo_list_divider · exo_player_control_ffwd_button · exo_player_control_rewind_button · exo_player_control_view · exo_player_view · exo_styled_player_control_ffwd_button · exo_styled_player_control_rewind_button · exo_styled_player_control_view · exo_styled_player_view · exo_styled_settings_list · exo_styled_settings_list_item · exo_styled_sub_settings_list_item
exo_track_selection_dialog · expand_button · ia_endcard_container · ia_layout_activity_internal_store_webpage · ia_layout_buffering_overlay · ia_layout_click_overlay · ia_layout_default_video_end_card · ia_layout_fullscreen_activity · ia_layout_fyber_ad_identifier · ia_layout_fyber_ad_identifier_relative · ia_layout_rich_media_video · ia_layout_skip_rewarded_dialog
ia_layout_video_view · ia_video_view_remote_ui · image_frame · ime_base_split_test_activity · ime_secondary_split_test_activity · instream_layout_detail_pause_overlay · instream_layout_detail_play_overlay · instream_layout_detail_play_overlay_simple · instream_layout_detail_preroll · linear_player_count_down · loading_alert · m3_alert_dialog
m3_alert_dialog_actions · m3_alert_dialog_title · m3_auto_complete_simple_item · m3_navigation_menu_divider · m3_navigation_menu_subheader · m3_side_sheet_dialog · mainlayout · map_selection_view · material_chip_input_combo · material_clock_display · material_clock_display_divider · material_clock_period_toggle
material_clock_period_toggle_land · material_clockface_textview · material_clockface_view · material_radial_view_group · material_textinput_timepicker · material_time_chip · material_time_input · material_timepicker · material_timepicker_dialog · material_timepicker_textinput_display · max_native_ad_banner_icon_and_text_layout · max_native_ad_banner_view
max_native_ad_leader_view · max_native_ad_media_banner_view · max_native_ad_medium_template_1 · max_native_ad_mrec_view · max_native_ad_recycler_view_item · max_native_ad_small_template_1 · max_native_ad_vertical_banner_view · max_native_ad_vertical_leader_view · max_native_ad_vertical_media_banner_view · mbridge_activity · mbridge_alertview_layout · mbridge_bt_container
mbridge_cm_alertview · mbridge_cm_feedback_notice_layout · mbridge_cm_feedbackview · mbridge_cm_loading_layout · mbridge_interstitial_activity · mbridge_more_offer_activity · mbridge_nativex_fullbasescreen · mbridge_nativex_fullscreen_top · mbridge_nativex_mbmediaview · mbridge_nativex_playerview · mbridge_order_layout_item · mbridge_order_layout_list_landscape
mbridge_order_layout_list_portrait · mbridge_playercommon_player_view · mbridge_reward_activity_video_templete · mbridge_reward_activity_video_templete_transparent · mbridge_reward_clickable_cta · mbridge_reward_end_card_layout_landscape · mbridge_reward_end_card_layout_landscape_1302 · mbridge_reward_end_card_layout_portrait · mbridge_reward_end_card_layout_portrait_1302 · mbridge_reward_end_card_more_offer_item · mbridge_reward_endcard_h5 · mbridge_reward_endcard_native_half_landscape
mbridge_reward_endcard_native_half_portrait · mbridge_reward_endcard_native_hor · mbridge_reward_endcard_native_land · mbridge_reward_endcard_vast · mbridge_reward_layer_floor · mbridge_reward_layer_floor_302 · mbridge_reward_layer_floor_5002010 · mbridge_reward_layer_floor_802 · mbridge_reward_layer_floor_902 · mbridge_reward_layer_floor_904 · mbridge_reward_layer_floor_bottom · mbridge_reward_layer_floor_half_portrait
mbridge_reward_more_offer_view · mbridge_reward_videoend_cover · mbridge_reward_videoview_item · mbridge_reward_view_tag_item · mbridge_same_choice_one_layout_landscape · mbridge_same_choice_one_layout_portrait · mbridge_splash_landscape · mbridge_splash_portrait · mdtb_interstitial_ad · mediation_debugger_ad_unit_detail_activity · mediation_debugger_list_item_right_detail · mediation_debugger_list_section
mediation_debugger_list_section_centered · mediation_debugger_list_view · mediation_debugger_multi_ad_activity · mediation_debugger_text_view_activity · mobilefuse_ad_fullscreen · mobilefuse_mraid_fullscreen_content · mobilefuse_native_interstitial_fullscreen · mobilefuse_native_interstitial_layout · mobilefuse_splash_ad_fullscreen · mobilefuse_vast_fullscreen_content · mobilefuse_vast_player_view · monetization_ads_feed_item
monetization_ads_feed_progressbar · monetization_ads_internal_adtune_container · monetization_ads_internal_divkit · monetization_ads_internal_instream_skin_v1 · monetization_ads_internal_instream_skin_v2 · monetization_ads_internal_multibanner_controls · monetization_ads_internal_native_interstitial_close · monetization_ads_internal_native_interstitial_content · monetization_ads_internal_native_interstitial_landscape_horizontal_media · monetization_ads_internal_native_interstitial_landscape_sponsored_container · monetization_ads_internal_native_interstitial_landscape_vetrical_media · monetization_ads_internal_native_interstitial_portrait
monetization_ads_internal_native_popup_item · monetization_ads_internal_native_rewarded_controls · monetization_ads_internal_native_rewarded_landscape · monetization_ads_internal_native_rewarded_portrait · monetization_ads_internal_outstream_controls_app_install · monetization_ads_internal_outstream_controls_default · monetization_ads_internal_outstream_controls_fullscreen_content_v1 · monetization_ads_internal_rewarded_close_verification · mrec_layout · mtrl_alert_dialog · mtrl_alert_dialog_actions · mtrl_alert_dialog_title
mtrl_alert_select_dialog_item · mtrl_alert_select_dialog_multichoice · mtrl_alert_select_dialog_singlechoice · mtrl_auto_complete_simple_item · mtrl_calendar_day · mtrl_calendar_day_of_week · mtrl_calendar_days_of_week · mtrl_calendar_horizontal · mtrl_calendar_month · mtrl_calendar_month_labeled · mtrl_calendar_month_navigation · mtrl_calendar_months
mtrl_calendar_vertical · mtrl_calendar_year · mtrl_layout_snackbar · mtrl_layout_snackbar_include · mtrl_navigation_rail_item · mtrl_picker_actions · mtrl_picker_dialog · mtrl_picker_fullscreen · mtrl_picker_header_dialog · mtrl_picker_header_fullscreen · mtrl_picker_header_selection_text · mtrl_picker_header_title_text
mtrl_picker_header_toggle · mtrl_picker_text_input_date · mtrl_picker_text_input_date_range · mtrl_search_bar · mtrl_search_view · notification_action · notification_action_tombstone · notification_media_action · notification_media_cancel_action · notification_template_big_media · notification_template_big_media_custom · notification_template_big_media_narrow
notification_template_big_media_narrow_custom · notification_template_custom_big · notification_template_icon_group · notification_template_lines_media · notification_template_media · notification_template_media_custom · notification_template_part_chronometer · notification_template_part_time · offline_ads_dialog · open_url · player_count_down · pob_custom_product_layout
pob_layout_rewardedad_skip_alert · pob_medium_template · pob_monitor_view · pob_small_template · preference · preference_category · preference_category_material · preference_dialog_edittext · preference_dropdown · preference_dropdown_material · preference_information · preference_information_material
preference_list_fragment · preference_material · preference_recyclerview · preference_widget_checkbox · preference_widget_seekbar · preference_widget_seekbar_material · preference_widget_switch · preference_widget_switch_compat · progress_count_down · rab_card_large · select_dialog_item_material · select_dialog_multichoice_material
select_dialog_singlechoice_material · selection_drag_btn · sentry_dialog_user_feedback · simple_list_item · simple_list_item_action · simple_list_item_multiple_choice · simple_list_item_single_choice · simpledialogfragment_check_box · simpledialogfragment_color_item · simpledialogfragment_color_wheel · simpledialogfragment_custom_view · simpledialogfragment_form
simpledialogfragment_form_item_check · simpledialogfragment_form_item_color · simpledialogfragment_form_item_datetime · simpledialogfragment_form_item_hint · simpledialogfragment_form_item_input · simpledialogfragment_form_item_spinner · simpledialogfragment_grid · simpledialogfragment_image · simpledialogfragment_image_hor_scroll · simpledialogfragment_image_vert_scroll · simpledialogfragment_input · simpledialogfragment_list
simpledialogfragment_list_item · simpledialogfragment_pin · simpledialogfragment_progress · simpledialogfragment_progress_circle · smaato_sdk_core_activity_internal_browser · smaato_sdk_interstitial_activity · smaato_sdk_interstitial_splash_activity · smaato_sdk_rewarded_ads_activity · smaato_sdk_richmedia_layout_closable · smaato_sdk_video_player_view · smaato_sdk_video_vast_video_player_view · support_simple_spinner_dropdown_item
taurusx_activity_appopen · taurusx_activity_appopen_native · taurusx_activity_fullscreen · taurusx_activity_h5 · taurusx_activity_web_view · taurusx_dialog_adtune_layout · temple904 · timer_count_down · tree_node_item · world_item · world_layer_item
```

**Layout milik aplikasi (20):** `activity_main`, `activity_drawer`, `drawer_header`, `activity_help`, `activity_create_world`, `activity_entities_nbt`, `activity_debug`, `activity_internal_browser`, `tree_node_item`, `world_item`, `world_layer_item`, `biome_item`, `map_selection_view`, `selection_drag_btn`, `custom_dialog`, `loading_alert`, `mainlayout`, `controls`, `deubg_panel_item_header`, `deubg_panel_item_key_value` (dua terakhir untuk panel debug internal). Sisanya 507 adalah library (BIGO ~165, mbridge ~50, exo/bm_exo ~30, material/mtrl/m3/abc/design ~200, iklan lain ~60).

### 3.3 SELURUH 6 menu

| Menu | Pemilik | Isi |
|---|---|---|
| `menu/drawer_menu` | **Aplikasi** | 5 item utama + submenu Advanced (7 item) |
| `menu/main_activity_toolbar_menu` | **Aplikasi** | Help (`action_help`, always), About (`action_about`, ifRoom) |
| `menu/create_world_activity_toolbar_menu` | **Aplikasi** | Save (`action_save`, always) |
| `menu/mediation_debugger_activity_menu` | AppLovin MAX | debugger mediasi |
| `menu/creative_debugger_displayed_ad_activity_menu` | AppLovin | creative debugger |
| `menu/dialog_buttons` | library (simple dialog) | tombol dialog generik |

### 3.4 SELURUH 954 drawable

```text
$applovin_ic_mute_to_unmute__0 · $applovin_ic_mute_to_unmute__1 · $applovin_ic_mute_to_unmute__2 · $applovin_ic_mute_to_unmute__3 · $applovin_ic_mute_to_unmute__4 · $applovin_ic_unmute_to_mute__0 · $applovin_ic_unmute_to_mute__1 · $applovin_ic_unmute_to_mute__2 · $applovin_ic_unmute_to_mute__3 · $applovin_ic_unmute_to_mute__4 · $avd_hide_password__0 · $avd_hide_password__1
$avd_hide_password__2 · $avd_show_password__0 · $avd_show_password__1 · $avd_show_password__2 · $ic_launcher_foreground__0 · $ic_palette_color__0 · $m3_avd_hide_password__0 · $m3_avd_hide_password__1 · $m3_avd_hide_password__2 · $m3_avd_show_password__0 · $m3_avd_show_password__1 · $m3_avd_show_password__2
$mtrl_checkbox_button_checked_unchecked__0 · $mtrl_checkbox_button_checked_unchecked__1 · $mtrl_checkbox_button_checked_unchecked__2 · $mtrl_checkbox_button_icon_checked_indeterminate__0 · $mtrl_checkbox_button_icon_checked_unchecked__0 · $mtrl_checkbox_button_icon_checked_unchecked__1 · $mtrl_checkbox_button_icon_checked_unchecked__2 · $mtrl_checkbox_button_icon_indeterminate_checked__0 · $mtrl_checkbox_button_icon_indeterminate_unchecked__0 · $mtrl_checkbox_button_icon_indeterminate_unchecked__1 · $mtrl_checkbox_button_icon_indeterminate_unchecked__2 · $mtrl_checkbox_button_icon_unchecked_checked__0
$mtrl_checkbox_button_icon_unchecked_checked__1 · $mtrl_checkbox_button_icon_unchecked_checked__2 · $mtrl_checkbox_button_icon_unchecked_indeterminate__0 · $mtrl_checkbox_button_icon_unchecked_indeterminate__1 · $mtrl_checkbox_button_icon_unchecked_indeterminate__2 · $mtrl_checkbox_button_unchecked_checked__0 · $mtrl_checkbox_button_unchecked_checked__1 · $mtrl_checkbox_button_unchecked_checked__2 · $mtrl_switch_thumb_checked_pressed__0 · $mtrl_switch_thumb_checked_unchecked__0 · $mtrl_switch_thumb_checked_unchecked__1 · $mtrl_switch_thumb_pressed_checked__0
$mtrl_switch_thumb_pressed_unchecked__0 · $mtrl_switch_thumb_unchecked_checked__0 · $mtrl_switch_thumb_unchecked_checked__1 · $mtrl_switch_thumb_unchecked_pressed__0 · abc_action_bar_item_background_material · abc_btn_borderless_material · abc_btn_check_material · abc_btn_check_material_anim · abc_btn_colored_material · abc_btn_default_mtrl_shape · abc_btn_radio_material · abc_btn_radio_material_anim
abc_cab_background_internal_bg · abc_cab_background_top_material · abc_control_background_material · abc_dialog_material_background · abc_edit_text_material · abc_ic_ab_back_material · abc_ic_arrow_drop_right_black_24dp · abc_ic_clear_material · abc_ic_go_search_api_material · abc_ic_menu_copy_mtrl_am_alpha · abc_ic_menu_cut_mtrl_alpha · abc_ic_menu_overflow_material
abc_ic_menu_paste_mtrl_am_alpha · abc_ic_menu_selectall_mtrl_alpha · abc_ic_menu_share_mtrl_alpha · abc_ic_search_api_material · abc_ic_voice_search_api_material · abc_item_background_holo_dark · abc_item_background_holo_light · abc_list_divider_material · abc_list_selector_background_transition_holo_dark · abc_list_selector_background_transition_holo_light · abc_list_selector_holo_dark · abc_list_selector_holo_light
abc_ratingbar_indicator_material · abc_ratingbar_material · abc_ratingbar_small_material · abc_seekbar_thumb_material · abc_seekbar_tick_mark_material · abc_seekbar_track_material · abc_spinner_textfield_background_material · abc_star_black_48dp · abc_star_half_black_48dp · abc_switch_thumb_material · abc_tab_indicator_material · abc_text_cursor_material
abc_textfield_search_material · abc_vector_test · ad_mob_outline_shape · ad_mob_rounded_button_shape · ad_mob_rounded_corners_shape · admob_close_button_black_circle_white_cross · admob_close_button_white_circle_black_cross · admob_close_button_white_cross · apd_ad_choice_preview · apd_admob_cta_round_outline · apd_admob_round_outline · apd_iab_ad_choice_preview
apd_icon_preview · apd_media_preview · applovin_creative_debugger_report_ad_rounded_button · applovin_ic_mediation_placeholder · applovin_ic_mute_to_unmute · applovin_ic_privacy_icon · applovin_ic_privacy_icon_layered_list · applovin_ic_unmute_to_mute · applovin_ic_white_small · applovin_mediation_debugger_switch_thumb · applovin_mediation_debugger_switch_track · applovin_rounded_black_background
applovin_rounded_button · applovin_rounded_text_view_border · arrow_mark · avd_hide_password · avd_show_password · bg_btn_get · bigo_ad_advertiser_background · bigo_ad_arrow_right · bigo_ad_banner_advertiser_background · bigo_ad_banner_button_bg_rectangle_blue · bigo_ad_banner_clickguide · bigo_ad_bg_ad_tag_white_border
bigo_ad_bg_mask · bigo_ad_btn_background · bigo_ad_btn_background_white · bigo_ad_btn_background_white_dark · bigo_ad_button_bg_circle_grey · bigo_ad_button_bg_rectangle_blue · bigo_ad_button_bg_rectangle_blue_2 · bigo_ad_button_bg_rectangle_green · bigo_ad_button_bg_rectangle_green_radius8 · bigo_ad_button_countdown_close · bigo_ad_button_stroke_circle_white · bigo_ad_cardview_bg
bigo_ad_click_guide · bigo_ad_click_guide2 · bigo_ad_click_guide_landscape · bigo_ad_click_guide_shadow · bigo_ad_click_ripple · bigo_ad_close_btn_background · bigo_ad_default_base_image · bigo_ad_default_progressbar · bigo_ad_domain_left_drawable · bigo_ad_double_video_play · bigo_ad_double_video_small_play · bigo_ad_edit_background
bigo_ad_edit_background_dark · bigo_ad_feeback_arrow_right · bigo_ad_feedback_advertise_info_background · bigo_ad_feedback_background · bigo_ad_feedback_top_slide_bar · bigo_ad_flash · bigo_ad_form_icon_back_up · bigo_ad_gift_widget · bigo_ad_gradual_background · bigo_ad_heart_widget · bigo_ad_ic_ad_label · bigo_ad_ic_back_gray
bigo_ad_ic_close · bigo_ad_ic_close2 · bigo_ad_ic_close3 · bigo_ad_ic_close4 · bigo_ad_ic_close5 · bigo_ad_ic_close_gray · bigo_ad_ic_close_gray_light · bigo_ad_ic_download · bigo_ad_ic_download_box · bigo_ad_ic_download_box_white · bigo_ad_ic_everyone · bigo_ad_ic_everyone_ic_info
bigo_ad_ic_everyone_white · bigo_ad_ic_info · bigo_ad_ic_info_gray · bigo_ad_ic_info_popup · bigo_ad_ic_info_white · bigo_ad_ic_left_round_black · bigo_ad_ic_left_round_white · bigo_ad_ic_loading_gp · bigo_ad_ic_media_mute · bigo_ad_ic_media_pause · bigo_ad_ic_media_play · bigo_ad_ic_media_unmute
bigo_ad_ic_phone · bigo_ad_ic_phone_white · bigo_ad_ic_recommend · bigo_ad_ic_recommend_white · bigo_ad_ic_right_black · bigo_ad_ic_right_round_black · bigo_ad_ic_right_round_white · bigo_ad_ic_right_white · bigo_ad_ic_safe · bigo_ad_ic_star · bigo_ad_ic_star2 · bigo_ad_ic_star2_white
bigo_ad_ic_star_half · bigo_ad_ic_star_half_white · bigo_ad_ic_star_normal · bigo_ad_ic_star_normal_white · bigo_ad_ic_star_white · bigo_ad_ic_web · bigo_ad_ic_web_back · bigo_ad_ic_web_back_disable · bigo_ad_ic_web_copy · bigo_ad_ic_web_forward · bigo_ad_ic_web_forward_enable · bigo_ad_ic_web_open_browser
bigo_ad_ic_web_refresh · bigo_ad_ic_web_white · bigo_ad_icon_background · bigo_ad_icon_background_dark · bigo_ad_icon_default · bigo_ad_icon_default_img · bigo_ad_icon_default_only_icon · bigo_ad_icon_default_radius22 · bigo_ad_icon_default_radius6 · bigo_ad_icon_novideo_default · bigo_ad_interaction_animation_bg · bigo_ad_interaction_shake_arrow
bigo_ad_interaction_shake_landscape_phone · bigo_ad_interaction_shake_phone · bigo_ad_interaction_slide_hand · bigo_ad_interaction_slide_line · bigo_ad_interaction_twist_arrow · bigo_ad_interaction_twist_landscape_phone · bigo_ad_interaction_twist_phone · bigo_ad_interstitial_landscape_ad_info_bg · bigo_ad_interstitial_rich_video_end_landscape_mask · bigo_ad_label_advertiser_background_raduis7 · bigo_ad_label_background · bigo_ad_landing_bottom_bar_bg
bigo_ad_landing_progress_bar · bigo_ad_landing_top_bar_bg · bigo_ad_landing_top_indicator · bigo_ad_landscape_slide_view · bigo_ad_layer_gift_color · bigo_ad_layer_gift_shadow · bigo_ad_layer_heart_color · bigo_ad_layer_heart_shadow · bigo_ad_mid_page_ic_gp_right · bigo_ad_multi_img_advertiser_background · bigo_ad_multi_img_horizontal_mask · bigo_ad_multi_img_vertical_mask
bigo_ad_popup_ad_info_background · bigo_ad_progressbar_white · bigo_ad_ru_background · bigo_ad_slide · bigo_ad_slide_gesture · bigo_ad_splash_endpage_bg · bigo_ad_splash_endpage_slide_bg · bigo_ad_splash_header_bg · bigo_ad_star_icon · bigo_ad_success · bigo_ad_tag_background · bigo_ad_top_left_advertiser_background
bigo_ad_view_ripple · bigo_ad_warning_background_radius8 · bigo_ad_web_loading_progress_bar · bm_bg_privacy_button · bm_bg_privacy_divider · bm_bg_privacy_sheet · bm_bg_ripple_circle · bm_ic_close · browser_background_selector · browser_backward_selector · browser_bottom_button_layout_bg · browser_forward_selector
browser_progress_bar · browser_secure_connection · browser_top_button_layout_bg · btn_checkbox_checked_mtrl · btn_checkbox_checked_to_unchecked_mtrl_animation · btn_checkbox_unchecked_mtrl · btn_checkbox_unchecked_to_checked_mtrl_animation · btn_radio_off_mtrl · btn_radio_off_to_on_mtrl_animation · btn_radio_on_mtrl · btn_radio_on_to_off_mtrl_animation · button_background
circle_progress_background · circular_shape · close_card_button · common_google_signin_btn_icon_dark · common_google_signin_btn_icon_dark_focused · common_google_signin_btn_icon_dark_normal · common_google_signin_btn_icon_disabled · common_google_signin_btn_icon_light · common_google_signin_btn_icon_light_focused · common_google_signin_btn_icon_light_normal · common_google_signin_btn_text_dark · common_google_signin_btn_text_dark_focused
common_google_signin_btn_text_dark_normal · common_google_signin_btn_text_disabled · common_google_signin_btn_text_light · common_google_signin_btn_text_light_focused · common_google_signin_btn_text_light_normal · cursor_drawable · debug_panel_icon_arrow_back · debug_panel_icon_arrow_navigate · debug_panel_icon_button_background · debug_panel_icon_error · debug_panel_icon_share · debug_panel_icon_success
debug_panel_icon_warning · debug_panel_item_action_button_background · debug_panel_item_mediation_adapter_background · debug_panel_item_mediation_adapter_icon_shape · debug_panel_item_navigation_background · design_fab_background · design_ic_visibility · design_ic_visibility_off · design_password_eye · design_snackbar_background · div_button_background · div_gallery_item_border
error_counter_background · exo_controls_fastforward · exo_controls_fullscreen_enter · exo_controls_fullscreen_exit · exo_controls_next · exo_controls_pause · exo_controls_play · exo_controls_previous · exo_controls_repeat_all · exo_controls_repeat_off · exo_controls_repeat_one · exo_controls_rewind
exo_controls_shuffle_off · exo_controls_shuffle_on · exo_controls_vr · exo_legacy_controls_fastforward · exo_legacy_controls_fullscreen_enter · exo_legacy_controls_fullscreen_exit · exo_legacy_controls_next · exo_legacy_controls_pause · exo_legacy_controls_play · exo_legacy_controls_previous · exo_legacy_controls_repeat_all · exo_legacy_controls_repeat_off
exo_legacy_controls_repeat_one · exo_legacy_controls_rewind · exo_legacy_controls_shuffle_off · exo_legacy_controls_shuffle_on · exo_legacy_controls_vr · exo_notification_fastforward · exo_notification_next · exo_notification_pause · exo_notification_play · exo_notification_previous · exo_notification_rewind · exo_notification_small_icon
exo_notification_stop · exo_rounded_rectangle · exo_styled_controls_audiotrack · exo_styled_controls_check · exo_styled_controls_fastforward · exo_styled_controls_fullscreen_enter · exo_styled_controls_fullscreen_exit · exo_styled_controls_next · exo_styled_controls_overflow_hide · exo_styled_controls_overflow_show · exo_styled_controls_pause · exo_styled_controls_play
exo_styled_controls_previous · exo_styled_controls_repeat_all · exo_styled_controls_repeat_off · exo_styled_controls_repeat_one · exo_styled_controls_rewind · exo_styled_controls_settings · exo_styled_controls_shuffle_off · exo_styled_controls_shuffle_on · exo_styled_controls_simple_fastforward · exo_styled_controls_simple_rewind · exo_styled_controls_speed · exo_styled_controls_subtitle_off
exo_styled_controls_subtitle_on · exo_styled_controls_vr · fab_label_background · home · ia_bg_circle_overlay · ia_bg_green · ia_bg_text_overlay · ia_circle_overlay_bg · ia_close · ia_digital_turbine_logo · ia_fyber_clip_drawable · ia_fyber_info_button
ia_ib_background · ia_ib_close · ia_ib_left_arrow · ia_ib_refresh · ia_ib_right_arrow · ia_ib_unleft_arrow · ia_ib_unright_arrow · ia_progress_bar_drawable · ia_round_overlay_bg · ia_round_overlay_bg_fyber · ia_round_overlay_bg_with_close · ia_sel_expand_collapse
ia_sel_mute · ic_100tb · ic_add · ic_arrow_back_black_24 · ic_arrow_down_24dp · ic_arrow_forward · ic_arrow_forward_accent · ic_book · ic_check · ic_clear_black_24 · ic_clear_search · ic_clock_black_24dp
ic_exit · ic_expand_less_22px · ic_expand_more_22px · ic_group_people · ic_human · ic_info · ic_keyboard_black_24dp · ic_launcher_foreground · ic_layers · ic_list · ic_m3_chip_check · ic_m3_chip_checked_circle
ic_m3_chip_close · ic_map · ic_mtrl_arrow_circle · ic_mtrl_checked_circle · ic_mtrl_chip_checked_black · ic_mtrl_chip_checked_circle · ic_mtrl_chip_close_circle · ic_person · ic_round_close_24 · ic_round_replay_24 · ic_round_skip_next_24 · ic_round_volume_off_24
ic_round_volume_up_24 · ic_save · ic_search · ic_search_black_24 · ic_tune · ic_upload · info_badge · instream_skip · m3_avd_hide_password · m3_avd_show_password · m3_bottom_sheet_drag_handle · m3_password_eye
m3_popupmenu_background_overlay · m3_radiobutton_ripple · m3_selection_control_ripple · m3_split_button_chevron_avd · m3_tabs_background · m3_tabs_line_indicator · m3_tabs_rounded_line_indicator · m3_tabs_transparent_background · material_cursor_drawable · material_ic_calendar_black_24dp · material_ic_clear_black_24dp · material_ic_edit_black_24dp
material_ic_keyboard_arrow_left_black_24dp · material_ic_keyboard_arrow_next_black_24dp · material_ic_keyboard_arrow_previous_black_24dp · material_ic_keyboard_arrow_right_black_24dp · material_ic_menu_arrow_down_black_24dp · material_ic_menu_arrow_up_black_24dp · mbridge_arrow_down_white · mbridge_arrow_down_white_blackbg · mbridge_arrow_up_black · mbridge_arrow_up_white · mbridge_banner_close · mbridge_bottom_media_control
mbridge_cm_alertview_bg · mbridge_cm_alertview_cancel_bg · mbridge_cm_alertview_cancel_bg_nor · mbridge_cm_alertview_cancel_bg_pressed · mbridge_cm_alertview_confirm_bg · mbridge_cm_alertview_confirm_bg_nor · mbridge_cm_alertview_confirm_bg_pressed · mbridge_cm_backward · mbridge_cm_backward_disabled · mbridge_cm_backward_nor · mbridge_cm_backward_selected · mbridge_cm_browser
mbridge_cm_btn_shake · mbridge_cm_circle_50black · mbridge_cm_end_animation · mbridge_cm_exits · mbridge_cm_exits_nor · mbridge_cm_exits_selected · mbridge_cm_feedback_btn_bg · mbridge_cm_feedback_choice_btn_bg · mbridge_cm_feedback_choice_btn_bg_nor · mbridge_cm_feedback_choice_btn_bg_pressed · mbridge_cm_feedback_dialog_view_bg · mbridge_cm_feedback_dialog_view_btn_bg
mbridge_cm_forward · mbridge_cm_forward_disabled · mbridge_cm_forward_nor · mbridge_cm_forward_selected · mbridge_cm_head · mbridge_cm_highlight · mbridge_cm_progress · mbridge_cm_progress_drawable · mbridge_cm_progress_icon · mbridge_cm_refresh · mbridge_cm_refresh_nor · mbridge_cm_refresh_selected
mbridge_cm_tail · mbridge_demo_star_nor · mbridge_demo_star_sel · mbridge_finger_media_control · mbridge_icon_click_circle · mbridge_icon_click_hand · mbridge_icon_play_bg · mbridge_interstitial_close · mbridge_interstitial_over · mbridge_native_advanced_close_icon · mbridge_native_bg_loading_camera · mbridge_nativex_close
mbridge_nativex_cta_land_nor · mbridge_nativex_cta_land_pre · mbridge_nativex_cta_por_nor · mbridge_nativex_cta_por_pre · mbridge_nativex_full_land_close · mbridge_nativex_full_protial_close · mbridge_nativex_fullview_background · mbridge_nativex_pause · mbridge_nativex_play · mbridge_nativex_play_bg · mbridge_nativex_play_progress · mbridge_nativex_sound1
mbridge_nativex_sound2 · mbridge_nativex_sound3 · mbridge_nativex_sound4 · mbridge_nativex_sound5 · mbridge_nativex_sound6 · mbridge_nativex_sound7 · mbridge_nativex_sound8 · mbridge_nativex_sound_animation · mbridge_nativex_sound_bg · mbridge_nativex_sound_close · mbridge_nativex_sound_open · mbridge_order_layout_list_bg
mbridge_reward_activity_ad_end_land_des_rl_hot · mbridge_reward_close · mbridge_reward_close_ec · mbridge_reward_end_card_shape_bg · mbridge_reward_end_close_shape_oval · mbridge_reward_end_land_shape · mbridge_reward_end_pager_logo · mbridge_reward_end_shape_oval · mbridge_reward_flag_cn · mbridge_reward_flag_en · mbridge_reward_more_offer_default_bg · mbridge_reward_notice
mbridge_reward_popview_close · mbridge_reward_shape_choice · mbridge_reward_shape_choice_rl · mbridge_reward_shape_end_pager · mbridge_reward_shape_mf_selector · mbridge_reward_shape_mof_like_normal · mbridge_reward_shape_mof_like_pressed · mbridge_reward_shape_order · mbridge_reward_shape_order_history · mbridge_reward_shape_progress · mbridge_reward_shape_videoend_buttonbg · mbridge_reward_sound_close
mbridge_reward_sound_open · mbridge_reward_two_title_arabia_land · mbridge_reward_two_title_arabia_por · mbridge_reward_two_title_en_land · mbridge_reward_two_title_en_por · mbridge_reward_two_title_france_land · mbridge_reward_two_title_france_por · mbridge_reward_two_title_germany_land · mbridge_reward_two_title_germany_por · mbridge_reward_two_title_japan_land · mbridge_reward_two_title_japan_por · mbridge_reward_two_title_korea_land
mbridge_reward_two_title_korea_por · mbridge_reward_two_title_russian_land · mbridge_reward_two_title_russian_por · mbridge_reward_two_title_zh · mbridge_reward_two_title_zh_trad · mbridge_reward_user · mbridge_reward_vast_end_close · mbridge_reward_vast_end_ok · mbridge_reward_video_icon · mbridge_reward_video_progress_bg · mbridge_reward_video_progressbar_bg · mbridge_reward_video_time_count_num_bg
mbridge_shape_btn · mbridge_shape_corners_bg · mbridge_shape_line · mbridge_shape_splash_circle_14 · mbridge_shape_splash_corners_14 · mbridge_shape_splash_rightbottom_corners_10 · mbridge_slide_hand · mbridge_slide_rightarrow · mbridge_splash_ad_right_bottom_corner_en · mbridge_splash_ad_right_bottom_corner_zh · mbridge_splash_btn_arrow_right · mbridge_splash_btn_circle
mbridge_splash_btn_finger · mbridge_splash_btn_go · mbridge_splash_btn_light · mbridge_splash_button_bg_gray · mbridge_splash_button_bg_gray_55 · mbridge_splash_button_bg_green · mbridge_splash_close_bg · mbridge_splash_m_circle · mbridge_splash_notice · mbridge_splash_pop_ad · mbridge_splash_pop_ad_en · mbridge_splash_popview_close
mbridge_splash_popview_default · mbridge_video_common_full_star · mbridge_video_common_full_while_star · mbridge_video_common_half_star · mobilefuse_close_btn · mobilefuse_vast_endcard_close_btn · mobilefuse_vast_endcard_transparent_close_btn · moloco_close · moloco_replay · moloco_star · moloco_twotone_pause_24 · moloco_twotone_play_arrow_24
moloco_twotone_volume_off_24 · moloco_twotone_volume_up_24 · moloco_volume_off · moloco_volume_on · monetization_ads_instream_internal_advertiser · monetization_ads_instream_internal_advertiser_social · monetization_ads_instream_internal_click_button · monetization_ads_instream_internal_progress_display · monetization_ads_instream_internal_progress_display_v2 · monetization_ads_instream_internal_skip · monetization_ads_instream_internal_skip_focused · monetization_ads_instream_internal_skip_unfocused
monetization_ads_internal_adtune_control_shape_white · monetization_ads_internal_adtune_webview_container_shape_white · monetization_ads_internal_adtune_white · monetization_ads_internal_adtune_white_background · monetization_ads_internal_app_install_circle_for_dark_background · monetization_ads_internal_app_install_circle_for_light_background · monetization_ads_internal_app_install_close_grey · monetization_ads_internal_app_install_close_white · monetization_ads_internal_app_install_feedback_grey · monetization_ads_internal_app_install_feedback_white · monetization_ads_internal_call_to_action_background · monetization_ads_internal_call_to_action_background_dark
monetization_ads_internal_call_to_action_background_light · monetization_ads_internal_call_to_action_background_shape_blue · monetization_ads_internal_call_to_action_blue_solid_background · monetization_ads_internal_call_to_action_blue_stroke_background · monetization_ads_internal_call_to_action_gradient_yellow_background · monetization_ads_internal_call_to_action_green_solid_background · monetization_ads_internal_circle_progress_bar · monetization_ads_internal_circular_close_progress · monetization_ads_internal_close_light · monetization_ads_internal_controls_background · monetization_ads_internal_default_adtune_feedback_icon · monetization_ads_internal_feed_progress
monetization_ads_internal_fullscreen_content_v1_background_tile · monetization_ads_internal_fullscreen_content_v1_background_tile_dark · monetization_ads_internal_fullscreen_content_v1_close_disabled · monetization_ads_internal_fullscreen_content_v1_close_enabled · monetization_ads_internal_fullscreen_content_v1_close_selector · monetization_ads_internal_fullscreen_content_v1_icon_background · monetization_ads_internal_google_play_icon_white · monetization_ads_internal_ic_close_gray · monetization_ads_internal_ic_sound_off_app_install · monetization_ads_internal_ic_sound_off_default · monetization_ads_internal_ic_sound_off_default_focused · monetization_ads_internal_ic_sound_off_default_unfocused
monetization_ads_internal_ic_sound_on_app_install · monetization_ads_internal_ic_sound_on_default · monetization_ads_internal_ic_sound_on_default_focused · monetization_ads_internal_ic_sound_on_default_unfocused · monetization_ads_internal_icon_empty · monetization_ads_internal_icon_grey_solid_background_large_corners · monetization_ads_internal_icon_grey_solid_background_small_corners · monetization_ads_internal_instream_adtune_control_v2 · monetization_ads_internal_instream_adtune_control_v2_focused · monetization_ads_internal_instream_adtune_control_v2_unfocused · monetization_ads_internal_instream_advertiser_block_background · monetization_ads_internal_instream_advertiser_block_background_focused
monetization_ads_internal_instream_advertiser_block_background_unfocused · monetization_ads_internal_instream_call_to_action_background · monetization_ads_internal_instream_ic_sound_off_v2 · monetization_ads_internal_instream_ic_sound_on_v2 · monetization_ads_internal_instream_mute_checkable_v2 · monetization_ads_internal_instream_skip_arrow · monetization_ads_internal_landscape_close_button · monetization_ads_internal_multibanner_left_button_icon · monetization_ads_internal_multibanner_right_button_icon · monetization_ads_internal_mute_checkable_app_install · monetization_ads_internal_mute_checkable_default · monetization_ads_internal_mute_checkable_fullscreen_content_v1
monetization_ads_internal_qrcode_container_background · monetization_ads_internal_qrcode_view_background · monetization_ads_internal_rating_bar · monetization_ads_internal_reward_timer_view_shape_gray · monetization_ads_internal_rewarded_close · monetization_ads_internal_rewarded_close_shape_gray · monetization_ads_internal_rewarded_close_verification_background · monetization_ads_internal_rewarded_close_verification_button_close_background · monetization_ads_internal_rewarded_close_verification_button_dismiss_background · monetization_ads_internal_rewarded_close_with_padding · monetization_ads_internal_rewarded_feedback · monetization_ads_internal_rewarded_feedback_with_padding
monetization_ads_internal_rewarded_timer_background · monetization_ads_internal_sponsored_container_background · monetization_ads_internal_star_black · monetization_ads_internal_star_half_black · monetization_ads_mediation_api_feedback_icon · monetization_ads_progress_view_background · monetization_ads_video_progress_bar_background · mtrl_bottomsheet_drag_handle · mtrl_checkbox_button · mtrl_checkbox_button_checked_unchecked · mtrl_checkbox_button_icon · mtrl_checkbox_button_icon_checked_indeterminate
mtrl_checkbox_button_icon_checked_unchecked · mtrl_checkbox_button_icon_indeterminate_checked · mtrl_checkbox_button_icon_indeterminate_unchecked · mtrl_checkbox_button_icon_unchecked_checked · mtrl_checkbox_button_icon_unchecked_indeterminate · mtrl_checkbox_button_unchecked_checked · mtrl_dialog_background · mtrl_dropdown_arrow · mtrl_ic_arrow_drop_down · mtrl_ic_arrow_drop_up · mtrl_ic_cancel · mtrl_ic_check_mark
mtrl_ic_checkbox_checked · mtrl_ic_checkbox_unchecked · mtrl_ic_error · mtrl_ic_indeterminate · mtrl_navigation_bar_item_background · mtrl_popupmenu_background · mtrl_popupmenu_background_overlay · mtrl_switch_thumb · mtrl_switch_thumb_checked · mtrl_switch_thumb_checked_pressed · mtrl_switch_thumb_checked_unchecked · mtrl_switch_thumb_pressed
mtrl_switch_thumb_pressed_checked · mtrl_switch_thumb_pressed_unchecked · mtrl_switch_thumb_unchecked · mtrl_switch_thumb_unchecked_checked · mtrl_switch_thumb_unchecked_pressed · mtrl_switch_track · mtrl_switch_track_decoration · mtrl_tabs_default_indicator · native_animation_background · navigation_empty_icon · nbt_byte · nbt_byte_arr
nbt_compound · nbt_double · nbt_float · nbt_int · nbt_int_arr · nbt_list · nbt_long · nbt_short · nbt_string · neutral_counter_background · notification_action_background · notification_bg
notification_bg_low · notification_icon_background · notification_oversize_large_icon_bg · notification_template_icon_bg · notification_template_icon_low_bg · notification_tile_bg · offline_dialog_background · offline_dialog_default_icon_42dp · open_url_background · pn_circular_progress · pob_close_button · pob_ic_action_back
pob_ic_action_cancel · pob_ic_action_forward · pob_ic_action_refresh · pob_ic_action_web_site · pob_install_btn_drawable · pob_template_button_drawable · preference_list_divider_material · question_mark · rab_action_bg · rab_badge_bg · round_18_ec3a48 · round_7_white_bg
round_80000000_100 · round_80000000_3 · round_9_black_bg · round_b3000000 · round_b4001b · round_bottom_b2c4ff00 · round_edc900_100 · round_edc900_3 · round_ff81a700 · round_ff81a700_100 · round_gray · round_white_bg
seekbar_progress_drawable · sentry_edit_text_border · sentry_logo_dark · sentry_oval_button_ripple_background · sentry_user_feedback_button_logo_24 · smaato_sdk_core_browser_bottom_button_layout_bg · smaato_sdk_core_browser_progress_bar · smaato_sdk_core_browser_top_button_layout_bg · smaato_sdk_core_count_down_bg · smaato_sdk_core_ic_browser_background_selector · smaato_sdk_core_ic_browser_backward_selector · smaato_sdk_core_ic_browser_forward_selector
smaato_sdk_core_ic_browser_secure_connection · smaato_sdk_core_progress_bar · smaato_sdk_core_video_progress_bar · step1_tut · step2_tut · step3_tut · step4_tut · steve · table_cell_background · taurusx_close_no_bg · taurusx_close_pressed · taurusx_ic_close
taurusx_ic_more · taurusx_ic_more_no_bg · taurusx_ic_privacy · taurusx_ic_reload · taurusx_inner_btn_bg_pressed · taurusx_inner_video_mute · taurusx_inner_video_no_mute · taurusx_inner_video_skip · taurusx_progress_rotate · taurusx_skip_pressed · taurusx_tax_btn_dark_bg · taurusx_tax_ic_skip
taurusx_tax_ic_volume_off · taurusx_tax_ic_volume_on · taurusx_tax_round_36_bg · taurusx_tax_round_36_bg2 · taurusx_tax_round_white_bg · taurusx_tax_volume_bg · test_level_drawable · tile · tiles · timer_count_down_background · tooltip_frame_dark · tooltip_frame_light
tt_ad_arrow_backward · tt_ad_arrow_forward · tt_ad_arrow_right · tt_ad_bg_header_gradient · tt_ad_closed_background_300_250 · tt_ad_closed_background_320_50 · tt_ad_feedback · tt_ad_feedback_new · tt_ad_link · tt_ad_logo · tt_ad_refresh · tt_ad_threedots
tt_ad_xmark · tt_ad_zoom_down · tt_ad_zoom_up · tt_app_open_top_bg · tt_blue_hand · tt_button_back · tt_button_blue_back · tt_close_btn · tt_close_move_details_normal · tt_close_move_details_pressed · tt_dislike_icon · tt_dislike_icon2
tt_dislike_toast · tt_enlarge_video · tt_item_background_material · tt_lefterbackicon_titlebar · tt_lefterbackicon_titlebar_for_dark · tt_lefterbackicon_titlebar_press · tt_lefterbackicon_titlebar_press_for_dark · tt_mute · tt_new_pause_video · tt_new_pause_video_press · tt_new_play_video · tt_normalscreen_loading
tt_pangle_ad_close_drawable · tt_refreshing_video_textpage_normal · tt_refreshing_video_textpage_pressed · tt_report_ad_arrow · tt_reward_full_feedback · tt_reward_full_mute · tt_reward_full_unmute · tt_shadow_fullscreen_top · tt_shadow_lefterback_titlebar · tt_shadow_lefterback_titlebar_press · tt_shadow_lefterback_titlebar_press_withoutnight · tt_shadow_lefterback_titlebar_withoutnight
tt_shrink_fullscreen · tt_shrink_video · tt_skip_btn · tt_splash_hand · tt_splash_hand2 · tt_splash_rock · tt_splash_shake_hand · tt_splash_slide_right_bg · tt_splash_slide_right_circle · tt_splash_slide_up_10 · tt_splash_slide_up_arrow · tt_splash_slide_up_bg
tt_splash_slide_up_circle · tt_splash_slide_up_finger · tt_splash_twist · tt_splash_unlock_btn_bg · tt_splash_unlock_icon_empty · tt_splash_unlock_image_arrow · tt_splash_unlock_image_go · tt_star · tt_star_thick · tt_star_thick_dark · tt_suggestion_logo · tt_titlebar_close_drawable
tt_titlebar_close_for_dark · tt_titlebar_close_press · tt_titlebar_close_press_for_dark · tt_ugen_rating_star · tt_unmute · tt_up_slide · tt_user · tt_user_info · tt_video_close_drawable · tt_white_hand · tt_white_lefterbackicon_titlebar · tt_white_lefterbackicon_titlebar_press
tt_white_righterbackicon_titlebar · tt_white_slide_up · tt_wriggle_union · tt_wriggle_union_white · warning_counter_background · warning_error_counter_background
```

**Drawable milik aplikasi (penting):** `ic_add`, `ic_layers`, `ic_upload`, `ic_map`, `ic_tune`, `ic_person`, `ic_group_people`, `ic_exit`, `ic_list`, `ic_search`, `ic_book`, `ic_info`, `ic_save`, `fab_label_background`, `step1_tut`–`step4_tut`, `steve`, `nbt_byte`, `nbt_byte_arr`, `nbt_compound`, `nbt_double`, `nbt_float`, `nbt_int`, `nbt_int_arr`, `nbt_list`, `nbt_long`, `nbt_short`, `nbt_string`, `test_level_drawable`, `tile`, `tiles`, `question_mark`, `home`, `ic_100tb`.

### 3.5 Mipmap, raw, xml, font

| Mipmap (9) | `close`, `ic_launcher`, `ic_launcher_background`, `ic_launcher_foreground`, `ic_launcher_round`, `mute`, `replay`, `skip`, `unmute` |
|---|---|
| **Raw (17)** | `applovin_consent_flow_unified_cmp`, `com_android_billingclient_heterodyne_info`, `com_android_billingclient_registration_info`, `html_player_vast`, `keep_cronet_api`, `license`, `mobilefuse_omsdk_license`, `mobilefuse_omsdk_v1`, `monetization_ads_bundled_cert`, `monetization_ads_keep`, `monetization_ads_sdkinternalca`, `omid_session_client_v1_5_3`, `omsdk_v1`, `omsdk_v1_5_3`, `russian_trusted_root_ca`, `russian_trusted_sub_ca`, `shape_corner` |
| **XML (13)** | `ad_services_config`, `com_android_billingclient_phenotype`, `debug_panel_file_paths`, **`file_paths`**, `image_share_filepaths`, `m3_button_group_child_size_change`, `m3_split_button_inner_corner_size_state_list`, `m3expressive_button_shape_state_list`, `m3expressive_connected_buttons_inner_corner_size_state_list`, `nath_tax_file_path`, **`network_security_config`**, `progress_bg`, `splits0` |
| **Font (11)** | `inter` (**font aplikasi**), `roboto_medium_numbers`, `monetization_ads_internal_font_*` (8 varian: bold, light, medium, regular, text_bold, text_light, text_medium, text_regular, text_regular_italic) |

`@xml/file_paths` (dari xmltree): `<paths><external-files-path name="external_files" path="." /></paths>` — akar direktori `getExternalFilesDir()` diekspos untuk FileProvider (digunakan “Open in Minecraft” dan ekspor texture pack).

### 3.6 SELURUH 742 string

```text
CronetProviderClassName · abc_action_bar_home_description · abc_action_bar_up_description · abc_action_menu_overflow_description · abc_action_mode_done · abc_activity_chooser_view_see_all · abc_activitychooserview_choose_application · abc_capital_off · abc_capital_on · abc_menu_alt_shortcut_label · abc_menu_ctrl_shortcut_label · abc_menu_delete_shortcut_label · abc_menu_enter_shortcut_label · abc_menu_function_shortcut_label
abc_menu_meta_shortcut_label · abc_menu_shift_shortcut_label · abc_menu_space_shortcut_label · abc_menu_sym_shortcut_label · abc_prepend_shortcut_label · abc_search_hint · abc_searchview_description_clear · abc_searchview_description_query · abc_searchview_description_search · abc_searchview_description_submit · abc_searchview_description_voice · abc_shareactionprovider_share_with · abc_shareactionprovider_share_with_application · abc_toolbar_collapse_description
ad_customisation_title · ad_mob_ad_badge · ad_units · ads_sdk_version · advanced_section · advertisement_network_settings · age_restricted_user · alphanumeric_only_error · androidx_startup · apd_admob_banner_ad_text · apd_admob_banner_body_tools · apd_admob_banner_cta_tools · apd_admob_banner_headline_tools · apd_admob_banner_icon_decs
api_level · app_name · app_review_status · app_version · appbar_scrolling_view_behavior · application_info · applovin_agree_message · applovin_alt_privacy_policy_text · applovin_continue_button_text · applovin_creative_debugger_disabled_text · applovin_creative_debugger_no_ads_text · applovin_list_item_image_description · applovin_pp_and_tos_title · applovin_pp_title
applovin_privacy_policy_text · applovin_terms_of_service_text · applovin_terms_of_use_text · aps_mraid_close_button · autonomous_entities_nbt · bigo_ad_about_app_default · bigo_ad_click_to_try · bigo_ad_comment_num_text · bigo_ad_cta_default · bigo_ad_cta_download_default · bigo_ad_description_default · bigo_ad_double_video_choose_video · bigo_ad_double_video_count_down · bigo_ad_download_num_text
bigo_ad_feedback_copied · bigo_ad_feedback_copy_ad_id · bigo_ad_feedback_copy_link · bigo_ad_feedback_hide_ads · bigo_ad_feedback_link_copied · bigo_ad_feedback_rec_rule · bigo_ad_feedback_report_ads · bigo_ad_feedback_user_privacy · bigo_ad_feedback_why_this_ad · bigo_ad_form_agree · bigo_ad_form_cancel · bigo_ad_form_privacy_content · bigo_ad_form_privacy_notice · bigo_ad_form_question_hint
bigo_ad_form_submit · bigo_ad_form_warning · bigo_ad_ic_application · bigo_ad_ic_blank · bigo_ad_ic_everyone · bigo_ad_ic_recommended · bigo_ad_ic_web · bigo_ad_interaction_jump_hint · bigo_ad_interaction_shake · bigo_ad_interaction_slide · bigo_ad_interaction_twist · bigo_ad_link_copied · bigo_ad_lose_reward · bigo_ad_multi_img_more_text
bigo_ad_resume · bigo_ad_skip · bigo_ad_skip_video · bigo_ad_splash_skip · bigo_ad_splash_skip_after · bigo_ad_tag · bigo_ad_tag_back · bigo_ad_tag_close · bigo_ad_tag_forward · bigo_ad_tag_progressbar · bigo_ad_tag_title · bigo_ad_title_default · bigo_ad_webview_loading · biomedata_nbt
bottom_sheet_behavior · bottomsheet_action_collapse · bottomsheet_action_expand · bottomsheet_action_expand_halfway · bottomsheet_drag_handle_content_description · browser_hostname_content_description · btn_browser_backward_content_description · btn_browser_close_content_description · btn_browser_forward_content_description · btn_browser_open_content_description · btn_browser_refresh_content_description · call_notification_answer_action · call_notification_answer_video_action · call_notification_decline_action
call_notification_hang_up_action · call_notification_incoming_text · call_notification_ongoing_text · call_notification_screening_text · campaign.appName · campaign.iconUrl · campaign.imageUrl · change_world · character_counter_content_description · character_counter_overflowed_content_description · character_counter_pattern · clear · clear_color · clear_text_end_icon_content_description
close_button_description · close_card_votes · close_drawer · close_sheet · color_picker · com_moloco_sdk_xenoss_player_learn_more · com_moloco_sdk_xenoss_player_skip · common_google_play_services_enable_button · common_google_play_services_enable_text · common_google_play_services_enable_title · common_google_play_services_install_button · common_google_play_services_install_text · common_google_play_services_install_title · common_google_play_services_notification_channel_name
common_google_play_services_notification_ticker · common_google_play_services_unknown_issue · common_google_play_services_unsupported_text · common_google_play_services_update_button · common_google_play_services_update_text · common_google_play_services_update_title · common_google_play_services_updating_text · common_google_play_services_wear_update_text · common_open_on_phone · common_signin_button_text · common_signin_button_text_long · completed_integration · content_info_icon · copy
copy_toast_msg · custom_cta_ad_customisation_label · custom_cta_button · custom_cta_label · date · debug_panel_ad_unit_format · debug_panel_ad_unit_id · default_error_message · default_popup_window_title · defaults · disable · div_slider_range_end · div_slider_range_start · do_you_want_to_enable_logging
dropdown_menu · dyAction.getClick · dyAction.getLogicClick · dyAction.getLongClick · dyAction.getMove · dyAction.getWobble · dyEffect.getCountDown · dyEffect.getVisible · dyEffect.getVisibleParam · dyEffect.getWobble · dyStrategy.feedback · dyStrategy.getActivity · dyStrategy.getClose · dyStrategy.getDeeplink
dyStrategy.getDownload · dyStrategy.notice · dyStrategy.permissionInfo · dyStrategy.privateAddress · email_address · enable · error_a11y_label · error_icon_content_description · exo_controls_cc_disabled_description · exo_controls_cc_enabled_description · exo_controls_custom_playback_speed · exo_controls_fastforward_description · exo_controls_fullscreen_enter_description · exo_controls_fullscreen_exit_description
exo_controls_hide · exo_controls_next_description · exo_controls_overflow_hide_description · exo_controls_overflow_show_description · exo_controls_pause_description · exo_controls_play_description · exo_controls_playback_speed · exo_controls_previous_description · exo_controls_repeat_all_description · exo_controls_repeat_off_description · exo_controls_repeat_one_description · exo_controls_rewind_description · exo_controls_seek_bar_description · exo_controls_settings_description
exo_controls_show · exo_controls_shuffle_off_description · exo_controls_shuffle_on_description · exo_controls_stop_description · exo_controls_time_placeholder · exo_controls_vr_description · exo_download_completed · exo_download_description · exo_download_downloading · exo_download_failed · exo_download_notification_channel_name · exo_download_paused · exo_download_paused_for_network · exo_download_paused_for_wifi
exo_download_removing · exo_item_list · exo_track_bitrate · exo_track_mono · exo_track_resolution · exo_track_role_alternate · exo_track_role_closed_captions · exo_track_role_commentary · exo_track_role_supplementary · exo_track_selection_auto · exo_track_selection_none · exo_track_selection_title_audio · exo_track_selection_title_text · exo_track_selection_title_video
exo_track_stereo · exo_track_surround · exo_track_surround_5_point_1 · exo_track_surround_7_point_1 · exo_track_unknown · exo_track_unknown_name · expand_button_title · exposed_dropdown_menu_content_description · fab_transformation_scrim_behavior · fab_transformation_sheet_behavior · fallback_menu_item_copy_link · fallback_menu_item_open_in_browser · fallback_menu_item_share_link · features
feedback_form · has_location_consent · has_user_consent · hex · hide_bottom_view_on_scroll_behavior · ia_ad_identifier · ia_skip_rewarded_dialog_close_button · ia_skip_rewarded_dialog_keep_watching · ia_skip_rewarded_dialog_sub_title · ia_skip_rewarded_dialog_title · ia_str_video_error · ia_video_app_info_text · ia_video_before_skip_format · ia_video_install_now_text
ia_video_instant_install_text · ia_video_skip_text · icon_content_description · in_progress · indeterminate · info_icon_view_description · input_not_a_given_option · integrated · integration_errors · invalid_email_address · invalid_integration · invalid_mediation_adapter_version · item_view_role_description · learn_more
letters_only_error · loading · logging_is_disabled · logging_is_enabled · login · m3_exceed_max_badge_text_suffix · m3_loading_indicator_content_description · m3_ref_typeface_brand_medium · m3_ref_typeface_brand_regular · m3_ref_typeface_plain_medium · m3_ref_typeface_plain_regular · m3_sys_motion_easing_emphasized · m3_sys_motion_easing_emphasized_accelerate · m3_sys_motion_easing_emphasized_decelerate
m3_sys_motion_easing_emphasized_path_data · m3_sys_motion_easing_legacy · m3_sys_motion_easing_legacy_accelerate · m3_sys_motion_easing_legacy_decelerate · m3_sys_motion_easing_linear · m3_sys_motion_easing_standard · m3_sys_motion_easing_standard_accelerate · m3_sys_motion_easing_standard_decelerate · mSplashData.setAdClickText · mSplashData.setAppInfo · mSplashData.setCountDownText · mSplashData.setLogoImage · mSplashData.setLogoText · mSplashData.setNoticeImage
material_clock_display_divider · material_clock_toggle_content_description · material_hour_24h_suffix · material_hour_selection · material_hour_suffix · material_minute_selection · material_minute_suffix · material_motion_easing_accelerated · material_motion_easing_decelerated · material_motion_easing_emphasized · material_motion_easing_linear · material_motion_easing_standard · material_slider_range_end · material_slider_range_start
material_slider_value · material_timepicker_am · material_timepicker_clock_mode_description · material_timepicker_hour · material_timepicker_minute · material_timepicker_pm · material_timepicker_select_time · material_timepicker_text_input_mode_description · mbridge_cm_feedback_btn_text · mbridge_cm_feedback_dialog_close_close · mbridge_cm_feedback_dialog_close_submit · mbridge_cm_feedback_dialog_content_fraud · mbridge_cm_feedback_dialog_content_misleading · mbridge_cm_feedback_dialog_content_not_play
mbridge_cm_feedback_dialog_content_other · mbridge_cm_feedback_dialog_content_por_violence · mbridge_cm_feedback_dialog_content_sound_problems · mbridge_cm_feedback_dialog_privacy_des · mbridge_cm_feedback_dialog_submit_notice · mbridge_cm_feedback_dialog_title · mbridge_download_notify_cancel · mbridge_download_notify_continue · mbridge_download_notify_download_failed · mbridge_download_notify_download_finish_to_install · mbridge_download_notify_download_interrupt · mbridge_download_notify_downloading · mbridge_download_notify_ins · mbridge_download_notify_ins_progress_des
mbridge_download_notify_network_error · mbridge_download_notify_open · mbridge_download_notify_open_progress_des · mbridge_download_notify_pause · mbridge_download_notify_prefix_download · mbridge_download_notify_prefix_pause · mbridge_download_notify_start_download · mbridge_download_notify_update_finish_to_install · mbridge_download_notify_update_package · mbridge_reward_appdesc · mbridge_reward_apptitle · mbridge_reward_clickable_cta_btntext · mbridge_reward_endcard_ad · mbridge_reward_endcard_vast_notice
mbridge_reward_heat_count_unit · mbridge_reward_install · mbridge_reward_video_view_reward_time_complete · mbridge_reward_video_view_reward_time_left · mbridge_reward_video_view_reward_time_left_skip_time · mbridge_reward_viewed_text_str · mbridge_splash_count_time_can_skip · mbridge_splash_count_time_can_skip_not · mbridge_splash_count_time_can_skip_s · missing_integration · moloco_sponsored · monetization_ads_internal_after_timer_value · monetization_ads_internal_before_timer_value · monetization_ads_internal_go
monetization_ads_internal_install · monetization_ads_internal_install_from_google_play · monetization_ads_internal_instream_ad_position · monetization_ads_internal_instream_call_to_action · monetization_ads_internal_instream_feedback · monetization_ads_internal_instream_muted · monetization_ads_internal_instream_skip · monetization_ads_internal_instream_sponsored_default · monetization_ads_internal_instream_sponsored_social · monetization_ads_internal_instream_unmuted · monetization_ads_internal_mediation_sponsored_text · monetization_ads_internal_rewarded_close_verification_button_close · monetization_ads_internal_rewarded_close_verification_button_dismiss · monetization_ads_internal_rewarded_close_verification_text
monetization_ads_internal_rewarded_close_verification_title · monetization_ads_internal_scan_qrcode · monetization_ads_internal_text_divider · monetization_ads_internal_text_divider_with_space · monetization_ads_internal_tools_age_text · monetization_ads_internal_tools_app_install_call_to_action_text · monetization_ads_internal_tools_app_install_call_to_action_text_large · monetization_ads_internal_tools_body_text · monetization_ads_internal_tools_call_to_action_text · monetization_ads_internal_tools_domain_text · monetization_ads_internal_tools_price_text · monetization_ads_internal_tools_rating_text · monetization_ads_internal_tools_review_count_text · monetization_ads_internal_tools_sponsored_text
monetization_ads_internal_tools_title_text · monetization_ads_internal_tools_warning_text · monetization_ads_mediation_api_sponsored_text · mtrl_badge_numberless_content_description · mtrl_button_collapsed_content_description · mtrl_button_expanded_content_description · mtrl_checkbox_button_icon_path_checked · mtrl_checkbox_button_icon_path_group_name · mtrl_checkbox_button_icon_path_indeterminate · mtrl_checkbox_button_icon_path_name · mtrl_checkbox_button_path_checked · mtrl_checkbox_button_path_group_name · mtrl_checkbox_button_path_name · mtrl_checkbox_button_path_unchecked
mtrl_checkbox_state_description_checked · mtrl_checkbox_state_description_indeterminate · mtrl_checkbox_state_description_unchecked · mtrl_chip_close_icon_content_description · mtrl_exceed_max_badge_number_content_description · mtrl_exceed_max_badge_number_suffix · mtrl_picker_a11y_next_month · mtrl_picker_a11y_prev_month · mtrl_picker_announce_current_range_selection · mtrl_picker_announce_current_selection · mtrl_picker_announce_current_selection_none · mtrl_picker_cancel · mtrl_picker_confirm · mtrl_picker_date_header_selected
mtrl_picker_date_header_title · mtrl_picker_date_header_unselected · mtrl_picker_day_of_week_column_header · mtrl_picker_end_date_description · mtrl_picker_invalid_format · mtrl_picker_invalid_format_example · mtrl_picker_invalid_format_use · mtrl_picker_invalid_range · mtrl_picker_navigate_to_current_year_description · mtrl_picker_navigate_to_current_year_description · mtrl_picker_out_of_range · mtrl_picker_range_header_only_end_selected · mtrl_picker_range_header_only_start_selected · mtrl_picker_range_header_selected
mtrl_picker_range_header_title · mtrl_picker_range_header_unselected · mtrl_picker_save · mtrl_picker_start_date_description · mtrl_picker_text_input_date_hint · mtrl_picker_text_input_date_range_end_hint · mtrl_picker_text_input_date_range_start_hint · mtrl_picker_text_input_day_abbr · mtrl_picker_text_input_month_abbr · mtrl_picker_text_input_year_abbr · mtrl_picker_today_description · mtrl_picker_toggle_to_calendar_input_mode · mtrl_picker_toggle_to_day_selection · mtrl_picker_toggle_to_text_input_mode
mtrl_picker_toggle_to_year_selection · mtrl_picker_toggled_to_day_selection · mtrl_picker_toggled_to_year_selection · mtrl_switch_thumb_group_name · mtrl_switch_thumb_path_checked · mtrl_switch_thumb_path_morphing · mtrl_switch_thumb_path_name · mtrl_switch_thumb_path_pressed · mtrl_switch_thumb_path_unchecked · mtrl_switch_track_decoration_path · mtrl_switch_track_path · mtrl_timepicker_cancel · mtrl_timepicker_confirm · multiplayer_nbt
mute_button · name · native_body · native_headline · native_media_view · nav_rail_collapsed_a11y_label · nav_rail_expanded_a11y_label · navigation_drawer_close · navigation_drawer_open · navigation_menu · nether_nbt · no · no_external_browser_found · no_value_set
not_integrated · not_selected · not_set · notifications_permission_confirm · notifications_permission_decline · notifications_permission_title · off · offline_dialog_image_description · offline_dialog_text · offline_notification_title · offline_notification_title_with_advertiser · offline_opt_in_confirm · offline_opt_in_decline · offline_opt_in_message
offline_opt_in_title · ok · on · open_app_button_description · open_app_custom_button_description · open_custom_nbt · openwrap_skip_dialog_close_btn · openwrap_skip_dialog_message · openwrap_skip_dialog_resume_btn · openwrap_skip_dialog_title · overworld_nbt · password · password_toggle_content_description · path_password_eye
path_password_eye_mask_strike_through · path_password_eye_mask_visible · path_password_strike_through · persistent_cta_description · phone_number · pin · pob_openwrap_install_button_title · portals_nbt · preference_copied · provided · range_end · range_start · required · reward_timer_complete
reward_timer_running · rrggbb · s1 · s2 · s3 · s4 · s5 · s6 · s7 · sdk_integration · sdk_integration_status · sdk_undefined · search_menu_title · searchbar_scrolling_view_behavior
searchview_clear_text_content_description · searchview_navigation_content_description · selected · show_map · side_sheet_accessibility_pane_title · side_sheet_behavior · singleplayer_nbt · skip_ad · skip_button_description · smaato_sdk_core_browser_hostname_content_description · smaato_sdk_core_btn_browser_backward_content_description · smaato_sdk_core_btn_browser_close_content_description · smaato_sdk_core_btn_browser_forward_content_description · smaato_sdk_core_btn_browser_open_content_description
smaato_sdk_core_btn_browser_refresh_content_description · smaato_sdk_core_fullscreen_dimension · smaato_sdk_core_no_external_browser_found · smaato_sdk_richmedia_collapse_mraid_ad · smaato_sdk_video_close_button_text · smaato_sdk_video_mute_button_text · smaato_sdk_video_skip_button_text · sponsored_text · status_bar_notification_info_overflow · strong_pw_requirements · summary_collapsed_preference_list · system · tab · taurusx_ad
taurusx_ads_know_more · taurusx_ads_skip · taurusx_ads_sponsored · tcf_consent · template_cta_max_lines · template_desc_max_lines · template_percent · template_title_max_lines · the_end_nbt · time · timer_notification_icon_description · tracer_mapping_uuid · transparency · tt_00_00
tt_ad_close_text · tt_ad_data_error · tt_ad_is_closed · tt_ad_logo_txt · tt_add_bad_reason · tt_adslot_empty · tt_adslot_id_error · tt_adslot_size_empty · tt_app_empty · tt_bad · tt_banner_ad_load_image_error · tt_choose_language · tt_comment_num · tt_content_type
tt_count_down_view · tt_dislike_header_tv_back · tt_dislike_header_tv_title · tt_display_error · tt_done · tt_error_access_method_pass · tt_error_ad_able_false_msg · tt_error_ad_type · tt_error_adtype_differ · tt_error_apk_sign_check_error · tt_error_bidding_type · tt_error_code_adcount_error · tt_error_code_click_event_error · tt_error_device_ip
tt_error_empty_content · tt_error_image_size · tt_error_interstitial_version · tt_error_media_id · tt_error_media_type · tt_error_new_register_limit · tt_error_origin_ad_error · tt_error_package_name · tt_error_redirect · tt_error_request_invalid · tt_error_slot_id_app_id_differ · tt_error_splash_ad_type · tt_error_union_os_error · tt_error_union_sdk_too_old
tt_error_unknow · tt_error_update_version · tt_error_verify_reward · tt_feedback_experience_text · tt_feedback_submit_text · tt_feedback_thank_text · tt_feel_hint · tt_frequent_call_erroe · tt_full_screen_skip_tx · tt_good · tt_init_setting_config_not_complete · tt_insert_ad_load_image_error · tt_lack_android_manifest_configuration · tt_like_this_ad
tt_list_end_tip · tt_load_creative_icon_error · tt_load_creative_icon_response_error · tt_loading_language · tt_loading_more · tt_logo_cn · tt_logo_en · tt_msgPlayable · tt_multiple_ad_indicator · tt_multiple_playable_next_ad_tips · tt_multiple_playable_wait_tips · tt_negtiveBtnBtnText · tt_negtive_txt · tt_net_error
tt_no_ad · tt_no_ad_parse · tt_no_network · tt_not_bad · tt_other_reason · tt_parse_fail · tt_postiveBtnText · tt_postiveBtnTextPlayable · tt_postive_txt · tt_privacy_title · tt_reder_ad_load_timeout · tt_render_diff_template_invalid · tt_render_fail_meta_invalid · tt_render_fail_template_parse_error
tt_render_fail_timeout · tt_render_fail_unknown · tt_render_main_template_invalid · tt_render_render_parse_error · tt_report_this_ad · tt_request_body_error · tt_request_pb_error · tt_reward_feedback · tt_reward_full_skip · tt_reward_msg · tt_reward_screen_skip_tx · tt_ror_code_show_event_error · tt_select_reason · tt_skip_ad_time_text
tt_slide_up_3d · tt_splash_ad_load_image_error · tt_splash_brush_mask_hint · tt_splash_brush_mask_title · tt_splash_cache_expired_error · tt_splash_cache_parse_error · tt_splash_default_click_shake · tt_splash_not_have_cache_error · tt_splash_rock_text · tt_splash_rock_top_text · tt_splash_wriggle_text · tt_splash_wriggle_top_text · tt_splash_wriggle_top_text_style_17 · tt_suggestion_commit
tt_sys_error · tt_template_load_fail · tt_text_privacy_app_version · tt_text_privacy_development · tt_try_now · tt_video_bytesize · tt_video_dial_phone · tt_video_download_apk · tt_video_mobile_go_detail · tt_video_retry_des_txt · tt_video_without_wifi_tips · tt_wap_empty · tt_web_title_default · unmute_button
user · user_privacy · v7_preference_off · v7_preference_on · volume_off_description · volume_on_description · watermark_label_prefix · world_nbt · wrong_pin · yandex_mobileads_age_restricted_user · yandex_mobileads_appmetrica_easy_integration_enabled · yandex_mobileads_automatic_sdk_initialization · yandex_mobileads_enable_logging · yes
```

#### String-string milik aplikasi beserta nilainya (terverifikasi)

| Nama | Nilai | ID |
|---|---|---|
| `app_name` | `MineTuner` | `0x7f13002b` |
| `show_map` | `Show Map` | `0x7f130233` |
| `world_nbt` | `World NBT` | `0x7f1302df` |
| `singleplayer_nbt` | `Singleplayer NBT` | `0x7f130236` |
| `multiplayer_nbt` | `Multiplayer NBT` | `0x7f1301e9` |
| `change_world` | `Change World` | `0x7f130085` |
| `advanced_section` | `Advanced` | `0x7f130020` |
| `open_custom_nbt` | `Open Custom NBT` | `0x7f13020b` |
| `overworld_nbt` | `Overworld NBT` | `0x7f130210` |
| `the_end_nbt` | `The End NBT` | `0x7f130254` |
| `nether_nbt` | `Nether NBT` | `0x7f1301f4` |
| `biomedata_nbt` | `BiomeData NBT` | `0x7f13006f` |
| `portals_nbt` | `Portals NBT` | `0x7f13021b` |
| `autonomous_entities_nbt` | `Autonomous Entities NBT` | `0x7f13003c` |
| `close_drawer` | `Close navigation menu` | `0x7f13008e` |
| `navigation_drawer_open/close` | `Open` / `Close` | `0x7f1301f2` / `0x7f1301f1` |
| `not_selected` / `selected` | `Not selected` / `Selected` | `0x7f1301f9` / `0x7f130232` |
| `default_error_message` | `Invalid input` | `0x7f1300af` |
| `alphanumeric_only_error` | `May only contain alphanumerical chars` | `0x7f130023` |
| `letters_only_error` | `May only contain letters (A-Z)` | `0x7f130126` |
| `name` / `date` / `time` / `hex` | `Name` / `Date` / `Time` / `HEX:` | — |
| `appbar_scrolling_view_behavior` | `com.google.android.material.appbar.AppBarLayout$ScrollingViewBehavior` | `0x7f13002e` |
| `tracer_mapping_uuid` | `360b25e0-5e45-11f0-bb63-3e3c84784f26` | `0x7f13024b-ish` (Appodeal) |

String hardcoded di layout (bukan resource): toolbar **“Exporting Worlds”**, **“Create Flat World”**, **“Chunk Entities”**, **“New World”**, **“Import World”**, **“Help”**, **“About”**, **“Save”**, **“Selected Biome”**, **“Prevent Slimes from Spawning”**, **“Flat World Layers”**, langkah 1–5 tutorial texture pack.

### 3.7 SELURUH 1.208 color

```text
POBTemplateBackground · POBTemplateButtonColor · POBTemplateTextColor · abc_background_cache_hint_selector_material_dark · abc_background_cache_hint_selector_material_light · abc_btn_colored_borderless_text_material · abc_btn_colored_text_material · abc_color_highlight_material · abc_decor_view_status_guard · abc_decor_view_status_guard_light · abc_hint_foreground_material_dark · abc_hint_foreground_material_light · abc_primary_text_disable_only_material_dark · abc_primary_text_disable_only_material_light
abc_primary_text_material_dark · abc_primary_text_material_light · abc_search_url_text · abc_search_url_text_normal · abc_search_url_text_pressed · abc_search_url_text_selected · abc_secondary_text_material_dark · abc_secondary_text_material_light · abc_tint_btn_checkable · abc_tint_default · abc_tint_edittext · abc_tint_seek_thumb · abc_tint_spinner · abc_tint_switch_track
accent_material_dark · accent_material_light · ad_mob_ad_badge_green · ad_mob_button_blue · ad_mob_button_light_blue · ad_mob_outline · ad_mob_white · androidx_core_ripple_material_light · androidx_core_secondary_text_default_material_light · apd_admob_banner_ad_background · apd_admob_banner_ad_text_color · apd_admob_banner_background · apd_admob_banner_cta_color · apd_admob_banner_cta_text_color
apd_admob_banner_headline_text_color · apd_admob_banner_icon_tools · applovin_sdk_adBadgeTextColor · applovin_sdk_adControlbutton_brightBlueColor · applovin_sdk_brand_color · applovin_sdk_brand_color_dark · applovin_sdk_checkmarkColor · applovin_sdk_ctaButtonColor · applovin_sdk_ctaButtonPressedColor · applovin_sdk_disclosureButtonColor · applovin_sdk_grayColor · applovin_sdk_greenColor · applovin_sdk_highlightListItemColor · applovin_sdk_highlightTextColor
applovin_sdk_listViewBackground · applovin_sdk_listViewSectionTextColor · applovin_sdk_orangeColor · applovin_sdk_starColor · applovin_sdk_switchThumbColor · applovin_sdk_switchTrackDisabledColor · applovin_sdk_textColorPrimary · applovin_sdk_warningColor · applovin_sdk_xmarkColor · background_floating_material_dark · background_floating_material_light · background_material_dark · background_material_light · black
black_transparent · bm_privacy_button_bg_color · bm_privacy_button_icon_color · bm_privacy_button_ripple_color · bm_privacy_button_text_color · bm_privacy_icon_ripple_color · bm_privacy_sheet_bg_color · bm_privacy_subtitle_color · bm_privacy_title_color · bright_foreground_disabled_material_dark · bright_foreground_disabled_material_light · bright_foreground_inverse_material_dark · bright_foreground_inverse_material_light · bright_foreground_material_dark
bright_foreground_material_light · browser_actions_bg_grey · browser_actions_divider_color · browser_actions_text_color · browser_actions_title_color · button_material_dark · button_material_light · call_notification_answer_color · call_notification_decline_color · cardview_dark_background · cardview_light_background · cardview_shadow_end_color · cardview_shadow_start_color · common_google_signin_btn_text_dark
common_google_signin_btn_text_dark_default · common_google_signin_btn_text_dark_disabled · common_google_signin_btn_text_dark_focused · common_google_signin_btn_text_dark_pressed · common_google_signin_btn_text_light · common_google_signin_btn_text_light_default · common_google_signin_btn_text_light_disabled · common_google_signin_btn_text_light_focused · common_google_signin_btn_text_light_pressed · common_google_signin_btn_tint · countdown_bg · debug_panel_black · debug_panel_dark_gray · debug_panel_gray
debug_panel_green · debug_panel_light_gray · debug_panel_orange · debug_panel_red · debug_panel_white · design_bottom_navigation_shadow_color · design_box_stroke_color · design_dark_default_color_background · design_dark_default_color_error · design_dark_default_color_on_background · design_dark_default_color_on_error · design_dark_default_color_on_primary · design_dark_default_color_on_secondary · design_dark_default_color_on_surface
design_dark_default_color_primary · design_dark_default_color_primary_dark · design_dark_default_color_primary_variant · design_dark_default_color_secondary · design_dark_default_color_secondary_variant · design_dark_default_color_surface · design_default_color_background · design_default_color_error · design_default_color_on_background · design_default_color_on_error · design_default_color_on_primary · design_default_color_on_secondary · design_default_color_on_surface · design_default_color_primary
design_default_color_primary_dark · design_default_color_primary_variant · design_default_color_secondary · design_default_color_secondary_variant · design_default_color_surface · design_error · design_fab_shadow_end_color · design_fab_shadow_mid_color · design_fab_shadow_start_color · design_fab_stroke_end_inner_color · design_fab_stroke_end_outer_color · design_fab_stroke_top_inner_color · design_fab_stroke_top_outer_color · design_icon_tint
design_snackbar_background_color · dim_foreground_disabled_material_dark · dim_foreground_disabled_material_light · dim_foreground_material_dark · dim_foreground_material_light · div_accent_color · div_button_color · div_indicator_default_color · div_indicator_selected_color · div_separator_color · div_tab_indicator · div_text_dark_disabled_40 · div_text_dark_disabled_50 · div_text_dark_disabled_80
div_title_menu_color · div_traffic_item_stroke_color · dtx_store_promo_bg_fade · error_color_material_dark · error_color_material_light · exo_black_opacity_60 · exo_black_opacity_70 · exo_bottom_bar_background · exo_edit_mode_background_color · exo_error_message_background_color · exo_styled_error_message_background · exo_white · exo_white_opacity_70 · foreground_material_dark
foreground_material_light · highlighted_text_material_dark · highlighted_text_material_light · ia_blank_background · ia_close_rewarded_button_background_color · ia_close_rewarded_button_color · ia_close_rewarded_dialog_background · ia_close_rewarded_sub_title_color · ia_close_rewarded_title_color · ia_endcard_background · ia_fullscreen_background · ia_fyber_accent · ia_fyber_identifier_bg_color · ia_fyber_white
ia_mraid_expanded_dimmed_bk · ia_overlay_bg_color · ia_overlay_stroke_color · ia_transparent_background · ia_video_background_color · ia_video_overlay_stroke · ia_video_overlay_text · ia_video_overlay_text_background · ia_video_overlay_text_background_pressed · ia_video_overlay_text_shadow · ia_video_progressbar · ia_video_progressbar_background · ia_video_progressbar_green · ia_video_transparent_overlay
m3_appbar_overlay_color · m3_assist_chip_icon_tint_color · m3_assist_chip_stroke_color · m3_bottom_sheet_drag_handle_color · m3_button_background_color_selector · m3_button_foreground_color_selector · m3_button_outline_color_selector · m3_button_ripple_color · m3_button_ripple_color_selector · m3_calendar_item_disabled_text · m3_calendar_item_stroke_color · m3_card_foreground_color · m3_card_ripple_color · m3_card_stroke_color
m3_checkbox_button_icon_tint · m3_checkbox_button_tint · m3_chip_assist_text_color · m3_chip_background_color · m3_chip_ripple_color · m3_chip_stroke_color · m3_chip_text_color · m3_dark_default_color_primary_text · m3_dark_default_color_secondary_text · m3_dark_highlighted_text · m3_dark_hint_foreground · m3_dark_primary_text_disable_only · m3_default_color_primary_text · m3_default_color_secondary_text
m3_dynamic_dark_default_color_primary_text · m3_dynamic_dark_default_color_secondary_text · m3_dynamic_dark_highlighted_text · m3_dynamic_dark_hint_foreground · m3_dynamic_dark_primary_text_disable_only · m3_dynamic_default_color_primary_text · m3_dynamic_default_color_secondary_text · m3_dynamic_highlighted_text · m3_dynamic_hint_foreground · m3_dynamic_primary_text_disable_only · m3_efab_ripple_color_selector · m3_elevated_chip_background_color · m3_fab_efab_background_color_selector · m3_fab_efab_foreground_color_selector
m3_fab_ripple_color_selector · m3_filled_icon_button_container_color_selector · m3_floating_toolbar_vibrant_icon_button_container_color_selector · m3_floating_toolbar_vibrant_icon_button_text_color_selector · m3_highlighted_text · m3_hint_foreground · m3_icon_button_icon_color_selector · m3_navigation_bar_item_with_indicator_icon_tint · m3_navigation_bar_item_with_indicator_label_tint · m3_navigation_bar_ripple_color_selector · m3_navigation_item_background_color · m3_navigation_item_icon_tint · m3_navigation_item_ripple_color · m3_navigation_item_text_color
m3_navigation_rail_item_with_indicator_icon_tint · m3_navigation_rail_item_with_indicator_label_tint · m3_navigation_rail_ripple_color_selector · m3_popupmenu_overlay_color · m3_primary_text_disable_only · m3_radiobutton_button_tint · m3_radiobutton_ripple_tint · m3_ref_palette_black · m3_ref_palette_blue0 · m3_ref_palette_blue10 · m3_ref_palette_blue100 · m3_ref_palette_blue20 · m3_ref_palette_blue30 · m3_ref_palette_blue40
m3_ref_palette_blue50 · m3_ref_palette_blue60 · m3_ref_palette_blue70 · m3_ref_palette_blue80 · m3_ref_palette_blue90 · m3_ref_palette_blue95 · m3_ref_palette_blue98 · m3_ref_palette_blue_variant0 · m3_ref_palette_blue_variant10 · m3_ref_palette_blue_variant100 · m3_ref_palette_blue_variant20 · m3_ref_palette_blue_variant30 · m3_ref_palette_blue_variant40 · m3_ref_palette_blue_variant50
m3_ref_palette_blue_variant60 · m3_ref_palette_blue_variant70 · m3_ref_palette_blue_variant80 · m3_ref_palette_blue_variant90 · m3_ref_palette_blue_variant95 · m3_ref_palette_blue_variant98 · m3_ref_palette_cyan0 · m3_ref_palette_cyan10 · m3_ref_palette_cyan100 · m3_ref_palette_cyan20 · m3_ref_palette_cyan30 · m3_ref_palette_cyan40 · m3_ref_palette_cyan50 · m3_ref_palette_cyan60
m3_ref_palette_cyan70 · m3_ref_palette_cyan80 · m3_ref_palette_cyan90 · m3_ref_palette_cyan95 · m3_ref_palette_cyan98 · m3_ref_palette_dynamic_neutral0 · m3_ref_palette_dynamic_neutral10 · m3_ref_palette_dynamic_neutral100 · m3_ref_palette_dynamic_neutral12 · m3_ref_palette_dynamic_neutral17 · m3_ref_palette_dynamic_neutral20 · m3_ref_palette_dynamic_neutral22 · m3_ref_palette_dynamic_neutral24 · m3_ref_palette_dynamic_neutral30
m3_ref_palette_dynamic_neutral4 · m3_ref_palette_dynamic_neutral40 · m3_ref_palette_dynamic_neutral50 · m3_ref_palette_dynamic_neutral6 · m3_ref_palette_dynamic_neutral60 · m3_ref_palette_dynamic_neutral70 · m3_ref_palette_dynamic_neutral80 · m3_ref_palette_dynamic_neutral87 · m3_ref_palette_dynamic_neutral90 · m3_ref_palette_dynamic_neutral92 · m3_ref_palette_dynamic_neutral94 · m3_ref_palette_dynamic_neutral95 · m3_ref_palette_dynamic_neutral96 · m3_ref_palette_dynamic_neutral98
m3_ref_palette_dynamic_neutral99 · m3_ref_palette_dynamic_neutral_variant0 · m3_ref_palette_dynamic_neutral_variant10 · m3_ref_palette_dynamic_neutral_variant100 · m3_ref_palette_dynamic_neutral_variant12 · m3_ref_palette_dynamic_neutral_variant17 · m3_ref_palette_dynamic_neutral_variant20 · m3_ref_palette_dynamic_neutral_variant22 · m3_ref_palette_dynamic_neutral_variant24 · m3_ref_palette_dynamic_neutral_variant30 · m3_ref_palette_dynamic_neutral_variant4 · m3_ref_palette_dynamic_neutral_variant40 · m3_ref_palette_dynamic_neutral_variant50 · m3_ref_palette_dynamic_neutral_variant6
m3_ref_palette_dynamic_neutral_variant60 · m3_ref_palette_dynamic_neutral_variant70 · m3_ref_palette_dynamic_neutral_variant80 · m3_ref_palette_dynamic_neutral_variant87 · m3_ref_palette_dynamic_neutral_variant90 · m3_ref_palette_dynamic_neutral_variant92 · m3_ref_palette_dynamic_neutral_variant94 · m3_ref_palette_dynamic_neutral_variant95 · m3_ref_palette_dynamic_neutral_variant96 · m3_ref_palette_dynamic_neutral_variant98 · m3_ref_palette_dynamic_neutral_variant99 · m3_ref_palette_dynamic_primary0 · m3_ref_palette_dynamic_primary10 · m3_ref_palette_dynamic_primary100
m3_ref_palette_dynamic_primary20 · m3_ref_palette_dynamic_primary30 · m3_ref_palette_dynamic_primary40 · m3_ref_palette_dynamic_primary50 · m3_ref_palette_dynamic_primary60 · m3_ref_palette_dynamic_primary70 · m3_ref_palette_dynamic_primary80 · m3_ref_palette_dynamic_primary90 · m3_ref_palette_dynamic_primary95 · m3_ref_palette_dynamic_primary98 · m3_ref_palette_dynamic_primary99 · m3_ref_palette_dynamic_secondary0 · m3_ref_palette_dynamic_secondary10 · m3_ref_palette_dynamic_secondary100
m3_ref_palette_dynamic_secondary20 · m3_ref_palette_dynamic_secondary30 · m3_ref_palette_dynamic_secondary40 · m3_ref_palette_dynamic_secondary50 · m3_ref_palette_dynamic_secondary60 · m3_ref_palette_dynamic_secondary70 · m3_ref_palette_dynamic_secondary80 · m3_ref_palette_dynamic_secondary90 · m3_ref_palette_dynamic_secondary95 · m3_ref_palette_dynamic_secondary98 · m3_ref_palette_dynamic_secondary99 · m3_ref_palette_dynamic_tertiary0 · m3_ref_palette_dynamic_tertiary10 · m3_ref_palette_dynamic_tertiary100
m3_ref_palette_dynamic_tertiary20 · m3_ref_palette_dynamic_tertiary30 · m3_ref_palette_dynamic_tertiary40 · m3_ref_palette_dynamic_tertiary50 · m3_ref_palette_dynamic_tertiary60 · m3_ref_palette_dynamic_tertiary70 · m3_ref_palette_dynamic_tertiary80 · m3_ref_palette_dynamic_tertiary90 · m3_ref_palette_dynamic_tertiary95 · m3_ref_palette_dynamic_tertiary98 · m3_ref_palette_dynamic_tertiary99 · m3_ref_palette_error0 · m3_ref_palette_error10 · m3_ref_palette_error100
m3_ref_palette_error20 · m3_ref_palette_error30 · m3_ref_palette_error40 · m3_ref_palette_error50 · m3_ref_palette_error60 · m3_ref_palette_error70 · m3_ref_palette_error80 · m3_ref_palette_error90 · m3_ref_palette_error95 · m3_ref_palette_error98 · m3_ref_palette_error99 · m3_ref_palette_green0 · m3_ref_palette_green10 · m3_ref_palette_green100
m3_ref_palette_green20 · m3_ref_palette_green30 · m3_ref_palette_green40 · m3_ref_palette_green50 · m3_ref_palette_green60 · m3_ref_palette_green70 · m3_ref_palette_green80 · m3_ref_palette_green90 · m3_ref_palette_green95 · m3_ref_palette_green98 · m3_ref_palette_grey0 · m3_ref_palette_grey10 · m3_ref_palette_grey100 · m3_ref_palette_grey20
m3_ref_palette_grey30 · m3_ref_palette_grey40 · m3_ref_palette_grey50 · m3_ref_palette_grey60 · m3_ref_palette_grey70 · m3_ref_palette_grey80 · m3_ref_palette_grey90 · m3_ref_palette_grey95 · m3_ref_palette_grey98 · m3_ref_palette_grey_variant0 · m3_ref_palette_grey_variant10 · m3_ref_palette_grey_variant100 · m3_ref_palette_grey_variant20 · m3_ref_palette_grey_variant30
m3_ref_palette_grey_variant40 · m3_ref_palette_grey_variant50 · m3_ref_palette_grey_variant60 · m3_ref_palette_grey_variant70 · m3_ref_palette_grey_variant80 · m3_ref_palette_grey_variant90 · m3_ref_palette_grey_variant95 · m3_ref_palette_grey_variant98 · m3_ref_palette_neutral0 · m3_ref_palette_neutral10 · m3_ref_palette_neutral100 · m3_ref_palette_neutral12 · m3_ref_palette_neutral17 · m3_ref_palette_neutral20
m3_ref_palette_neutral22 · m3_ref_palette_neutral24 · m3_ref_palette_neutral30 · m3_ref_palette_neutral4 · m3_ref_palette_neutral40 · m3_ref_palette_neutral50 · m3_ref_palette_neutral6 · m3_ref_palette_neutral60 · m3_ref_palette_neutral70 · m3_ref_palette_neutral80 · m3_ref_palette_neutral87 · m3_ref_palette_neutral90 · m3_ref_palette_neutral92 · m3_ref_palette_neutral94
m3_ref_palette_neutral95 · m3_ref_palette_neutral96 · m3_ref_palette_neutral98 · m3_ref_palette_neutral99 · m3_ref_palette_neutral_variant0 · m3_ref_palette_neutral_variant10 · m3_ref_palette_neutral_variant100 · m3_ref_palette_neutral_variant20 · m3_ref_palette_neutral_variant30 · m3_ref_palette_neutral_variant40 · m3_ref_palette_neutral_variant50 · m3_ref_palette_neutral_variant60 · m3_ref_palette_neutral_variant70 · m3_ref_palette_neutral_variant80
m3_ref_palette_neutral_variant90 · m3_ref_palette_neutral_variant95 · m3_ref_palette_neutral_variant98 · m3_ref_palette_neutral_variant99 · m3_ref_palette_orange0 · m3_ref_palette_orange10 · m3_ref_palette_orange100 · m3_ref_palette_orange20 · m3_ref_palette_orange30 · m3_ref_palette_orange40 · m3_ref_palette_orange50 · m3_ref_palette_orange60 · m3_ref_palette_orange70 · m3_ref_palette_orange80
m3_ref_palette_orange90 · m3_ref_palette_orange95 · m3_ref_palette_orange98 · m3_ref_palette_pink0 · m3_ref_palette_pink10 · m3_ref_palette_pink100 · m3_ref_palette_pink20 · m3_ref_palette_pink30 · m3_ref_palette_pink40 · m3_ref_palette_pink50 · m3_ref_palette_pink60 · m3_ref_palette_pink70 · m3_ref_palette_pink80 · m3_ref_palette_pink90
m3_ref_palette_pink95 · m3_ref_palette_pink98 · m3_ref_palette_primary0 · m3_ref_palette_primary10 · m3_ref_palette_primary100 · m3_ref_palette_primary20 · m3_ref_palette_primary30 · m3_ref_palette_primary40 · m3_ref_palette_primary50 · m3_ref_palette_primary60 · m3_ref_palette_primary70 · m3_ref_palette_primary80 · m3_ref_palette_primary90 · m3_ref_palette_primary95
m3_ref_palette_primary98 · m3_ref_palette_primary99 · m3_ref_palette_purple0 · m3_ref_palette_purple10 · m3_ref_palette_purple100 · m3_ref_palette_purple20 · m3_ref_palette_purple30 · m3_ref_palette_purple40 · m3_ref_palette_purple50 · m3_ref_palette_purple60 · m3_ref_palette_purple70 · m3_ref_palette_purple80 · m3_ref_palette_purple90 · m3_ref_palette_purple95
m3_ref_palette_purple98 · m3_ref_palette_red0 · m3_ref_palette_red10 · m3_ref_palette_red100 · m3_ref_palette_red20 · m3_ref_palette_red30 · m3_ref_palette_red40 · m3_ref_palette_red50 · m3_ref_palette_red60 · m3_ref_palette_red70 · m3_ref_palette_red80 · m3_ref_palette_red90 · m3_ref_palette_red95 · m3_ref_palette_red98
m3_ref_palette_secondary0 · m3_ref_palette_secondary10 · m3_ref_palette_secondary100 · m3_ref_palette_secondary20 · m3_ref_palette_secondary30 · m3_ref_palette_secondary40 · m3_ref_palette_secondary50 · m3_ref_palette_secondary60 · m3_ref_palette_secondary70 · m3_ref_palette_secondary80 · m3_ref_palette_secondary90 · m3_ref_palette_secondary95 · m3_ref_palette_secondary98 · m3_ref_palette_secondary99
m3_ref_palette_tertiary0 · m3_ref_palette_tertiary10 · m3_ref_palette_tertiary100 · m3_ref_palette_tertiary20 · m3_ref_palette_tertiary30 · m3_ref_palette_tertiary40 · m3_ref_palette_tertiary50 · m3_ref_palette_tertiary60 · m3_ref_palette_tertiary70 · m3_ref_palette_tertiary80 · m3_ref_palette_tertiary90 · m3_ref_palette_tertiary95 · m3_ref_palette_tertiary98 · m3_ref_palette_tertiary99
m3_ref_palette_white · m3_ref_palette_yellow0 · m3_ref_palette_yellow10 · m3_ref_palette_yellow100 · m3_ref_palette_yellow20 · m3_ref_palette_yellow30 · m3_ref_palette_yellow40 · m3_ref_palette_yellow50 · m3_ref_palette_yellow60 · m3_ref_palette_yellow70 · m3_ref_palette_yellow80 · m3_ref_palette_yellow90 · m3_ref_palette_yellow95 · m3_ref_palette_yellow98
m3_selection_control_ripple_color_selector · m3_simple_item_ripple_color · m3_slider_active_tick_marks_color · m3_slider_active_track_color · m3_slider_active_track_color_legacy · m3_slider_halo_color_legacy · m3_slider_inactive_tick_marks_color · m3_slider_inactive_track_color · m3_slider_inactive_track_color_legacy · m3_slider_thumb_color · m3_slider_thumb_color_legacy · m3_standard_toolbar_button_text_color_selector · m3_standard_toolbar_icon_button_container_color_selector · m3_standard_toolbar_icon_button_icon_color_selector
m3_standard_toolbar_icon_button_ripple_color_selector · m3_switch_thumb_tint · m3_switch_track_tint · m3_sys_color_dark_background · m3_sys_color_dark_error · m3_sys_color_dark_error_container · m3_sys_color_dark_inverse_on_surface · m3_sys_color_dark_inverse_primary · m3_sys_color_dark_inverse_surface · m3_sys_color_dark_on_background · m3_sys_color_dark_on_error · m3_sys_color_dark_on_error_container · m3_sys_color_dark_on_primary · m3_sys_color_dark_on_primary_container
m3_sys_color_dark_on_secondary · m3_sys_color_dark_on_secondary_container · m3_sys_color_dark_on_surface · m3_sys_color_dark_on_surface_variant · m3_sys_color_dark_on_tertiary · m3_sys_color_dark_on_tertiary_container · m3_sys_color_dark_outline · m3_sys_color_dark_outline_variant · m3_sys_color_dark_primary · m3_sys_color_dark_primary_container · m3_sys_color_dark_secondary · m3_sys_color_dark_secondary_container · m3_sys_color_dark_surface · m3_sys_color_dark_surface_bright
m3_sys_color_dark_surface_container · m3_sys_color_dark_surface_container_high · m3_sys_color_dark_surface_container_highest · m3_sys_color_dark_surface_container_low · m3_sys_color_dark_surface_container_lowest · m3_sys_color_dark_surface_dim · m3_sys_color_dark_surface_variant · m3_sys_color_dark_tertiary · m3_sys_color_dark_tertiary_container · m3_sys_color_dynamic_dark_background · m3_sys_color_dynamic_dark_error · m3_sys_color_dynamic_dark_error_container · m3_sys_color_dynamic_dark_inverse_on_surface · m3_sys_color_dynamic_dark_inverse_primary
m3_sys_color_dynamic_dark_inverse_surface · m3_sys_color_dynamic_dark_on_background · m3_sys_color_dynamic_dark_on_error · m3_sys_color_dynamic_dark_on_error_container · m3_sys_color_dynamic_dark_on_primary · m3_sys_color_dynamic_dark_on_primary_container · m3_sys_color_dynamic_dark_on_secondary · m3_sys_color_dynamic_dark_on_secondary_container · m3_sys_color_dynamic_dark_on_surface · m3_sys_color_dynamic_dark_on_surface_variant · m3_sys_color_dynamic_dark_on_tertiary · m3_sys_color_dynamic_dark_on_tertiary_container · m3_sys_color_dynamic_dark_outline · m3_sys_color_dynamic_dark_outline_variant
m3_sys_color_dynamic_dark_primary · m3_sys_color_dynamic_dark_primary_container · m3_sys_color_dynamic_dark_secondary · m3_sys_color_dynamic_dark_secondary_container · m3_sys_color_dynamic_dark_surface · m3_sys_color_dynamic_dark_surface_bright · m3_sys_color_dynamic_dark_surface_container · m3_sys_color_dynamic_dark_surface_container_high · m3_sys_color_dynamic_dark_surface_container_highest · m3_sys_color_dynamic_dark_surface_container_low · m3_sys_color_dynamic_dark_surface_container_lowest · m3_sys_color_dynamic_dark_surface_dim · m3_sys_color_dynamic_dark_surface_variant · m3_sys_color_dynamic_dark_tertiary
m3_sys_color_dynamic_dark_tertiary_container · m3_sys_color_dynamic_light_background · m3_sys_color_dynamic_light_error · m3_sys_color_dynamic_light_error_container · m3_sys_color_dynamic_light_inverse_on_surface · m3_sys_color_dynamic_light_inverse_primary · m3_sys_color_dynamic_light_inverse_surface · m3_sys_color_dynamic_light_on_background · m3_sys_color_dynamic_light_on_error · m3_sys_color_dynamic_light_on_error_container · m3_sys_color_dynamic_light_on_primary · m3_sys_color_dynamic_light_on_primary_container · m3_sys_color_dynamic_light_on_secondary · m3_sys_color_dynamic_light_on_secondary_container
m3_sys_color_dynamic_light_on_surface · m3_sys_color_dynamic_light_on_surface_variant · m3_sys_color_dynamic_light_on_tertiary · m3_sys_color_dynamic_light_on_tertiary_container · m3_sys_color_dynamic_light_outline · m3_sys_color_dynamic_light_outline_variant · m3_sys_color_dynamic_light_primary · m3_sys_color_dynamic_light_primary_container · m3_sys_color_dynamic_light_secondary · m3_sys_color_dynamic_light_secondary_container · m3_sys_color_dynamic_light_surface · m3_sys_color_dynamic_light_surface_bright · m3_sys_color_dynamic_light_surface_container · m3_sys_color_dynamic_light_surface_container_high
m3_sys_color_dynamic_light_surface_container_highest · m3_sys_color_dynamic_light_surface_container_low · m3_sys_color_dynamic_light_surface_container_lowest · m3_sys_color_dynamic_light_surface_dim · m3_sys_color_dynamic_light_surface_variant · m3_sys_color_dynamic_light_tertiary · m3_sys_color_dynamic_light_tertiary_container · m3_sys_color_dynamic_on_primary_fixed · m3_sys_color_dynamic_on_primary_fixed_variant · m3_sys_color_dynamic_on_secondary_fixed · m3_sys_color_dynamic_on_secondary_fixed_variant · m3_sys_color_dynamic_on_tertiary_fixed · m3_sys_color_dynamic_on_tertiary_fixed_variant · m3_sys_color_dynamic_primary_fixed
m3_sys_color_dynamic_primary_fixed_dim · m3_sys_color_dynamic_secondary_fixed · m3_sys_color_dynamic_secondary_fixed_dim · m3_sys_color_dynamic_tertiary_fixed · m3_sys_color_dynamic_tertiary_fixed_dim · m3_sys_color_light_background · m3_sys_color_light_error · m3_sys_color_light_error_container · m3_sys_color_light_inverse_on_surface · m3_sys_color_light_inverse_primary · m3_sys_color_light_inverse_surface · m3_sys_color_light_on_background · m3_sys_color_light_on_error · m3_sys_color_light_on_error_container
m3_sys_color_light_on_primary · m3_sys_color_light_on_primary_container · m3_sys_color_light_on_secondary · m3_sys_color_light_on_secondary_container · m3_sys_color_light_on_surface · m3_sys_color_light_on_surface_variant · m3_sys_color_light_on_tertiary · m3_sys_color_light_on_tertiary_container · m3_sys_color_light_outline · m3_sys_color_light_outline_variant · m3_sys_color_light_primary · m3_sys_color_light_primary_container · m3_sys_color_light_secondary · m3_sys_color_light_secondary_container
m3_sys_color_light_surface · m3_sys_color_light_surface_bright · m3_sys_color_light_surface_container · m3_sys_color_light_surface_container_high · m3_sys_color_light_surface_container_highest · m3_sys_color_light_surface_container_low · m3_sys_color_light_surface_container_lowest · m3_sys_color_light_surface_dim · m3_sys_color_light_surface_variant · m3_sys_color_light_tertiary · m3_sys_color_light_tertiary_container · m3_sys_color_on_primary_fixed · m3_sys_color_on_primary_fixed_variant · m3_sys_color_on_secondary_fixed
m3_sys_color_on_secondary_fixed_variant · m3_sys_color_on_tertiary_fixed · m3_sys_color_on_tertiary_fixed_variant · m3_sys_color_primary_fixed · m3_sys_color_primary_fixed_dim · m3_sys_color_secondary_fixed · m3_sys_color_secondary_fixed_dim · m3_sys_color_tertiary_fixed · m3_sys_color_tertiary_fixed_dim · m3_tabs_icon_color · m3_tabs_icon_color_secondary · m3_tabs_ripple_color · m3_tabs_ripple_color_secondary · m3_tabs_text_color
m3_tabs_text_color_secondary · m3_text_button_background_color_selector · m3_text_button_foreground_color_selector · m3_text_button_ripple_color_selector · m3_textfield_filled_background_color · m3_textfield_indicator_text_color · m3_textfield_input_text_color · m3_textfield_label_color · m3_textfield_stroke_color · m3_timepicker_button_background_color · m3_timepicker_button_ripple_color · m3_timepicker_button_text_color · m3_timepicker_clock_text_color · m3_timepicker_display_background_color
m3_timepicker_display_ripple_color · m3_timepicker_display_text_color · m3_timepicker_secondary_text_button_ripple_color · m3_timepicker_secondary_text_button_text_color · m3_timepicker_time_input_stroke_color · m3_tonal_button_ripple_color_selector · m3_vibrant_toolbar_button_text_color_selector · m3_vibrant_toolbar_icon_button_container_color_selector · m3_vibrant_toolbar_icon_button_icon_color_selector · m3_vibrant_toolbar_icon_button_ripple_color_selector · m3expressive_bottom_nav_item_icon_tint · m3expressive_bottom_nav_item_label_tint · m3expressive_bottom_nav_item_ripple_tint · m3expressive_button_outline_color_selector
m3expressive_button_outlined_background_color_selector · m3expressive_nav_rail_item_icon_tint · m3expressive_nav_rail_item_label_tint · m3expressive_nav_rail_item_ripple_tint · material_blue_grey_800 · material_blue_grey_900 · material_blue_grey_950 · material_cursor_color · material_deep_teal_200 · material_deep_teal_500 · material_divider_color · material_dynamic_color_dark_error · material_dynamic_color_dark_error_container · material_dynamic_color_dark_on_error
material_dynamic_color_dark_on_error_container · material_dynamic_color_light_error · material_dynamic_color_light_error_container · material_dynamic_color_light_on_error · material_dynamic_color_light_on_error_container · material_dynamic_neutral0 · material_dynamic_neutral10 · material_dynamic_neutral100 · material_dynamic_neutral20 · material_dynamic_neutral30 · material_dynamic_neutral40 · material_dynamic_neutral50 · material_dynamic_neutral60 · material_dynamic_neutral70
material_dynamic_neutral80 · material_dynamic_neutral90 · material_dynamic_neutral95 · material_dynamic_neutral99 · material_dynamic_neutral_variant0 · material_dynamic_neutral_variant10 · material_dynamic_neutral_variant100 · material_dynamic_neutral_variant20 · material_dynamic_neutral_variant30 · material_dynamic_neutral_variant40 · material_dynamic_neutral_variant50 · material_dynamic_neutral_variant60 · material_dynamic_neutral_variant70 · material_dynamic_neutral_variant80
material_dynamic_neutral_variant90 · material_dynamic_neutral_variant95 · material_dynamic_neutral_variant99 · material_dynamic_primary0 · material_dynamic_primary10 · material_dynamic_primary100 · material_dynamic_primary20 · material_dynamic_primary30 · material_dynamic_primary40 · material_dynamic_primary50 · material_dynamic_primary60 · material_dynamic_primary70 · material_dynamic_primary80 · material_dynamic_primary90
material_dynamic_primary95 · material_dynamic_primary99 · material_dynamic_secondary0 · material_dynamic_secondary10 · material_dynamic_secondary100 · material_dynamic_secondary20 · material_dynamic_secondary30 · material_dynamic_secondary40 · material_dynamic_secondary50 · material_dynamic_secondary60 · material_dynamic_secondary70 · material_dynamic_secondary80 · material_dynamic_secondary90 · material_dynamic_secondary95
material_dynamic_secondary99 · material_dynamic_tertiary0 · material_dynamic_tertiary10 · material_dynamic_tertiary100 · material_dynamic_tertiary20 · material_dynamic_tertiary30 · material_dynamic_tertiary40 · material_dynamic_tertiary50 · material_dynamic_tertiary60 · material_dynamic_tertiary70 · material_dynamic_tertiary80 · material_dynamic_tertiary90 · material_dynamic_tertiary95 · material_dynamic_tertiary99
material_grey_100 · material_grey_300 · material_grey_50 · material_grey_600 · material_grey_800 · material_grey_850 · material_grey_900 · material_harmonized_color_error · material_harmonized_color_error_container · material_harmonized_color_on_error · material_harmonized_color_on_error_container · material_on_background_disabled · material_on_background_emphasis_high_type · material_on_background_emphasis_medium
material_on_primary_disabled · material_on_primary_emphasis_high_type · material_on_primary_emphasis_medium · material_on_surface_disabled · material_on_surface_emphasis_high_type · material_on_surface_emphasis_medium · material_on_surface_stroke · material_personalized__highlighted_text · material_personalized__highlighted_text_inverse · material_personalized_color_background · material_personalized_color_control_activated · material_personalized_color_control_highlight · material_personalized_color_control_normal · material_personalized_color_error
material_personalized_color_error_container · material_personalized_color_on_background · material_personalized_color_on_error · material_personalized_color_on_error_container · material_personalized_color_on_primary · material_personalized_color_on_primary_container · material_personalized_color_on_secondary · material_personalized_color_on_secondary_container · material_personalized_color_on_surface · material_personalized_color_on_surface_inverse · material_personalized_color_on_surface_variant · material_personalized_color_on_tertiary · material_personalized_color_on_tertiary_container · material_personalized_color_outline
material_personalized_color_outline_variant · material_personalized_color_primary · material_personalized_color_primary_container · material_personalized_color_primary_inverse · material_personalized_color_primary_text · material_personalized_color_primary_text_inverse · material_personalized_color_secondary · material_personalized_color_secondary_container · material_personalized_color_secondary_text · material_personalized_color_secondary_text_inverse · material_personalized_color_surface · material_personalized_color_surface_bright · material_personalized_color_surface_container · material_personalized_color_surface_container_high
material_personalized_color_surface_container_highest · material_personalized_color_surface_container_low · material_personalized_color_surface_container_lowest · material_personalized_color_surface_dim · material_personalized_color_surface_inverse · material_personalized_color_surface_variant · material_personalized_color_tertiary · material_personalized_color_tertiary_container · material_personalized_color_text_hint_foreground_inverse · material_personalized_color_text_primary_inverse · material_personalized_color_text_primary_inverse_disable_only · material_personalized_color_text_secondary_and_tertiary_inverse · material_personalized_color_text_secondary_and_tertiary_inverse_disabled · material_personalized_hint_foreground
material_personalized_hint_foreground_inverse · material_personalized_primary_inverse_text_disable_only · material_personalized_primary_text_disable_only · material_slider_active_tick_marks_color · material_slider_active_track_color · material_slider_halo_color · material_slider_inactive_tick_marks_color · material_slider_inactive_track_color · material_slider_thumb_color · material_timepicker_button_background · material_timepicker_button_stroke · material_timepicker_clock_text_color · material_timepicker_clockface · material_timepicker_modebutton_tint
mbridge_black · mbridge_black_66 · mbridge_black_alpha_50 · mbridge_cm_feedback_dialog_chice_bg_pressed · mbridge_cm_feedback_rb_text_color_color_list · mbridge_color_999999 · mbridge_color_cc000000 · mbridge_color_cc0000001 · mbridge_common_white · mbridge_cpb_blue · mbridge_cpb_blue_dark · mbridge_cpb_green · mbridge_cpb_green_dark · mbridge_cpb_grey
mbridge_cpb_red · mbridge_cpb_red_dark · mbridge_cpb_white · mbridge_dd_grey · mbridge_ee_grey · mbridge_interstitial_black · mbridge_interstitial_white · mbridge_more_offer_list_bg · mbridge_nativex_cta_txt_nor · mbridge_nativex_cta_txt_pre · mbridge_nativex_land_cta_bg_nor · mbridge_nativex_por_cta_bg_nor · mbridge_nativex_por_cta_bg_pre · mbridge_nativex_sound_bg
mbridge_purple_200 · mbridge_purple_500 · mbridge_purple_700 · mbridge_reward_black · mbridge_reward_cta_bg · mbridge_reward_desc_textcolor · mbridge_reward_endcard_hor_bg · mbridge_reward_endcard_land_bg · mbridge_reward_endcard_line_bg · mbridge_reward_endcard_vast_bg · mbridge_reward_kiloo_background · mbridge_reward_layer_text_bg · mbridge_reward_minicard_bg · mbridge_reward_six_black_transparent
mbridge_reward_six_black_transparent1 · mbridge_reward_six_black_transparent2 · mbridge_reward_title_textcolor · mbridge_reward_white · mbridge_splash_count_time_skip_text_color · mbridge_teal_200 · mbridge_teal_700 · mbridge_video_common_alertview_bg · mbridge_video_common_alertview_cancel_button_bg_default · mbridge_video_common_alertview_cancel_button_bg_pressed · mbridge_video_common_alertview_cancel_button_textcolor · mbridge_video_common_alertview_confirm_button_bg_default · mbridge_video_common_alertview_confirm_button_bg_pressed · mbridge_video_common_alertview_confirm_button_textcolor
mbridge_video_common_alertview_content_textcolor · mbridge_video_common_alertview_feedback_rb_bg · mbridge_video_common_alertview_title_textcolor · mbridge_white · md_theme_light_background · md_theme_light_error · md_theme_light_onPrimary · md_theme_light_primary · md_theme_light_secondary · md_theme_light_surface · mobilefuse_black · mobilefuse_default_banner_bg · mobilefuse_transparent · mobilefuse_white
monetization_ads_internal_age_text_color · monetization_ads_internal_app_install_call_to_action_green · monetization_ads_internal_app_install_media_view_foreground · monetization_ads_internal_app_install_timer_text_color_dark · monetization_ads_internal_app_install_timer_text_color_light · monetization_ads_internal_call_to_action_background_color · monetization_ads_internal_call_to_action_background_color_blue · monetization_ads_internal_call_to_action_background_color_yellow · monetization_ads_internal_call_to_action_blue · monetization_ads_internal_call_to_action_ripple_highlight_dark_color · monetization_ads_internal_call_to_action_ripple_highlight_light_color · monetization_ads_internal_call_to_action_text_color · monetization_ads_internal_call_to_action_text_color_black · monetization_ads_internal_domain_grey_text_color
monetization_ads_internal_grey_text_color · monetization_ads_internal_header_background_end · monetization_ads_internal_header_background_start · monetization_ads_internal_instream_advertiser_block_color_focused · monetization_ads_internal_instream_advertiser_block_color_unfocused · monetization_ads_internal_instream_background · monetization_ads_internal_instream_call_to_action_block_color · monetization_ads_internal_instream_progress_bar_color · monetization_ads_internal_price_text_color · monetization_ads_internal_review_count_text_color · monetization_ads_internal_reward_controls_background_gray · monetization_ads_internal_rewarded_close_verification_text_color · monetization_ads_internal_sponsored_text_color · monetization_ads_internal_text_color_white
monetization_ads_internal_warning_text_color · mtrl_btn_bg_color_selector · mtrl_btn_ripple_color · mtrl_btn_stroke_color_selector · mtrl_btn_text_btn_bg_color_selector · mtrl_btn_text_btn_ripple_color · mtrl_btn_text_color_disabled · mtrl_btn_text_color_selector · mtrl_btn_transparent_bg_color · mtrl_calendar_item_stroke_color · mtrl_calendar_selected_range · mtrl_card_view_foreground · mtrl_card_view_ripple · mtrl_chip_background_color
mtrl_chip_close_icon_tint · mtrl_chip_surface_color · mtrl_chip_text_color · mtrl_choice_chip_background_color · mtrl_choice_chip_ripple_color · mtrl_choice_chip_text_color · mtrl_error · mtrl_fab_bg_color_selector · mtrl_fab_icon_text_color_selector · mtrl_fab_ripple_color · mtrl_filled_background_color · mtrl_filled_icon_tint · mtrl_filled_stroke_color · mtrl_indicator_text_color
mtrl_navigation_bar_colored_item_tint · mtrl_navigation_bar_colored_ripple_color · mtrl_navigation_bar_item_tint · mtrl_navigation_bar_ripple_color · mtrl_navigation_item_background_color · mtrl_navigation_item_icon_tint · mtrl_navigation_item_text_color · mtrl_on_primary_text_btn_text_color_selector · mtrl_on_surface_ripple_color · mtrl_outlined_icon_tint · mtrl_outlined_stroke_color · mtrl_popupmenu_overlay_color · mtrl_scrim_color · mtrl_switch_thumb_icon_tint
mtrl_switch_thumb_tint · mtrl_switch_track_decoration_tint · mtrl_switch_track_tint · mtrl_tabs_colored_ripple_color · mtrl_tabs_icon_color_selector · mtrl_tabs_icon_color_selector_colored · mtrl_tabs_legacy_text_color_selector · mtrl_tabs_ripple_color · mtrl_text_btn_text_color_selector · mtrl_textinput_default_box_stroke_color · mtrl_textinput_disabled_color · mtrl_textinput_filled_box_default_background_color · mtrl_textinput_focused_box_stroke_color · mtrl_textinput_hovered_box_stroke_color
notification_action_color_filter · notification_icon_bg_color · notification_material_background_media_default_color · pin_normal · pink · pob_controls_background_color · pob_controls_stroke_color · pob_custom_page_background_color · pob_install_button_background_color · pob_monitor_background · pob_monitor_close_background · pob_skip_button_background_color · preference_fallback_accent_color · primary_color_state
primary_dark_material_dark · primary_dark_material_light · primary_material_dark · primary_material_light · primary_text_default_material_dark · primary_text_default_material_light · primary_text_disabled_material_dark · primary_text_disabled_material_light · progressbar_bg · rab_colorBtnBlue · rab_colorBtnBluePressed · red · ripple_material_dark · ripple_material_light
secondary_text_default_material_dark · secondary_text_default_material_light · secondary_text_disabled_material_dark · secondary_text_disabled_material_light · seek_bar_complete · seek_bar_incomplete · simpledialogfragment_error_color · smaato_sdk_core_countdown_bg · smaato_sdk_core_progressbar_bg · smaato_sdk_core_ui_ctrl_almost_white · smaato_sdk_core_ui_ctrl_black · smaato_sdk_core_ui_ctrl_grey · smaato_sdk_core_ui_semitransparent · smaato_sdk_richmedia_ui_semitransparent
smaato_sdk_video_grey · smaato_sdk_video_white · switch_thumb_disabled_material_dark · switch_thumb_disabled_material_light · switch_thumb_material_dark · switch_thumb_material_light · switch_thumb_normal_material_dark · switch_thumb_normal_material_light · tooltip_background_dark · tooltip_background_light · tt_000000 · tt_00000000 · tt_1e000000 · tt_2a90d7
tt_4a90e2 · tt_72000000 · tt_7f2a90d7 · tt_88000000 · tt_b2000000 · tt_e0e0e0 · tt_e4ffffff · tt_fde6e6e6 · tt_fdffffff · tt_ff1a1a1a · tt_ffffff · ui_ctrl_almost_white · ui_ctrl_black · ui_ctrl_grey
ui_semitransparent · vector_tint_color · vector_tint_theme_color · white
```

#### Warna tema aplikasi (`md_theme_light_*` — lengkap, 6 entri)

| Resource | Nilai ARGB | Peran |
|---|---|---|
| `md_theme_light_primary` | `#ff429e46` | Hijau utama (toolbar, FAB, drawer header) |
| `md_theme_light_secondary` | `#ff66bb6a` | Hijau sekunder (ripple FAB) |
| `md_theme_light_surface` | `#ffffffff` | Kartu putih |
| `md_theme_light_background` | `#fff1f8e9` | Latar hijau sangat pucat |
| `md_theme_light_onPrimary` | `#ffffffff` | Teks/icon di atas primary |
| `md_theme_light_error` | `#ffb00020` | Error |

Distribusi 1.208 warna: `m3_*` 598, `material_*` 164, `mbridge_*` 60, `mtrl_*` 55, `design_*` 38, `monetization_ads_*` 29, `abc_*` 25, `ia_*` 25, `applovin_*` 21, `tt_*` 15, sisanya <12 per prefiks. **Tidak ada varian `md_theme_dark_*`** — aplikasi secara efektif light-only.

#### Style tema & FAB

| Style | Parent | Atribut |
|---|---|---|
| `style/Base.Theme.Minetuner` (`0x7f140079`) | `Theme.Material3.Light.NoActionBar` | `colorError=@color/md_theme_light_error`, `colorOnBackground=@color/md_theme_light_background`, `colorOnPrimary=@color/md_theme_light_onPrimary`, `colorPrimary=@color/md_theme_light_primary`, `colorSecondary=@color/md_theme_light_secondary`, `colorSurface=@color/md_theme_light_surface`, `floatingActionButtonStyle=@style/Widget.Minetuner.FAB` |
| `style/Theme.Minetuner` (`0x7f14033e`) | `Base.Theme.Minetuner` | tanpa override (size=0); dipakai di `<application android:theme>` |
| `style/Widget.Minetuner.FAB` (`0x7f140555`) | `Widget.Material3.FloatingActionButton.Primary` | `backgroundTint=@color/md_theme_light_primary`, `rippleColor=@color/md_theme_light_secondary`, `shapeAppearanceOverlay=@style/ShapeAppearance.Minetuner.FAB`, `tint=@color/md_theme_light_onPrimary` |
| `style/ShapeAppearance.Minetuner.FAB` (`0x7f14021b`) | `ShapeAppearance.Material3.SmallComponent` | `cornerFamily=0` (rounded), `cornerSize=0.5%` |
| `style/ThemeOverlay.AppCompat.Dark.ActionBar` (`0x7f140342`) | — | dipakai pada semua `AppBarLayout` aplikasi → ikon/toolbar kontras terang di atas hijau |

---

## 4. DECODE XMLTREE — STRUKTUR VIEW APLIKASI

Semua ID numerik diterjemahkan ke nama melalui peta resource `0x7f0aXXXX → id/*`, `0x7f13XXXX → string/*`.

### 4.1 `res/layout/activity_main.xml` (79 baris xmltree)

```text
androidx.coordinatorlayout.widget.CoordinatorLayout (clipChildren=false, match_parent)
├── com.google.android.material.appbar.AppBarLayout (theme=ThemeOverlay.AppCompat.Dark.ActionBar, wrap_content)
│   └── androidx.appcompat.widget.Toolbar
│       id=@id/toolbar | background=?attr/colorPrimary | height=?attr/actionBarSize
│       popupTheme=@style/ThemeOverlay.AppCompat.Light
│       title=@string/app_name ("MineTuner") | titleTextColor=@android:color/white
└── androidx.recyclerview.widget.RecyclerView
    id=@id/worldRecyclerView | match_parent | layout_behavior=AppBarLayout$ScrollingViewBehavior
└── LinearLayout id=@id/fabMenu
    gravity=bottom|end (0x00800055), orientation=vertical, marginBottom=16dp, marginEnd=16dp, clipChildren=false
    ├── LinearLayout id=@id/createWorldFabHolder (visibility=gone, margin=4dp, gravity=center)
    │   ├── TextView text="New World", textColor=@android:color/white, background=@drawable/fab_label_background
    │   └── FloatingActionButton id=@id/createWorldFab, src=@drawable/ic_layers, fabCustomSize=40dp
    ├── LinearLayout id=@id/addWorldFabHolder (visibility=gone, margin=4dp, gravity=center)
    │   ├── TextView text="Import World", textColor=@android:color/white, background=@drawable/fab_label_background
    │   └── FloatingActionButton id=@id/addWorldFab, src=@drawable/ic_upload, fabCustomSize=40dp
    └── FloatingActionButton id=@id/mainFab, src=@drawable/ic_add, marginTop=4dp
```

Catatan: label FAB berada di **kiri** ikon (urutan TextView → FAB), warna teks putih di atas `fab_label_background`.

### 4.2 `res/layout/activity_drawer.xml` (32 baris)

```text
androidx.drawerlayout.widget.DrawerLayout id=@id/drawer_layout (match_parent)
├── LinearLayout orientation=vertical
│   ├── AppBarLayout theme=ThemeOverlay.AppCompat.Dark.ActionBar
│   │   └── Toolbar id=@id/toolbar, background=?colorPrimary, height=?actionBarSize,
│   │       popupTheme=ThemeOverlay.AppCompat.Light, titleTextColor=white
│   └── FrameLayout id=@id/fragment_container (match_parent)
└── com.google.android.material.navigation.NavigationView
    id=@id/nav_view | width=wrap_content | height=match_parent | gravity=start
    app:headerLayout=@layout/drawer_header
    app:menu=@menu/drawer_menu
```

### 4.3 `res/layout/drawer_header.xml` (29 baris)

```text
LinearLayout (match_parent × 160dp) background=?attr/colorPrimary, padding=16dp, gravity=center_vertical
├── ImageView id=@id/nav_header_icon — 64dp × 64dp, scaleType=6 (CENTER_CROP)
├── TextView id=@id/nav_header_title — 20sp, bold(default), white, maxLines=1, ellipsize=end, paddingTop=8dp
└── TextView id=@id/nav_header_subtitle — white, maxLines=1, ellipsize=end
```

### 4.4 `res/layout/activity_help.xml` (85 baris)

Toolbar `id=@id/toolbar`, background `?colorPrimary`, title hardcoded **“Exporting Worlds”**, titleTextColor white.
Di bawahnya `ScrollView` (paddingHorizontal=8dp, `layout_behavior=AppBarLayout$ScrollingViewBehavior`) → `LinearLayout` vertikal berisi 5 pasangan teks/gambar:

| Teks (16sp) | Gambar (match_parent, wrap_content, scaleType=3 FIT_CENTER, adjustViewBounds=true) |
|---|---|
| “Step 1: Click the button below to import the texture pack into Minecraft.” | `@drawable/step1_tut` |
| **Button** `id=@id/button_import`, text **“Import”**, match_parent | — |
| “Step 2: Activate the texture pack in Minecraft.” | `@drawable/step2_tut` |
| “Step 3: Open a world in Minecraft and go into Game settings.” | `@drawable/step3_tut` |
| “Step 4: Scroll down to the bottom and press 'Export Template'.” | `@drawable/step4_tut` |
| “Select a file location then wait for the world to export.” | — |

Tidak ada ID untuk TextView langkah; hanya `button_import` yang interaktif.

### 4.5 `res/layout/activity_create_world.xml` (74 baris)

```text
CoordinatorLayout
├── AppBarLayout → Toolbar id=@id/toolbar, title="Create Flat World", background=?colorPrimary, white title
└── ScrollView paddingTop=16dp, paddingHorizontal=16dp
    └── LinearLayout vertical
        ├── TextView "Selected Biome" — 16sp, bold, marginBottom=8dp
        ├── <include id=@id/selected_biome_card layout=@layout/biome_item>
        ├── LinearLayout horizontal gravity=center
        │   └── CheckBox id=@id/prevent_slimes_checkbox
        │       text="Prevent Slimes from Spawning", 16sp, textColor=?android:textColorSecondary
        ├── LinearLayout horizontal gravity=center
        │   ├── TextView "Flat World Layers" — 16sp, bold, width=0dp weight=1
        │   └── ImageButton id=@id/add_layer_button — 48×48dp, src=@drawable/ic_add,
        │       background=?attr/selectableItemBackgroundBorderless, scaleType=7, tint=?android:textColorSecondary
        └── RecyclerView id=@id/layers_recycler — match_parent × wrap_content, nestedScrollingEnabled=true
```

### 4.6 `res/layout/activity_entities_nbt.xml` (27 baris)

```text
CoordinatorLayout
├── AppBarLayout → Toolbar id=@id/toolbar, title="Chunk Entities", background=?colorPrimary
├── RecyclerView id=@id/entitiesRecyclerView (match_parent)
└── FrameLayout id=@id/fragment_container (visibility=gone) — tempat fragment editor NBT muncul saat entitas dipilih
```

### 4.7 `res/layout/tree_node_item.xml` (28 baris)

```text
LinearLayout horizontal id=@id/node_container
background=?attr/selectableItemBackground | focusable=true | clickable=true
minHeight=48dp | paddingHorizontal=12dp | paddingVertical=6dp | gravity=center_vertical
├── ImageView id=@id/node_icon — 32×32dp, contentDescription="icon", marginEnd=12dp
└── TextView id=@id/key_text — 18sp, bold, white(?android:textColorPrimary tergantung theme),
    text="Key" (placeholder), maxLines=1, ellipsize=end
```

### 4.8 `res/layout/world_item.xml` (54 baris)

```text
androidx.cardview.widget.CardView (clickable=true)
cardCornerRadius=8dp | cardElevation=4dp | cardUseCompatPadding=true | cardPreventCornerOverlap=false
foreground=?attr/selectableItemBackground
└── LinearLayout horizontal, padding=12dp, gravity=center_vertical
    ├── ImageView id=@id/worldIcon — 48×48dp, scaleType=CENTER_CROP, marginEnd=12dp
    └── LinearLayout vertical (width=0dp, weight=1)
        ├── TextView id=@id/worldName — 18sp, bold, textColor=?android:textColorPrimary, maxLines=1, ellipsize=end
        ├── TextView id=@id/worldSize — 14sp, textColor=?android:textColorTertiary(0x01060000), maxLines=1
        └── TextView id=@id/worldInternalName — 14sp, italic, textColor=?android:textColorTertiary, maxLines=1
```

### 4.9 `res/layout/map_selection_view.xml` (14 baris)

```text
<merge>
├── <include id=@id/topLeft     layout=@layout/selection_drag_btn>
├── <include id=@id/topRight    layout=@layout/selection_drag_btn>
├── <include id=@id/bottomLeft  layout=@layout/selection_drag_btn>
└── <include id=@id/bottomRight layout=@layout/selection_drag_btn>
```
`selection_drag_btn` = `AppCompatButton` 24×24dp, background `?selectableItemBackground`, foreground `?colorPrimary`, stateListAnimator=`@null`.

### 4.10 `res/menu/drawer_menu.xml` (57 baris)

Grup `checkableBehavior=2` (single):

| ID | Ikon | Judul | String ID |
|---|---|---|---|
| `nav_map` | `ic_map` | Show Map | `0x7f130233` |
| `nav_world_nbt` | `ic_tune` | World NBT | `0x7f1302df` |
| `nav_singleplayer_nbt` | `ic_person` | Singleplayer NBT | `0x7f130236` |
| `nav_multiplayer_nbt` | `ic_group_people` | Multiplayer NBT | `0x7f1301e9` |
| `nav_change_world` | `ic_exit` | Change World | `0x7f130085` |

Submenu **Advanced** (`@string/advanced_section`) — grup checkable kedua:
`nav_overworld_nbt` (ic_list), `nav_the_end_nbt` (ic_list), `nav_nether_nbt` (ic_list), `nav_biomedata_nbt` (ic_list), `nav_portals_nbt` (ic_list), `nav_autonomous_entities_nbt` (ic_list), `nav_custom_nbt` (ic_search / “Open Custom NBT”).

### 4.11 `res/menu/main_activity_toolbar_menu.xml` (13 baris)

| ID | Ikon | Judul | showAsAction |
|---|---|---|---|
| `action_help` | `ic_book` | Help | `2` = **always** |
| `action_about` | `ic_info` | About | `1` = ifRoom |

### 4.12 `res/menu/create_world_activity_toolbar_menu.xml` (8 baris)

| ID | Ikon | Judul | showAsAction |
|---|---|---|---|
| `action_save` | `ic_save` | Save | `2` = always |

### 4.13 Bonus: layout pendukung

`biome_item.xml` = CardView (8dp radius, 4dp elevasi, padding 12dp) + ikon 40dp + TextView 16sp bold (`id 0x7f0a00f2`) + TextView 14sp abu (`0x7f0a00f1`).
`world_layer_item.xml` = CardView serupa + dua TextView (16sp bold `0x7f0a0385`, 14sp `0x7f0a0384`) + TextView count (`0x7f0a0386`, marginEnd 8dp) + ImageButton hapus.
`res/xml/file_paths.xml` = `<paths><external-files-path name="external_files" path="."/></paths>`.

---

## 5. NATIVE LIBRARIES & STRUKTUR APK

### 5.1 `config.arm64_v8a.apk` — 12 file `.so` (3.863.628 byte terkompresi)

| `.so` | Ukuran | Peran (verifikasi) |
|---|---:|---|
| `libleveldb.so` | 1.261.744 | **JNI LevelDB milik aplikasi** — `System.loadLibrary("leveldb")` dipanggil `com/hextetrahedral/leveldb/LevelDB.java`. Menyimpan/membaca kunci Bedrock (`~local_player`, `player_server_*`, `Overworld`, `TheEnd`, `Nether`, `BiomeData`, `Portals`, `AutonomousEntities`). |
| `libsentry.so` | 681.784 | Sentry Native (NDK crash handler); terhubung ke DSN `sentry.appodeal.com/42`, `io.sentry.ndk.scope-sync.enable=true` |
| `libapplovin-native-crash-reporter.so` | 860.200 | AppLovin MAX native crash reporter |
| `libtt_ugen_layout.so` | 440.672 | Pangle/ByteDance (Toutiao `tt_`) — rendering layout iklan UGEN |
| `libnms.so` | 297.832 | Pangle/ByteDance-native helper (non-max-suppression / layout engine) |
| `libapminsightb.so` | 124.352 | AppMetrica / Appodeal APM insight (modul B) |
| `libapminsighta.so` | 102.256 | AppMetrica / Appodeal APM insight (modul A) |
| `libpglarmor.so` | 68.600 | Pangle/ByteDance — proteksi/enkripsi resource (`armor`) |
| `libsentry-android.so` | 16.832 | JNI bridge Sentry Android |
| `libbuffer_pgl.so` | 9.000 | Pangle — buffer helper (`pgl`) |
| `libtobEmbedPagEncrypt.so` | 8.304 | Pangle — enkripsi embed PAG (`tobEmbedPagEncrypt`) |
| `libfile_lock_pgl.so` | 6.312 | Pangle — file locking (`pgl`) |

Isi lain split ini: `AndroidManifest.xml` (1.052 B), `stamp-cert-sha256` (32 B), `META-INF/BNDLTOOL.SF|.RSA|MANIFEST.MF`. **Tidak ada** `armeabi-v7a`, `x86`, atau `x86_64`.

### 5.2 Split lain

- **`config.en.apk`** (9 file): `AndroidManifest.xml`, 3 drawable `res/drawable-en/monetization_ads_instream_internal_*`, `resources.arsc` (76.392 B), stamp, 3 META-INF.
- **`config.mdpi.apk`**: `AndroidManifest.xml`, resource drawable densitas-mdpi untuk AppLovin/ExoPlayer/iklan, `resources.arsc`, stamp, META-INF.
- Semua split bertanda `1981-01-01 01:01` (ZIP epoch normalisasi bundletool) dan ditandatangani sertifikat Google yang sama.

### 5.3 Base APK — struktur

| Item | Jumlah / detail |
|---|---|
| Total entri ZIP | **2.360 file**, 110.289.674 byte uncompressed |
| `AndroidManifest.xml` | 70.564 byte (biner) |
| `resources.arsc` | ada (tabel resource, package id `0x7f`) |
| `classes*.dex` | **13**: `classes.dex` (10.918.168), `classes2` (27.304), `classes3` (11.048.188), `classes4` (8.334.172), `classes5` (9.808.836), `classes6` (5.156.832), `classes7` (9.353.420), `classes8` (7.435.364), `classes9` (9.589.124), `classes10` (8.335.756), `classes11` (10.467.440), `classes12` (8.778.944) |
| DEX tambahan | `assets/audience_network.dex` (5.017.908) — Meta Audience Network dimuat dinamis |
| Lainnya | `DebugProbesKt.bin`, `androidsupportmultidexversion.txt`, `stamp-cert-sha256`, `META-INF/*` (banyak `*.version`, `*.kotlin_module`, lisensi) |

**Assets (136 file)** — milik aplikasi:
- `assets/worldTemplate/level.dat` dan `levelname.txt` (template world flat)
- `assets/texture_pack.mcpack` (texture pack untuk editor peta)
- `assets/entityIcons/*.png` — **82 ikon entitas + `credits.txt`** (allay, armadillo, axolotl, bat, bee, blaze, bogged, breeze, camel, cat, cave_spider, chicken, cod, cow, creeper, dolphin, donkey, drowned, elder_guardian, ender_dragon, enderman, endermite, evoker, fox, frog, ghast, glow_squid, goat, guardian, hoglin, horse, husk, illusioner, iron_golem, llama, magma_cube, mooshroom, mule, ocelot, panda, parrot, phantom, pig, piglin, piglin_brute, pillager, polar_bear, pufferfish, rabbit, ravager, salmon, sheep, shulker, silver_fish, skeleton, skeleton_horse, slime, sniffer, snow_golem, spider, squid, stray, strider, tadpole, trader_llama, tropical_fish, turtle, vex, villager, vindicator, wandering_trader, warden, witch, wither, wither_skeleton, wither_skull, wolf, zoglin, zombie, zombie_horse, zombie_villager, zombified_piglin)

**Assets pihak ketiga (penting):** `assets/mraid*.js`, `assets/omsdk-v1.js`, `assets/ad-viewer/omid-session-client-v1.js`, `assets/dtb-m.js`, `assets/aps-mraid.js`, `assets/pob_mraid.js`, `assets/taurusx_mraid.js`, `assets/com.moloco.sdk.xenoss.sdkdevkit.mraid.js`, `assets/mobilefuse/{mraid,vast,ad_template}*`, `assets/bm_networks/*.bmnetwork` (8 file: amazon, gam, gam_dynamic, meta_audience, mintegral, my_target, pangle_sdk, vungle), `assets/fyb_*_endcard_tmpl.html`, `assets/builddatas.json`, `assets/rv_binddatas.xml`, `assets/a.json`, `assets/dsa_page.html`, `assets/dexopt/baseline.prof|profm` (baseline profile), `assets/PubMatic_Logo.svg`, `assets/pob_*.svg`.

### 5.4 Versi dependensi dari `META-INF/*.properties`

| Pustaka | Versi | Pustaka | Versi |
|---|---|---|---|
| `play-services-ads`, `play-services-ads-api` | 24.7.0 | `billing` | 7.1.1 |
| `play-services-ads-identifier` | 18.2.0 | `play-services-auth` | 20.7.0 |
| `play-services-appset` | 16.1.0 | `play-services-auth-api-phone` | 18.0.1 |
| `play-services-base` | 18.5.0 | `play-services-basement` | 18.4.0 |
| `play-services-cronet` | 18.0.1 | `play-services-fido` | 20.0.1 |
| `play-services-location` | 21.0.1 | `play-services-measurement-base/-sdk-api` | 20.1.2 |
| `play-services-tasks` | 18.2.0 | `transport-api` | 3.0.0 |
| `transport-runtime`, `transport-backend-cct` | 3.1.8 | `user-messaging-platform` | 3.2.0 |
| `firebase-encoders` | 17.0.0 | `firebase-encoders-json` | 18.0.0 |
| `firebase-encoders-proto` | 16.0.0 | — | — |

Kotlin modules mencakup `applovin_max_productionRelease`, `bidmachine-android-sdk_*_3_4_0`, `ChartboostMonetization-9.9.1_productionRelease`, `DTBAndroidSDK_release`, `bigoads_productionRelease`, `unityads_productionRelease`, `vungle-ads_release`, `mintegral_productionRelease`, `mobilefuse-*`, `moloco-*`, `sentry-android-replay_release`, `okhttp`, `okio`, `ktor-*`, `coil-*`, `room-*`, `work-runtime-*`, dll.

---

## 6. PIHAK KETIGA / SDK / IKLAN — INVENTARISASI

### 6.1 Orkestrasi iklan utama

Bukti langsung dari JADX `MainActivity.java`:
```java
import com.appodeal.ads.Appodeal;
...
Appodeal.initialize(this, "79e2afef82af6ff44ec14c4cd547374550cfb9f45c77e338", 3, new x.a());
...
if (System.currentTimeMillis() - d1.b.b >= 10000 && Appodeal.isLoaded(3)) {
    Appodeal.show(this, 3, "video_interstitial");
}
```
**Appodeal** adalah SDK iklan yang dipanggil langsung aplikasi (app key `79e2af…e338`, tipe ad `3` = interstitial, cooldown 10 detik, placement `video_interstitial`). Appodeal kemudian memuat **AppLovin MAX** sebagai mediator (20 komponen debugger) dan puluhan adapter di bawahnya.

String About: *“The app is supported by ads, but only a minimal amount to avoid disrupting your experience.”* + Discord `https://www.discord.gg/Mr2za3VkUQ`.

### 6.2 Matriks SDK terdeteksi

| SDK / jaringan | Bukti utama | Komponen | Catatan |
|---|---|---:|---|
| **Appodeal** | import + `Appodeal.initialize` di MainActivity | 4 | orkestrator; ConsentActivity, TestActivity, PackageAddedReceiver |
| **AppLovin / MAX** | 20 komponen, `applovin_*` drawable/anim/raw, kotlin module, native crash reporter | 20 | MaxDebugger* (15 activity), AppLovinInitProvider, FullscreenAdService |
| **Google Mobile Ads (AdMob)** | APPLICATION_ID `ca-app-pub-335…8728`, `play-services-ads 24.7.0` | 7 | AdActivity, AdService, MobileAdsInitProvider, flag OPTIMIZE_* |
| **Google Play Billing** | `BILLING`, metadata versi 7.1.1 | 2 | ProxyBillingActivity + V2 |
| **Google Play Auth / GMS** | SignInHubActivity, RevocationBoundService, play-services-auth | 2 | akun Google |
| **Google Datatransport / Firebase** | 3 komponen + firebase-encoders + `CronetProviderClassName` | 3 | telemetri/CrONet |
| **Chartboost** | 4 komponen, module `ChartboostMonetization-9.9.1`, `VideoRepositoryDownloadService` | 4 | ExoPlayer untuk video |
| **Fyber / Inneractive** | 1.688 file JADX, `ia_*` layout/drawable/color, `fyb_*` HTML | 5 | report/fullscreen/browser |
| **Smaato** | 4 komponen, `smaato_sdk_*` resources | 4 | auto-init=false |
| **Amazon DTB / APS** | `DTBAndroidSDK_release`, `amazon_productionRelease`, `dtb-m.js` | 3 | DTBInterstitial(exported), DTBAdActivity, ApsInterstitial |
| **PubMatic / OpenWrap** | 3 komponen, `pob_*`, `PubMatic_Logo.svg`, query `com.pubmatic.openwrapapp` | 3 | MRAID/video/browser |
| **Yandex Mobile Ads** | 4 komponen, metadata auto-init=false, drawable mediator | 4 | AppMetrica integration |
| **AppMetrica** | 2 komponen, `libapminsight*.so`, `io.appmetrica.*` JADX | 2 | analytics (+,Yandex) |
| **InMobi** | `InMobiAdActivity`, `inmobi_productionRelease` | 1 | — |
| **Pangle / ByteDance** | 15 komponen `TT*`, pangle.version `7.3.0.5`, 6 `.so`, `libtt_*` drawables/strings | 15 + service | heavy native |
| **Unity Ads** | 5 komponen + `unity-ads_release`, `unityads_productionRelease`, `loadLibrary("unity")` | 5+1 | Startup initializer |
| **ironSource / LevelPlay** | 6 komponen, `level_play_productionRelease`, `mediationsdk_release` | 6 | 2 lifecycle provider |
| **Mintegral / Mbridge** | 4 komponen, ~50 layout `mbridge_*`, ~100 drawable | 4 | warna `mbridge_*` 60 |
| **Vungle** | 2 komponen, `vungle-ads_release`, `vungle_productionRelease` | 2 | initOrder 102 |
| **BIGO Ads** | 11 komponen, ~165 layout `bigo_ad_*`, ratusan drawable/string | 11 | splash/interstitial/reward/form |
| **TaurusX** | 9 komponen, `taurusx_*` resources + MRAID JS | 9 | file provider `nath_tax_file_path` |
| **OGury / Presage** | 4 komponen, `ogury*` JADX (589 file) | 4 | LogEnablerReceiver exported |
| **StartApp** | 6 komponen, `startio_productionRelease`, SPLASH_ENABLED=false, BOOT_COMPLETED | 6 | background + scheduler service |
| **MobileFuse** | 5 komponen, module `mobilefuse-*`, auto-init=false, OMID license | 5 | vast/mraid assets |
| **Moloco** | 4 activity + Startup initializer + `moloco-*` modul | 4+1 | Xenoss renderer |
| **MyTarget** | 2 komponen, JADX `com.my.tracker.obfuscated` | 2 | content provider |
| **Facebook Audience Network** | 2 komponen + `audience_network.dex` (5 MB) | 2 | dimuat dinamis |
| **BidMachine** | 5 komponen, 3 modul `bidmachine-android-sdk_*_3_4_0`, 8 `.bmnetwork` | 5 | version 3.4.0 |
| **ExploreStack IAB** | 3 komponen MRAID/VAST | 3 | — |
| **PubNative Lite** | 8 komponen, JADX 927 file | 8 | consent/browser/volume receiver |
| **Digital Turbine / DT Exchange** | query `IgniteRemoteService`, `dtx_store_promo_layout`, `dt_store_promo_layout` | 0 | tanpa komponen eksplisit |
| **Sentry** | DSN + 5 meta-data + `.so` + `sentry_dialog_user_feedback` | 0 | crash reporting Appodeal |
| **OkHttp / Okio / Ktor / Coil / Gson** | kotlin modules + JADX 171/79/32/93 file | 0 | networking, JSON, gambar |
| **ExoPlayer / Media3** | `exo_*` layout/drawable/string/color, Chartboost download service | 1 | pemutar video iklan |
| **Picasso** | `PicassoProvider` | 1 | gambar (adapter iklan) |
| **PairIP** | 2 komponen + `CHECK_LICENSE` | 2 | lihat §7 |

Jumlah aktivitas iklan/SDK saja ≈ **124** dari 129 activity (termasuk MAX debugger 15, TT 14, bigo 10, taurusx 8, mbridge 4, pubnative 7, dll.).

---

## 7. PAIRIP — LISENSI & DIALOG “Something went wrong”

| Aspek | Temuan |
|---|---|
| Komponen | `com.pairip.licensecheck.LicenseActivity` (`exported=false`) + `com.pairip.licensecheck.LicenseContentProvider` (`exported=false`, authority `com.hextetrahedral.minetuner.com.pairip.licensecheck.LicenseContentProvider`) |
| Izin | `com.android.vending.CHECK_LICENSE` (satu-satunya entri di akhir manifest) |
| Peran | Google Play **App Integrity / licensing check** (PairIP). Dijalankan saat cold-start sebelum `MainActivity` agar APK terlindungi dari modifikasi/piracy. Activity tidak diekspor → hanya dipanggil internal. |
| Observasi runtime (dari artefak emulator di workspace) | `am start -W -n com.hextetrahedral.minetuner/.MainActivity` → topActivity `com.pairip.licensecheck.LicenseActivity`; logcat `pid sent binder code 2 with flags 2 and got error -74` (tanpa Play Store fungsional). |
| Isi dialog (uiautomator `ui_back.xml` / `dyn_01_main.xml`) | Judul: **“Something went wrong”**; pesan: **“Check that Google Play is enabled on your device and that you're using an up-to-date version before opening the app. If the problem persists try reinstalling the app.”**; tombol **CLOSE** (`android:id/button1`). |
| Tombol BACK | Tidak menutup dialog; CLOSE mengakhiri proses (focus ke launcher). |
| AVD dengan Play Store asli | PairIP membuka halaman sign-in `com.google.android.finsky.unauthenticated.activity.UnauthenticatedMainActivity`; tanpa akun Google, BACK → aplikasi keluar. |
| Kesimpulan | Dialog **bukan fitur MineTuner**, melainkan efek lingkungan (emulator tanpa lisensi Play). World list tidak pernah tercapai secara dinamis; analisis UI statis tetap valid. |

---

## 8. KESIMPULAN & CATATAN PEMBEDA

1. **Aplikasi inti sangat kecil dibanding bungkusnya:** 5 activity + 1 provider vs 172 komponen pihak ketiga; 20 layout aplikasi vs 507 layout iklan/library; 13 DEX sebagian besar adalah kode iklan/analytics.
2. **Desain Visual:** Material3 Light, hijau `#429E46`, latar `#F1F8E9`, kartu putih radius 8dp/elevasi 4dp, FAB primary dengan ripple `#66BB6A`, font `Inter`. Tidak ada dark theme.
3. **Fitur editor:** Berbasis Android View (bukan Compose) + RecyclerView; navigasi 12 entri (5 utama + 7 Advanced); `libleveldb.so` memberi akses langsung ke database Bedrock.
4. **Monetisasi berlapis:** Appodeal → AppLovin MAX → ≥20 adapter; ditambah Sentry (via Appodeal), AppMetrica, Billing, PairIP. Appodeal app key dan AdMob app ID bersifat publik (non-rahasia) namun tetap dicatat.
5. **Build:** dihasilkan dari Android App Bundle via bundletool (v2/v3 signing, SourceStamp, `BNDLTOOL.*`, `splitTypes`), diunggah ke APKPure sebagai XAPK v2.
6. **Tidak ditemukan:** permission kontak/kamera/mikrofon/lokasi presisi; komponen exported selain 12 yang tercantum; library native untuk arsitektur selain arm64-v8a.

— **Akhir laporan.** Seluruh angka di atas berasal dari dump langsung (`aapt2`, `unzip`, `apksigner`, `sha256sum`) dan diverifikasi silang dengan dekompilasi JADX serta artefak dinamis yang tersedia di workspace.
