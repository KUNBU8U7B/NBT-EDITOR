# Bagian 11 — Inventarisasi FUNGSI/METHOD seluruh kode aplikasi

Diekstrak otomatis dari sumber JADX (method signature) dan smali PairIP. Ini melengkapi Bagian 9 (file) dan Bagian 10 (cara kerja): setiap method aplikasi tercantum; library pihak ketiga tetap dihitung per paket di Bagian 9.3/9.4.

## 11.1 Ringkasan

67 class aplikasi, **302 method/signature** tercatat.

| Paket | Jumlah method |
|---|---:|
| `d0` | 40 |
| `z` | 37 |
| `com.hextetrahedral.minetuner` | 32 |
| `t` | 32 |
| `s` | 25 |
| `f0` | 24 |
| `com.hextetrahedral.minetuner.app` | 22 |
| `y` | 19 |
| `com.hextetrahedral.leveldb` | 17 |
| `h0` | 12 |
| `e0` | 11 |
| `x` | 8 |
| `a0` | 7 |
| `b0` | 6 |
| `q` | 3 |
| `i0` | 2 |
| `u` | 2 |
| `w` | 2 |
| `c0` | 1 |

## 11.2 Daftar method per class aplikasi

### `a0.a` — 2 method

- `public /* synthetic */ a(Object obj, int i)`
- `public final void onClick(View view)`

### `a0.c` — 5 method

- `for (final View view : this.b)`
- `public c(FloatingActionButton floatingActionButton, ArrayList arrayList)`
- `public final OvershootInterpolator d = new OvershootInterpolator()`
- `public final void a()`
- `public final void run()`

### `b0.a` — 2 method

- `public /* synthetic */ a(int i)`
- `public final void run()`

### `b0.b` — 2 method

- `public static boolean a()`
- `public static final Handler a = new Handler(Looper.getMainLooper())`

### `b0.c` — 2 method

- `public c(long j, long j2)`
- `public final boolean a(String str)`

### `c0.a` — 1 method

- `void a(RecyclerView.ViewHolder viewHolder, int i)`

### `com.hextetrahedral.leveldb.LevelDB` — 17 method

- `private static native byte[] nativeGet(long j, byte[] bArr) throws Exception`
- `private static native byte[][] nativeIterateKeys(long j, byte[] bArr) throws Exception`
- `private static native long nativeOpen(String str) throws Exception`
- `private static native void nativeClose(long j) throws Exception`
- `private static native void nativePut(long j, byte[] bArr, byte[] bArr2) throws Exception`
- `public final ArrayList c() throws a`
- `public final byte[] b(byte[] bArr) throws a`
- `public final void a() throws a`
- `public final void d(String str) throws a`
- `public final void e(byte[] bArr, byte[] bArr2) throws a`
- `return nativeGet(this.a, bArr)`
- `throw new a("Error closing LevelDB", e)`
- `throw new a("Error getting value for key: " + Arrays.toString(bArr), e)`
- `throw new a("Error iterating keys with binary prefix", e)`
- `throw new a("Error putting key-value pair: " + Arrays.toString(bArr), e)`
- `throw new a(android.support.v4.media.a.m("Failed to open LevelDB at path: ", str))`
- `throw new a(coil.intercept.a.o(e, android.support.v4.media.a.B("Error opening LevelDB at path: ", str, " - More info: ")), e)`

### `com.hextetrahedral.minetuner.DrawerActivity` — 11 method

- `public final Fragment l(int i)`
- `public final boolean onNavigationItemSelected(MenuItem menuItem)`
- `public final boolean onResult(String str, int i, Bundle bundle)`
- `public final void m(int i, boolean z2)`
- `public final void n()`
- `public final void o(int i, Fragment fragment, String str)`
- `public final void onCreate(Bundle bundle)`
- `public final void onSaveInstanceState(Bundle bundle)`
- `return new b()`
- `return new e()`
- `return new h()`

