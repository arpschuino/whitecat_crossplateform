# Release 0.9.1 — État et points en cours

Document de suivi (synchronisé Nextcloud + GitHub) — accessible depuis n'importe quel poste.
Dernière mise à jour : 2026-06-02.

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

### 2. Build Raspberry Pi (ARM64) — pas encore fait
Compiler SUR le Pi (cross-compil trop lourde depuis ce poste) :
```bash
make -f Makefile.linux PI3=1 -j4
```
Le flag `PI3=1` active les caps FPS réduits (`-DWC_PI3`). Puis packager comme le x86_64.

### 3. Finalisation release GitHub
- Tag `v0.9.1`
- GitHub Release avec les 3 archives (Windows zip + Linux tar.gz + AppImage [+ Pi quand prêt])
- Notes de version (reprendre CHANGELOG.md section 0.9.1)

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
