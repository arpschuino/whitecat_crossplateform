# Release 0.9.1 — État et points en cours

Document de suivi (synchronisé Nextcloud + GitHub) — accessible depuis n'importe quel poste.
Dernière mise à jour : 2026-06-18.

---

## ✅ Ce qui est FAIT

### Builds fonctionnels
- **Windows x64** : `build.bat` → `whitecatbuild/build/white_cat_for_mingw/Whitecat_Crossplatform.exe`
- **Linux x86_64** : build dans un chroot Ubuntu 18.04 (glibc 2.27) via WSL.
  - Compatible Mint 19+, Ubuntu 18.04+, Debian 10+ ET toutes versions plus récentes
    (un binaire glibc ancien tourne sur les glibc plus récentes, pas l'inverse).

### Packages de release (dans `whitecatbuild/release/`)
- `Whitecat_Crossplatform_0.9.1_windows_x64.zip` — exe + 20 DLLs + assets
- `Whitecat_Crossplatform_0.9.1_linux_x86_64.tar.gz` — autonome, lanceur `run.sh`, 38 libs dans `lib/`
- `WhiteCat-0.9.1-x86_64.AppImage` — fichier unique double-cliquable

### Bugs Linux corrigés (tous = vrais bugs, profitent aussi à Windows et au Pi)
1. **KMOD_SCROLL** (SDL < 2.0.18) — guard `SDL_VERSION_ATLEAST`. Commit `47893ee0`.
2. **DPI hints** SDL_HINT_WINDOWS_DPI_* (SDL < 2.24) — `#ifndef` fallback. Commit `a75f3eb5`.
3. **Buffer overflow `do_sprintf_job`** : `string_ratio_x1x2[4]` trop petit pour `sprintf("%.2f")`
   (≥5 octets). Agrandi à `[8]`. + `affichage_time_format` borné. Crash SIGABRT ~1s après l'accueil.
   Commit `2bc2ebad`.
4. **Séparateurs de chemin** `\\` (Windows) → `/` (portable) : 18 chemins dans 9 fichiers.
   La Phase 7H n'avait corrigé QUE les chemins `user/`, pas `saves/`, `audio/`, `import_export/`,
   `plans/`, `ressources/`. Sur Linux les `\\` devenaient des noms de fichiers cassés.
5. **Tableaux Arduino sous-dimensionnés** : `digital_limit` 127→128, `analog_limit` 63→64.
   Les `fread`/`fwrite` utilisent `arduino_*_size` = 128/64, mais les tableaux étaient déclarés
   à 127/63 → débordement d'1 élément → `__fread_chk` abort au chargement d'un show.
   Fichiers : `arduino.h`, `arduino.cpp`.
6. **Art-Net broadcast cassé sous Linux** : `SO_BROADCAST` était positionné depuis le `char`
   global `broadcast='1'` (`sizeof` = 1), mais Linux exige un `int` (4 octets) → option non
   appliquée → `sendto` broadcast échoue avec EACCES (errno 13). Passé un vrai `int`.
   Windows/Winsock tolérait. Commit `9f9f1775`.
7. **Une interface DMX en échec coupait TOUTES les sorties** : `index_init_dmx_ok` est global ;
   un Enttec Pro activé dans la config mais non branché le remettait à 0, ce qui désactivait
   aussi l'Art-Net (qui marchait). Recalcul après `Init_dmx_interface()` : prêt dès qu'au moins
   une interface est active. Commit `37a1227d`. Cross-platform (Windows + Pi).
8. **Label MIDI flottant « 999/999 » + traînées** : en mode affectation MIDI, `DoMouse()`
   dessinait `string_shortview_midi` au curseur (affectation de l'élément survolé, souvent
   `999/999` = non assigné). Doublon avec le moniteur MIDI, et traînées (fond non réeffacé sous
   le dirty-rects sur simple mouvement). Label retiré ; `show_type_midi` conservé (alimente
   `string_last_midi_id` dans la fenêtre MIDI). Commit `7a46dfe5`. Cross-platform.

### Config Art-Net — broadcast dirigé (à documenter côté utilisateur)
Sur une machine à **plusieurs interfaces** (Ethernet + WiFi, ou surtout une interface
**virtuelle** créée par **WSL / VM / VPN / Hyper-V**), le broadcast *limité* `255.255.255.255`
n'est émis que sur **UNE** interface (souvent la mauvaise) → le projecteur ne reçoit rien
alors qu'un moniteur local (Protokol) reçoit. **Utiliser le broadcast dirigé du sous-réseau du
projecteur**, ex. `192.168.1.255`. Le Pi (1 seule interface) marche avec `255.255.255.255`
« par chance ». → à expliquer dans la doc utilisateur (page DMX / Art-Net).

### Outil de diagnostic ajouté (gardé volontairement)
- Handler **SIGSEGV/SIGBUS/SIGFPE** + backtrace POSIX dans `MAIN.cpp` (`posix_crash_handler`).
- Link avec `-rdynamic` dans `Makefile.linux` (noms de fonctions dans le backtrace).
- Le crash écrit la pile dans `wc_debug.txt` (`/tmp/wc_debug.txt` sous AppImage).
- **Pour résoudre un offset en ligne source** (binaire Linux compilé avec `-g`) :
  ```
  ADDR=$(nm BINAIRE | grep " _Z9Load_Showv$" | awk '{print $1}')   # ex. pour Load_Show
  addr2line -e BINAIRE -f -C 0x$(printf "%x" $((0x$ADDR + OFFSET)))
  ```
- Code mort de debug (`debug_crash.txt` avec chemins `C:\...` en dur) : **nettoyé** de save_show.cpp.

### Cycle save → reload Linux : VALIDÉ (testé sur Mint, fonctionne)

---

## 🔧 EN COURS — à faire demain / autre poste

### 0. Placement des fenêtres & install.sh Linux (packaging) — ✅ FAIT (2026-06-18)
**Réalisé** : `install.sh` créé (icône du chat + raccourci menu/bureau + window rule labwc),
committé (`081a57ef`) et testé sur le Pi (insertion window rule + `.desktop` à chemin auto).
`.gitattributes` ajouté (LF forcé pour les `.sh`, `3e820194`). `config_screens.txt` à `0 0`
et `config_dmx` à `0/0/0/0` dans les paquets. `last_save` clean (préparé par l'utilisateur ;
logs parasites `openlayer.log`/`debug_crash.txt`/`wc_debug.txt`/`wcat_audio.log` retirés à la
copie). Date du splash → « 0.9.1 - 17 juin 2026 » (`globals.cpp`, `63538734`). Doc
`installer_whitecat.html` FR+EN : nouvelles sections Linux & Pi, lancement par l'icône (`baa2a018`).
**Paquets régénérés** : Windows `.zip` (14,6 Mo) ✅ et Raspberry Pi `.tar.gz` aarch64 (7,2 Mo) ✅
— voir §2 et §4.

Détail du plan initial (conservé pour référence) :
- **Fenêtre principale en (0,0)** :
  - Windows / X11 (Mint, Ubuntu, Debian classiques) : déjà géré par le code
    (`MoveWindow` / `SDL_SetWindowPosition` à `posX/posY_mainwindow`). Il suffit de mettre
    `pos_x pos_y = 0 0` dans `user/config_screens.txt` des paquets.
  - **Wayland (Pi / labwc)** : l'app NE PEUT PAS se positionner (protocole). Window rule labwc :
    `~/.config/labwc/rc.xml` →
    `<windowRule title="WhiteCat"><action name="MoveTo" x="0" y="0"/></windowRule>`
    (matche aussi `identifier="whitecat"` = app_id défini via `SDL_HINT_APP_ID`). Recharger : `kill -SIGHUP $(pidof labwc)`.
    → à installer par `install.sh`. Testé OK sur le Pi.
- **Position des sous-fenêtres (dont la fenêtre de log « Loaded file… »)** : sauvegardée PAR SHOW
  dans `saves/<show>/config_windows.txt` (ligne « report_SL_X report_SL_Y / … »). PAS dans le code
  (l'init `report_SL_Y` est écrasée par le fichier au chargement). Le `last_save` livré dans les
  paquets doit avoir de BONNES positions — le défaut `30 40` met la fenêtre de log trop haut
  (Windows ok car le show de travail l'avait déplacée à `197 276`). → régler les positions dans le
  `last_save` modèle avant packaging.
- **install.sh** (Linux, à créer) : génère `~/.local/share/applications/whitecat.desktop`
  (chemins auto + `StartupWMClass=whitecat` + `Icon=whitecat`), installe l'icône
  (`~/.local/share/icons/hicolor/256x256/apps/whitecat.png`), et — sous labwc — ajoute la window
  rule de position. Donne l'icône du chat (barre des tâches + menu) et le placement de la fenêtre.

### 1. Enttec Pro non détecté sous Linux (PRIORITÉ)
Le backend Linux (`Src/dmx_backend_linux.h`) scanne uniquement `/dev/ttyUSB0..9`
(`Detect_EnttecProOut`, ligne ~238). Ouverture via `wc_serial_open_dmx(DeviceName, 57600, false)`.

**Diagnostic à lancer sur la machine, Enttec Pro branché :**
```bash
ls -l /dev/ttyUSB*              # le port apparait-il ?
ls -l /dev/ttyACM*             # ou en ACM (CDC) ?
dmesg | grep -i ftdi | tail    # quel driver ?
groups                         # suis-je dans 'dialout' ?
```

**Causes probables (par ordre) :**
- **Permissions** : ports série = groupe `dialout`. Si absent → "Permission denied".
  Fix : `sudo usermod -aG dialout $USER` puis **déconnexion/reconnexion** (obligatoire).
- **Port en ttyACM** : certaines Enttec Pro récentes / clones (DMXKing) apparaissent en
  `/dev/ttyACM0` (CDC), pas `ttyUSB`. Le code ne scanne PAS ttyACM → à AJOUTER dans
  `Detect_EnttecProOut` / `Detect_EnttecProIn` (boucle ttyACM en plus de ttyUSB).
- **brltty** : le service braille d'Ubuntu/Mint capture les puces FTDI.
  Vérifiable dans `dmesg`. Fix : `sudo apt remove brltty` ou le désactiver.

**Action probable côté code** : étendre le scan à `/dev/ttyACM%d` dans les deux fonctions
Detect du backend Linux. Modèle exact de l'interface à confirmer (DMX USB Pro / Pro Mk2 / clone).

**MISE À JOUR (2026-06-02)** : diagnostic fait sur une machine Mint. L'Enttec Pro est bien
détectée : `/dev/ttyUSB0` apparaît (driver FTDI, `crw-rw---- root dialout`). Donc **le code
Linux est correct** (scan ttyUSB OK), pas besoin de modif ttyACM pour cette interface.
Le SEUL blocage était les **permissions** : l'utilisateur n'était pas dans le groupe `dialout`,
et le poste de test n'avait pas de droits sudo. → À refaire sur un poste avec sudo :
`sudo usermod -aG dialout $USER` + déconnexion/reconnexion, puis tester la sortie DMX réelle.
Le scan ttyACM reste une amélioration souhaitable pour les clones (DMXKing), mais non bloquant.

### 2. Build Raspberry Pi (ARM64) — ✅ FAIT (2026-06-18)
**Réalisé** : compilé sur le Pi 4 via SSH (`jacques@192.168.1.135`) avec
`make -f Makefile.linux clean && make -f Makefile.linux -j4` (build propre car
`graphics_backend.h` = source du PCH avait changé). Binaire ELF ARM aarch64, testé OK
(tourne sans crash, date splash correcte, validé visuellement par l'utilisateur).
**Paquet** `Whitecat_Crossplatform_0.9.1_linux_aarch64.tar.gz` (7,2 Mo) créé dans
`whitecatbuild/release/` : cloné depuis le paquet x86_64 (mêmes assets clean), binaire
aarch64 swappé, `chmod +x` (binaire + `.sh`), puis `tar` créé **sur le Pi** (préserve les
permissions — impossible depuis Windows). README adapté (Pi/ARM64, Pi 4-5 recommandé).
NB : `PI3=1` non utilisé (c'est un Pi 4). Reste : compléter `doc/hardware.html` section Pi.

Procédure initiale (référence) :
Compiler SUR le Pi (cross-compil trop lourde depuis ce poste) :
```bash
make -f Makefile.linux PI3=1 -j4
```
Le flag `PI3=1` active les caps FPS réduits (`-DWC_PI3`). Puis packager comme le x86_64.

**Doc à mettre à jour quand le Pi sera prêt** : compléter `doc/hardware.html` (et `hardware_eng.html`)
avec la section Raspberry Pi — config matérielle recommandée, modèle de Pi, installation,
sortie DMX (Enttec Pro / libftdi1 + groupe `dialout`), performances. À reformater au passage
si encore à l'ancien format DokuWiki (modèle : introduction.html / interface.html).

### 3. Doc cue list — crossfade dans les deux sens
Documenter dans `doc/sequentiel.html` (et `sequentiel_eng.html`) l'option **Continuous xfade**
(crossfade enchaînable en descendant PUIS en remontant les potards, sans raccrochage).
La fonctionnalité est déjà codée (0.9.1) ; il manque sa description dans la page cue list.

### 4. Finalisation release GitHub
**État des paquets (2026-06-18, soir)** :
- ✅ **Linux x86_64** — RÉGÉNÉRÉ ce soir : binaire glibc 2.27 (chroot bionic) à jour
  (tous les fixes du jour + **fix label MIDI**), `.tar.gz` (12,8 Mo) + **AppImage** (12,9 Mo),
  38 libs embarquées, aucune dépendance manquante.
  → **À TESTER lundi** sur une machine Linux (transfert clé USB / Nextcloud).
  → Profiter du test pour générer un `last_save` propre SOUS LINUX (New Show + placer les
     fenêtres + quitter) puis régénérer le paquet — voir note last_save ci-dessous.
- 🔴 **Windows** `.zip` — binaire à RÉGÉNÉRER : le zip (14,6 Mo) date d'AVANT le fix label MIDI.
  Copier l'exe à jour de `whitecatbuild/build/white_cat_for_mingw/` dans le paquet + re-zipper.
- 🔴 **Raspberry Pi** `.tar.gz` — binaire à RÉGÉNÉRER : le tar.gz (7,2 Mo) date d'AVANT le fix
  label MIDI. Le binaire aarch64 à jour est DÉJÀ compilé sur le Pi (carte SD) ; le récupérer
  par SSH (`jacques@192.168.1.135`) quand le Pi est rallumé, swap dans le paquet + re-tar SUR le Pi.
- Tag `v0.9.1` + GitHub Release : uploader les 3 archives + AppImage
  (rappel : `whitecatbuild/` n'est PAS dans git).
- Notes de version (reprendre CHANGELOG.md section 0.9.1).

**Note last_save** : le `last_save` du paquet x86_64 (195 fichiers, version 5 juin) diffère de
celui de Windows (240 fichiers, version 18 juin). Le fix `fclose(NULL)` (fichier manquant) évite
le crash, mais pour la cohérence, générer un `last_save` propre sous Linux lundi et le diffuser
aux paquets x86_64 + aarch64 (mêmes .whc 64-bit little-endian).

**Procédure build Linux x86_64 (scripts présents dans `~/wcbuild/` sous WSL Ubuntu, ce poste)** :
chroot bionic `/opt/wcbionic` (glibc 2.27), `sudo` NOPASSWD, projet déjà bind sur `/mnt/d`.
- `bash ~/wcbuild/rebclean.sh` — clean + build dans le chroot (clean OBLIGATOIRE si `.h` du PCH changé)
- `bash ~/wcbuild/repackage_linux.sh` — bundle libs (ldd + blacklist) + `.tar.gz` + AppImage (appimagetool)
- ⚠️ `build_linux.bat` (racine) = ancienne approche Ubuntu-22.04 (glibc récent) → NE PAS utiliser pour la release.

### 4bis. Débogage paquet Linux x86_64 — libs non résolues (2026-06-21)
**Symptôme** : au lancement (`run.sh` ou double-clic), rien ne s'affiche ; `whitecat.log`
contient `./Whitecat_Crossplatform: error while loading shared libraries: libtiff.so.5:
cannot open shared object file`, exit 127. (Testé sur une vraie machine Linux.)

**3 causes en cascade, toutes corrigées** :

1. **`DT_RUNPATH` au lieu de `DT_RPATH`** — `Makefile.linux` linkait `-Wl,-rpath,'$ORIGIN/lib'`
   mais les « new dtags » par défaut produisent `DT_RUNPATH`, qui NE se propage PAS aux
   dépendances transitives (`libtiff` est une dep de `libSDL2_image`, pas du binaire).
   → **FIX commité** `b3286250` : ajout de `-Wl,--disable-new-dtags`.
   Vérif : `readelf -d <bin> | grep -E 'RPATH|RUNPATH'` doit montrer **RPATH (0x0f)**, pas RUNPATH.

2. **`libmodplug.so.1` + `libmad.so.0` absentes du bundle** — deps de `libSDL2_mixer` ;
   `repackage_linux.sh` ne les a pas copiées (cause non élucidée : `ldd` dans le chroot les
   résout pourtant). → contournées à la main :
   `sudo cp -L /opt/wcbionic/usr/lib/x86_64-linux-gnu/{libmodplug.so.1,libmad.so.0} <pkg>/lib/`

3. **`libpulse` avec RUNPATH absolu** — `libpulse.so.0` / `libpulse-simple.so.0` ont
   `RUNPATH = /usr/lib/x86_64-linux-gnu/pulseaudio` (absolu) → `libpulsecommon-11.1.so`
   introuvable (le `DT_RPATH` du binaire ne couvre pas : un `DT_RUNPATH` intermédiaire casse
   la chaîne). `libpulse` vient de `libfluidsynth` (dep de SDL2_mixer). → patché :
   `patchelf --set-rpath '$ORIGIN' <pkg>/lib/libpulse.so.0 <pkg>/lib/libpulse-simple.so.0`
   (`libpulsecommon` est à plat dans `lib/`, et `$ORIGIN` de libpulse = `lib/`).
   Script utilisé : `whitecatbuild/fix_pulse.sh` (NON versionné, sur le poste actuel).

**Validation** : `cd <pkg> && ldd ./Whitecat_Crossplatform | grep 'not found'` → vide (« AUCUNE »).
Le `.tar.gz` corrigé (≈13 Mo) est dans `whitecatbuild/release/` (poste actuel) — À RETESTER.

**À CORRIGER dans `repackage_linux.sh` (`~/wcbuild/`) pour la reproductibilité** — sinon le
prochain repack recasse le paquet :
- passe `ldd` **récursive** sur les `.so` de `lib/` (pas seulement sur le binaire) pour copier
  les deps manquantes (modplug, mad) ;
- passe `patchelf --set-rpath '$ORIGIN'` sur toute lib bundlée ayant un RUNPATH **absolu** (libpulse*).

⚠️ **Sur un AUTRE poste** : le chroot bionic (`/opt/wcbionic`) et les scripts (`~/wcbuild/`)
n'existent QUE sur le poste actuel (WSL « Ubuntu »). Pour rebuilder Linux ailleurs : recréer le
chroot via `build_bionic.sh` (debootstrap bionic + deps, cf. procédure §4), puis `rebclean.sh`
+ `repackage_linux.sh` + les 3 correctifs ci-dessus. `patchelf` requis (`apt install patchelf`).

### 5. Petit nettoyage code
- **Log de debug `[wc_hook] len=…`** affiché dans la console au démarrage (lecture de fichiers,
  ex. `curves_matrix.whc` = 16384). Sans gravité mais à retirer (hook de debug oublié).

---

## Environnement de build Linux (reproductible)

- **WSL** sur le poste Windows, chroot Ubuntu 18.04 dans `/opt/wcbionic` (créé via debootstrap).
- Outils dans `~/wcbuild/` (WSL) : `appimagetool` extrait dans `squashfs-root/`.
- Scripts de build/repack : recréés à la volée (non versionnés). Procédure :
  1. Build dans le chroot : monter `/proc /dev` + bind le projet sur `/opt/wcbionic/proj`,
     `chroot ... make -f Makefile.linux -j8`. Clean obligatoire si `.h` du PCH modifié.
  2. Bundle libs : `ldd` du binaire DANS le chroot, copier les `.so` sauf blacklist
     (glibc/toolchain/GL/X11/wayland = laissées au système cible).
  3. tar.gz : archiver le dossier `Whitecat_Crossplatform_0.9.1_linux_x86_64/`.
  4. AppImage : AppDir (AppRun + .desktop + icône gfx/logo.png) → `appimagetool`.
- Dépendances chroot : `build-essential pkg-config libsdl2-dev libsdl2-ttf-dev
  libsdl2-image-dev libsdl2-mixer-dev libhpdf-dev libasound2-dev libftdi1-dev zlib1g-dev`.

## Casse des polices (rappel cross-platform)
Le code charge `Fonts/xxx.ttf` (F majuscule, nom minuscule) mais le dossier source est
`fonts/` avec noms capitalisés. Linux est sensible à la casse → les packages contiennent
un dossier `Fonts/` (majuscule) avec les 4 polices renommées en minuscule
(axaxax, doom, neuropol, prototype). Windows insensible → marche aussi.