### `com.hextetrahedral.minetuner.MainActivity` — 21 method

- `final Context applicationContext = getApplicationContext()`
- `final String strD = jVar.d()`
- `final Uri data = intent.getData()`
- `final f fVar = (f) viewHolder`
- `final j jVar = ((e) this.k.get(i)).a`
- `final j jVar = new j(new File(string2))`
- `final j jVar2 = new j(new File(string10))`
- `public final boolean onCreateOptionsMenu(Menu menu)`
- `public final boolean onOptionsItemSelected(MenuItem menuItem)`
- `public final boolean onResult(String str, int i, Bundle bundle)`
- `public final void a(RecyclerView.ViewHolder viewHolder, int i)`
- `public final void l()`
- `public final void onActivityResult(final int i, int i2, Intent intent)`
- `public final void onClick(View view)`
- `public final void onClick(View view2)`
- `public final void onCreate(Bundle bundle)`
- `public final void onRequestPermissionsResult(int i, String[] strArr, int[] iArr)`
- `public final void onResume()`
- `public final void run()`
- `throw new RuntimeException(e)`
- `throw new RuntimeException(e2)`

### `com.hextetrahedral.minetuner.app.CreateWorldActivity` — 12 method

- `new ItemTouchHelper(new d(this)).attachToRecyclerView(recyclerViewFindViewById)`
- `new e().pos("CANCEL").show(createWorldActivity, "pick_biome_dialog")`
- `new j().pos("CANCEL").show(createWorldActivity, "pick_block_dialog")`
- `public final String validate(String str, String str2, Bundle bundle)`
- `public final boolean onCreateOptionsMenu(Menu menu)`
- `public final boolean onOptionsItemSelected(MenuItem menuItem)`
- `public final boolean onResult(String str, int i, Bundle bundle)`
- `public final void a(RecyclerView.ViewHolder viewHolder, int i)`
- `public final void l(t.a aVar)`
- `public final void onClick(View view)`
- `public final void onCreate(Bundle bundle)`
- `public final void onSaveInstanceState(Bundle bundle)`

### `com.hextetrahedral.minetuner.app.EntitiesNBTActivity` — 7 method

- `public final ArrayList n = new ArrayList()`
- `public final boolean l()`
- `public final boolean onOptionsItemSelected(MenuItem menuItem)`
- `public final boolean onResult(String str, int i, Bundle bundle)`
- `public final void a(RecyclerView.ViewHolder viewHolder, int i)`
- `public final void m()`
- `public final void onCreate(Bundle bundle)`

### `com.hextetrahedral.minetuner.app.HelpActivity` — 3 method

- `public final boolean onOptionsItemSelected(MenuItem menuItem)`
- `public final boolean onResult(String str, int i, Bundle bundle)`
- `public final void onCreate(Bundle bundle)`

### `d0.a` — 3 method

- `public /* synthetic */ a(e eVar)`
- `public Bitmap a(i1.a aVar, Context context)`
- `public void b(MotionEvent motionEvent)`

### `d0.b` — 3 method

- `public /* synthetic */ b(e eVar, Bundle bundle)`
- `public final void accept(Object obj)`
- `public final void run()`

### `d0.e` — 17 method

- `final t.f fVar = (t.f) t.f.d.get(((l) optionalM.get()).g().c)`
- `new Path()`
- `public final HashMap f = new HashMap()`
- `public final HashSet g = new HashSet()`
- `public final View onCreateView(LayoutInflater layoutInflater, ViewGroup viewGroup, Bundle bundle)`
- `public final boolean onResult(String str, int i2, Bundle bundle)`
- `public final e0.a d = new e0.a()`
- `public final e0.a e = new e0.a()`
- `public final void b(s.a aVar)`
- `public final void onDestroy()`
- `public final void onPause()`
- `public final void onResume()`
- `public final void onSaveInstanceState(Bundle bundle)`
- `public final void run()`
- `public static final int h = Math.floorDiv(536870912, 16)`
- `public static final int i = Math.floorDiv(536870912, 16)`
- `public static void c(String str, Canvas canvas)`

### `d0.g` — 7 method

- `public final boolean onSingleTapConfirmed(MotionEvent motionEvent)`
- `public final boolean onTouchEvent(MotionEvent motionEvent)`
- `public final void onLongPress(MotionEvent motionEvent)`
- `public final void onScrollChanged(int i, int i2, int i3, int i4)`
- `public void setOnLongPressListener(f fVar)`
- `public void setSelectionView(i iVar)`
- `public void setTouchable(boolean z2)`

### `d0.i` — 10 method

- `public Rect getSelection()`
- `public final Parcelable onSaveInstanceState()`
- `public final void onDraw(Canvas canvas)`
- `public final void onLayout(boolean z2, int i, int i2, int i3, int i4)`
- `public final void onRestoreInstanceState(Parcelable parcelable)`
- `public final void onViewAdded(View view)`
- `public i(Context context)`
- `public void setSelectionChangedListener(@Nullable h hVar)`
- `public void setTileView(g gVar)`
- `return new Rect(this.h)`

### `e0.a` — 10 method

- `public static int a(t.b bVar, int i, int i2, int i3)`
- `public void b(s.a aVar, Canvas canvas, int i, int i2, int i3, int i4, Paint paint)`
- `public void onInterstitialClicked()`
- `public void onInterstitialClosed()`
- `public void onInterstitialExpired()`
- `public void onInterstitialFailedToLoad()`
- `public void onInterstitialLoaded(boolean z2)`
- `public void onInterstitialShowFailed()`
- `public void onInterstitialShown()`
- `throw new RuntimeException(e)`

### `e0.b` — 1 method

- `public /* synthetic */ b(a aVar, int i, int i2, int i3, int i4, s.a aVar2, s.a aVar3, s.a aVar4, Paint paint, Canvas canvas)`

### `f0.a` — 4 method

- `public final void d()`
- `public final void onCreate(Bundle bundle)`
- `public final void onViewCreated(View view, Bundle bundle)`
- `public static a f(String str)`

### `f0.b` — 4 method

- `public final ArrayList d = new ArrayList()`
- `public final View onCreateView(LayoutInflater layoutInflater, ViewGroup viewGroup, Bundle bundle)`
- `public final void a(RecyclerView.ViewHolder viewHolder, int i)`
- `public final void onViewCreated(View view, Bundle bundle)`

### `f0.d` — 1 method

- `public final boolean test(Object obj)`

### `f0.e` — 1 method

- `public final Object apply(int i)`

### `f0.g` — 12 method

- `final String strTrim = ((hVar.j() == aVar2) || (hVar.j() == aVar3 || hVar.j() == x2.a.n)) ? null : str.trim()`
- `public final String validate(String str, String str2, Bundle bundle)`
- `public final View onCreateView(LayoutInflater layoutInflater, ViewGroup viewGroup, Bundle bundle)`
- `public final boolean onResult(String str, int i, Bundle bundle)`
- `public final boolean test(Object obj)`
- `public final void b(Context context, String str, String str2, x2.a aVar, int i, Bundle bundle)`
- `public final void c(h0.d dVar)`
- `public final void e(h0.d dVar, int i)`
- `public final void onClick(View view)`
- `public final void onSaveInstanceState(Bundle bundle)`
- `public final void onViewStateRestored(Bundle bundle)`
- `public void d()`

### `f0.h` — 2 method

- `public final void d()`
- `public final void onViewCreated(View view, Bundle bundle)`

### `h0.b` — 1 method

- `public b(View view)`

### `h0.c` — 9 method

- `final b bVar = (b) viewHolder`
- `final d dVar = (d) this.j.get(i)`
- `public c(Context context, ArrayList arrayList)`
- `public final RecyclerView.ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i)`
- `public final boolean onLongClick(View view)`
- `public final d b(int i)`
- `public final int getItemCount()`
- `public final void onBindViewHolder(RecyclerView.ViewHolder viewHolder, int i)`
- `return new b(LayoutInflater.from(this.k).inflate(2131558924, viewGroup, false))`

### `h0.d` — 2 method

- `public d(String str, boolean z2, l lVar)`
- `public final ArrayList d = new ArrayList()`

### `i0.a` — 1 method

- `public final Object apply(Object obj)`

### `i0.b` — 1 method

- `public final void accept(Object obj)`

### `q.a` — 3 method

- `public a(Runnable runnable)`
- `public final Object doInBackground(Object[] objArr)`
- `public final void onPostExecute(Object obj)`

### `s.a` — 19 method

- `public a(int i, int i2, j jVar, int i3)`
- `public final ArrayList c()`
- `public final AtomicReference f = new AtomicReference(null)`
- `public final ConcurrentHashMap e = new ConcurrentHashMap()`
- `public final Optional b(byte[] bArr)`
- `public final ThreadLocal g = ThreadLocal.withInitial(new androidx.emoji2.text.flatbuffer.a(1))`
- `public final byte[] a(int i, int i2, int i3, byte b, boolean z2)`
- `public final int e(int i, int i2, t.b bVar) throws Throwable`
- `public final void d(e0.b bVar)`
- `throw new IllegalArgumentException("Not enough block data.")`
- `throw new IllegalArgumentException("Unexpected chunk index value: " + i8)`
- `throw new IllegalArgumentException("Unexpected end of stream when reading palette size.")`
- `throw new IllegalArgumentException("Unexpected non-save persistence version found.")`
- `throw new IllegalArgumentException("Unexpected number of storage areas: " + i5)`
- `throw new IllegalArgumentException("Unexpectedly large number of palette entries: " + iJ)`
- `throw new IllegalArgumentException("Unknown or corrupted subchunk version: " + i19)`
- `throw new RuntimeException("Error reading entity data", e)`
- `throw new RuntimeException("Error reading subchunk data", e)`
- `throw new RuntimeException(e)`

### `s.b` — 2 method

- `public static /* synthetic */ byte a(int i)`
- `public static /* synthetic */ int b(int i)`

### `s.c` — 4 method

- `public c(int i, int i2, int i3, int i4)`
- `public final ReentrantReadWriteLock g = new ReentrantReadWriteLock()`
- `public final int a(int i)`
- `throw new IllegalArgumentException("Block data too short for expected number of blocks")`

### `t.b` — 2 method

- `public final u.a b = new u.a(0, 0, 0)`
- `public final u.a c = new u.a(0, 0, 0)`

### `t.c` — 1 method

- `public static final Map Ql = (Map) Arrays.stream(values()).collect(Collectors.toMap(new i0.a(1), new i0.a(2)))`

### `t.d` — 2 method

- `public d(j jVar)`
- `public final Object create(Object obj)`

### `t.e` — 3 method

- `public e(int i, int i2, int i3)`
- `public final boolean equals(Object obj)`
- `public final int hashCode()`

### `t.f` — 1 method

- `public static final Map d = (Map) Arrays.stream(values()).collect(Collectors.toMap(new i0.a(3), new i0.a(4)))`

### `t.g` — 1 method

- `public final int applyAsInt(Object obj)`

### `t.h` — 1 method

- `public h(c cVar, int i)`

### `t.i` — 4 method

- `public final FileVisitResult postVisitDirectory(Object obj, IOException iOException)`
- `public final FileVisitResult visitFile(Object obj, BasicFileAttributes basicFileAttributes)`
- `public final FileVisitResult visitFileFailed(Object obj, IOException iOException)`
- `public i(AtomicLong atomicLong)`

### `t.j` — 17 method

- `public final ArrayList e()`
- `public final Bitmap h()`
- `public final Optional f(l lVar)`
- `public final ReentrantReadWriteLock d = new ReentrantReadWriteLock()`
- `public final String d()`
- `public final String g()`
- `public final boolean i()`
- `public final boolean j()`
- `public final byte[] c(byte[] bArr)`
- `public final l l()`
- `public final s.a b(int i, int i2, int i3)`
- `public final void a()`
- `public final void k()`
- `public final void m(l lVar)`
- `public j(File file)`
- `return new File(this.c, "world_icon.jpeg").exists()`
- `return new String(Files.readAllBytes(file.toPath()))`

### `u.a` — 2 method

- `public a(int i, int i2, int i3)`
- `public final String toString()`

### `w.a` — 2 method

- `public /* synthetic */ a(AppCompatActivity appCompatActivity, int i)`
- `public final void handleOnBackPressed()`

### `x.a` — 1 method

- `public final void onInitializationFinished(List list)`

### `x.b` — 2 method

- `public static final AtomicReference a = new AtomicReference(null)`
- `public static j a()`

### `x.d` — 5 method

- `public d(CreateWorldActivity createWorldActivity)`
- `public final boolean isLongPressDragEnabled()`
- `public final boolean onMove(RecyclerView recyclerView, RecyclerView.ViewHolder viewHolder, RecyclerView.ViewHolder viewHolder2)`
- `public final int getMovementFlags(RecyclerView recyclerView, RecyclerView.ViewHolder viewHolder)`
- `public final void onSwiped(RecyclerView.ViewHolder viewHolder, int i)`

### `y.a` — 1 method

- `public a(View view)`

### `y.b` — 5 method

- `public b(ArrayList arrayList, CreateWorldActivity createWorldActivity)`
- `public final RecyclerView.ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i)`
- `public final int getItemCount()`
- `public final void onBindViewHolder(RecyclerView.ViewHolder viewHolder, int i)`
- `return new a(LayoutInflater.from(viewGroup.getContext()).inflate(2131558926, viewGroup, false))`

### `y.c` — 1 method

- `public c(View view)`

### `y.d` — 5 method

- `public d(ArrayList arrayList, c0.a aVar)`
- `public final RecyclerView.ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i)`
- `public final int getItemCount()`
- `public final void onBindViewHolder(RecyclerView.ViewHolder viewHolder, int i)`
- `return new c(LayoutInflater.from(viewGroup.getContext()).inflate(2131558924, viewGroup, false))`

### `y.e` — 1 method

- `public e(j jVar)`

### `y.f` — 1 method

- `public f(View view)`

### `y.g` — 5 method

- `public final RecyclerView.ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i)`
- `public final int getItemCount()`
- `public final void onBindViewHolder(RecyclerView.ViewHolder viewHolder, int i)`
- `public g(ArrayList arrayList, MainActivity mainActivity)`
- `return new f(LayoutInflater.from(viewGroup.getContext()).inflate(2131558925, viewGroup, false))`

### `z.a` — 3 method

- `public a(e eVar)`
- `public final boolean onQueryTextChange(String str)`
- `public final boolean onQueryTextSubmit(String str)`

### `z.b` — 5 method

- `public b(d dVar, ArrayList arrayList)`
- `public final boolean areContentsTheSame(int i, int i2)`
- `public final boolean areItemsTheSame(int i, int i2)`
- `public final int getNewListSize()`
- `public final int getOldListSize()`

### `z.c` — 1 method

- `public c(View view)`

### `z.d` — 5 method

- `public d(t.a[] aVarArr)`
- `public final RecyclerView.ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i)`
- `public final int getItemCount()`
- `public final void onBindViewHolder(RecyclerView.ViewHolder viewHolder, int i)`
- `return new c(LayoutInflater.from(viewGroup.getContext()).inflate(2131558606, viewGroup, false))`

### `z.e` — 1 method

- `public final View onCreateContentView(Bundle bundle)`

### `z.f` — 3 method

- `public f(j jVar)`
- `public final boolean onQueryTextChange(String str)`
- `public final boolean onQueryTextSubmit(String str)`

### `z.g` — 5 method

- `public final boolean areContentsTheSame(int i, int i2)`
- `public final boolean areItemsTheSame(int i, int i2)`
- `public final int getNewListSize()`
- `public final int getOldListSize()`
- `public g(i iVar, ArrayList arrayList)`

### `z.h` — 1 method

- `public h(View view)`

### `z.i` — 5 method

- `public final RecyclerView.ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i)`
- `public final int getItemCount()`
- `public final void onBindViewHolder(RecyclerView.ViewHolder viewHolder, int i)`
- `public i(t.c[] cVarArr)`
- `return new h(LayoutInflater.from(viewGroup.getContext()).inflate(2131558926, viewGroup, false))`

### `z.j` — 1 method

- `public final View onCreateContentView(Bundle bundle)`

### `z.k` — 4 method

- `public final void onProgressChanged(SeekBar seekBar, int i, boolean z2)`
- `public final void onStartTrackingTouch(SeekBar seekBar)`
- `public final void onStopTrackingTouch(SeekBar seekBar)`
- `public k(l lVar, int i)`

### `z.l` — 3 method

- `public final Bundle onResult(int i)`
- `public final View onCreateContentView(Bundle bundle)`
- `public final void onSaveInstanceState(Bundle bundle)`


## 11.3 Method PairIP (dari smali, 22 class)

### `ILicenseV2ResultListener$Stub` — 4 method

- `.method public constructor <init>()V`
- `.method private static readTypedObject(Landroid/os/Parcel;Landroid/os/Parcelable$Creator;)Ljava/lang/Object;`
- `.method public asBinder()Landroid/os/IBinder;`
- `.method public onTransact(ILandroid/os/Parcel;Landroid/os/Parcel;I)Z`

### `ILicenseV2ResultListener` — 1 method

- `.method public abstract verifyLicense(ILandroid/os/Bundle;)V`

### `LicenseActivity$$ExternalSyntheticLambda0` — 2 method

- `.method public synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseActivity;Landroid/app/PendingIntent;)V`
- `.method public final run()V`

### `LicenseActivity$$ExternalSyntheticLambda1` — 2 method

- `.method public synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseActivity;)V`
- `.method public final run()V`

### `LicenseActivity$$ExternalSyntheticLambda2` — 2 method

- `.method public synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseActivity;)V`
- `.method public final onClick(Landroid/content/DialogInterface;I)V`

### `LicenseActivity$ActivityType` — 5 method

- `.method private static synthetic $values()[Lcom/pairip/licensecheck/LicenseActivity$ActivityType;`
- `.method static constructor <clinit>()V`
- `.method private constructor <init>(Ljava/lang/String;I)V`
- `.method public static valueOf(Ljava/lang/String;)Lcom/pairip/licensecheck/LicenseActivity$ActivityType;`
- `.method public static values()[Lcom/pairip/licensecheck/LicenseActivity$ActivityType;`

### `LicenseActivity` — 13 method

- `.method public static synthetic $r8$lambda$N5_Pzpb-eSKmOONXn3Kn0QvMbys(Lcom/pairip/licensecheck/LicenseActivity;)V`
- `.method public static synthetic $r8$lambda$fE_XZ7S0hhHsxQNTfy8mxeJ7kEU(Lcom/pairip/licensecheck/LicenseActivity;Landroid/content/DialogInterface;I)V`
- `.method public static synthetic $r8$lambda$x-JmBIDmuVzGN23Wk7Dd1TBpzO0(Lcom/pairip/licensecheck/LicenseActivity;Landroid/app/PendingIntent;)V`
- `.method public constructor <init>()V`
- `.method private synthetic lambda$showErrorDialog$0()V`
- `.method private synthetic lambda$showErrorDialog$1(Landroid/content/DialogInterface;I)V`
- `.method private synthetic lambda$showPaywallAndCloseApp$0(Landroid/app/PendingIntent;)V`
- `.method private logAndShowErrorDialog(Ljava/lang/String;)V`
- `.method private logAndShowErrorDialog(Ljava/lang/String;Ljava/lang/Exception;)V`
- `.method private showErrorDialog()V`
- `.method private showPaywallAndCloseApp()V`
- `.method protected closeApp()V`
- `.method public onStart()V`

### `LicenseCheckException` — 2 method

- `.method public constructor <init>(Ljava/lang/String;)V`
- `.method public constructor <init>(Ljava/lang/String;Ljava/lang/Throwable;)V`

### `LicenseClient$$ExternalSyntheticLambda0` — 2 method

- `.method public synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseClient;)V`
- `.method public final run()V`

### `LicenseClient$$ExternalSyntheticLambda1` — 2 method

- `.method public synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseClient;Z)V`
- `.method public final run()V`

### `LicenseClient$$ExternalSyntheticLambda2` — 2 method

- `.method public synthetic constructor <init>()V`
- `.method public final run(Ljava/lang/Runnable;)V`

### `LicenseClient$$ExternalSyntheticLambda3` — 2 method

- `.method public synthetic constructor <init>(Landroid/os/Handler;)V`
- `.method public final run(Ljava/lang/Runnable;)V`

### `LicenseClient$$ExternalSyntheticLambda4` — 2 method

- `.method public synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseClient;)V`
- `.method public final run()V`

### `LicenseClient$$ExternalSyntheticLambda5` — 2 method

- `.method public synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseClient;Landroid/os/IBinder;)V`
- `.method public final run()V`

### `LicenseClient$$ExternalSyntheticLambda6` — 2 method

- `.method public synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseClient;Landroid/os/IBinder;)V`
- `.method public final run()V`

### `LicenseClient$$ExternalSyntheticLambda7` — 2 method

- `.method public synthetic constructor <init>()V`
- `.method public final run()V`

### `LicenseClient$1` — 2 method

- `.method constructor <init>()V`
- `.method public run()V`

### `LicenseClient$2` — 2 method

- `.method constructor <init>(Lcom/pairip/licensecheck/LicenseClient;)V`
- `.method public verifyLicense(ILandroid/os/Bundle;)V`

### `LicenseClient$DelayedTaskExecutor` — 3 method

- `.method private constructor <init>()V`
- `.method synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseClient-IA;)V`
- `.method public schedule(Ljava/lang/Runnable;J)V`

### `LicenseClient$ImmediateTaskExecutor` — 1 method

- `.method public abstract run(Ljava/lang/Runnable;)V`

### `LicenseClient$LicenseCheckState` — 5 method

- `.method private static synthetic $values()[Lcom/pairip/licensecheck/LicenseClient$LicenseCheckState;`
- `.method static constructor <clinit>()V`
- `.method private constructor <init>(Ljava/lang/String;I)V`
- `.method public static valueOf(Ljava/lang/String;)Lcom/pairip/licensecheck/LicenseClient$LicenseCheckState;`
- `.method public static values()[Lcom/pairip/licensecheck/LicenseClient$LicenseCheckState;`

### `LicenseClient` — 33 method

- `.method public static synthetic $r8$lambda$GS82Fij7VQePgSFog-s63-Rcyb0(Lcom/pairip/licensecheck/LicenseClient;)V`
- `.method public static synthetic $r8$lambda$gb-vmUiJUmqdCloCudVdY-igh7I(Lcom/pairip/licensecheck/LicenseClient;Landroid/os/IBinder;)V`
- `.method public static synthetic $r8$lambda$q2q7YKfx3jIZHqiUNn7fQ55wwzI(Lcom/pairip/licensecheck/LicenseClient;Z)V`
- `.method public static synthetic $r8$lambda$tTRuJInP7s484yRu-m6AsnoI1z4(Lcom/pairip/licensecheck/LicenseClient;)V`
- `.method public static synthetic $r8$lambda$xzrAfByzooHDT9oIsgTdQvzthuE(Lcom/pairip/licensecheck/LicenseClient;Landroid/os/IBinder;)V`
- `.method static bridge synthetic -$$Nest$mprocessResponse(Lcom/pairip/licensecheck/LicenseClient;ILandroid/os/Bundle;)V`
- `.method static constructor <clinit>()V`
- `.method public constructor <init>(Landroid/content/Context;)V`
- `.method private checkLicenseInternal(Landroid/os/IBinder;)V`
- `.method private connectToLicensingService()V`
- `.method private createCloseAppIntentOrExitIfAppInBackground()Landroid/content/Intent;`
- `.method private static createResultListener(Lcom/pairip/licensecheck/LicenseClient;)Lcom/pairip/licensecheck/ILicenseV2ResultListener;`
- `.method public static getLicensePubKey()Ljava/lang/String;`
- `.method private handleError(Lcom/pairip/licensecheck/LicenseCheckException;)V`
- `.method private isForeground()Z`
- `.method private synthetic lambda$initializeLicenseCheck$0()V`
- `.method private synthetic lambda$initializeLicenseCheck$1(Z)V`
- `.method private synthetic lambda$onServiceConnected$0(Landroid/os/IBinder;)V`
- `.method private synthetic lambda$onServiceConnected$1(Landroid/os/IBinder;)V`
- `.method static synthetic lambda$reportSuccessfulLicenseCheck$0()V`
- `.method static synthetic lambda$static$0(Ljava/lang/Runnable;)V`
- `.method private performLocalInstallerCheck()Z`
- `.method private populateInputDataForLicenseCheckV2(Landroid/os/Parcel;Landroid/os/IBinder;)V`
- `.method private populateInputDataForReportAutoVerifiedLicense(Landroid/os/Parcel;Landroid/os/IBinder;)V`
- `.method private processResponse(ILandroid/os/Bundle;)V`
- `.method private retryOrThrow(Lcom/pairip/licensecheck/LicenseCheckException;)V`
- `.method private retryOrThrow(Lcom/pairip/licensecheck/LicenseCheckException;Z)V`
- `.method private startErrorDialogActivity()V`
- `.method private startPaywallActivity(Landroid/app/PendingIntent;)V`
- `.method public initializeLicenseCheck()V`
- `.method public onServiceConnected(Landroid/content/ComponentName;Landroid/os/IBinder;)V`
- `.method public onServiceDisconnected(Landroid/content/ComponentName;)V`
- `.method public reportSuccessfulLicenseCheck(Landroid/os/IBinder;)V`

### `LicenseContentProvider` — 7 method

- `.method public constructor <init>()V`
- `.method public delete(Landroid/net/Uri;Ljava/lang/String;[Ljava/lang/String;)I`
- `.method public getType(Landroid/net/Uri;)Ljava/lang/String;`
- `.method public insert(Landroid/net/Uri;Landroid/content/ContentValues;)Landroid/net/Uri;`
- `.method public onCreate()Z`
- `.method public query(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;`
- `.method public update(Landroid/net/Uri;Landroid/content/ContentValues;Ljava/lang/String;[Ljava/lang/String;)I`

### `ResponseValidator` — 6 method

- `.method static constructor <clinit>()V`
- `.method private constructor <init>()V`
- `.method private static base64ToJson(Ljava/lang/String;)Lorg/json/JSONObject;`
- `.method private static getPublicKey()Ljava/security/PublicKey;`
- `.method public static validateResponse(Landroid/os/Bundle;Ljava/lang/String;)V`
- `.method private static verifySignature(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/security/PublicKey;)V`
